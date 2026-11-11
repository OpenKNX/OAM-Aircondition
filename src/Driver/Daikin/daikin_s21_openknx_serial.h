#pragma once

#include <cstdint>
#include <string_view>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <Arduino.h>
#include "daikin_s21_openknx_types.h"
#include "hal/SerialHAL.h"

namespace daikin {

// Timeout categories (namespace scope for use in class signatures)
enum class TimeoutType : uint8_t {
  None,
  Ack,
  Busy,
  Rx
};

// Forward declaration - DaikinDriver now handles S21 protocol directly
class DaikinDriver;

class DaikinSerial {
 public:
  static constexpr std::size_t MAX_COMMAND_SIZE{6};
  static constexpr std::size_t STANDARD_PAYLOAD_SIZE{4};
  static constexpr std::size_t EXTENDED_PAYLOAD_SIZE{32}; // FU extension pages are 32 bytes
  static constexpr std::size_t MAX_RESPONSE_SIZE{MAX_COMMAND_SIZE + EXTENDED_PAYLOAD_SIZE + 1U};  // +1 for checksum

  enum class Result : uint8_t {
    Ack,        // Standalone ACK (0x06) with no frame payload
    Nak,        // NAK (0x15)
    Frame,      // Completed framed reply (STX..ETX) with validated checksum
    Timeout,    // Timed out waiting for frame/ack
    Error,      // Framing / checksum / escape error
  };

  // Callback types for parent communication
  using ResultCallback = std::function<void(Result, uint8_t*, size_t)>;
  using IdleCallback = std::function<void()>;

  DaikinSerial(HardwareSerial &uart, int rx_pin, int tx_pin, 
               bool rx_inverted = false, bool tx_inverted = false,
               ResultCallback result_callback = nullptr, 
               IdleCallback idle_callback = nullptr);

  bool initialized() const { return bool(serial_hal_); }
  bool configureTimeouts(uint32_t ack_ms, uint32_t rx_ms);
  bool busy() const { return comm_state != CommState::Idle; }
  void begin();
  void loop();

  bool send_frame(std::string_view cmd, const uint8_t* payload = nullptr, size_t payload_size = 0, bool ack_only = false);
  void force_framed_mode(const char* reason = nullptr); // Force immediate switch back to framed mode (for driver-level recovery)


protected:
  // Timing (will be tuned to spec / wiki guidance)
  static constexpr uint32_t ack_delay_period_ms{45};        // Delay before ACK normally appears
  static constexpr uint32_t rx_timeout_period_ms{900};      // Generous window for framed reply (ACK may precede data)
  
  // Airtime calculation for 2400 baud, 8E2 (1 start + 8 data + 1 parity + 2 stop = 12 bits/byte)
  static constexpr uint32_t ms_per_byte_2400_8e2{5};        // ~5ms per byte at 2400 bps

  // Framing per S21 wiki - no escaping, simple STX + payload + SUM + ETX
  static constexpr uint8_t STX{0x02};
  static constexpr uint8_t ETX{0x03};
  static constexpr uint8_t ACK{0x06};
  static constexpr uint8_t NAK{0x15};
  static constexpr uint8_t ESC{0x1B}; // unused but kept for reference

  struct FrameBuilder {
    std::vector<uint8_t> buf; // contains STX .. payload .. SUM .. ETX (no escaping)
  };

  enum class CommState : uint8_t {
    Idle,          // Waiting to send
    WaitingAck,    // After TX, waiting for ACK/NAK or directly framed data (some units ACK first, some not)
    ReceivingFrame // After ACK (or immediately if no ACK) collecting a framed reply
  };

  void ack_timeout_handler();
  void set_busy_timeout(uint32_t delay_ms);
  void busy_timeout_handler();
  void rx_timeout_handler();
  
  uint8_t calculate_checksum(const uint8_t* data, size_t size); // SUM checksum
  void start_rx_timeout(uint32_t dur_ms, TimeoutType type);

  void finalize_frame();

  std::unique_ptr<hal::SerialHAL> serial_hal_;
  ResultCallback result_callback;
  IdleCallback idle_callback;
  CommState comm_state{};
  std::vector<uint8_t> response{};
  
  std::string expected_response_;
  bool expecting_reply_{true};
  uint32_t ack_timeout_ms_{900}, rx_timeout_ms_{900};

  // Timer management
  uint32_t timeout_start_ms{0};
  uint32_t timeout_duration_ms{0};
  bool timeout_active{false};
  TimeoutType timeout_type{TimeoutType::None};
  // Adaptive fallback tracking
  bool rx_any_since_tx_{false};
  uint32_t last_rx_timestamp_ms{0};
  
  // Non-blocking TX timing
  uint32_t last_tx_start_ms_{0};        // When transmission started
  uint32_t tx_airtime_ms_{0};           // Calculated airtime for current frame

};

} // namespace daikin