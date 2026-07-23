// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/microcontroller/Pin.h"

#if CIRCUITPY_AUDIOI2SIN
#include "common-hal/audioi2sin/I2SIn.h"
#include "shared-module/audiocore/__init__.h"
#endif

extern const mp_obj_type_t audioi2sin_i2sin_type;

#if CIRCUITPY_AUDIOI2SIN
void common_hal_audioi2sin_i2sin_construct(audioi2sin_i2sin_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, const mcu_pin_obj_t *main_clock,
    uint32_t sample_rate, uint8_t bit_depth, uint8_t output_bit_depth,
    bool mono, bool left_justified, bool samples_signed,
    bool clock_follower, bool invert_bit_clock);
void common_hal_audioi2sin_i2sin_deinit(audioi2sin_i2sin_obj_t *self);
bool common_hal_audioi2sin_i2sin_deinited(audioi2sin_i2sin_obj_t *self);
uint32_t common_hal_audioi2sin_i2sin_record_to_buffer(audioi2sin_i2sin_obj_t *self,
    void *buffer, uint32_t length);
uint8_t common_hal_audioi2sin_i2sin_get_bit_depth(audioi2sin_i2sin_obj_t *self);
uint32_t common_hal_audioi2sin_i2sin_get_sample_rate(audioi2sin_i2sin_obj_t *self);
bool common_hal_audioi2sin_i2sin_get_samples_signed(audioi2sin_i2sin_obj_t *self);
// Reads and clears the sticky "samples were dropped" flag.
bool common_hal_audioi2sin_i2sin_get_overflow(audioi2sin_i2sin_obj_t *self);

// audiosample protocol: streaming source support. fill_buffer converts the
// frames currently available from the live mic into `buffer` (output depth,
// interleaved), padding with silence on underrun; it never blocks.
void common_hal_audioi2sin_i2sin_fill_buffer(audioi2sin_i2sin_obj_t *self,
    uint8_t *buffer, uint32_t frames);
void common_hal_audioi2sin_i2sin_reset_buffer(audioi2sin_i2sin_obj_t *self,
    bool single_channel_output, uint8_t channel);
audioio_get_buffer_result_t common_hal_audioi2sin_i2sin_get_buffer(
    audioi2sin_i2sin_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length);
#endif
