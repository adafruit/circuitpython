// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/audiobusio/__init__.h"
#include "common-hal/microcontroller/Pin.h"

#if CIRCUITPY_AUDIOBUSIO_PDMIN

typedef struct {
    i2s_t i2s;
    i2s_chan_handle_t rx_chan;
    const mcu_pin_obj_t *clock_pin;
    const mcu_pin_obj_t *data_pin;
    uint32_t sample_rate;
    uint8_t bit_depth;
} audiobusio_pdmin_obj_t;

#endif
