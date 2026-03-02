#if !defined(ARDUINO_ARCH_ESP32)

#include "SerialHAL_Arduino.h"
#include <Arduino.h>

#if defined(ARDUINO_ARCH_RP2040)
  #include <hardware/gpio.h>  // for gpio_pull_up(), gpio_set_input_hysteresis_enabled(), gpio_set_inover(), gpio_set_outover()
#endif

// Optional: allow users to disable our internal bias if they have external biasing
// #define DAIKIN_DISABLE_INTERNAL_RX_PULLUP 1

namespace daikin {
namespace hal {

SerialHAL_Arduino::SerialHAL_Arduino(HardwareSerial& uart_ref) 
    : uart_(uart_ref) {
}

bool SerialHAL_Arduino::begin(int rx_pin, int tx_pin, bool rx_inverted, bool tx_inverted, uint32_t baud_rate) {
#if defined(ARDUINO_ARCH_RP2040)
    int uart_idx = -1;
    if (&uart_ == &Serial1) uart_idx = 0;
    else if (&uart_ == &Serial2) uart_idx = 1;
    else {
        Serial.println("[RP2040] Unsupported HardwareSerial. Use Serial1 (UART0) or Serial2 (UART1).");
        return false;
    }

    auto valid_tx = uart_idx == 0
        ? (tx_pin==0 || tx_pin==12 || tx_pin==16 || tx_pin==28)
        : (tx_pin==4 || tx_pin==8  || tx_pin==20 || tx_pin==24);

    auto valid_rx = uart_idx == 0
        ? (rx_pin==1 || rx_pin==13 || rx_pin==17 || rx_pin==29)
        : (rx_pin==5 || rx_pin==9  || rx_pin==21 || rx_pin==25);

    if (!valid_tx || !valid_rx) {
        Serial.println("[RP2040] Invalid UART pin choice for this Serial{1/2}. Check mapping: "
                       "UART0 TX={0,12,16,28} RX={1,13,17,29}; "
                       "UART1 TX={4,8,20,24} RX={5,9,21,25}");
        return false;
    }

    // Assign pins to UART function
    gpio_set_function(rx_pin, GPIO_FUNC_UART);
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
#endif

    // --- Bias + hysteresis ---
#if !defined(DAIKIN_DISABLE_INTERNAL_RX_PULLUP)
    #if defined(ARDUINO_ARCH_RP2040)
        gpio_pull_up(rx_pin);
        gpio_set_input_hysteresis_enabled(rx_pin, true);
    #else
        pinMode(rx_pin, INPUT_PULLUP);
    #endif
#endif

#ifdef SERIAL_8E2
    const uint32_t cfg = SERIAL_8E2;
#else
    #error "SERIAL_8E2 not supported by this core; Daikin S21 requires 8E2."
#endif
    uart_.begin(baud_rate, cfg);

    // --- Optional: GPIO-level inversion on RP2040 if available ---
#if defined(ARDUINO_ARCH_RP2040)
    // Always clear to NORMAL first to avoid stale overrides across re-inits
    gpio_set_inover(rx_pin, GPIO_OVERRIDE_NORMAL);
    gpio_set_outover(tx_pin, GPIO_OVERRIDE_NORMAL);
    if (rx_inverted) gpio_set_inover(rx_pin, GPIO_OVERRIDE_INVERT);
    if (tx_inverted) gpio_set_outover(tx_pin, GPIO_OVERRIDE_INVERT);
#else
    (void)rx_inverted; (void)tx_inverted;
#endif

    // --- Drain any garbage BEFORE marking initialized_  ---
    while (uart_.available()) { (void)uart_.read(); }
    
    initialized_ = true;
    
    Serial.print("Arduino UART configured: RX ");
    Serial.print(rx_pin);
#if defined(ARDUINO_ARCH_RP2040)
    if (rx_inverted) Serial.print(" (inverted)");
#endif
    Serial.print(", TX ");
    Serial.print(tx_pin);
#if defined(ARDUINO_ARCH_RP2040)
    if (tx_inverted) Serial.print(" (inverted)");
#endif
    Serial.print(", ");
    Serial.print(baud_rate);
    Serial.print(" ");
    Serial.println("8E2");

    return true;
}

size_t SerialHAL_Arduino::available() {
    if (!initialized_) return 0;
    return uart_.available();
}

int SerialHAL_Arduino::read(uint8_t* buffer, size_t max_bytes) {
    if (!initialized_ || !buffer || max_bytes == 0) return 0;
    
    size_t count = 0;
    while (count < max_bytes && uart_.available()) {
        buffer[count] = uart_.read();
        count++;
    }
    
    return count;
}

size_t SerialHAL_Arduino::write(const uint8_t* data, size_t length) {
    if (!initialized_ || !data || length == 0) return 0;
    
    return uart_.write(data, length);
}

bool SerialHAL_Arduino::flush(uint32_t timeout_ms) {
    if (!initialized_) return false;
    
    // Non-blocking approach: return immediately to avoid stalling main loop
    // TX airtime is calculated and accounted for in the S21 protocol layer
    // uart_.flush();  // Commented out to prevent ~150ms blocking
    return true;
}

void SerialHAL_Arduino::clearRxBuffer() {
    if (!initialized_) return;
    
    while (uart_.available()) {
        uart_.read();
    }
}

} // namespace hal
} // namespace daikin

#endif // !ARDUINO_ARCH_ESP32