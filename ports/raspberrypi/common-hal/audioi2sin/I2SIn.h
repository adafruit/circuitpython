// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "bindings/rp2pio/StateMachine.h"

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint32_t sample_rate;
    uint8_t bit_depth;
    uint8_t output_bit_depth;
    bool mono;
    bool samples_signed;
    bool left_justified;
    bool settled;
    rp2pio_statemachine_obj_t state_machine;
    // Background DMA ring buffer. The state machine alternates DMA writes
    // between the two halves so BCLK never stalls between record() calls.
    uint8_t *ring;
    size_t ring_size;
    size_t half_size;
    size_t read_pos;
    int dma_channel;
    bool overflow;
} audioi2sin_i2sin_obj_t;
