// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Mark Komus
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiodelays/Echo.h"

extern const mp_obj_type_t audiodelays_echo_type;

void common_hal_audiodelays_echo_construct(audiodelays_echo_obj_t *self, uint32_t max_delay_ms,
    mp_obj_t delay_ms, mp_obj_t decay, mp_obj_t mix,
    uint32_t buffer_size, uint8_t bits_per_sample, bool samples_signed,
    uint8_t channel_count, uint32_t sample_rate, bool freq_shift);

void common_hal_audiodelays_echo_deinit(audiodelays_echo_obj_t *self);

mp_obj_t common_hal_audiodelays_echo_get_delay_ms(audiodelays_echo_obj_t *self);
void common_hal_audiodelays_echo_set_delay_ms(audiodelays_echo_obj_t *self, mp_obj_t delay_ms);

bool common_hal_audiodelays_echo_get_freq_shift(audiodelays_echo_obj_t *self);
void common_hal_audiodelays_echo_set_freq_shift(audiodelays_echo_obj_t *self, bool freq_shift);

mp_obj_t common_hal_audiodelays_echo_get_decay(audiodelays_echo_obj_t *self);
void common_hal_audiodelays_echo_set_decay(audiodelays_echo_obj_t *self, mp_obj_t decay);

mp_obj_t common_hal_audiodelays_echo_get_mix(audiodelays_echo_obj_t *self);
void common_hal_audiodelays_echo_set_mix(audiodelays_echo_obj_t *self, mp_obj_t arg);

bool common_hal_audiodelays_echo_get_playing(audiodelays_echo_obj_t *self);
void common_hal_audiodelays_echo_play(audiodelays_echo_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiodelays_echo_stop(audiodelays_echo_obj_t *self);
