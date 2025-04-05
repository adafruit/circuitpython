// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Mark Komus
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiodelays/Reverb.h"

extern const mp_obj_type_t audiodelays_reverb_type;

void common_hal_audiodelays_reverb_construct(audiodelays_reverb_obj_t *self,
    mp_obj_t roomsize, mp_obj_t damp, mp_obj_t mix,
    uint32_t buffer_size, uint8_t bits_per_sample, bool samples_signed,
    uint8_t channel_count, uint32_t sample_rate);

void common_hal_audiodelays_reverb_deinit(audiodelays_reverb_obj_t *self);
bool common_hal_audiodelays_reverb_deinited(audiodelays_reverb_obj_t *self);

uint32_t common_hal_audiodelays_reverb_get_sample_rate(audiodelays_reverb_obj_t *self);
uint8_t common_hal_audiodelays_reverb_get_channel_count(audiodelays_reverb_obj_t *self);
uint8_t common_hal_audiodelays_reverb_get_bits_per_sample(audiodelays_reverb_obj_t *self);

mp_obj_t common_hal_audiodelays_reverb_get_roomsize(audiodelays_reverb_obj_t *self);
void common_hal_audiodelays_reverb_set_roomsize(audiodelays_reverb_obj_t *self, mp_obj_t feedback);

mp_obj_t common_hal_audiodelays_reverb_get_damp(audiodelays_reverb_obj_t *self);
void common_hal_audiodelays_reverb_set_damp(audiodelays_reverb_obj_t *self, mp_obj_t damp);

mp_obj_t common_hal_audiodelays_reverb_get_mix(audiodelays_reverb_obj_t *self);
void common_hal_audiodelays_reverb_set_mix(audiodelays_reverb_obj_t *self, mp_obj_t mix);

bool common_hal_audiodelays_reverb_get_playing(audiodelays_reverb_obj_t *self);
void common_hal_audiodelays_reverb_play(audiodelays_reverb_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiodelays_reverb_stop(audiodelays_reverb_obj_t *self);
