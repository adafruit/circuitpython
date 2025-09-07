// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "TinyCircuits Thumby"
#define MICROPY_HW_MCU_NAME         "rp2040"

#define DEFAULT_SPI_BUS_SCK         (&pin_GPIO18)
#define DEFAULT_SPI_BUS_MOSI        (&pin_GPIO19)

#define CIRCUITPY_BOARD_OLED_DC     (&pin_GPIO17)
#define CIRCUITPY_BOARD_OLED_CS     (&pin_GPIO16)
#define CIRCUITPY_BOARD_OLED_RESET  (&pin_GPIO20)

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = DEFAULT_SPI_BUS_SCK, .mosi = DEFAULT_SPI_BUS_MOSI, .miso = NULL}}

// For entering safe mode
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO6)
