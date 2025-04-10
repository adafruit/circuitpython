// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-module/pathlib/__init__.h"
#include "shared-bindings/pathlib/PosixPath.h"

mp_obj_t common_hal_pathlib_path_new(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return pathlib_posixpath_from_str(MP_OBJ_NEW_QSTR(MP_QSTR_));
    }

    if (n_args == 1) {
        return common_hal_pathlib_posixpath_new(args[0]);
    }

    // Join multiple path components
    mp_obj_t path = args[0];
    for (size_t i = 1; i < n_args; i++) {
        path = common_hal_pathlib_posixpath_joinpath(
            common_hal_pathlib_posixpath_new(path),
            args[i]
            );
    }

    return path;
}
