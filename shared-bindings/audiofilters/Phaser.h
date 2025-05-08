// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiofilters/Phaser.h"

extern const mp_obj_type_t audiofilters_phaser_type;

void common_hal_audiofilters_phaser_construct(audiofilters_phaser_obj_t *self,
    mp_obj_t frequency, mp_obj_t feedback, mp_obj_t mix, uint8_t stages,
    uint32_t buffer_size, uint8_t bits_per_sample, bool samples_signed,
    uint8_t channel_count, uint32_t sample_rate);

void common_hal_audiofilters_phaser_deinit(audiofilters_phaser_obj_t *self);

mp_obj_t common_hal_audiofilters_phaser_get_frequency(audiofilters_phaser_obj_t *self);
void common_hal_audiofilters_phaser_set_frequency(audiofilters_phaser_obj_t *self, mp_obj_t arg);

mp_obj_t common_hal_audiofilters_phaser_get_feedback(audiofilters_phaser_obj_t *self);
void common_hal_audiofilters_phaser_set_feedback(audiofilters_phaser_obj_t *self, mp_obj_t arg);

mp_obj_t common_hal_audiofilters_phaser_get_mix(audiofilters_phaser_obj_t *self);
void common_hal_audiofilters_phaser_set_mix(audiofilters_phaser_obj_t *self, mp_obj_t arg);

uint8_t common_hal_audiofilters_phaser_get_stages(audiofilters_phaser_obj_t *self);
void common_hal_audiofilters_phaser_set_stages(audiofilters_phaser_obj_t *self, uint8_t arg);

bool common_hal_audiofilters_phaser_get_playing(audiofilters_phaser_obj_t *self);
void common_hal_audiofilters_phaser_play(audiofilters_phaser_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_audiofilters_phaser_stop(audiofilters_phaser_obj_t *self);
