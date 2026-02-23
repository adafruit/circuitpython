// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 ajs256
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Hack Club Sprig"
#define MICROPY_HW_MCU_NAME "rp2040"

#define MICROPY_HW_LED_STATUS (&pin_GPIO4)

#define CIRCUITPY_BOARD_TFT_DC  (&pin_GPIO22)
#define CIRCUITPY_BOARD_TFT_CS  (&pin_GPIO20)
#define CIRCUITPY_BOARD_TFT_RESET (&pin_GPIO26)
#define CIRCUITPY_BOARD_TFT_BACKLIGHT (&pin_GPIO17)

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO18, .mosi = &pin_GPIO19, .miso = &pin_GPIO16 }}
