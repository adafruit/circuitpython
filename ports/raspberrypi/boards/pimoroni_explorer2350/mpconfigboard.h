// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Pimoroni Explorer"
#define MICROPY_HW_MCU_NAME "rp2350b"

#define MICROPY_HW_LED_STATUS (&pin_GPIO25)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO21, .sda = &pin_GPIO20}}

#define DEFAULT_UART_BUS_RX (&pin_GPIO1)
#define DEFAULT_UART_BUS_TX (&pin_GPIO0)
