// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Yaraslau Kurmyza
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "ESP32-S3 Super Mini"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

// WS2812 RGB LED on GPIO48 (shared with Red power LED - hardware coexists:
// NeoPixel ignores static DC, Red LED tolerates pulses. Exposed as board.RED_LED
// and board.NEOPIXEL; cannot use both simultaneously in CircuitPython due to
// single-pin driver limitation)
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO48)

// Default I2C pins (using safe GPIO pins)
#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO6)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO7)

// Default SPI pins (using FSPI on safe GPIOs)
#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO36)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO35)
#define DEFAULT_SPI_BUS_MISO        (&pin_GPIO37)

// Default UART (UART0 on GPIO43/44)
#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)

// Double-tap on BOOT button (GPIO0)
#define DOUBLE_TAP_PIN              (&pin_GPIO0)

// Reduce WiFi TX power due to small PCB antenna
#define CIRCUITPY_WIFI_DEFAULT_TX_POWER   (15)
