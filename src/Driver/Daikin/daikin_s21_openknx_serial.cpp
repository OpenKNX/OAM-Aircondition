#include <type_traits>
#include <string>
#include <Arduino.h>
#include "daikin_s21_openknx_serial.h"
#include "hal/SerialHAL_Factory.h"

namespace daikin {
DaikinSerial::DaikinSerial(HardwareSerial &uart, int rx_pin, int tx_pin, 
                           bool rx_inverted, bool tx_inverted,
                           ResultCallback result_callback, IdleCallback idle_callback)
    : result_callback(result_callback), idle_callback(idle_callback) {
    // Create platform-specific HAL implementation
    serial_hal_ = hal::SerialHAL_Factory::create(uart);
    
    // Initialize the HAL with the provided parameters
    if (!serial_hal_->begin(rx_pin, tx_pin, rx_inverted, tx_inverted, 2400)) {
        serial_hal_.reset(); // Reset on failure
    }
}

void DaikinSerial::begin() {
    // HAL initialization is now done in the constructor
    // This method is kept for compatibility but doesn't need to do anything
    if (!serial_hal_) {
        Serial.println("Error: Serial HAL not initialized");
    }
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
  if (!serial_hal_) return;
  
  constexpr int MAX_BYTES_PER_LOOP = 16;
  uint8_t buffer[MAX_BYTES_PER_LOOP];
  int bytes_processed = 0;

  while (serial_hal_->available() > 0 && bytes_processed < MAX_BYTES_PER_LOOP) {
    int bytes_read = serial_hal_->read(buffer, sizeof(buffer));
    if (bytes_read <= 0) break;
    
    for (int i = 0; i < bytes_read && bytes_processed < MAX_BYTES_PER_LOOP; i++) {
      uint8_t b = buffer[i];
      bytes_processed++;
    
    // Filter out common noise bytes like Faikin does (0x80, 0xE0, etc.) when idle
    // These are often seen as lone bytes between frames in noisy environments
    if ((comm_state == CommState::Idle) && 
        (b == 0x80 || b == 0xE0 || b == 0xF0 || b == 0x00 || b == 0xFF)) {
      //Serial.printf("[RX] NOISE 0x%02X filtered (discarded cleanly)\n", b);
      continue; // Skip processing and don't set rx_any_since_tx_ to avoid timeout reset
    }
    
    //Serial.printf("[RX] 0x%02X state=%d\n", b, (int)comm_state);
    rx_any_since_tx_ = true;

    // Detect STX inline even when in legacy mode - switch back to framed immediately
    if (b == STX && protocol_mode_ == ProtocolMode::UnframedSum) {
      protocol_mode_ = ProtocolMode::FramedSum;
      consecutive_timeouts_ = 0;
      // Serial.println("[S21] Detected STX while in UnframedSum -> switching to FramedSum mode");
      // Continue processing this STX byte in the new framed mode
    }

    switch (comm_state) {
      case CommState::Idle:
        // Ignore noise until we transmit something (but STX detection above still applies)
        break;
      case CommState::WaitingAck:
        if (b == ACK) {
          //Serial.printf("[RX] ACK received");
          if (result_callback) result_callback(Result::Ack, nullptr, 0);
          // Now expect framed reply (may or may not come)
          response.clear();
          comm_state = CommState::ReceivingFrame; // Wait for STX
          start_rx_timeout(rx_timeout_period_ms, TimeoutType::Rx);
        } else if (b == NAK) {
          //Serial.printf("[RX] NAK received");
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
          Serial.printf("[RX] Unexpected byte 0x%02X while waiting for ACK (0x06), NAK (0x15), or STX (0x02)\n", b);
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
  
  // If there were rx bytes, update last-rx timestamp
  if (bytes_processed > 0) {
    last_rx_timestamp_ms = millis();
  }
}

void DaikinSerial::send_frame(std::string_view cmd, const uint8_t* payload, size_t payload_size) {
  if (cmd.size() + payload_size > MAX_COMMAND_SIZE + EXTENDED_PAYLOAD_SIZE) {
    Serial.printf("[ERROR] Tx: Command '%.*s' too large\n", (int)cmd.size(), cmd.data());
    if (result_callback) result_callback(Result::Error, nullptr, 0);
    return;
  }

  // Build raw (unescaped) body: command + payload
  std::vector<uint8_t> body;
  body.reserve(cmd.size() + payload_size);
  for (char c : cmd) body.push_back(static_cast<uint8_t>(c));
  if (payload && payload_size) for (size_t i = 0; i < payload_size; ++i) body.push_back(payload[i]);

  if (protocol_mode_ == ProtocolMode::FramedSum) {
    uint8_t checksum = calculate_checksum(body.data(), body.size());
    std::vector<uint8_t> framed;
    framed.reserve(body.size() + 3); // STX + body + SUM + ETX
    framed.push_back(STX);
    for (uint8_t b : body) framed.push_back(b);
    framed.push_back(checksum);
    framed.push_back(ETX);
    
    // Create TX log with hex dump and JSON format
    // std::string hex_dump;
    // std::string cmd_str(cmd);
    // for (auto v : framed) {
    //   char hex_byte[3];
    //   sprintf(hex_byte, "%02X", v);
    //   hex_dump += hex_byte;
    // }
    //Serial.printf("[S21-TX] {\"protocol\":\"S21\",\"dump\":\"%s\",\"%s\":\"\"}\n", hex_dump.c_str(), cmd_str.c_str());
    
    // Calculate airtime for non-blocking approach
    const uint32_t frame_bytes = framed.size();
    tx_airtime_ms_ = frame_bytes * ms_per_byte_2400_8e2;
    last_tx_start_ms_ = millis();
    
    serial_hal_->write(framed.data(), framed.size());
    // Do NOT flush() - avoid blocking ~150ms for transmission
  } else { // UnframedSum
    uint8_t sum = 0; for (auto b : body) sum = uint8_t(sum + b);
    body.push_back(sum);
    
    // Create TX log for legacy mode too
    // std::string hex_dump;
    // std::string cmd_str(cmd);
    // for (auto v : body) {
    //   char hex_byte[3];
    //   sprintf(hex_byte, "%02X", v);
    //   hex_dump += hex_byte;
    // }
    //Serial.printf("[S21-TX] {\"protocol\":\"S21\",\"dump\":\"%s\",\"%s\":\"\"}\n", hex_dump.c_str(), cmd_str.c_str());
    
    // Calculate airtime for non-blocking approach                
    const uint32_t frame_bytes = body.size();
    tx_airtime_ms_ = frame_bytes * ms_per_byte_2400_8e2;
    last_tx_start_ms_ = millis();
    
    serial_hal_->write(body.data(), body.size());
    // Do NOT flush() - avoid blocking ~150ms for transmission
  }
  rx_any_since_tx_ = false;

  comm_state = CommState::WaitingAck;
  // Include TX airtime in ACK timeout to account for transmission delay
  start_rx_timeout(tx_airtime_ms_ + rx_timeout_period_ms, TimeoutType::Ack);
}

uint8_t DaikinSerial::calculate_checksum(const uint8_t* data, size_t size) {
  uint8_t sum = 0;
  for (size_t i = 0; i < size; ++i) {
    sum += data[i];
  }
  return sum;
}

void DaikinSerial::start_rx_timeout(uint32_t dur_ms, TimeoutType type) {
  timeout_start_ms = millis();
  timeout_duration_ms = dur_ms;
  timeout_type = type;
  timeout_active = true;
}

void DaikinSerial::ack_timeout_handler() {
  // ACK timeout - treat similar to RX timeout if absolutely no bytes arrived
  //Serial.printf("[RX] ACK timeout");
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
  //Serial.printf("[RX] Timeout");
  //Serial.printf("[S21][Dbg] mode=%u rx_any=%d consecutive_timeouts=%u state=%u\n", (unsigned)protocol_mode_, rx_any_since_tx_, consecutive_timeouts_, (unsigned)comm_state);
  if (comm_state == CommState::ReceivingFrame && !response.empty()) {
    //Serial.printf("[RX] Incomplete frame discarded on timeout");
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
    //Serial.printf("[RX] Frame too short");
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
    //Serial.printf("[RX] No payload data in frame");
    if (result_callback) result_callback(Result::Error, nullptr, 0);
    return;
  }

  uint8_t received_checksum = response[response.size() - 2]; // SUM is before ETX
  uint8_t calc = calculate_checksum(payload.data(), payload.size());
  if (received_checksum != calc) {
    Serial.printf("[RX] Checksum mismatch recv=0x%02X calc=0x%02X\n", received_checksum, calc);
    if (result_callback) result_callback(Result::Error, nullptr, 0);
    return;
  }

  // Create RX log with hex dump and JSON format
  // std::string hex_dump;
  // for (auto v : response) {
  //   char hex_byte[3];
  //   sprintf(hex_byte, "%02X", v);
  //   hex_dump += hex_byte;
  // }
  
  // Extract command (first character after STX) and response data for JSON
  std::string cmd_code_hex;
  std::string response_data_hex;
  if (payload.size() >= 1) {
    // Convert command code to hex string (safe for any byte value)
    char hex_byte[3];
    sprintf(hex_byte, "%02X", payload[0]);
    cmd_code_hex = hex_byte;
    if (payload.size() > 1) {
      // Convert response payload to hex string (safe for binary data)
      for (size_t i = 1; i < payload.size(); i++) {
        char hex_byte[3];
        sprintf(hex_byte, "%02X", payload[i]);
        response_data_hex += hex_byte;
      }
    }
  }
  
  //Serial.printf("[S21-RX] {\"protocol\":\"S21\",\"dump\":\"%s\",\"cmd\":\"%s\",\"data\":\"%s\"}\n", hex_dump.c_str(), cmd_code_hex.c_str(), response_data_hex.c_str());

  // Deliver frame payload (command + payload) to upper layer
  if (result_callback) result_callback(Result::Frame, payload.data(), payload.size());
  // Successful frame reception in framed mode resets timeout counter
  if (protocol_mode_ == ProtocolMode::FramedSum) consecutive_timeouts_ = 0;
}

void DaikinSerial::maybe_switch_mode_on_timeout() {
  if (protocol_mode_ == ProtocolMode::FramedSum) {
    if (!rx_any_since_tx_) {
      consecutive_timeouts_++;
      //Serial.printf("FramedSum timeout without RX (%u/6)\n", consecutive_timeouts_);
      

      
      if (consecutive_timeouts_ >= 6) {
        //Serial.printf("[S21] No inbound data after 6 framed attempts -> switching to UnframedSum fallback");
        protocol_mode_ = ProtocolMode::UnframedSum;
        consecutive_timeouts_ = 0;
      }
    } else {
      // We got SOME RX data - reset timeout counter to stay in current mode longer
      consecutive_timeouts_ = 0;
      //Serial.printf("[S21] RX activity detected - resetting timeout counter");
    }
  } else { // UnframedSum
    if (!rx_any_since_tx_) {
      consecutive_timeouts_++;
      //Serial.printf("UnframedSum timeout without RX (%u/4)\n", consecutive_timeouts_);
      
      // Nach 4 stillen Legacy-Versuchen: wieder Framed probieren
      if (consecutive_timeouts_ >= 4) {
        protocol_mode_ = ProtocolMode::FramedSum;
        consecutive_timeouts_ = 0;
        //Serial.println("[S21] Legacy silent after 4 attempts -> probing FramedSum again");
        
        // Optional: RX-Buffer flushen für sauberen Neustart
        serial_hal_->clearRxBuffer();
      }
    } else {
      consecutive_timeouts_ = 0;
      // STX detection is now handled inline in the RX loop for immediate response
    }
  }
}

void DaikinSerial::force_legacy_fallback(const char* reason) {
  if (protocol_mode_ == ProtocolMode::UnframedSum) return; // already legacy
  uint32_t now = millis();
  if (now - last_force_fallback_ms_ < 5000) return; // rate limit
  //Serial.printf("FORCE legacy fallback requested%s%s\n", reason?": ":"", reason?reason:"");
  protocol_mode_ = ProtocolMode::UnframedSum;
  consecutive_timeouts_ = 0;
  last_force_fallback_ms_ = now;
}

void DaikinSerial::force_framed_mode(const char* reason) {
  if (protocol_mode_ == ProtocolMode::FramedSum) return; // already framed
  uint32_t now = millis();
  if (now - last_force_fallback_ms_ < 5000) return; // rate limit (same as legacy)
  //Serial.printf("FORCE framed mode requested%s%s\n", reason?": ":"", reason?reason:"");
  protocol_mode_ = ProtocolMode::FramedSum;
  consecutive_timeouts_ = 0;
  last_force_fallback_ms_ = now;
  
  // Flush RX buffer for clean restart
  serial_hal_->clearRxBuffer();
}
} // namespace daikin
