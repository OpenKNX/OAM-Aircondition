#pragma once

#ifdef ARDUINO_ARCH_ESP32

#include "SerialHAL.h"
#include <memory>
#include <Arduino.h>
#include "driver/uart.h"

namespace daikin {
namespace hal {

/**
 * ESP32-specific SerialHAL implementation using ESP-IDF UART APIs
 */
class SerialHAL_ESP32 : public SerialHAL {
public:
    explicit SerialHAL_ESP32(HardwareSerial& uart_ref);
    ~SerialHAL_ESP32() override = default;

    bool begin(int rx_pin, int tx_pin, bool rx_inverted, bool tx_inverted, uint32_t baud_rate = 2400) override;
    size_t available() override;
    int read(uint8_t* buffer, size_t max_bytes) override;
    size_t write(const uint8_t* data, size_t length) override;
    bool flush(uint32_t timeout_ms = 50) override;
    void clearRxBuffer() override;

private:
    static const char* TAG;
    HardwareSerial& uart_ref_;
    uart_port_t port_{UART_NUM_1};
    bool initialized_{false};
    
    uart_port_t get_uart_port();
};

} // namespace hal
} // namespace daikin

#endif // ARDUINO_ARCH_ESP32