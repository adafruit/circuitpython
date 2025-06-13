// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/audiobusio/I2S.h"
#include "common-hal/microcontroller/Pin.h"
#include "extmod/vfs_fat.h"

extern const mp_obj_type_t audiobusio_i2s_type;

#if CIRCUITPY_AUDIOBUSIO_I2SOUT && CIRCUITPY_AUDIOBUSIO_I2SIN

void common_hal_audiobusio_i2s_construct(audiobusio_i2s_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select, const mcu_pin_obj_t *data_out, const mcu_pin_obj_t *data_in,
    const mcu_pin_obj_t *main_clock, bool left_justified,
    uint32_t buffer_size, uint8_t channel_count, uint32_t sample_rate, uint8_t bits_per_sample,
    bool samples_signed);

void common_hal_audiobusio_i2s_deinit(audiobusio_i2s_obj_t *self);
bool common_hal_audiobusio_i2s_deinited(audiobusio_i2s_obj_t *self);

uint32_t common_hal_audiobusio_i2s_record_to_buffer(audiobusio_i2s_obj_t *self,
    int16_t *buffer, uint32_t length);

void common_hal_audiobusio_i2s_play(audiobusio_i2s_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiobusio_i2s_stop(audiobusio_i2s_obj_t *self);
bool common_hal_audiobusio_i2s_get_playing(audiobusio_i2s_obj_t *self);
void common_hal_audiobusio_i2s_pause(audiobusio_i2s_obj_t *self);
void common_hal_audiobusio_i2s_resume(audiobusio_i2s_obj_t *self);
bool common_hal_audiobusio_i2s_get_paused(audiobusio_i2s_obj_t *self);

uint32_t common_hal_audiobusio_i2s_get_sample_rate(audiobusio_i2s_obj_t *self);
uint8_t common_hal_audiobusio_i2s_get_channel_count(audiobusio_i2s_obj_t *self);
uint8_t common_hal_audiobusio_i2s_get_bits_per_sample(audiobusio_i2s_obj_t *self);

#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT && CIRCUITPY_AUDIOBUSIO_I2SIN
