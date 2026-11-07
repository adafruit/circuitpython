// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Brian Nelson
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "Freenove ESP32 Display 3.5 inch"
#define MICROPY_HW_MCU_NAME         "ESP32"

// Blue element of the common-anode RGB LED
#define MICROPY_HW_LED_STATUS       (&pin_GPIO17)
#define MICROPY_HW_LED_STATUS_INVERTED (1)

#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO0)

#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO32)
#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO25)

#define CIRCUITPY_BOARD_SPI         (2)
#define CIRCUITPY_BOARD_SPI_PIN     { \
        {.clock = &pin_GPIO18, .mosi = &pin_GPIO23, .miso = &pin_GPIO19}, /* SD card and SPI header */ \
        {.clock = &pin_GPIO14, .mosi = &pin_GPIO13, .miso = &pin_GPIO12}, /* LCD and touch */ \
}

// UART pins attached to the CH340C USB-to-serial converter
#define CIRCUITPY_CONSOLE_UART_TX   (&pin_GPIO1)
#define CIRCUITPY_CONSOLE_UART_RX   (&pin_GPIO3)
