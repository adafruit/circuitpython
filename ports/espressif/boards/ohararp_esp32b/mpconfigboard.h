// This file is part of the CircuitPython project: https://circuitpython.org
// SPDX-FileCopyrightText: Copyright (c) 2025 OHARARP LLC
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "OHARARP ESP32-B"
#define MICROPY_HW_MCU_NAME         "ESP32-S3"

// USB configuration
#define MICROPY_HW_USB_VID          0x303A
#define MICROPY_HW_USB_PID          0x8249

// NeoPixel
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO48)

// Status LED
#define MICROPY_HW_LED              (&pin_GPIO21)
#define MICROPY_HW_LED_STATUS       (&pin_GPIO21)

// Default bus pins
#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO9)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO8)

#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO12)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO        (&pin_GPIO13)

#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)
#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
