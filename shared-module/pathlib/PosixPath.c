// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/objstr.h"
#include "py/gc.h"

#include "shared-bindings/os/__init__.h"
#include "shared-bindings/pathlib/PosixPath.h"
#include "shared-bindings/util.h"

// Path directory iterator type
typedef struct _mp_obj_path_dir_iter_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext; // Iterator function pointer
    mp_obj_t path;       // Parent path
    mp_obj_t iter;       // Iterator for the filenames
} mp_obj_path_dir_iter_t;

mp_obj_t pathlib_posixpath_from_str(mp_obj_t path_str) {
    pathlib_posixpath_obj_t *self = m_new_obj(pathlib_posixpath_obj_t);
    self->base.type = &pathlib_posixpath_type;
    self->path_str = path_str;
    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t common_hal_pathlib_posixpath_new(mp_obj_t path) {
    if (mp_obj_is_str(path)) {
        return pathlib_posixpath_from_str(path);
    } else if (mp_obj_is_type(path, &pathlib_posixpath_type)) {
        // Return a copy to ensure immutability
        pathlib_posixpath_obj_t *p = MP_OBJ_TO_PTR(path);
        return pathlib_posixpath_from_str(p->path_str);
    }

    const mp_obj_type_t *path_type = mp_obj_get_type(path);
    mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), MP_QSTR_path, MP_QSTR_str, MP_QSTR_PosixPath, path_type->name);
    return mp_const_none;
}

mp_obj_t common_hal_pathlib_posixpath_joinpath(pathlib_posixpath_obj_t *self, mp_obj_t arg) {

    mp_obj_t path_part;
    if (mp_obj_is_str(arg)) {
        path_part = arg;
    } else if (mp_obj_is_type(arg, &pathlib_posixpath_type)) {
        pathlib_posixpath_obj_t *p = MP_OBJ_TO_PTR(arg);
        path_part = p->path_str;
    } else {
        const mp_obj_type_t *arg_type = mp_obj_get_type(arg);
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), MP_QSTR_path, MP_QSTR_str, MP_QSTR_PosixPath, arg_type->name);
        return mp_const_none;
    }

    // Get string values
    GET_STR_DATA_LEN(self->path_str, path_str, path_len);
    GET_STR_DATA_LEN(path_part, part_str, part_len);

    // If either part is empty, return the other
    if (path_len == 0) {
        return pathlib_posixpath_from_str(path_part);
    }
    if (part_len == 0) {
        return pathlib_posixpath_from_str(self->path_str);
    }

    // Calculate the required buffer size (path + '/' + part + '\0')
    size_t need_slash = (path_str[path_len - 1] != '/') ? 1 : 0;
    size_t new_len = path_len + need_slash + part_len;

    // Allocate new string
    byte *new_str = m_new(byte, new_len + 1);

    // Copy first part
    memcpy(new_str, path_str, path_len);

    // Add slash if needed
    if (need_slash) {
        new_str[path_len] = '/';
    }

    // Copy second part
    memcpy(new_str + path_len + need_slash, part_str, part_len);
    new_str[new_len] = '\0';

    // Create a new string object
    mp_obj_t result_str = mp_obj_new_str((const char *)new_str, new_len);

    // Free the temporary buffer
    m_del(byte, new_str, new_len + 1);

    return pathlib_posixpath_from_str(result_str);
}

mp_obj_t common_hal_pathlib_posixpath_parent(pathlib_posixpath_obj_t *self) {
    GET_STR_DATA_LEN(self->path_str, path_str, path_len);

    // Handle root directory and empty path
    if (path_len == 0 || (path_len == 1 && path_str[0] == '/')) {
        return MP_OBJ_FROM_PTR(self); // Root is its own parent
    }

    // Find the last slash
    int last_slash = -1;
    for (int i = path_len - 1; i >= 0; i--) {
        if (path_str[i] == '/') {
            last_slash = i;
            break;
        }
    }

    // No slash found, return "."
    if (last_slash == -1) {
        return pathlib_posixpath_from_str(MP_OBJ_NEW_QSTR(MP_QSTR__dot_));
    }

    // Root directory
    if (last_slash == 0) {
        return pathlib_posixpath_from_str(MP_OBJ_NEW_QSTR(MP_QSTR__slash_));
    }

    // Create parent path
    mp_obj_t parent = mp_obj_new_str((const char *)path_str, last_slash);
    return pathlib_posixpath_from_str(parent);
}

mp_obj_t common_hal_pathlib_posixpath_name(pathlib_posixpath_obj_t *self) {
    GET_STR_DATA_LEN(self->path_str, path_str, path_len);

    // Empty path
    if (path_len == 0) {
        return MP_OBJ_NEW_QSTR(MP_QSTR_);
    }

    // Find the last slash
    int last_slash = -1;
    for (int i = path_len - 1; i >= 0; i--) {
        if (path_str[i] == '/') {
            last_slash = i;
            break;
        }
    }

    // No slash found, return the whole path
    if (last_slash == -1) {
        return self->path_str;
    }

    // Slash at the end
    if (last_slash == (int)(path_len - 1)) {
        return MP_OBJ_NEW_QSTR(MP_QSTR_);
    }

    // Return the component after the last slash
    return mp_obj_new_str((const char *)(path_str + last_slash + 1), path_len - last_slash - 1);
}

