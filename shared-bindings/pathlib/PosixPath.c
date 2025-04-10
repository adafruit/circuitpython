// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/gc.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/objtype.h"

#include "shared-bindings/pathlib/PosixPath.h"
#include "shared-module/pathlib/PosixPath.h"

//| class PosixPath:
//|     """Object representing a path on Posix systems"""
//|
//|     def __init__(self, path: Union[str, "PosixPath"]) -> None:
//|         """Construct a PosixPath object from a string or another PosixPath object.
//|
//|         :param path: A string or PosixPath object representing a path
//|         """
//|         ...
//|

static mp_obj_t pathlib_posixpath_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    return common_hal_pathlib_posixpath_new(args[0]);
}

//|     def joinpath(self, *args: Union[str, "PosixPath"]) -> "PosixPath":
//|         """Join path components.
//|
//|         :param args: Path components to join
//|         :return: A new PosixPath object
//|         """
//|         ...
//|
//|     def __truediv__(self, other: Union[str, "PosixPath"]) -> "PosixPath":
//|         """Implement path / other for joining paths.
//|
//|         :param other: Path component to join
//|         :return: A new PosixPath object
//|         """
//|         ...
//|

static mp_obj_t pathlib_posixpath_joinpath(mp_obj_t self_in, mp_obj_t arg) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_pathlib_posixpath_joinpath(self, arg);
}
MP_DEFINE_CONST_FUN_OBJ_2(pathlib_posixpath_joinpath_obj, pathlib_posixpath_joinpath);

