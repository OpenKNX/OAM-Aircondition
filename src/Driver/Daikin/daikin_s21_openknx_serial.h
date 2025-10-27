#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>
#include <functional>
#include <Arduino.h>
#include "daikin_s21_openknx_types.h"
// #include "driver/uart.h"

// Conditional compilation for Daikin S21 debug output
#ifdef DAIKIN_SERIAL_DEBUG
  #define DAIKIN_DEBUG_PRINT(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
  #define DAIKIN_DEBUG_PRINTLN(str) Serial.println(str)
#else
  #define DAIKIN_DEBUG_PRINT(fmt, ...) do {} while(0)
  #define DAIKIN_DEBUG_PRINTLN(str) do {} while(0)
#endif

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
  static constexpr std::size_t EXTENDED_PAYLOAD_SIZE{14}; // for MiscQuery::SoftwareVersion
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

  DaikinSerial(SerialUART &uart, int rx_pin, int tx_pin, 
               ResultCallback result_callback = nullptr, 
               IdleCallback idle_callback = nullptr);

  void begin();
  void restart();
  void loop();
  
  void send_frame(std::string_view cmd, const uint8_t* payload = nullptr, size_t payload_size = 0);
  void force_legacy_fallback(const char* reason = nullptr); // Force immediate switch to legacy unframed + sum mode (used if first cycle entirely silent)
  void force_framed_mode(const char* reason = nullptr); // Force immediate switch back to framed mode (for driver-level recovery)


protected:
  // Timing (will be tuned to spec / wiki guidance)
  static constexpr uint32_t ack_delay_period_ms{45};        // Delay before ACK normally appears
  static constexpr uint32_t next_tx_delay_period_ms{35};    // Inter-command delay
  static constexpr uint32_t error_delay_period_ms{3000};    // Cooldown after error
  static constexpr uint32_t rx_timeout_period_ms{900};      // Generous window for framed reply (ACK may precede data)

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

  enum class ProtocolMode : uint8_t {
    FramedSum,     // STX/ETX + SUM checksum (no escaping)
    UnframedSum    // Legacy fallback: raw command+payload+SUM checksum, expect simple ACK or raw bytes
  };

  void set_ack_timeout(int bytes_received);
  void ack_timeout_handler();
  void set_busy_timeout(uint32_t delay_ms);
  void busy_timeout_handler();
  void rx_timeout_handler();
  
  uint8_t calculate_checksum(std::span<const uint8_t> data); // SUM checksum
  void start_rx_timeout(uint32_t dur_ms, TimeoutType type);
  void maybe_switch_mode_on_timeout();
  void record_rx_byte();

  void finalize_frame();

  SerialUART *uart;
  int rx_pin;
  int tx_pin;
  bool rx_inverted;
  bool tx_inverted;
  // uart_port_t port_{UART_NUM_1}; // Cached port number
  ResultCallback result_callback;
  IdleCallback idle_callback;
  CommState comm_state{};
  std::vector<uint8_t> response{MAX_RESPONSE_SIZE};
  
  // Timer management
  uint32_t timeout_start_ms{0};
  uint32_t timeout_duration_ms{0};
  bool timeout_active{false};
  TimeoutType timeout_type{TimeoutType::None};
  // Adaptive fallback tracking
  ProtocolMode protocol_mode_{ProtocolMode::FramedSum};
  uint16_t consecutive_timeouts_{0};
  bool rx_any_since_tx_{false};
  uint32_t last_force_fallback_ms_{0};

};

} // namespace daikin