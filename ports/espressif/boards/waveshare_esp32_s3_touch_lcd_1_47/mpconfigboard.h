// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 natheihei
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Waveshare ESP32-S3 Touch LCD 1.47"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.rx = &pin_GPIO44, .tx = &pin_GPIO43}}

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO41, .sda = &pin_GPIO42}} /* for Touchscreen */

#define CIRCUITPY_BOARD_SPI         (2)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO38, .mosi = &pin_GPIO39}, /* for LCD display */ \
                                     {.clock = &pin_GPIO16, .mosi = &pin_GPIO15, .miso = &pin_GPIO17} /* for SD Card */}
