// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Benjamin Shockley
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Waveshare ESP32-S3 LCD 1.47"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO17)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO16)

#define CIRCUITPY_BOARD_SPI         (2)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO40, .mosi = &pin_GPIO45}, \
                                     {.clock = &pin_GPIO14, .mosi = &pin_GPIO15, .miso = &pin_GPIO16}}
