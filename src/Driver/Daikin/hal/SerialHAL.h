#pragma once

#include <cstdint>
#include <cstddef>

namespace daikin {
namespace hal {

/**
 * Hardware Abstraction Layer for Serial Communication
 * 
 * This interface abstracts platform-specific serial operations
 * to enable the same S21 protocol code to work on ESP32, RP2040, etc.
 */
class SerialHAL {
public:
    virtual ~SerialHAL() = default;

    /**
     * Initialize the serial port with the specified configuration
     * @param rx_pin RX pin number (absolute, not signed)
     * @param tx_pin TX pin number (absolute, not signed)  
     * @param rx_inverted true if RX signal should be inverted
     * @param tx_inverted true if TX signal should be inverted
     * @param baud_rate baud rate (typically 2400 for S21)
     * @return true if initialization succeeded, false otherwise
     */
    virtual bool begin(int rx_pin, int tx_pin, bool rx_inverted, bool tx_inverted, uint32_t baud_rate = 2400) = 0;

    /**
     * Check how many bytes are available in the RX buffer
     * @return number of bytes available to read
     */
    virtual size_t available() = 0;

    /**
     * Read bytes from the RX buffer (non-blocking)
     * @param buffer buffer to store read bytes
     * @param max_bytes maximum number of bytes to read
     * @return actual number of bytes read (0 if none available)
     */
    virtual int read(uint8_t* buffer, size_t max_bytes) = 0;

    /**
     * Write bytes to the TX buffer
     * @param data data to write
     * @param length number of bytes to write
     * @return number of bytes actually written
     */
    virtual size_t write(const uint8_t* data, size_t length) = 0;

    /**
     * Wait for all pending TX data to be transmitted
     * @param timeout_ms maximum time to wait in milliseconds
     * @return true if all data was transmitted, false on timeout
     */
    virtual bool flush(uint32_t timeout_ms = 50) = 0;

    /**
     * Clear the RX buffer
     */
    virtual void clearRxBuffer() = 0;
};

} // namespace hal
} // namespace daikin