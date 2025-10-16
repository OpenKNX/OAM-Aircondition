#include <numeric>
#include <type_traits>
#include <Arduino.h>
#include "daikin_s21_openknx_serial.h"
#include "utils.h"
// // ESP-IDF includes for UART configuration
// #include "soc/gpio_reg.h"
// #include "driver/gpio.h"
// #include "driver/uart.h"
// #include "esp_log.h"
// #include "esp_err.h"

// Auto-polarity detection order: RX inv, both inv, none, TX inv
static constexpr uint8_t kAutoOrder[4] = { 2, 3, 0, 1 };
// #include "hal/uart_types.h"

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

// // Helper function to map HardwareSerial objects to uart_port_t
// constexpr uart_port_t get_uart_port() {
//   // Map build flag serial object to ESP-IDF UART port
//   if (&OPENKNX_AIR_CONDITION_SERIAL == &Serial) {
//     return UART_NUM_0;
//   } else if (&OPENKNX_AIR_CONDITION_SERIAL == &Serial1) {
//     return UART_NUM_1;
//   } else if (&OPENKNX_AIR_CONDITION_SERIAL == &Serial2) {
//     return UART_NUM_2;
//   } else {
//     return UART_NUM_2;
//   }
// }

DaikinSerial::DaikinSerial(SerialUART &uart, int rx_pin, int tx_pin, 
                           ResultCallback result_callback, IdleCallback idle_callback,
                           bool initial_rx_invert, bool initial_tx_invert,
                           bool user_specified_polarity)
    : rx_pin(rx_pin), tx_pin(tx_pin), 
      result_callback(result_callback), idle_callback(idle_callback),
      current_rx_invert_(initial_rx_invert), current_tx_invert_(initial_tx_invert),
      user_specified_polarity_(user_specified_polarity),
      original_rx_invert_(initial_rx_invert), original_tx_invert_(initial_tx_invert) {}

void DaikinSerial::begin() {

    bool err = false;
    // err = OPENKNX_AIR_CONDITION_SERIAL.setRX(OPENKNX_AIR_CONDITION_SERIAL_RX);
    // err = err || OPENKNX_AIR_CONDITION_SERIAL.setTX(OPENKNX_AIR_CONDITION_SERIAL_TX);
    uart = uart1;
    // GPIO-Funktion setzen
    gpio_set_function(rx_pin, GPIO_FUNC_UART);
    gpio_set_function(tx_pin, GPIO_FUNC_UART);

    // pins invertieren
    gpio_set_inover(rx_pin, 0); // current_rx_invert_ ? 1 : 0);
    gpio_set_inover(tx_pin, 0); // current_tx_invert_ ? 1 : 0);
    
    // RX Pull-up aktivieren
    gpio_pull_up(rx_pin);

    // UART initialisieren
    uart_init(uart, 2400);
    uart_set_format(uart, 8, 2, UART_PARITY_EVEN);
    uart_set_hw_flow(uart, false, false);
    uart_set_fifo_enabled(uart, false); // Optional: FIFO deaktivieren


    restart();
  // Use UART port derived from build flags
  // const uart_port_t uart_num = get_uart_port();
  
  // uart_config_t uart_config = {
  //   .baud_rate = 2400,  // S21 protocol
  //   .data_bits = UART_DATA_8_BITS,
  //   .parity = UART_PARITY_EVEN,
  //   .stop_bits = UART_STOP_BITS_2,  // S21 protocol
  //   .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
  //   .source_clk = UART_SCLK_DEFAULT, 
  // };
  
  // if (!err) {
  //   err = uart_param_config(uart_num, &uart_config); // Configure UART parameters first (before driver install)
  // }
  
  // if (!err) {
  //   err = uart_set_pin(uart_num, tx_pin, rx_pin, -1, -1); // Set pins, disable the RTS/CTS hardware flow control pins
  // }
  
  // if (!err) {
  //   err = gpio_pullup_en((gpio_num_t)rx_pin);  // Enable RX pull-up
  // }
  
  // if (!err) {
  //   // Use the inversion settings passed to constructor
  //   uint8_t invert_mask = 0;
  //   if (current_rx_invert_) invert_mask |= UART_SIGNAL_RXD_INV;
  //   if (current_tx_invert_) invert_mask |= UART_SIGNAL_TXD_INV;
    
  //   err = uart_set_line_inverse(uart_num, invert_mask);
  // }
  
  // if (!err) {
  //   err = uart_driver_install(uart_num, 1024, 0, 0, NULL, 0);   // Install UART driver --> 1024 RX buffer, 0 TX buffer, no event queue
  // }
  
  // if (!err) {
  //   err = uart_set_rx_full_threshold(uart_num, 1);   // Set RX threshold
  // }
  
  // if (!err) {
  //   err = uart_flush(uart_num); //flush after setup (non-blocking version)
  // }
  
  // if (err) {
  //   ESP_LOGE(TAG, "UART setup failed: %s", esp_err_to_name(err));
  //   return;
  // }
  
}

