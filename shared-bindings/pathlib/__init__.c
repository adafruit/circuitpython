// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/objtype.h"

#include "shared-bindings/pathlib/__init__.h"
#include "shared-bindings/pathlib/PosixPath.h"

//| """Filesystem path operations"""
//|

//| def Path(path: Union[str, "PosixPath"]) -> "PosixPath":
//|     """Factory function that returns a new PosixPath."""
//|     ...
//|

/* Path is just an alias for PosixPath in CircuitPython */

static const mp_rom_map_elem_t pathlib_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pathlib) },
    { MP_ROM_QSTR(MP_QSTR_Path), MP_ROM_PTR(&pathlib_posixpath_type) },
    { MP_ROM_QSTR(MP_QSTR_PosixPath), MP_ROM_PTR(&pathlib_posixpath_type) },
};

static MP_DEFINE_CONST_DICT(pathlib_module_globals, pathlib_module_globals_table);

const mp_obj_module_t pathlib_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pathlib_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_pathlib, pathlib_module);
