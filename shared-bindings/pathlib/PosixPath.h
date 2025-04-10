// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

extern const mp_obj_type_t pathlib_posixpath_type;

// Include the struct definition from shared-module
#include "shared-module/pathlib/PosixPath.h"

mp_obj_t common_hal_pathlib_posixpath_new(mp_obj_t path);
mp_obj_t common_hal_pathlib_posixpath_joinpath(pathlib_posixpath_obj_t *self, mp_obj_t arg);
mp_obj_t common_hal_pathlib_posixpath_parent(pathlib_posixpath_obj_t *self);
mp_obj_t common_hal_pathlib_posixpath_name(pathlib_posixpath_obj_t *self);
mp_obj_t common_hal_pathlib_posixpath_stem(pathlib_posixpath_obj_t *self);
mp_obj_t common_hal_pathlib_posixpath_suffix(pathlib_posixpath_obj_t *self);
mp_obj_t common_hal_pathlib_posixpath_exists(pathlib_posixpath_obj_t *self);
mp_obj_t common_hal_pathlib_posixpath_is_dir(pathlib_posixpath_obj_t *self);
mp_obj_t common_hal_pathlib_posixpath_is_file(pathlib_posixpath_obj_t *self);
mp_obj_t common_hal_pathlib_posixpath_absolute(pathlib_posixpath_obj_t *self);
mp_obj_t common_hal_pathlib_posixpath_resolve(pathlib_posixpath_obj_t *self);
mp_obj_t common_hal_pathlib_posixpath_iterdir(pathlib_posixpath_obj_t *self);
