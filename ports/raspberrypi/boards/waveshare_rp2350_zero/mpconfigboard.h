// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Waveshare RP2350-Zero"
#define MICROPY_HW_MCU_NAME "rp2350"

#define DEFAULT_UART_BUS_TX (&pin_GPIO0)
#define DEFAULT_UART_BUS_RX (&pin_GPIO1)

#define MICROPY_HW_NEOPIXEL (&pin_GPIO16)
