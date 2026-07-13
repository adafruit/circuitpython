// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-module/audiofilewriter/AudioFileWriter.h"

extern const mp_obj_type_t audiofilewriter_audiofilewriter_type;

void common_hal_audiofilewriter_audiofilewriter_construct(audiofilewriter_audiofilewriter_obj_t *self,
    mp_obj_t file, uint32_t buffer_size);

void common_hal_audiofilewriter_audiofilewriter_deinit(audiofilewriter_audiofilewriter_obj_t *self);
bool common_hal_audiofilewriter_audiofilewriter_deinited(audiofilewriter_audiofilewriter_obj_t *self);

void common_hal_audiofilewriter_audiofilewriter_play(audiofilewriter_audiofilewriter_obj_t *self, mp_obj_t sample);
void common_hal_audiofilewriter_audiofilewriter_stop(audiofilewriter_audiofilewriter_obj_t *self);
bool common_hal_audiofilewriter_audiofilewriter_get_playing(audiofilewriter_audiofilewriter_obj_t *self);
