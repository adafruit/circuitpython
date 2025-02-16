// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objproperty.h"

#define AUDIOSAMPLE_FIELDS \
    { MP_ROM_QSTR(MP_QSTR_sample_rate), MP_ROM_PTR(&audiosample_sample_rate_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_bits_per_sample), MP_ROM_PTR(&audiosample_bits_per_sample_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_channel_count), MP_ROM_PTR(&audiosample_channel_count_obj) }

typedef struct audiosample_base audiosample_base_t;
extern const mp_obj_property_getset_t audiosample_sample_rate_obj;
extern const mp_obj_property_getter_t audiosample_bits_per_sample_obj;
extern const mp_obj_property_getter_t audiosample_channel_count_obj;
void audiosample_check_for_deinit(const audiosample_base_t *self);
bool audiosample_deinited(const audiosample_base_t *self);
void audiosample_mark_deinit(audiosample_base_t *self);
