#include "SerialHAL_Factory.h"

#ifdef ARDUINO_ARCH_ESP32
  #include "SerialHAL_ESP32.h"
#else
  #include "SerialHAL_Arduino.h"
#endif

namespace daikin {
namespace hal {

std::unique_ptr<SerialHAL> SerialHAL_Factory::create(HardwareSerial& uart_ref) {
#ifdef ARDUINO_ARCH_ESP32
    return std::make_unique<SerialHAL_ESP32>(uart_ref);
#else
    return std::make_unique<SerialHAL_Arduino>(uart_ref);
#endif
}

} // namespace hal
} // namespace daikin