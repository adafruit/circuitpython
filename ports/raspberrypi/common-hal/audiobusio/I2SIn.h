// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/rp2pio/StateMachine.h"

#include "audio_dma.h"
#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"

#if CIRCUITPY_AUDIOBUSIO_I2SIN && !CIRCUITPY_AUDIOBUSIO_I2SOUT

// We don't bit pack because we'll only have two at most. Its better to save code size instead.
typedef struct {
    mp_obj_base_t base;
    rp2pio_statemachine_obj_t state_machine;
    audio_dma_t dma;
    uint32_t buffer_size;
    uint8_t channel_count;
    uint32_t sample_rate;
    uint8_t bits_per_sample;
    bool samples_signed;
    uint8_t last_index;
} audiobusio_i2sin_obj_t;


// These are not available from Python because it may be called in an interrupt.
void audiobusio_i2sin_reset_buffer(audiobusio_i2sin_obj_t *self,
    bool single_channel_output,
    uint8_t channel);
audioio_get_buffer_result_t audiobusio_i2sin_get_buffer(audiobusio_i2sin_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length);                                                      // length in bytes
void audiobusio_i2sin_get_buffer_structure(audiobusio_i2sin_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing);

#endif // CIRCUITPY_AUDIOBUSIO_I2SIN && !CIRCUITPY_AUDIOBUSIO_I2SOUT
