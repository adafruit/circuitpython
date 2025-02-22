// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiofilters/Filter.h"

extern const mp_obj_type_t audiofilters_filter_type;

void common_hal_audiofilters_filter_construct(audiofilters_filter_obj_t *self,
    mp_obj_t filter, mp_obj_t mix,
    uint32_t buffer_size, uint8_t bits_per_sample, bool samples_signed,
    uint8_t channel_count, uint32_t sample_rate);

void common_hal_audiofilters_filter_deinit(audiofilters_filter_obj_t *self);

mp_obj_t common_hal_audiofilters_filter_get_filter(audiofilters_filter_obj_t *self);
void common_hal_audiofilters_filter_set_filter(audiofilters_filter_obj_t *self, mp_obj_t arg);

mp_obj_t common_hal_audiofilters_filter_get_mix(audiofilters_filter_obj_t *self);
void common_hal_audiofilters_filter_set_mix(audiofilters_filter_obj_t *self, mp_obj_t arg);

bool common_hal_audiofilters_filter_get_playing(audiofilters_filter_obj_t *self);
void common_hal_audiofilters_filter_play(audiofilters_filter_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiofilters_filter_stop(audiofilters_filter_obj_t *self);
