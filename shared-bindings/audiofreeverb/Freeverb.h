// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Mark Komus
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiofreeverb/Freeverb.h"

extern const mp_obj_type_t audiofreeverb_freeverb_type;

void common_hal_audiofreeverb_freeverb_construct(audiofreeverb_freeverb_obj_t *self,
    mp_obj_t roomsize, mp_obj_t damp, mp_obj_t mix,
    uint32_t buffer_size, uint8_t bits_per_sample, bool samples_signed,
    uint8_t channel_count, uint32_t sample_rate);

void common_hal_audiofreeverb_freeverb_deinit(audiofreeverb_freeverb_obj_t *self);
bool common_hal_audiofreeverb_freeverb_deinited(audiofreeverb_freeverb_obj_t *self);

mp_obj_t common_hal_audiofreeverb_freeverb_get_roomsize(audiofreeverb_freeverb_obj_t *self);
void common_hal_audiofreeverb_freeverb_set_roomsize(audiofreeverb_freeverb_obj_t *self, mp_obj_t feedback);

mp_obj_t common_hal_audiofreeverb_freeverb_get_damp(audiofreeverb_freeverb_obj_t *self);
void common_hal_audiofreeverb_freeverb_set_damp(audiofreeverb_freeverb_obj_t *self, mp_obj_t damp);

mp_obj_t common_hal_audiofreeverb_freeverb_get_mix(audiofreeverb_freeverb_obj_t *self);
void common_hal_audiofreeverb_freeverb_set_mix(audiofreeverb_freeverb_obj_t *self, mp_obj_t mix);

bool common_hal_audiofreeverb_freeverb_get_playing(audiofreeverb_freeverb_obj_t *self);
void common_hal_audiofreeverb_freeverb_play(audiofreeverb_freeverb_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiofreeverb_freeverb_stop(audiofreeverb_freeverb_obj_t *self);