void DaikinSerial::restart() {
    // Flush UART
    uart_tx_wait_blocking(uart);
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
  
  while (uart_is_readable(uart) && bytes_processed < MAX_BYTES_PER_LOOP) {
    uint8_t b = uart_getc(uart);
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
          // ESP_LOGE(TAG, "[RX] Unexpected byte 0x%02X while waiting for ACK (0x06), NAK (0x15), or STX (0x02)\n", b);
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
    // ESP_LOGE(TAG, "[ERROR] Tx: Command '%.*s' too large\n", (int)cmd.size(), cmd.data());
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
    
    // uart.write(framed.data(), framed.size());
    for (int i = 0; i < framed.size(); ++i) {
        uart_putc_raw(uart, framed.data()[i]);
    }
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
                  
    // uart.write(body.data(), body.size());
    for (int i = 0; i < body.size(); ++i) {
        uart_putc_raw(uart, body.data()[i]);
    }
  }
  // uart.flush();
  // Flush UART
  uart_tx_wait_blocking(uart);
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
    // ESP_LOGE(TAG, "[RX] Checksum mismatch recv=0x%02X calc=0x%02X\n", received_checksum, calc);
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
      // ESP_LOGW(TAG, "FramedSum timeout without RX (%u/8)", consecutive_timeouts_);
      
      if (user_specified_polarity_) {
        // User specified polarity - return to original settings after failures
        if (consecutive_timeouts_ == 4) {
          // Reset to original user-specified polarity instead of toggling
          current_rx_invert_ = original_rx_invert_;
          current_tx_invert_ = original_tx_invert_;
          
          // Also update Arduino HardwareSerial for compatibility
          restart();

          //   ESP_LOGI(TAG, "No RX after 4 attempts -> returning to user-specified polarity: RX=%s, TX=%s", 
          //            current_rx_invert_?"inverted":"normal", current_tx_invert_?"inverted":"normal");
          // } else {
          //   ESP_LOGE(TAG, "Failed to reset to user polarity: %s", esp_err_to_name(err));
          // }
          consecutive_timeouts_ = 0; // Reset counter to give original config more attempts
        }
        // Skip UART variant cycling for user-specified polarity
        // if (consecutive_timeouts_ == 6) { /* skip try_next_uart_variant() */ }
      } else {
        // Auto-detection mode - use original cycling logic
        // After 4 no-RX timeouts, try toggling both inversions and reset counter
        if (consecutive_timeouts_ == 4) {
          current_rx_invert_ = !current_rx_invert_;
          current_tx_invert_ = !current_tx_invert_;
          
          // // Apply polarity change using ESP-IDF UART driver
          // const uart_port_t uart_num = get_uart_port();
          
          // uint8_t invert_mask = 0;
          // if (current_rx_invert_) invert_mask |= UART_SIGNAL_RXD_INV;
          // if (current_tx_invert_) invert_mask |= UART_SIGNAL_TXD_INV;
          
          // esp_err_t err = uart_set_line_inverse(uart_num, invert_mask);
          // if (err == ESP_OK) {
          //   uart_flush(uart_num); // Flush after polarity change
            
          //   ESP_LOGI(TAG, "No RX after 4 attempts -> toggling inversions: RX=%s, TX=%s", 
          //            current_rx_invert_?"inverted":"normal", current_tx_invert_?"inverted":"normal");
          // } else {
          //   ESP_LOGE(TAG, "Failed to toggle polarity: %s", esp_err_to_name(err));
          // }
          restart();
          consecutive_timeouts_ = 0; // Reset counter to give new config full attempts
        }
        // After 6 timeouts still nothing: cycle to next UART variant (parity/stop tweak)
        if (consecutive_timeouts_ == 6) {
          try_next_uart_variant();
        }
      }
      
      if (consecutive_timeouts_ >= 8) {
        DAIKIN_DEBUG_PRINTLN("[S21] No inbound data after 8 framed attempts -> switching to UnframedSum fallback");
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
  // ESP_LOGW(TAG, "FORCE legacy fallback requested%s%s", reason?": ":"", reason?reason:"");
  protocol_mode_ = ProtocolMode::UnframedSum;
  consecutive_timeouts_ = 0;
  last_force_fallback_ms_ = now;
}

void DaikinSerial::try_next_uart_variant() {
  uart_variant_index_++;
  
  // Enhanced variant order with TX/RX inversion combinations:
  // 0: 8E2 RX only inverted (Faikin default)
  // 1: 8E2 no inversion
  // 2: 8E2 RX+TX inverted
  // 3: 8E2 TX only inverted
  uint32_t config = SERIAL_8E2;
  switch (uart_variant_index_) {
    case 1: current_rx_invert_ = false; current_tx_invert_ = false; config = SERIAL_8E2; break;
    case 2: current_rx_invert_ = true; current_tx_invert_ = true; config = SERIAL_8E2; break;
    case 3: current_rx_invert_ = false; current_tx_invert_ = true; config = SERIAL_8E2; break;
    default:
      // ESP_LOGW(TAG, "UART variant cycling exhausted - staying on 8E2 RX only inverted");
      uart_variant_index_ = 3; // clamp
      current_rx_invert_ = true; current_tx_invert_ = false; config = SERIAL_8E2; break;
  }
  
  // const uart_port_t uart_num = get_uart_port();   // Apply polarity change using ESP-IDF UART driver
  
  // uint8_t invert_mask = 0;
  // if (current_rx_invert_) invert_mask |= UART_SIGNAL_RXD_INV;
  // if (current_tx_invert_) invert_mask |= UART_SIGNAL_TXD_INV;
  
  // esp_err_t err = uart_set_line_inverse(uart_num, invert_mask);
  // if (err != ESP_OK) {
  //   ESP_LOGE(TAG, "Failed to set UART variant %u polarity: %s", uart_variant_index_, esp_err_to_name(err));
  //   return;
  // }
  
  // uart_flush(uart_num);  // Flush after polarity change
          

  // Also update Arduino HardwareSerial for compatibility
  restart();
  // uart.setRxInvert(current_rx_invert_);
  
  // ESP_LOGI(TAG, "UART variant %u applied: config=%s RX_invert=%s TX_invert=%s", 
  //          (unsigned)uart_variant_index_,
  //          (config==SERIAL_8E2?"8E2": config==SERIAL_8E1?"8E1":"8N1"), 
  //          current_rx_invert_?"inverted":"normal", current_tx_invert_?"inverted":"normal");
  consecutive_timeouts_ = 0; // Reset counter to give new variant full attempts
}

// try all 4 combinations systematically
bool DaikinSerial::try_next_polarity_combo() {
  if (user_specified_polarity_ && !fallback_mode_) {
    // Nothing to rotate in user-fixed mode (unless in fallback)
    // ESP_LOGI(TAG, "User-specified polarity fixed; not rotating combos");
    return false;
  }

  auto_order_cursor_++;
  if (auto_order_cursor_ >= 4) {
    if (fallback_mode_) {
      // Fallback auto-scan also failed, revert to user hint and give up
      // ESP_LOGW(TAG, "Fallback auto-scan exhausted, reverting to user-specified polarity");
      current_rx_invert_ = original_rx_invert_;
      current_tx_invert_ = original_tx_invert_;
      fallback_mode_ = false;
      user_hint_timeout_count_ = 0;
      
      // Apply the reverted polarity
      restart();

    //   uint8_t invert_mask = 0;
    //   if (current_rx_invert_) invert_mask |= UART_SIGNAL_RXD_INV;
    //   if (current_tx_invert_) invert_mask |= UART_SIGNAL_TXD_INV;
    //   uart_set_line_inverse(get_uart_port(), invert_mask);
    //   uart_flush(get_uart_port());
    //   uart.setRxInvert(current_rx_invert_);
      
    //   ESP_LOGI(TAG, "Reverted to user hint: TX=%s RX=%s",
    //            current_tx_invert_ ? "inverted" : "normal",
    //            current_rx_invert_ ? "inverted" : "normal");
    // } else {
    //   ESP_LOGW(TAG, "All polarity combinations exhausted (order: 2,3,0,1)");
    }
    return false;
  }
  polarity_combo_index_ = kAutoOrder[auto_order_cursor_];

  // Apply TX/RX flags for this index
  switch (polarity_combo_index_) {
    case 0: current_tx_invert_ = false; current_rx_invert_ = false; break; // none
    case 1: current_tx_invert_ = true;  current_rx_invert_ = false; break; // TX inv
    case 2: current_tx_invert_ = false; current_rx_invert_ = true;  break; // RX inv
    case 3: current_tx_invert_ = true;  current_rx_invert_ = true;  break; // both
  }

  // uint8_t invert_mask = 0;
  // if (current_rx_invert_) invert_mask |= UART_SIGNAL_RXD_INV;
  // if (current_tx_invert_) invert_mask |= UART_SIGNAL_TXD_INV;
  // esp_err_t err = uart_set_line_inverse(get_uart_port(), invert_mask);
  // if (err != ESP_OK) {
  //   ESP_LOGE(TAG, "Failed to set polarity combo %u: %s", polarity_combo_index_, esp_err_to_name(err));
  //   return false;
  // }

  // uart_flush(get_uart_port());
  // uart.setRxInvert(current_rx_invert_);

  // ESP_LOGI(TAG, "Switched to polarity combo %u: TX=%s RX=%s",
  //          polarity_combo_index_,
  //          current_tx_invert_ ? "inverted" : "normal",
  //          current_rx_invert_ ? "inverted" : "normal");
  restart();

  consecutive_timeouts_ = 0;
  rx_any_since_tx_ = false;
  user_hint_timeout_count_ = 0;  // Reset user hint timeout on successful communication
  return true;
}

void DaikinSerial::reset_polarity_detection() {
  if (user_specified_polarity_) {
    // Apply exactly what the user asked for and STOP
    current_rx_invert_ = original_rx_invert_;
    current_tx_invert_ = original_tx_invert_;

    // uint8_t invert_mask = 0;
    // if (current_rx_invert_) invert_mask |= UART_SIGNAL_RXD_INV;
    // if (current_tx_invert_) invert_mask |= UART_SIGNAL_TXD_INV;
    // uart_set_line_inverse(get_uart_port(), invert_mask);
    // uart_flush(get_uart_port());
    // uart.setRxInvert(current_rx_invert_);

    restart();

    // Keep polarity_combo_index_ consistent with the applied choice
    polarity_combo_index_ = (current_tx_invert_ ? 1 : 0) + (current_rx_invert_ ? 2 : 0);
    auto_order_cursor_ = -1; // not used in user-specified mode

    // ESP_LOGI(TAG, "Reset to user-specified polarity: TX=%s RX=%s",
    //          current_tx_invert_ ? "inverted" : "normal",
    //          current_rx_invert_ ? "inverted" : "normal");
  } else {
    // Auto mode: start before the first entry in our preferred order
    auto_order_cursor_ = -1;
    polarity_combo_index_ = -1; // will be set by try_next_polarity_combo
    // Immediately move to the first auto combo
    try_next_polarity_combo();
  }
  
  // Reset fallback state
  user_hint_timeout_count_ = 0;
  fallback_mode_ = false;
  
  // Also update Arduino HardwareSerial for compatibility
  // uart.setRxInvert(current_rx_invert_);
  
  consecutive_timeouts_ = 0;
  rx_any_since_tx_ = false;
}

void DaikinSerial::handle_timeout_fallback() {
  if (!user_specified_polarity_ || fallback_mode_) {
    // Already in auto mode or fallback, nothing special to do
    return;
  }
  
  user_hint_timeout_count_++;
  // ESP_LOGD(TAG, "User hint timeout %u/%u", user_hint_timeout_count_, MAX_USER_HINT_TIMEOUTS);
  
  if (user_hint_timeout_count_ >= MAX_USER_HINT_TIMEOUTS) {
    // User hint appears dead, fall back to auto-scan for recovery
    fallback_mode_ = true;
    auto_order_cursor_ = -1;  // Start from beginning of auto order
    polarity_combo_index_ = -1;
    
    // ESP_LOGW(TAG, "No response with user-specified polarity (TX=%s RX=%s) after %u attempts; scanning RX/TX inversion", 
    //          original_tx_invert_ ? "inverted" : "normal",
    //          original_rx_invert_ ? "inverted" : "normal",
    //          MAX_USER_HINT_TIMEOUTS);
    
    // Start auto-scan
    // try_next_polarity_combo();
  }
}

} // namespace daikin
