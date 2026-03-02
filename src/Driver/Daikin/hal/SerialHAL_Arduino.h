#pragma once

#ifndef ARDUINO_ARCH_ESP32

#include "SerialHAL.h"
#include <Arduino.h>
#include <memory>

namespace daikin {
namespace hal {

/**
 * Generic Arduino SerialHAL implementation for RP2040 and other platforms
 */
class SerialHAL_Arduino : public SerialHAL {
public:
    explicit SerialHAL_Arduino(HardwareSerial& uart_ref);
    ~SerialHAL_Arduino() override = default;

    bool begin(int rx_pin, int tx_pin, bool rx_inverted, bool tx_inverted, uint32_t baud_rate = 2400) override;
    size_t available() override;
    int read(uint8_t* buffer, size_t max_bytes) override;
    size_t write(const uint8_t* data, size_t length) override;
    bool flush(uint32_t timeout_ms = 50) override;
    void clearRxBuffer() override;

private:
    HardwareSerial& uart_;
    bool initialized_{false};
};

} // namespace hal
} // namespace daikin

#endif // !ARDUINO_ARCH_ESP32