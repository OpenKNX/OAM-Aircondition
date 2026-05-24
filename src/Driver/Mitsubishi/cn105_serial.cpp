#include "cn105_serial.h"

#if defined(ARDUINO_ARCH_RP2040)
#include <hardware/gpio.h>
#endif

namespace mit {

CN105Serial::CN105Serial(HardwareSerial &uart, int rx_pin, int tx_pin,
                         ResultCallback result_cb, IdleCallback idle_cb)
    : uart_(uart), rxPin_(rx_pin), txPin_(tx_pin),
      resultCb_(std::move(result_cb)), idleCb_(std::move(idle_cb)) {}

void CN105Serial::begin() {
#if defined(ARDUINO_ARCH_ESP32)
  uart_.begin(2400, SERIAL_8E1, rxPin_, txPin_);
#elif defined(ARDUINO_ARCH_RP2040)
  // RP2040 Arduino core: pins are assigned via the SDK before begin().
  gpio_set_function(rxPin_, GPIO_FUNC_UART);
  gpio_set_function(txPin_, GPIO_FUNC_UART);
  gpio_pull_up(rxPin_);
  uart_.begin(2400, SERIAL_8E1);
#else
  uart_.begin(2400, SERIAL_8E1);
#endif
  // Drain any garbage that arrived during init
  while (uart_.available())
    (void)uart_.read();
  resetRx();
}

void CN105Serial::sendFrame(const uint8_t *buf, size_t len) {
  // Non-blocking: hand bytes to the UART driver and return. At 2400 baud
  // a flush() would stall the cooperative OpenKNX loop for 30–90 ms.
  uart_.write(buf, len);
  lastTxMs_ = millis();
  txPending_ = true;
  rxBytesSinceTx_ = 0;
  firstRxByteSinceTx_ = 0;
  resetRx();
}

void CN105Serial::resetRx() {
  rxLen_ = 0;
  expectedLen_ = 0;
  lastByteMs_ = 0;
}

void CN105Serial::emit(Result r, const uint8_t *payload, size_t len,
                       uint8_t frameType) {
  if (resultCb_)
    resultCb_(r, payload, len, frameType);
}

void CN105Serial::loop() {
  const uint32_t now = millis();

  // Drain available bytes
  while (uart_.available() > 0) {
    int b = uart_.read();
    if (b < 0)
      break;
    uint8_t byte = static_cast<uint8_t>(b);
    lastByteMs_ = now;
    if (rxBytesSinceTx_ == 0)
      firstRxByteSinceTx_ = byte;
    rxBytesSinceTx_++;

    // Sync to header byte
    if (rxLen_ == 0) {
      if (byte != HEADER_BYTE)
        continue; // skip noise
      rxBuf_[rxLen_++] = byte;
      continue;
    }

    if (rxLen_ < MAX_FRAME_LEN) {
      rxBuf_[rxLen_++] = byte;
    } else {
      // Buffer overrun — drop and resync
      emit(Result::BadFormat, nullptr, 0, 0);
      resetRx();
      if (byte == HEADER_BYTE) {
        rxBuf_[rxLen_++] = byte;
      }
      continue;
    }

    // Once we have the length field, compute expected total length
    if (rxLen_ == PACKET_HEADER_LEN) {
      expectedLen_ = expectedFrameLen(rxBuf_[4]);
      if (expectedLen_ > MAX_FRAME_LEN) {
        emit(Result::BadFormat, nullptr, 0, 0);
        resetRx();
        continue;
      }
    }

    if (expectedLen_ != 0 && rxLen_ == expectedLen_) {
      ParsedFrame pf;
      if (parseFrame(rxBuf_, rxLen_, pf)) {
        txPending_ = false;
        emit(Result::Frame, pf.data, pf.dataLen, pf.type);
      } else {
        emit(Result::BadChecksum, nullptr, 0, 0);
      }
      resetRx();
    }
  }

  // Inter-byte stall: drop partial frame
  if (rxLen_ > 0 && (now - lastByteMs_) > INTERBYTE_TIMEOUT_MS) {
    emit(Result::BadFormat, nullptr, 0, 0);
    resetRx();
  }

  // RX timeout after TX
  if (txPending_ && (now - lastTxMs_) > rxTimeoutMs_) {
    txPending_ = false;
    emit(Result::Timeout, nullptr, 0, 0);
  }

  if (idleCb_ && rxLen_ == 0 && !txPending_) {
    idleCb_();
  }
}

} // namespace mit
