// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Bernhard Bablok
//
// SPDX-License-Identifier: MIT

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Seeed Xiao ESP32-S3 Trmnl DIY Kit"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_LED_STATUS (&pin_GPIO21)

// the unpopulated NFC-header also provides UART
#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.rx = &pin_GPIO41, .tx = &pin_GPIO42}}

// SPI0 is used by the display connector
// SPI1 is used by a two secondary flash chips (one unpopulated)
#define CIRCUITPY_BOARD_SPI         (2)
#define CIRCUITPY_BOARD_SPI_PIN     \
    {{.clock = &pin_GPIO7, .mosi = &pin_GPIO9}, \
     {.clock = &pin_GPIO13, .mosi = &pin_GPIO11, .miso = &pin_GPIO12} \
    }
