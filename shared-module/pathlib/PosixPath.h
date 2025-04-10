// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objstr.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t path_str;
} pathlib_posixpath_obj_t;

// Forward declaration
extern const mp_obj_type_t pathlib_posixpath_type;

// Helper functions
mp_obj_t pathlib_posixpath_from_str(mp_obj_t path_str);
mp_obj_t mp_obj_new_path_dir_iter(mp_obj_t path, mp_obj_t iter);
