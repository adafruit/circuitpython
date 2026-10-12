// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/picodvi/AudioOut.h"

extern const mp_obj_type_t picodvi_audioout_type;

void common_hal_picodvi_audioout_construct(picodvi_audioout_obj_t *self, picodvi_framebuffer_obj_t *framebuffer);
void common_hal_picodvi_audioout_deinit(picodvi_audioout_obj_t *self);
bool common_hal_picodvi_audioout_deinited(picodvi_audioout_obj_t *self);
void common_hal_picodvi_audioout_play(picodvi_audioout_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_picodvi_audioout_stop(picodvi_audioout_obj_t *self);
bool common_hal_picodvi_audioout_get_playing(picodvi_audioout_obj_t *self);
void common_hal_picodvi_audioout_pause(picodvi_audioout_obj_t *self);
void common_hal_picodvi_audioout_resume(picodvi_audioout_obj_t *self);
bool common_hal_picodvi_audioout_get_paused(picodvi_audioout_obj_t *self);
