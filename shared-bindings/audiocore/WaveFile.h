// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "extmod/vfs_fat.h"

#include "shared-module/audiocore/WaveFile.h"

extern const mp_obj_type_t audioio_wavefile_type;

void common_hal_audioio_wavefile_construct(audioio_wavefile_obj_t *self,
    pyb_file_obj_t *file, uint8_t *buffer, size_t buffer_size);

void common_hal_audioio_wavefile_deinit(audioio_wavefile_obj_t *self);
