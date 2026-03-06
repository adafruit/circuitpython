// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
// SPDX-FileCopyrightText: Copyright (c) 2021 skieast/Bruce Segal
//
// SPDX-License-Identifier: MIT

#pragma once

// Board setup
#define MICROPY_HW_BOARD_NAME       "Xteink X4"
#define MICROPY_HW_MCU_NAME         "ESP32-C3"

#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO8, .mosi = &pin_GPIO10, .miso = &pin_GPIO7}}

// For entering safe mode
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO3)
