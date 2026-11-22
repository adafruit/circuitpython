// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/mpconfig.h"

void reset_board_buses(void);

#if CIRCUITPY_BOARD_I2C
// Add one board-owned user, such as an IO expander, of board I2C bus `instance`.
// reset_board_buses() keeps the bus while it has a user. Does not raise or allocate.
void board_i2c_retain(uint8_t instance);
#endif