mp_obj_t common_hal_pathlib_posixpath_stem(pathlib_posixpath_obj_t *self) {
    mp_obj_t name = common_hal_pathlib_posixpath_name(self);
    GET_STR_DATA_LEN(name, name_str, name_len);

    // Empty name
    if (name_len == 0) {
        return name;
    }

    // Find the last dot
    int last_dot = -1;
    for (int i = name_len - 1; i >= 0; i--) {
        if (name_str[i] == '.') {
            last_dot = i;
            break;
        }
    }

    // No dot found or dot at the start, return the whole name
    if (last_dot <= 0) {
        return name;
    }

    // Return the part before the last dot
    return mp_obj_new_str((const char *)name_str, last_dot);
}

mp_obj_t common_hal_pathlib_posixpath_suffix(pathlib_posixpath_obj_t *self) {
    mp_obj_t name = common_hal_pathlib_posixpath_name(self);
    GET_STR_DATA_LEN(name, name_str, name_len);

    // Empty name
    if (name_len == 0) {
        return MP_OBJ_NEW_QSTR(MP_QSTR_);
    }

    // Find the last dot
    int last_dot = -1;
    for (int i = name_len - 1; i >= 0; i--) {
        if (name_str[i] == '.') {
            last_dot = i;
            break;
        }
    }

    // No dot found or dot at the start, return empty string
    if (last_dot <= 0) {
        return MP_OBJ_NEW_QSTR(MP_QSTR_);
    }

    // Return the part after the last dot (including the dot)
    return mp_obj_new_str((const char *)(name_str + last_dot), name_len - last_dot);
}

mp_obj_t common_hal_pathlib_posixpath_exists(pathlib_posixpath_obj_t *self) {
    if (path_exists(mp_obj_str_get_str(self->path_str))) {
        return mp_const_true;
    } else {
        return mp_const_false;
    }
}

mp_obj_t common_hal_pathlib_posixpath_is_dir(pathlib_posixpath_obj_t *self) {

    // Use common_hal_os_stat to check if path is directory
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        const char *path = mp_obj_str_get_str(self->path_str);
        mp_obj_t stat_result = common_hal_os_stat(path);
        mp_obj_tuple_t *stat_result_tpl = MP_OBJ_TO_PTR(stat_result);

        // Check if it's a directory (S_IFDIR flag in st_mode)
        mp_obj_t mode_obj = stat_result_tpl->items[0];
        mp_int_t mode_val = mp_obj_get_int(mode_obj);
        nlr_pop();

        // 0x4000 is S_IFDIR (directory)
        return mp_obj_new_bool((mode_val & 0x4000) != 0);
    } else {
        // Path doesn't exist
        return mp_const_false;
    }
}

mp_obj_t common_hal_pathlib_posixpath_is_file(pathlib_posixpath_obj_t *self) {

    // Use common_hal_os_stat to check if path is a regular file
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        const char *path = mp_obj_str_get_str(self->path_str);
        mp_obj_t stat_result = common_hal_os_stat(path);
        mp_obj_tuple_t *stat_result_tpl = MP_OBJ_TO_PTR(stat_result);

        // Check if it's a regular file (S_IFREG flag in st_mode)
        mp_obj_t mode_obj = stat_result_tpl->items[0];
        mp_int_t mode_val = mp_obj_get_int(mode_obj);
        nlr_pop();

        // 0x8000 is S_IFREG (regular file)
        return mp_obj_new_bool((mode_val & 0x8000) != 0);
    } else {
        // Path doesn't exist
        return mp_const_false;
    }
}

mp_obj_t common_hal_pathlib_posixpath_absolute(pathlib_posixpath_obj_t *self) {
    GET_STR_DATA_LEN(self->path_str, path_str, path_len);

    // Already absolute
    if (path_len > 0 && path_str[0] == '/') {
        return MP_OBJ_FROM_PTR(self);
    }

    // Get current directory using common_hal_os_getcwd()
    mp_obj_t cwd = common_hal_os_getcwd();

    // Join with current path
    mp_obj_t abs_path = common_hal_pathlib_posixpath_joinpath(
        MP_OBJ_TO_PTR(pathlib_posixpath_from_str(cwd)),
        MP_OBJ_FROM_PTR(self)
        );

    return abs_path;
}

mp_obj_t common_hal_pathlib_posixpath_resolve(pathlib_posixpath_obj_t *self) {
    // For now, just call absolute() since CircuitPython doesn't support symlinks
    return common_hal_pathlib_posixpath_absolute(self);
}

// Iterator iternext function - called for each iteration
static mp_obj_t path_dir_iter_iternext(mp_obj_t self_in) {
    mp_obj_path_dir_iter_t *self = MP_OBJ_TO_PTR(self_in);

    // Get the next filename from the iterator
    mp_obj_t next = mp_iternext(self->iter);
    if (next == MP_OBJ_STOP_ITERATION) {
        return MP_OBJ_STOP_ITERATION; // End of iteration
    }

    // Create a new path by joining the parent path with the filename
    return common_hal_pathlib_posixpath_joinpath(self->path, next);
}

// Create a new path directory iterator
mp_obj_t mp_obj_new_path_dir_iter(mp_obj_t path, mp_obj_t iter) {
    mp_obj_path_dir_iter_t *o = m_new_obj(mp_obj_path_dir_iter_t);
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = path_dir_iter_iternext;
    o->path = path;
    o->iter = iter;
    return MP_OBJ_FROM_PTR(o);
}

mp_obj_t common_hal_pathlib_posixpath_iterdir(pathlib_posixpath_obj_t *self) {
    // Get the path as a C string
    const char *path = mp_obj_str_get_str(self->path_str);

    // Use os.listdir to get the directory contents
    mp_obj_t dir_list = common_hal_os_listdir(path);

    // Get an iterator for the list of filenames
    mp_obj_t iter = mp_getiter(dir_list, NULL);

    // Create and return a path directory iterator
    return mp_obj_new_path_dir_iter(MP_OBJ_FROM_PTR(self), iter);
}
