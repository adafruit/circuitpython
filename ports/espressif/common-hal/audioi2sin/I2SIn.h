// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"

#include "driver/i2s_std.h"

#if CIRCUITPY_AUDIOI2SIN

typedef struct {
    mp_obj_base_t base;
    i2s_chan_handle_t rx_chan;
    const mcu_pin_obj_t *bit_clock;
    const mcu_pin_obj_t *word_select;
    const mcu_pin_obj_t *data;
    const mcu_pin_obj_t *mclk;
    uint32_t sample_rate;
    uint8_t bit_depth;
    uint8_t output_bit_depth;
    bool mono;
    bool samples_signed;
} audioi2sin_i2sin_obj_t;

#endif
