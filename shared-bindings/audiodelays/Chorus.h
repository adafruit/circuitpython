// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Mark Komus
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiodelays/Chorus.h"

extern const mp_obj_type_t audiodelays_chorus_type;

void common_hal_audiodelays_chorus_construct(audiodelays_chorus_obj_t *self, uint32_t max_delay_ms,
    mp_obj_t delay_ms, mp_obj_t voices, mp_obj_t mix,
    uint32_t buffer_size, uint8_t bits_per_sample,
    bool samples_signed, uint8_t channel_count, uint32_t sample_rate);

void common_hal_audiodelays_chorus_deinit(audiodelays_chorus_obj_t *self);
bool common_hal_audiodelays_chorus_deinited(audiodelays_chorus_obj_t *self);

mp_obj_t common_hal_audiodelays_chorus_get_delay_ms(audiodelays_chorus_obj_t *self);
void common_hal_audiodelays_chorus_set_delay_ms(audiodelays_chorus_obj_t *self, mp_obj_t delay_ms);

mp_obj_t common_hal_audiodelays_chorus_get_voices(audiodelays_chorus_obj_t *self);
void common_hal_audiodelays_chorus_set_voices(audiodelays_chorus_obj_t *self, mp_obj_t voices);

mp_obj_t common_hal_audiodelays_chorus_get_mix(audiodelays_chorus_obj_t *self);
void common_hal_audiodelays_chorus_set_mix(audiodelays_chorus_obj_t *self, mp_obj_t arg);

bool common_hal_audiodelays_chorus_get_playing(audiodelays_chorus_obj_t *self);
void common_hal_audiodelays_chorus_play(audiodelays_chorus_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiodelays_chorus_stop(audiodelays_chorus_obj_t *self);
