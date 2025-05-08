// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2015 Josef Gajdusek
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stddef.h>
#include <string.h>

#include "extmod/vfs.h"
#include "py/mperrno.h"
#include "py/mpstate.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/os/__init__.h"

// This provides all VFS related OS functions so that ports can share the code
// as needed.

// Version of mp_vfs_lookup_path that takes and returns uPy string objects.
static mp_vfs_mount_t *lookup_path(const char *path, mp_obj_t *path_out) {
    const char *p_out;
    *path_out = mp_const_none;
    mp_vfs_mount_t *vfs = mp_vfs_lookup_path(path, &p_out);
    if (vfs != MP_VFS_NONE && vfs != MP_VFS_ROOT) {
        *path_out = mp_obj_new_str_of_type(&mp_type_str,
            (const byte *)p_out, strlen(p_out));
    }
    return vfs;
}

// Strip off trailing slashes to please underlying libraries
static mp_vfs_mount_t *lookup_dir_path(const char *path, mp_obj_t *path_out) {
    const char *p_out;
    *path_out = mp_const_none;
    mp_vfs_mount_t *vfs = mp_vfs_lookup_path(path, &p_out);
    if (vfs != MP_VFS_NONE && vfs != MP_VFS_ROOT) {
        size_t len = strlen(p_out);
        while (len > 1 && p_out[len - 1] == '/') {
            len--;
        }
        *path_out = mp_obj_new_str_of_type(&mp_type_str, (const byte *)p_out, len);
    }
    return vfs;
}

static mp_obj_t mp_vfs_proxy_call(mp_vfs_mount_t *vfs, qstr meth_name, size_t n_args, const mp_obj_t *args) {
    if (vfs == MP_VFS_NONE) {
        // mount point not found
        mp_raise_OSError(MP_ENODEV);
    }
    if (vfs == MP_VFS_ROOT) {
        // can't do operation on root dir
        mp_raise_OSError(MP_EPERM);
    }
    mp_obj_t meth[n_args + 2];
    mp_load_method(vfs->obj, meth_name, meth);
    if (args != NULL) {
        memcpy(meth + 2, args, n_args * sizeof(*args));
    }
    return mp_call_method_n_kw(n_args, 0, meth);
}

const char *common_hal_os_path_abspath(const char *path) {
    const char *cwd;
    if (path[0] == '/') {
        cwd = "";
    } else {
        cwd = MP_STATE_VM(cwd_path);
        if (cwd == NULL) {
            char *new_cwd = m_malloc_without_collect(2);
            strcpy(new_cwd, "/");
            MP_STATE_VM(cwd_path) = new_cwd;
            cwd = new_cwd;
        }
    }

    // Store the current output length for previous components so we can rewind to before them.
    char *full_path = m_malloc_without_collect(strlen(cwd) + strlen(path) + 2);
    size_t full_path_len = 0;
    memcpy(full_path, cwd, strlen(cwd));
    full_path_len += strlen(cwd);
    if (full_path_len > 0 && full_path[full_path_len - 1] != '/') {
        full_path[full_path_len++] = '/';
    }
    memcpy(full_path + full_path_len, path, strlen(path) + 1);

    // Scan to see if the path has any `..` in it and return the same string if it doesn't
    bool found_dot_dot = false;
    size_t slash_count = 0;
    for (size_t i = 0; i < strlen(full_path); i++) {
        if (full_path[i] == '/') {
            slash_count++;
        }
        if (i + 2 < strlen(full_path) && full_path[i] == '/' && full_path[i + 1] == '.' && full_path[i + 2] == '.' && (i + 3 == strlen(full_path) || full_path[i + 3] == '/')) {
            found_dot_dot = true;
        }
    }
    if (!found_dot_dot) {
        return full_path;
    }

    size_t slashes[slash_count];
    size_t output_len = 0;
    size_t component_len = 0;
    slash_count = 0;

    // Remove `..` and `.`
    size_t original_len = strlen(full_path);
    for (size_t i = 0; i <= original_len; i++) {
        full_path[output_len++] = full_path[i];
        // Treat the final nul character as a slash.
        if (full_path[i] == '/' || full_path[i] == '\0') {
            if (component_len == 1 && full_path[i - 1] == '.') {
                // Remove the dot
                output_len = slashes[slash_count - 1];
            } else if (component_len == 2 && full_path[i - 1] == '.' && full_path[i - 2] == '.') {
                // Remove the double dot and the previous component if it exists
                slash_count--;
                output_len = slashes[slash_count - 1];
            } else {
                slashes[slash_count] = output_len;
                slash_count++;
            }
            component_len = 0;
        } else {
            component_len++;
        }
    }
    full_path[output_len] = '\0';
    return full_path;
}

void common_hal_os_chdir(const char *path) {
    MP_STATE_VM(cwd_path) = common_hal_os_path_abspath(path);
    mp_obj_t path_out;
    mp_vfs_mount_t *vfs = lookup_dir_path(MP_STATE_VM(cwd_path), &path_out);
    MP_STATE_VM(vfs_cur) = vfs;
    if (vfs == MP_VFS_ROOT) {
        // If we change to the root dir and a VFS is mounted at the root then
        // we must change that VFS's current dir to the root dir so that any
        // subsequent relative paths begin at the root of that VFS.
        for (vfs = MP_STATE_VM(vfs_mount_table); vfs != NULL; vfs = vfs->next) {
            if (vfs->len == 1) {
                mp_obj_t root = mp_obj_new_str("/", 1);
                mp_vfs_proxy_call(vfs, MP_QSTR_chdir, 1, &root);
                break;
            }
        }
    } else {
        mp_vfs_proxy_call(vfs, MP_QSTR_chdir, 1, &path_out);
    }
}

