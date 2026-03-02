#ifdef ARDUINO_ARCH_ESP32

#include "SerialHAL_ESP32.h"
#include <Arduino.h>
#include "driver/uart.h"
#include "driver/gpio.h" 
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

namespace daikin {
namespace hal {

const char* SerialHAL_ESP32::TAG = "daikin_hal_esp32";

uart_port_t SerialHAL_ESP32::get_uart_port() {
    // Map HardwareSerial reference to UART port number based on constructor parameter
    // Note: Serial (UART0) is typically used for USB/debug, avoid comparison due to HWCDC type issues on ESP32-S3
    if (&uart_ref_ == &Serial1) return UART_NUM_1;
    if (&uart_ref_ == &Serial2) return UART_NUM_2;
    
    // For air conditioning communication, we should only use Serial1 or Serial2
    ESP_LOGW(TAG, "Unknown HardwareSerial reference, defaulting to UART_NUM_1");
    return UART_NUM_1; // Default fallback
}

SerialHAL_ESP32::SerialHAL_ESP32(HardwareSerial& uart_ref) 
    : uart_ref_(uart_ref) {
    port_ = get_uart_port();
}

bool SerialHAL_ESP32::begin(int rx_pin, int tx_pin, bool rx_inverted, bool tx_inverted, uint32_t baud_rate) {
    esp_err_t err = 0;
    
    // GPIO validation and reset
    if (!err && (rx_pin < 0 || !GPIO_IS_VALID_GPIO(rx_pin))) {
        ESP_LOGE(TAG, "Invalid RX pin: %d", rx_pin);
        err = ESP_FAIL;
    }
    if (!err) {
        err = gpio_reset_pin((gpio_num_t)rx_pin);
    }
    if (!err && (tx_pin < 0 || !GPIO_IS_VALID_OUTPUT_GPIO(tx_pin))) {
        ESP_LOGE(TAG, "Invalid TX pin: %d", tx_pin);
        err = ESP_FAIL;
    }
    if (!err) {
        err = gpio_reset_pin((gpio_num_t)tx_pin);
    }
    if (err) {
        ESP_LOGE(TAG, "GPIO setup failed: %s", esp_err_to_name(err));
        return false;
    }
    
    uart_config_t uart_config = {
        .baud_rate = static_cast<int>(baud_rate),
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_2,  // S21 protocol uses 8E2
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    if (!err) {
        err = uart_param_config(port_, &uart_config);
    }
    
    if (!err) {
        err = uart_set_pin(port_, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    }
    
    if (!err) {
        err = gpio_pullup_en((gpio_num_t)rx_pin);
    }
    
    if (!err) {
        err = uart_driver_install(port_, 1024, 0, 0, NULL, 0);
    }
    
    // Per-line inversion AFTER driver install
    if (!err) {
        uint32_t inv = 0;
        if (rx_inverted) inv |= UART_SIGNAL_RXD_INV;
        if (tx_inverted) inv |= UART_SIGNAL_TXD_INV;
        err = uart_set_line_inverse(port_, inv);
    }
    
    if (!err) {
        err = uart_set_rx_full_threshold(port_, 1);
    }
    
    if (!err) {
        err = uart_flush_input(port_);
    }
    
    if (err) {
        ESP_LOGE(TAG, "UART setup failed: %s", esp_err_to_name(err));
        return false;
    }
    
    initialized_ = true;
    ESP_LOGI(TAG, "ESP32 UART configured: RX %d (%s), TX %d (%s), %lu 8E2",
             rx_pin, rx_inverted ? "inverted" : "normal",
             tx_pin, tx_inverted ? "inverted" : "normal",
             baud_rate);
    
    return true;
}

size_t SerialHAL_ESP32::available() {
    if (!initialized_) return 0;
    
    size_t avail = 0;
    uart_get_buffered_data_len(port_, &avail);
    return avail;
}

int SerialHAL_ESP32::read(uint8_t* buffer, size_t max_bytes) {
    if (!initialized_ || !buffer || max_bytes == 0) return 0;
    
    return uart_read_bytes(port_, buffer, max_bytes, 0 /* no wait */);
}

size_t SerialHAL_ESP32::write(const uint8_t* data, size_t length) {
    if (!initialized_ || !data || length == 0) return 0;
    
    int result = uart_write_bytes(port_, (const char*)data, length);
    return (result >= 0) ? result : 0;
}

bool SerialHAL_ESP32::flush(uint32_t timeout_ms) {
    if (!initialized_) return false;
    
    // Non-blocking approach: return immediately to avoid stalling main loop  
    // TX airtime is calculated and accounted for in the S21 protocol layer
    // esp_err_t err = uart_wait_tx_done(port_, pdMS_TO_TICKS(timeout_ms));
    // return (err == ESP_OK);
    return true;
}

void SerialHAL_ESP32::clearRxBuffer() {
    if (!initialized_) return;
    
    uart_flush_input(port_);
}

} // namespace hal
} // namespace daikin

#endif // ARDUINO_ARCH_ESP32