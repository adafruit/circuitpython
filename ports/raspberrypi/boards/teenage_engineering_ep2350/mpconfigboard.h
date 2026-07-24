// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks
//
// SPDX-License-Identifier: MIT

#define MICROPY_HW_BOARD_NAME "Teenage Engineering ting fx EP-2350"
#define MICROPY_HW_MCU_NAME "rp2350a"

// Top white LED. Also available as board.LED_WHITE1.
#define MICROPY_HW_LED_STATUS (&pin_GPIO0)

// GPIO2 is the power hold latch: high keeps the unit powered, low powers it
// off immediately. It is asserted in board_reset_pin_number() so that it
// survives every pin reset.
#define MICROPY_HW_POWER_HOLD_PIN_NUMBER (2)

// Codec (0x1b) and accelerometer (0x18) share I2C1. External pull-ups.
#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO15, .sda = &pin_GPIO14}}