mp_obj_t common_hal_os_getcwd(void) {
    const char *cwd = MP_STATE_VM(cwd_path);
    if (cwd == NULL) {
        return MP_OBJ_NEW_QSTR(MP_QSTR__slash_);
    }
    return mp_obj_new_str_of_type(&mp_type_str, (const byte *)cwd, strlen(cwd));
}

mp_obj_t common_hal_os_listdir(const char *path) {
    const char *abspath = common_hal_os_path_abspath(path);
    mp_obj_t path_out;
    mp_vfs_mount_t *vfs = lookup_dir_path(abspath, &path_out);
    if (vfs == MP_VFS_ROOT) {
        vfs = MP_STATE_VM(vfs_mount_table);
        while (vfs != NULL) {
            if (vfs->len == 1) {
                break;
            }
            vfs = vfs->next;
        }
        path_out = MP_OBJ_NEW_QSTR(MP_QSTR__slash_);
    }

    mp_obj_t iter_obj = mp_vfs_proxy_call(vfs, MP_QSTR_ilistdir, 1, &path_out);

    mp_obj_t dir_list = mp_obj_new_list(0, NULL);
    mp_obj_t next;
    while ((next = mp_iternext(iter_obj)) != MP_OBJ_STOP_ITERATION) {
        // next[0] is the filename.
        mp_obj_list_append(dir_list, mp_obj_subscr(next, MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_SENTINEL));
        RUN_BACKGROUND_TASKS;
    }
    return dir_list;
}

void common_hal_os_mkdir(const char *path) {
    const char *abspath = common_hal_os_path_abspath(path);
    mp_obj_t path_out;
    mp_vfs_mount_t *vfs = lookup_dir_path(abspath, &path_out);
    if (vfs == MP_VFS_ROOT || (vfs != MP_VFS_NONE && !strcmp(mp_obj_str_get_str(path_out), "/"))) {
        mp_raise_OSError(MP_EEXIST);
    }
    mp_vfs_proxy_call(vfs, MP_QSTR_mkdir, 1, &path_out);
}

void common_hal_os_remove(const char *path) {
    const char *abspath = common_hal_os_path_abspath(path);
    mp_obj_t path_out;
    mp_vfs_mount_t *vfs = lookup_path(abspath, &path_out);
    mp_vfs_proxy_call(vfs, MP_QSTR_remove, 1, &path_out);
}

void common_hal_os_rename(const char *old_path, const char *new_path) {
    mp_obj_t args[2];
    mp_vfs_mount_t *old_vfs = lookup_path(old_path, &args[0]);
    mp_vfs_mount_t *new_vfs = lookup_path(new_path, &args[1]);
    if (old_vfs != new_vfs) {
        // can't rename across filesystems
        mp_raise_OSError(MP_EPERM);
    }
    mp_vfs_proxy_call(old_vfs, MP_QSTR_rename, 2, args);
}

void common_hal_os_rmdir(const char *path) {
    const char *abspath = common_hal_os_path_abspath(path);
    mp_obj_t path_out;
    mp_vfs_mount_t *vfs = lookup_dir_path(abspath, &path_out);
    mp_vfs_proxy_call(vfs, MP_QSTR_rmdir, 1, &path_out);
}

mp_obj_t common_hal_os_stat(const char *path) {
    const char *abspath = common_hal_os_path_abspath(path);
    mp_obj_t path_out;
    mp_vfs_mount_t *vfs = lookup_path(abspath, &path_out);
    if (vfs == MP_VFS_ROOT) {
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
        t->items[0] = MP_OBJ_NEW_SMALL_INT(MP_S_IFDIR); // st_mode
        for (int i = 1; i <= 9; ++i) {
            t->items[i] = MP_OBJ_NEW_SMALL_INT(0); // dev, nlink, uid, gid, size, atime, mtime, ctime
        }
        return MP_OBJ_FROM_PTR(t);
    }
    return mp_vfs_proxy_call(vfs, MP_QSTR_stat, 1, &path_out);
}

mp_obj_t common_hal_os_statvfs(const char *path) {
    const char *abspath = common_hal_os_path_abspath(path);
    mp_obj_t path_out;
    mp_vfs_mount_t *vfs = lookup_path(abspath, &path_out);
    if (vfs == MP_VFS_ROOT) {
        // statvfs called on the root directory, see if there's anything mounted there
        for (vfs = MP_STATE_VM(vfs_mount_table); vfs != NULL; vfs = vfs->next) {
            if (vfs->len == 1) {
                break;
            }
        }

        // If there's nothing mounted at root then return a mostly-empty tuple
        if (vfs == NULL) {
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));

            // fill in: bsize, frsize, blocks, bfree, bavail, files, ffree, favail, flags
            for (int i = 0; i <= 8; ++i) {
                t->items[i] = MP_OBJ_NEW_SMALL_INT(0);
            }

            // Put something sensible in f_namemax
            t->items[9] = MP_OBJ_NEW_SMALL_INT(MICROPY_ALLOC_PATH_MAX);

            return MP_OBJ_FROM_PTR(t);
        }

        // VFS mounted at root so delegate the call to it
        path_out = MP_OBJ_NEW_QSTR(MP_QSTR__slash_);
    }
    return mp_vfs_proxy_call(vfs, MP_QSTR_statvfs, 1, &path_out);
}

void common_hal_os_utime(const char *path, mp_obj_t times) {
    const char *abspath = common_hal_os_path_abspath(path);
    mp_obj_t args[2];
    mp_vfs_mount_t *vfs = lookup_path(abspath, &args[0]);
    args[1] = times;
    mp_vfs_proxy_call(vfs, MP_QSTR_utime, 2, args);
}

MP_REGISTER_ROOT_POINTER(const char *cwd_path);
