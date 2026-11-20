// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "LILYGO T-Keyboard-S3"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL         (&pin_GPIO11)
#define MICROPY_HW_NEOPIXEL_COUNT   (4)

#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO47)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO48)

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = DEFAULT_SPI_BUS_SCK, .mosi = DEFAULT_SPI_BUS_MOSI, .miso = NULL}}

#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)
