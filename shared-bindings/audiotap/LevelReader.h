// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/audiotap/LevelReader.h"

extern const mp_obj_type_t audiotap_levelreader_type;

void common_hal_audiotap_levelreader_construct(audiotap_levelreader_obj_t *self, audiotap_tap_obj_t *tap);
void common_hal_audiotap_levelreader_deinit(audiotap_levelreader_obj_t *self);
bool common_hal_audiotap_levelreader_deinited(audiotap_levelreader_obj_t *self);

mp_float_t common_hal_audiotap_levelreader_get_peak(audiotap_levelreader_obj_t *self);
mp_float_t common_hal_audiotap_levelreader_get_rms(audiotap_levelreader_obj_t *self);
