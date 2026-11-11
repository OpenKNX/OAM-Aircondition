#include <type_traits>
#include <string>
#include <Arduino.h>
#include "daikin_s21_openknx_serial.h"
#include "hal/SerialHAL_Factory.h"
#include "s21_protocol.h"

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
    int bytes_read = serial_hal_->read(buffer, MAX_BYTES_PER_LOOP - bytes_processed);
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

    switch (comm_state) {
      case CommState::Idle:
        // Ignore noise until we transmit something (no unsolicited S21 data is expected)
        break;
      case CommState::WaitingAck:
        if (b == ACK) {
          response.clear();
          comm_state = expecting_reply_ ? CommState::ReceivingFrame : CommState::Idle;
          timeout_active = false;
          if (expecting_reply_) start_rx_timeout(rx_timeout_ms_, TimeoutType::Rx);
          if (result_callback) result_callback(Result::Ack, nullptr, 0);
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
          start_rx_timeout(rx_timeout_ms_, TimeoutType::Rx);
        } else {
          // Unknown byte - log but keep waiting for proper ACK/NAK/STX
          Serial.printf("[RX] Unexpected byte 0x%02X while waiting for ACK (0x06), NAK (0x15), or STX (0x02)\n", b);
        }
        break;
      case CommState::ReceivingFrame:
        if (b == STX) {
          response.clear(); // resynchronize after a truncated/corrupt frame
          response.push_back(STX);
          start_rx_timeout(rx_timeout_ms_, TimeoutType::Rx);
          break;
        }
        if (response.empty()) break; // ACK is not the frame's STX
        if (response.size() >= MAX_RESPONSE_SIZE + 2) {
          response.clear();
          comm_state = CommState::Idle;
          timeout_active = false;
          if (result_callback) result_callback(Result::Error, nullptr, 0);
          break;
        }
        response.push_back(b);
        start_rx_timeout(rx_timeout_ms_, TimeoutType::Rx);
        if (b == ETX) {
          comm_state = CommState::Idle;
          timeout_active = false;
          finalize_frame();
          response.clear();
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

bool DaikinSerial::configureTimeouts(uint32_t ack_ms, uint32_t rx_ms) {
  if (busy() || ack_ms<50 || rx_ms<50 || ack_ms>30000 || rx_ms>30000) return false;
  ack_timeout_ms_=ack_ms; rx_timeout_ms_=rx_ms; return true;
}

bool DaikinSerial::send_frame(std::string_view cmd, const uint8_t* payload, size_t payload_size, bool ack_only) {
  if (busy()) return false;
  auto error=[&](){ if(result_callback) result_callback(Result::Error,nullptr,0); return false; };
  if (!serial_hal_ || cmd.empty() || cmd.size()>MAX_COMMAND_SIZE || payload_size>EXTENDED_PAYLOAD_SIZE || (payload_size && !payload)) return error();
  for(char c:cmd) if(!((c>='A' && c<='Z') || (c>='a' && c<='z') || (c>='0' && c<='9'))) return error();
  for(size_t i=0;i<payload_size;i++) if(s21::forbidden(payload[i])) return error();
  expecting_reply_ = !ack_only && cmd[0]!='D' && !(cmd=="FB" && payload_size);
  expected_response_ = s21::responseHeader(cmd);
  response.clear();
  std::vector<uint8_t> body(cmd.begin(),cmd.end());
  if(payload_size) body.insert(body.end(),payload,payload+payload_size);
  const uint8_t checksum=calculate_checksum(body.data(),body.size());
  std::vector<uint8_t> framed{STX}; framed.insert(framed.end(),body.begin(),body.end());
  framed.push_back(checksum); framed.push_back(ETX);
  tx_airtime_ms_=framed.size()*ms_per_byte_2400_8e2;
  last_tx_start_ms_=millis();
  if(serial_hal_->write(framed.data(),framed.size())!=framed.size()) return error();
  comm_state=CommState::WaitingAck;
  start_rx_timeout(tx_airtime_ms_+ack_timeout_ms_,TimeoutType::Ack);
  return true;
}

uint8_t DaikinSerial::calculate_checksum(const uint8_t* data, size_t size) {
  uint8_t sum = 0;
  for (size_t i = 0; i < size; ++i) {
    sum += data[i];
  }
  // S21 shields only these checksum collisions; payload bytes are unchanged.
  if (sum == STX || sum == ETX || sum == ACK || sum == NAK) sum += 2;
  return sum;
}

void DaikinSerial::start_rx_timeout(uint32_t dur_ms, TimeoutType type) {
  timeout_start_ms = millis();
  timeout_duration_ms = dur_ms;
  timeout_type = type;
  timeout_active = true;
}

void DaikinSerial::ack_timeout_handler() {
  rx_timeout_handler();
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
  comm_state = CommState::Idle;
  response.clear();
  timeout_active = false;
  if (result_callback) result_callback(Result::Timeout, nullptr, 0);
}

void DaikinSerial::finalize_frame() {
  // response currently: STX ... payload ... SUM ... ETX (no escaping)
  if (response.size() < 5) { // STX + 2-byte command + checksum + ETX
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

  for(size_t i=0;i<payload.size();i++) if(s21::forbidden(payload[i])) {
    if(result_callback) result_callback(Result::Error,nullptr,0);
    return;
  }

  uint8_t received_checksum = response[response.size() - 2]; // SUM is before ETX
  uint8_t calc = calculate_checksum(payload.data(), payload.size());
  if (received_checksum != calc) {
    Serial.printf("[RX] Checksum mismatch recv=0x%02X calc=0x%02X\n", received_checksum, calc);
    if (result_callback) result_callback(Result::Error, nullptr, 0);
    return;
  }

  // Complete the read handshake only after validating the full frame.
  if (serial_hal_->write(&ACK, 1) != 1) { // queued TX; never flush/block the task loop
    if (result_callback) result_callback(Result::Error, nullptr, 0);
    return;
  }

  // A valid but unrelated reply is acknowledged, then ignored while the original
  // transaction remains active. Do not let a stale frame consume a newer query.
  if (payload.size() <= expected_response_.size() ||
      !std::equal(expected_response_.begin(), expected_response_.end(), payload.begin())) {
    comm_state = CommState::ReceivingFrame;
    start_rx_timeout(rx_timeout_ms_, TimeoutType::Rx);
    return;
  }

  // Deliver frame payload (command + payload) to upper layer
  if (result_callback) result_callback(Result::Frame, payload.data(), payload.size());

}

void DaikinSerial::force_framed_mode(const char*) {
  // Recovery always uses the documented STX/ETX transport on every S21 version.
  comm_state = CommState::Idle;
  response.clear();
  timeout_active = false;
  if (serial_hal_) serial_hal_->clearRxBuffer();
}
} // namespace daikin
