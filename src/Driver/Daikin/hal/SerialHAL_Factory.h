#pragma once

#include "SerialHAL.h"
#include <memory>
#include <Arduino.h>

namespace daikin {
namespace hal {

/**
 * Factory for creating platform-specific SerialHAL implementations
 */
class SerialHAL_Factory {
public:
    /**
     * Create a SerialHAL instance for the current platform
     * @param uart_ref Reference to the Arduino HardwareSerial object (for compatibility)
     * @return Unique pointer to the created SerialHAL instance
     */
    static std::unique_ptr<SerialHAL> create(HardwareSerial& uart_ref);
};

} // namespace hal
} // namespace daikin