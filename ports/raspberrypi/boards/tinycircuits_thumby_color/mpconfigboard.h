// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME           "TinyCircuits Thumby Color"
#define MICROPY_HW_MCU_NAME             "rp2350"

#define DEFAULT_I2C_BUS_SCL             (&pin_GPIO9)
#define DEFAULT_I2C_BUS_SDA             (&pin_GPIO8)

#define CIRCUITPY_BOARD_I2C             (1)
#define CIRCUITPY_BOARD_I2C_PIN         {{.scl = DEFAULT_I2C_BUS_SCL, .sda = DEFAULT_I2C_BUS_SDA}}

#define DEFAULT_SPI_BUS_SCK             (&pin_GPIO18)
#define DEFAULT_SPI_BUS_MOSI            (&pin_GPIO19)

#define CIRCUITPY_BOARD_LCD_DC          (&pin_GPIO16)
#define CIRCUITPY_BOARD_LCD_CS          (&pin_GPIO17)
#define CIRCUITPY_BOARD_LCD_RESET       (&pin_GPIO11)
#define CIRCUITPY_BOARD_LCD_BACKLIGHT   (&pin_GPIO20)

#define CIRCUITPY_BOARD_SPI             (1)
#define CIRCUITPY_BOARD_SPI_PIN         {{.clock = DEFAULT_SPI_BUS_SCK, .mosi = DEFAULT_SPI_BUS_MOSI, .miso = NULL}}
