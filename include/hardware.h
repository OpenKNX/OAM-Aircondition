#pragma once

#include "HardwareConfig.h"

    #ifdef BOARD_AB_PRE_MR16
        #define BOARD_AB_PRE_BASE
        #define DEVICE_ID "AB-PRE-MR16"
        #define HARDWARE_NAME "AB-PRE-MR16"
        #define DEVICE_NAME "AB-SmartHouse Presence MR16"
    #endif

    #ifdef BOARD_AB_PRE_WALL
        #define BOARD_AB_PRE_BASE
        #define DEVICE_ID "AB-PRE-WALL"
        #define HARDWARE_NAME "AB-PRE-WALL"
        #define DEVICE_NAME "AB-SmartHouse Presence Wall"
    #endif

    #ifdef BOARD_AB_TOUCH_ROUND_RP2350
        #define DEVICE_ID "AB-TOUCH-ROUND"
        #define HARDWARE_NAME "AB-TOUCH-ROUND"
        #define DEVICE_NAME "AB-SmartHouse Touch Round"
        #define INFO_LED_PIN 11
        #define INFO_LED_PIN_ACTIVE_ON HIGH
        #define PROG_LED_PIN 10
        #define PROG_LED_PIN_ACTIVE_ON HIGH
        #define PROG_BUTTON_PIN 9
        #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING

        #define KNX_UART_NUM 0
        #define KNX_UART_TX_PIN 12
        #define KNX_UART_RX_PIN 13

        #define SAVE_INTERRUPT_PIN 0

        #define TOUCH_LEFT_PIN 24
        #define TOUCH_RIGHT_PIN 25

        #define I2C_WIRE Wire1
        #define I2C_SDA_PIN 26
        #define I2C_SCL_PIN 27
    #endif

    // Sensormodul auf RP2040 Basis
    #ifdef BOARD_AB_PRE_BASE
        #define PROG_LED_PIN 10
        #define PROG_LED_PIN_ACTIVE_ON HIGH
        #define PROG_BUTTON_PIN 9
        #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING
        #define INFO_LED_PIN 11
        #define INFO_LED_PIN_ACTIVE_ON HIGH
        #define PRESENCE_LED_PIN 28
        #define PRESENCE_LED_PIN_ACTIVE_ON HIGH
        #define MOVE_LED_PIN 29
        #define MOVE_LED_PIN_ACTIVE_ON HIGH
        #define KNX_UART_TX_PIN 12
        #define KNX_UART_RX_PIN 13
        #define SAVE_INTERRUPT_PIN 0
        #define I2C_WIRE Wire1
        #define I2C_SDA_PIN 2
        #define I2C_SCL_PIN 3
        #define HF_SERIAL Serial2
        #define HF_SERIAL_SPEED 115200
        #define HF_POWER_PIN 27
        #define HF_UART_TX_PIN 4
        #define HF_UART_RX_PIN 5
        #define PIR_PIN 26
        #define OPENKNX_BI_GPIO_PINS 14, 15
        #define OPENKNX_BI_GPIO_COUNT 2
        #define OPENKNX_BI_ONLEVEL LOW
    #endif
