// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "ESP32-P4 Stamp XL"
#define MICROPY_HW_MCU_NAME         "ESP32P4"

#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO0)

#define DEFAULT_UART_BUS_RX         (&pin_GPIO38)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO37)

#define CIRCUITPY_CONSOLE_UART_TX DEFAULT_UART_BUS_TX
#define CIRCUITPY_CONSOLE_UART_RX DEFAULT_UART_BUS_RX
