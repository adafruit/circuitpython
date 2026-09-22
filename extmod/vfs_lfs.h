/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef MICROPY_INCLUDED_EXTMOD_VFS_LFS_H
#define MICROPY_INCLUDED_EXTMOD_VFS_LFS_H

#include "py/obj.h"

extern const mp_obj_type_t mp_type_vfs_lfs1;
extern const mp_obj_type_t mp_type_vfs_lfs1_fileio;
extern const mp_obj_type_t mp_type_vfs_lfs1_textio;

extern const mp_obj_type_t mp_type_vfs_lfs2;
extern const mp_obj_type_t mp_type_vfs_lfs2_fileio;
extern const mp_obj_type_t mp_type_vfs_lfs2_textio;

// CIRCUITPY-CHANGE: Export the lfs2 VFS object so the supervisor can mount
// littlefs directly (before the VM and its GC are running) using the same
// layout the VFS methods in vfs_lfsx.c expect.
#if MICROPY_VFS_LFS2
#include "lib/littlefs/lfs2.h"
#include "extmod/vfs.h"

typedef struct _mp_obj_vfs_lfs2_t {
    mp_obj_base_t base;
    mp_vfs_blockdev_t blockdev;
    bool enable_mtime;
    vstr_t cur_dir;
    struct lfs2_config config;
    lfs2_t lfs;
} mp_obj_vfs_lfs2_t;

// Mount (and optionally format first) a littlefs filesystem on the
// caller-prepared lfs2_config at self->config. self must be zeroed static or
// VM-heap storage; buffers are allocated via alloc(). Returns self_in on
// success or MP_OBJ_NULL on mount failure (the errno-like lfs2 return code is
// passed out via mount_err). formatted_out is set to true when a fresh
// filesystem was formatted before mounting successfully; it is never cleared,
// so the caller must initialize it to false (and may set it true itself after
// an explicit format).
mp_obj_t mp_vfs_lfs2_mount_supervisor(mp_obj_vfs_lfs2_t *self, void *(*alloc)(size_t), bool format_allowed, bool *formatted_out, int *mount_err);
#endif

#endif // MICROPY_INCLUDED_EXTMOD_VFS_LFS_H
