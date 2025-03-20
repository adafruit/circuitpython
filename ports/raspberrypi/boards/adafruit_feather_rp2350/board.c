// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "supervisor/board.h"

#include "common-hal/picodvi/__init__.h"

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.

void board_init(void) {
    picodvi_autoconstruct();
}
