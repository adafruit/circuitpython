// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiodelays/PitchShift.h"

extern const mp_obj_type_t audiodelays_pitch_shift_type;

void common_hal_audiodelays_pitch_shift_construct(audiodelays_pitch_shift_obj_t *self,
    mp_obj_t semitones, mp_obj_t mix, uint32_t window, uint32_t overlap,
    uint32_t buffer_size, uint8_t bits_per_sample, bool samples_signed,
    uint8_t channel_count, uint32_t sample_rate);

void common_hal_audiodelays_pitch_shift_deinit(audiodelays_pitch_shift_obj_t *self);

mp_obj_t common_hal_audiodelays_pitch_shift_get_semitones(audiodelays_pitch_shift_obj_t *self);
void common_hal_audiodelays_pitch_shift_set_semitones(audiodelays_pitch_shift_obj_t *self, mp_obj_t semitones);

mp_obj_t common_hal_audiodelays_pitch_shift_get_mix(audiodelays_pitch_shift_obj_t *self);
void common_hal_audiodelays_pitch_shift_set_mix(audiodelays_pitch_shift_obj_t *self, mp_obj_t arg);

bool common_hal_audiodelays_pitch_shift_get_playing(audiodelays_pitch_shift_obj_t *self);
void common_hal_audiodelays_pitch_shift_play(audiodelays_pitch_shift_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiodelays_pitch_shift_stop(audiodelays_pitch_shift_obj_t *self);
