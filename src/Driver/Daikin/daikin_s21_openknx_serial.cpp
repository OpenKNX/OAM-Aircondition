#include <numeric>
#include <type_traits>
#include <Arduino.h>
#include "daikin_s21_openknx_serial.h"
#include "utils.h"
// ESP-IDF includes for UART configuration
#include "soc/gpio_reg.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_err.h"

#include "hal/uart_types.h"

// Build flag validation - ensure serial configuration is defined
#ifndef OPENKNX_AIR_CONDITION_SERIAL
  #error "OPENKNX_AIR_CONDITION_SERIAL must be defined (e.g., -D OPENKNX_AIR_CONDITION_SERIAL=Serial2)"
#endif
#ifndef OPENKNX_AIR_CONDITION_SERIAL_RX
  #error "OPENKNX_AIR_CONDITION_SERIAL_RX must be defined (e.g., -D OPENKNX_AIR_CONDITION_SERIAL_RX=26)"
#endif
#ifndef OPENKNX_AIR_CONDITION_SERIAL_TX
  #error "OPENKNX_AIR_CONDITION_SERIAL_TX must be defined (e.g., -D OPENKNX_AIR_CONDITION_SERIAL_TX=27)"
#endif

namespace daikin {

static const char *const TAG = "daikin_s21.serial";

// Helper function to map HardwareSerial objects to uart_port_t
constexpr uart_port_t get_uart_port() {
  // Map build flag serial object to ESP-IDF UART port
  if (&OPENKNX_AIR_CONDITION_SERIAL == &Serial) {
    return UART_NUM_0;
  } else if (&OPENKNX_AIR_CONDITION_SERIAL == &Serial1) {
    return UART_NUM_1;
  } else if (&OPENKNX_AIR_CONDITION_SERIAL == &Serial2) {
    return UART_NUM_2;
  } else {
    return UART_NUM_2;
  }
}

DaikinSerial::DaikinSerial(HardwareSerial &uart, int rx_pin, int tx_pin, 
                           ResultCallback result_callback, IdleCallback idle_callback)
    : uart(uart), rx_pin(rx_pin), tx_pin(tx_pin), 
      result_callback(result_callback), idle_callback(idle_callback) {}

void DaikinSerial::begin() {
  esp_err_t err = 0;
  
  // GPIO validation and reset
  if (!err && (rx_pin < 0 || !GPIO_IS_VALID_GPIO(rx_pin))) {
    ESP_LOGE(TAG, "Invalid RX pin: %d", rx_pin);
    err = ESP_FAIL;
  }
  if (!err) {
    err = gpio_reset_pin((gpio_num_t)rx_pin);
  }
  if (!err && (tx_pin < 0 || !GPIO_IS_VALID_OUTPUT_GPIO(tx_pin))) {
    ESP_LOGE(TAG, "Invalid TX pin: %d", tx_pin);
    err = ESP_FAIL;
  }
  if (!err) {
    err = gpio_reset_pin((gpio_num_t)tx_pin);
  }
  if (err) {
    ESP_LOGE(TAG, "GPIO setup failed: %s", esp_err_to_name(err));
    return;
  }
  
  // Use UART port derived from build flags
  const uart_port_t uart_num = get_uart_port();
  
  uart_config_t uart_config = {
    .baud_rate = 2400,  // S21 protocol
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_EVEN,
    .stop_bits = UART_STOP_BITS_2,  // S21 protocol
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT, 
  };
  
  if (!err) {
    err = uart_param_config(uart_num, &uart_config); // Configure UART parameters first (before driver install)
  }
  
  if (!err) {
    err = uart_set_pin(uart_num, tx_pin, rx_pin, -1, -1); // Set pins, disable the RTS/CTS hardware flow control pins
  }
  
  if (!err) {
    err = gpio_pullup_en((gpio_num_t)rx_pin);  // Enable RX pull-up
  }
  
  if (!err) {
    // Parse polarity from build flags - negative pin numbers indicate inversion
    bool rx_invert = (OPENKNX_AIR_CONDITION_SERIAL_RX < 0);
    bool tx_invert = (OPENKNX_AIR_CONDITION_SERIAL_TX < 0);
    
    uint8_t invert_mask = 0;
    if (rx_invert) invert_mask |= UART_SIGNAL_RXD_INV;
    if (tx_invert) invert_mask |= UART_SIGNAL_TXD_INV;
    
    err = uart_set_line_inverse(uart_num, invert_mask);
  }
  
  if (!err) {
    err = uart_driver_install(uart_num, 1024, 0, 0, NULL, 0);   // Install UART driver --> 1024 RX buffer, 0 TX buffer, no event queue
  }
  
  if (!err) {
    err = uart_set_rx_full_threshold(uart_num, 1);   // Set RX threshold
  }
  
  if (!err) {
    err = uart_flush(uart_num); //flush after setup (non-blocking version)
  }
  
  if (err) {
    ESP_LOGE(TAG, "UART setup failed: %s", esp_err_to_name(err));
    return;
  }
  
  // Parse polarity from build flags - negative pin numbers indicate inversion
  bool rx_invert = (OPENKNX_AIR_CONDITION_SERIAL_RX < 0);
  bool tx_invert = (OPENKNX_AIR_CONDITION_SERIAL_TX < 0);
  
  // Also initialize Arduino HardwareSerial for compatibility with existing code
  uart.begin(2400, SERIAL_8E2, rx_pin, tx_pin, rx_invert, tx_invert);
  
  ESP_LOGI(TAG, "S21 UART configured: RX pin %d (%s, pull-up), TX pin %d (%s), 2400 8E2", 
           rx_pin, rx_invert ? "inverted" : "normal", 
           tx_pin, tx_invert ? "inverted" : "normal");
}

void DaikinSerial::loop() {
  // Handle timeouts
  if (timeout_active && (millis() - timeout_start_ms >= timeout_duration_ms)) {
    timeout_active = false;
    switch (timeout_type) {
      case TimeoutType::Ack:
        ack_timeout_handler();
        break;
      case TimeoutType::Busy:
        busy_timeout_handler();
        break;
      case TimeoutType::Rx:
        rx_timeout_handler();
        break;
      default:
        break;
    }
  }
  
  // Non-blocking serial processing: limit bytes per loop iteration
  constexpr int MAX_BYTES_PER_LOOP = 8; // Prevent blocking on large bursts
  int bytes_processed = 0;
  
  while (uart.available() && bytes_processed < MAX_BYTES_PER_LOOP) {
    uint8_t b = uart.read();
    bytes_processed++;
    
    // Filter out common noise bytes like Faikin does (0x80, 0xE0, etc.) when idle
    // These are often seen as lone bytes between frames in noisy environments
    if ((comm_state == CommState::Idle) && 
        (b == 0x80 || b == 0xE0 || b == 0xF0 || b == 0x00 || b == 0xFF)) {
      DAIKIN_DEBUG_PRINT("[RX] NOISE 0x%02X filtered (discarded cleanly)\n", b);
      continue; // Skip processing and don't set rx_any_since_tx_ to avoid timeout reset
    }
    
    DAIKIN_DEBUG_PRINT("[RX] 0x%02X state=%d\n", b, (int)comm_state);
    rx_any_since_tx_ = true;

    switch (comm_state) {
      case CommState::Idle:
        // Ignore noise until we transmit something
        break;
      case CommState::WaitingAck:
        if (b == ACK) {
          DAIKIN_DEBUG_PRINTLN("[RX] ACK received");
          if (result_callback) result_callback(Result::Ack, nullptr, 0);
          // Now expect framed reply (may or may not come)
          response.clear();
          comm_state = CommState::ReceivingFrame; // Wait for STX
          start_rx_timeout(rx_timeout_period_ms, TimeoutType::Rx);
        } else if (b == NAK) {
          DAIKIN_DEBUG_PRINTLN("[RX] NAK received");
            if (result_callback) result_callback(Result::Nak, nullptr, 0);
            comm_state = CommState::Idle;
            timeout_active = false;
        } else if (b == STX) {
          // Some units may skip ACK and go straight to frame
          response.clear();
          response.push_back(b); // store STX sentinel separately (not included in checksum calc)
          comm_state = CommState::ReceivingFrame;
          start_rx_timeout(rx_timeout_period_ms, TimeoutType::Rx);
        } else {
          // Unknown byte - log but keep waiting for proper ACK/NAK/STX
          ESP_LOGE(TAG, "[RX] Unexpected byte 0x%02X while waiting for ACK (0x06), NAK (0x15), or STX (0x02)\n", b);
        }
        break;
      case CommState::ReceivingFrame:
        response.push_back(b);
        start_rx_timeout(rx_timeout_period_ms, TimeoutType::Rx); // refresh window
        if (b == ETX) {
          // Attempt to decode + validate frame
          finalize_frame();
          comm_state = CommState::Idle;
          timeout_active = false;
        }
        break;
    }
  }
}

void DaikinSerial::send_frame(std::string_view cmd, const uint8_t* payload, size_t payload_size) {
  if (cmd.size() + payload_size > MAX_COMMAND_SIZE + EXTENDED_PAYLOAD_SIZE) {
    ESP_LOGE(TAG, "[ERROR] Tx: Command '%.*s' too large\n", (int)cmd.size(), cmd.data());
    if (result_callback) result_callback(Result::Error, nullptr, 0);
    return;
  }

  // Build raw (unescaped) body: command + payload
  std::vector<uint8_t> body;
  body.reserve(cmd.size() + payload_size);
  for (char c : cmd) body.push_back(static_cast<uint8_t>(c));
  if (payload && payload_size) for (size_t i = 0; i < payload_size; ++i) body.push_back(payload[i]);

  if (protocol_mode_ == ProtocolMode::FramedSum) {
    uint8_t checksum = calculate_checksum(std::span<const uint8_t>(body.begin(), body.end()));
    std::vector<uint8_t> framed;
    framed.reserve(body.size() + 3); // STX + body + SUM + ETX
    framed.push_back(STX);
    for (uint8_t b : body) framed.push_back(b);
    framed.push_back(checksum);
    framed.push_back(ETX);
    
    // Create TX log with hex dump and JSON format
    std::string hex_dump;
    std::string cmd_str(cmd);
    for (auto v : framed) {
      char hex_byte[3];
      sprintf(hex_byte, "%02X", v);
      hex_dump += hex_byte;
    }
    DAIKIN_DEBUG_PRINT("[S21-TX] {\"protocol\":\"S21\",\"dump\":\"%s\",\"%s\":\"\"}\n", 
                       hex_dump.c_str(), cmd_str.c_str());
    
    uart.write(framed.data(), framed.size());
  } else { // UnframedSum
    uint8_t sum = 0; for (auto b : body) sum = uint8_t(sum + b);
    body.push_back(sum);
    
    // Create TX log for legacy mode too
    std::string hex_dump;
    std::string cmd_str(cmd);
    for (auto v : body) {
      char hex_byte[3];
      sprintf(hex_byte, "%02X", v);
      hex_dump += hex_byte;
    }
    DAIKIN_DEBUG_PRINT("[S21-TX] {\"protocol\":\"S21\",\"dump\":\"%s\",\"%s\":\"\"}\n", 
                       hex_dump.c_str(), cmd_str.c_str());
                  
    uart.write(body.data(), body.size());
  }
  uart.flush();
  rx_any_since_tx_ = false;

  comm_state = CommState::WaitingAck;
  start_rx_timeout(rx_timeout_period_ms, TimeoutType::Ack);
}

uint8_t DaikinSerial::calculate_checksum(std::span<const uint8_t> data) {
  uint8_t sum = 0;
  for (auto b : data) sum = uint8_t(sum + b);
  return sum;
}

void DaikinSerial::start_rx_timeout(uint32_t dur_ms, TimeoutType type) {
  timeout_start_ms = millis();
  timeout_duration_ms = dur_ms;
  timeout_type = type;
  timeout_active = true;
}

void DaikinSerial::set_ack_timeout(int bytes_received) {
  timeout_start_ms = millis();
  timeout_duration_ms = ack_delay_period_ms;
  timeout_type = TimeoutType::Ack;
  timeout_active = true;
}

void DaikinSerial::ack_timeout_handler() {
  // ACK timeout - treat similar to RX timeout if absolutely no bytes arrived
  DAIKIN_DEBUG_PRINTLN("[RX] ACK timeout");
  if (!rx_any_since_tx_) {
    // escalate as if full timeout for fallback purposes
    maybe_switch_mode_on_timeout();
  }
}

void DaikinSerial::set_busy_timeout(uint32_t delay_ms) {
  timeout_start_ms = millis();
  timeout_duration_ms = delay_ms;
  timeout_type = TimeoutType::Busy;
  timeout_active = true;
}

void DaikinSerial::busy_timeout_handler() {
  if (idle_callback) idle_callback();
}

void DaikinSerial::rx_timeout_handler() {
  DAIKIN_DEBUG_PRINTLN("[RX] Timeout");
  DAIKIN_DEBUG_PRINT("[S21][Dbg] mode=%u rx_any=%d consecutive_timeouts=%u state=%u\n", (unsigned)protocol_mode_, rx_any_since_tx_, consecutive_timeouts_, (unsigned)comm_state);
  if (comm_state == CommState::ReceivingFrame && !response.empty()) {
    DAIKIN_DEBUG_PRINTLN("[RX] Incomplete frame discarded on timeout");
    if (result_callback) result_callback(Result::Timeout, nullptr, 0);
  } else if (comm_state == CommState::WaitingAck) {
    if (result_callback) result_callback(Result::Timeout, nullptr, 0);
  }
  comm_state = CommState::Idle;
  maybe_switch_mode_on_timeout();
}





void DaikinSerial::finalize_frame() {
  // response currently: STX ... payload ... SUM ... ETX (no escaping)
  if (response.size() < 4) { // STX + min(1 data) + checksum + ETX
    DAIKIN_DEBUG_PRINTLN("[RX] Frame too short");
    if (result_callback) result_callback(Result::Error, nullptr, 0);
    return;
  }

  // Extract payload between STX and ETX (excluding checksum)
  std::vector<uint8_t> payload;
  payload.reserve(response.size() - 3); // exclude STX, SUM, ETX
  for (size_t i = 1; i < response.size() - 2; ++i) { // skip STX, exclude SUM+ETX
    payload.push_back(response[i]);
  }
  
  if (payload.empty()) {
    DAIKIN_DEBUG_PRINTLN("[RX] No payload data in frame");
    if (result_callback) result_callback(Result::Error, nullptr, 0);
    return;
  }

  uint8_t received_checksum = response[response.size() - 2]; // SUM is before ETX
  uint8_t calc = calculate_checksum(std::span<const uint8_t>(payload.begin(), payload.end()));
  if (received_checksum != calc) {
    ESP_LOGE(TAG, "[RX] Checksum mismatch recv=0x%02X calc=0x%02X\n", received_checksum, calc);
    if (result_callback) result_callback(Result::Error, nullptr, 0);
    return;
  }

  // Create RX log with hex dump and JSON format
  std::string hex_dump;
  for (auto v : response) {
    char hex_byte[3];
    sprintf(hex_byte, "%02X", v);
    hex_dump += hex_byte;
  }
  
  // Extract command (first character after STX) and response data for JSON
  std::string cmd_code;
  std::string response_data;
  if (payload.size() >= 1) {
    cmd_code = char(payload[0]); // First char is the response command (G1, G5, etc.)
    if (payload.size() > 1) {
      // Convert response payload to string (may contain non-printable chars)
      for (size_t i = 1; i < payload.size(); i++) {
        response_data += char(payload[i]);
      }
    }
  }
  
  DAIKIN_DEBUG_PRINT("[S21-RX] {\"protocol\":\"S21\",\"dump\":\"%s\",\"%s\":\"%s\"}\n", 
                     hex_dump.c_str(), cmd_code.c_str(), response_data.c_str());

  // Deliver frame payload (command + payload) to upper layer
  if (result_callback) result_callback(Result::Frame, payload.data(), payload.size());
  // Successful frame reception in framed mode resets timeout counter
  if (protocol_mode_ == ProtocolMode::FramedSum) consecutive_timeouts_ = 0;
}

void DaikinSerial::maybe_switch_mode_on_timeout() {
  if (protocol_mode_ == ProtocolMode::FramedSum) {
    if (!rx_any_since_tx_) {
      consecutive_timeouts_++;
      ESP_LOGW(TAG, "FramedSum timeout without RX (%u/6)", consecutive_timeouts_);
      

      
      if (consecutive_timeouts_ >= 6) {
        DAIKIN_DEBUG_PRINTLN("[S21] No inbound data after 6 framed attempts -> switching to UnframedSum fallback");
        protocol_mode_ = ProtocolMode::UnframedSum;
        consecutive_timeouts_ = 0;
      }
    } else {
      // We got SOME RX data - reset timeout counter to stay in current mode longer
      consecutive_timeouts_ = 0;
      DAIKIN_DEBUG_PRINTLN("[S21] RX activity detected - resetting timeout counter");
    }
  } else { // UnframedSum
    if (rx_any_since_tx_) {
      // If we detect an STX (framed) inside legacy mode, switch back
      for (auto b : response) if (b == STX) { protocol_mode_ = ProtocolMode::FramedSum; DAIKIN_DEBUG_PRINTLN("[S21] Detected framed STX while in legacy -> switching back to FramedSum"); break; }
    }
  }
}

void DaikinSerial::force_legacy_fallback(const char* reason) {
  if (protocol_mode_ == ProtocolMode::UnframedSum) return; // already legacy
  uint32_t now = millis();
  if (now - last_force_fallback_ms_ < 5000) return; // rate limit
  ESP_LOGW(TAG, "FORCE legacy fallback requested%s%s", reason?": ":"", reason?reason:"");
  protocol_mode_ = ProtocolMode::UnframedSum;
  consecutive_timeouts_ = 0;
  last_force_fallback_ms_ = now;
}







} // namespace daikin