// Binary operator for implementing the / operator
static mp_obj_t pathlib_posixpath_binary_op(mp_binary_op_t op, mp_obj_t lhs, mp_obj_t rhs) {
    switch (op) {
        case MP_BINARY_OP_TRUE_DIVIDE: {
            // Implement path / other
            pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(lhs);
            return common_hal_pathlib_posixpath_joinpath(self, rhs);
        }
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

//|     @property
//|     def parent(self) -> "PosixPath":
//|         """The logical parent of the path."""
//|         ...
//|

static mp_obj_t pathlib_posixpath_parent(mp_obj_t self_in) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_pathlib_posixpath_parent(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(pathlib_posixpath_parent_obj, pathlib_posixpath_parent);

MP_PROPERTY_GETTER(pathlib_posixpath_parent_property_obj,
    (mp_obj_t)&pathlib_posixpath_parent_obj);

//|     @property
//|     def name(self) -> str:
//|         """The final path component, excluding the drive and root, if any."""
//|         ...
//|

static mp_obj_t pathlib_posixpath_name(mp_obj_t self_in) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_pathlib_posixpath_name(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(pathlib_posixpath_name_obj, pathlib_posixpath_name);

MP_PROPERTY_GETTER(pathlib_posixpath_name_property_obj,
    (mp_obj_t)&pathlib_posixpath_name_obj);

//|     @property
//|     def stem(self) -> str:
//|         """The final path component, without its suffix."""
//|         ...
//|

static mp_obj_t pathlib_posixpath_stem(mp_obj_t self_in) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_pathlib_posixpath_stem(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(pathlib_posixpath_stem_obj, pathlib_posixpath_stem);

MP_PROPERTY_GETTER(pathlib_posixpath_stem_property_obj,
    (mp_obj_t)&pathlib_posixpath_stem_obj);

//|     @property
//|     def suffix(self) -> str:
//|         """The final component's extension."""
//|         ...
//|

static mp_obj_t pathlib_posixpath_suffix(mp_obj_t self_in) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_pathlib_posixpath_suffix(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(pathlib_posixpath_suffix_obj, pathlib_posixpath_suffix);

MP_PROPERTY_GETTER(pathlib_posixpath_suffix_property_obj,
    (mp_obj_t)&pathlib_posixpath_suffix_obj);

//|     def exists(self) -> bool:
//|         """Check whether the path exists."""
//|         ...
//|

static mp_obj_t pathlib_posixpath_exists(mp_obj_t self_in) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_pathlib_posixpath_exists(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(pathlib_posixpath_exists_obj, pathlib_posixpath_exists);

//|     def is_dir(self) -> bool:
//|         """Check whether the path is a directory."""
//|         ...
//|

static mp_obj_t pathlib_posixpath_is_dir(mp_obj_t self_in) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_pathlib_posixpath_is_dir(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(pathlib_posixpath_is_dir_obj, pathlib_posixpath_is_dir);

//|     def is_file(self) -> bool:
//|         """Check whether the path is a regular file."""
//|         ...
//|

static mp_obj_t pathlib_posixpath_is_file(mp_obj_t self_in) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_pathlib_posixpath_is_file(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(pathlib_posixpath_is_file_obj, pathlib_posixpath_is_file);

//|     def absolute(self) -> "PosixPath":
//|         """Return an absolute version of this path."""
//|         ...
//|

static mp_obj_t pathlib_posixpath_absolute(mp_obj_t self_in) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_pathlib_posixpath_absolute(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(pathlib_posixpath_absolute_obj, pathlib_posixpath_absolute);

//|     def resolve(self) -> "PosixPath":
//|         """Make the path absolute, resolving any symlinks."""
//|         ...
//|

static mp_obj_t pathlib_posixpath_resolve(mp_obj_t self_in) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_pathlib_posixpath_resolve(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(pathlib_posixpath_resolve_obj, pathlib_posixpath_resolve);

//|     def iterdir(self) -> Iterator["PosixPath"]:
//|         """Iterate over the files in this directory.
//|         Does not include the special paths '.' and '..'.
//|
//|         :return: An iterator yielding path objects
//|
//|         Example::
//|
//|             for path in Path('.').iterdir():
//|                 print(path)
//|         """
//|         ...
//|

static mp_obj_t pathlib_posixpath_iterdir(mp_obj_t self_in) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_pathlib_posixpath_iterdir(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(pathlib_posixpath_iterdir_obj, pathlib_posixpath_iterdir);

static const mp_rom_map_elem_t pathlib_posixpath_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_joinpath), MP_ROM_PTR(&pathlib_posixpath_joinpath_obj) },
    { MP_ROM_QSTR(MP_QSTR_exists), MP_ROM_PTR(&pathlib_posixpath_exists_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_dir), MP_ROM_PTR(&pathlib_posixpath_is_dir_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_file), MP_ROM_PTR(&pathlib_posixpath_is_file_obj) },
    { MP_ROM_QSTR(MP_QSTR_absolute), MP_ROM_PTR(&pathlib_posixpath_absolute_obj) },
    { MP_ROM_QSTR(MP_QSTR_resolve), MP_ROM_PTR(&pathlib_posixpath_resolve_obj) },
    { MP_ROM_QSTR(MP_QSTR_iterdir), MP_ROM_PTR(&pathlib_posixpath_iterdir_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_parent), MP_ROM_PTR(&pathlib_posixpath_parent_property_obj) },
    { MP_ROM_QSTR(MP_QSTR_name), MP_ROM_PTR(&pathlib_posixpath_name_property_obj) },
    { MP_ROM_QSTR(MP_QSTR_stem), MP_ROM_PTR(&pathlib_posixpath_stem_property_obj) },
    { MP_ROM_QSTR(MP_QSTR_suffix), MP_ROM_PTR(&pathlib_posixpath_suffix_property_obj) },
};

static MP_DEFINE_CONST_DICT(pathlib_posixpath_locals_dict, pathlib_posixpath_locals_dict_table);

// String representation for PosixPath objects
static void pathlib_posixpath_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pathlib_posixpath_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Just print the path string directly
    mp_obj_print_helper(print, self->path_str, kind);
}

MP_DEFINE_CONST_OBJ_TYPE(
    pathlib_posixpath_type,
    MP_QSTR_PosixPath,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, pathlib_posixpath_make_new,
    print, pathlib_posixpath_print,
    binary_op, pathlib_posixpath_binary_op,
    locals_dict, &pathlib_posixpath_locals_dict
    );
