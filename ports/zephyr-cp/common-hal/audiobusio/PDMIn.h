// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"

#if CIRCUITPY_AUDIOBUSIO_PDMIN

typedef struct {
    mp_obj_base_t base;
} audiobusio_pdmin_obj_t;

#endif // CIRCUITPY_AUDIOBUSIO_PDMIN
