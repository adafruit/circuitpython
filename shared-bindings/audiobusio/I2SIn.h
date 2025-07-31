// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/audiobusio/I2SIn.h"
#include "common-hal/microcontroller/Pin.h"
#include "extmod/vfs_fat.h"

extern const mp_obj_type_t audiobusio_i2sin_type;

#if CIRCUITPY_AUDIOBUSIO_I2SIN && !CIRCUITPY_AUDIOBUSIO_I2SOUT
void common_hal_audiobusio_i2sin_construct(audiobusio_i2sin_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select, const mcu_pin_obj_t *data,
    uint32_t buffer_size, uint8_t channel_count, uint32_t sample_rate, uint8_t bits_per_sample,
    bool samples_signed);

void common_hal_audiobusio_i2sin_deinit(audiobusio_i2sin_obj_t *self);
bool common_hal_audiobusio_i2sin_deinited(audiobusio_i2sin_obj_t *self);

uint32_t common_hal_audiobusio_i2sin_get_sample_rate(audiobusio_i2sin_obj_t *self);
uint8_t common_hal_audiobusio_i2sin_get_channel_count(audiobusio_i2sin_obj_t *self);
uint8_t common_hal_audiobusio_i2sin_get_bits_per_sample(audiobusio_i2sin_obj_t *self);

#endif // CIRCUITPY_AUDIOBUSIO_I2SIN && !CIRCUITPY_AUDIOBUSIO_I2SOUT
