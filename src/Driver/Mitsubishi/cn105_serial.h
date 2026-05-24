#pragma once

#include "cn105_packet.h"
#include <Arduino.h>
#include <cstddef>
#include <cstdint>
#include <functional>

namespace mit {

class CN105Serial {
public:
  enum class Result : uint8_t {
    Frame,       // Valid frame received (header + checksum OK)
    Timeout,     // No bytes within rx_timeout_ms after a TX
    BadChecksum, // Header looked valid but checksum mismatch
    BadFormat,   // Bytes received but not a valid frame
  };

  using ResultCallback = std::function<void(Result, const uint8_t *payload,
                                            size_t len, uint8_t frameType)>;
  using IdleCallback = std::function<void()>;

  CN105Serial(HardwareSerial &uart, int rx_pin, int tx_pin,
              ResultCallback result_cb, IdleCallback idle_cb);

  void begin();
  void loop();
  void sendFrame(const uint8_t *buf, size_t len);

  bool isBusyTx() const { return txPending_; }

  // RX timeout configurable for connect retry tuning.
  void setRxTimeoutMs(uint32_t ms) { rxTimeoutMs_ = ms; }

  // Diagnostics: total bytes seen on RX since last TX, and the first such byte.
  size_t rxBytesSinceTx() const { return rxBytesSinceTx_; }
  uint8_t firstRxByteSinceTx() const { return firstRxByteSinceTx_; }

private:
  void emit(Result r, const uint8_t *payload, size_t len, uint8_t frameType);
  void resetRx();

  HardwareSerial &uart_;
  int rxPin_;
  int txPin_;
  ResultCallback resultCb_;
  IdleCallback idleCb_;

  static constexpr size_t MAX_FRAME_LEN = PACKET_LEN_STANDARD; // 22

  uint8_t rxBuf_[MAX_FRAME_LEN]{};
  size_t rxLen_{0};
  size_t expectedLen_{0};
  uint32_t lastByteMs_{0};
  uint32_t lastTxMs_{0};
  bool txPending_{false};
  uint32_t rxTimeoutMs_{1500};

  // Diagnostic counters reset on every TX.
  size_t rxBytesSinceTx_{0};
  uint8_t firstRxByteSinceTx_{0};

  // Inter-byte timeout: at 2400 8E1 each byte is ~4.6 ms. If a partial
  // frame stalls for >100 ms we drop it and re-sync on the next 0xFC.
  static constexpr uint32_t INTERBYTE_TIMEOUT_MS = 100;
};

} // namespace mit
