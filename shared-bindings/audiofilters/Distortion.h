// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiofilters/Distortion.h"

extern const mp_obj_type_t audiofilters_distortion_type;
extern const mp_obj_type_t audiofilters_distortion_mode_type;

void common_hal_audiofilters_distortion_construct(audiofilters_distortion_obj_t *self,
    mp_obj_t drive, mp_obj_t pre_gain, mp_obj_t post_gain,
    audiofilters_distortion_mode mode, bool soft_clip, mp_obj_t mix,
    uint32_t buffer_size, uint8_t bits_per_sample, bool samples_signed,
    uint8_t channel_count, uint32_t sample_rate);

void common_hal_audiofilters_distortion_deinit(audiofilters_distortion_obj_t *self);

mp_obj_t common_hal_audiofilters_distortion_get_drive(audiofilters_distortion_obj_t *self);
void common_hal_audiofilters_distortion_set_drive(audiofilters_distortion_obj_t *self, mp_obj_t arg);

mp_obj_t common_hal_audiofilters_distortion_get_pre_gain(audiofilters_distortion_obj_t *self);
void common_hal_audiofilters_distortion_set_pre_gain(audiofilters_distortion_obj_t *self, mp_obj_t arg);

mp_obj_t common_hal_audiofilters_distortion_get_post_gain(audiofilters_distortion_obj_t *self);
void common_hal_audiofilters_distortion_set_post_gain(audiofilters_distortion_obj_t *self, mp_obj_t arg);

audiofilters_distortion_mode common_hal_audiofilters_distortion_get_mode(audiofilters_distortion_obj_t *self);
void common_hal_audiofilters_distortion_set_mode(audiofilters_distortion_obj_t *self, audiofilters_distortion_mode mode);

bool common_hal_audiofilters_distortion_get_soft_clip(audiofilters_distortion_obj_t *self);
void common_hal_audiofilters_distortion_set_soft_clip(audiofilters_distortion_obj_t *self, bool soft_clip);

mp_obj_t common_hal_audiofilters_distortion_get_mix(audiofilters_distortion_obj_t *self);
void common_hal_audiofilters_distortion_set_mix(audiofilters_distortion_obj_t *self, mp_obj_t arg);

bool common_hal_audiofilters_distortion_get_playing(audiofilters_distortion_obj_t *self);
void common_hal_audiofilters_distortion_play(audiofilters_distortion_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiofilters_distortion_stop(audiofilters_distortion_obj_t *self);
