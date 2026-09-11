// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stddef.h>
#include <stdint.h>

#include "supervisor/filesystem.h"


void filesystem_background(void) {
    return;
}

void filesystem_tick(void) {
    return;
}

bool filesystem_init(bool create_allowed, bool force_create) {
    (void)create_allowed;
    (void)force_create;
    return true;
}

void filesystem_flush(void) {
}

void filesystem_set_internal_writable_by_usb(bool writable) {
    (void)writable;
    return;
}

void filesystem_set_writable_by_usb(supervisor_vfs_t *vfs, bool usb_writable) {
    (void)vfs;
    (void)usb_writable;
    return;
}

bool filesystem_is_writable_by_python(supervisor_vfs_t *vfs) {
    (void)vfs;
    return true;
}

bool filesystem_is_writable_by_usb(fs_user_mount_t *vfs) {
    return true;
}

void filesystem_set_internal_concurrent_write_protection(bool concurrent_write_protection) {
    (void)concurrent_write_protection;
    return;
}

void filesystem_set_concurrent_write_protection(supervisor_vfs_t *vfs, bool concurrent_write_protection) {
    (void)vfs;
    (void)concurrent_write_protection;
    return;
}

bool filesystem_present(void) {
    return false;
}

// Without a filesystem, nothing is supported and all file operations fail.
bool supervisor_vfs_supported(supervisor_vfs_t *fs_mount) {
    (void)fs_mount;
    return false;
}

supervisor_fs_err_t supervisor_vfs_open_file(supervisor_vfs_t *vfs, const char *path, uint32_t flags,
    uint64_t mtime_ns, supervisor_vfs_file_t *file) {
    (void)vfs;
    (void)path;
    (void)flags;
    (void)mtime_ns;
    (void)file;
    return SUPERVISOR_FS_IO;
}

supervisor_fs_err_t supervisor_vfs_close_file(supervisor_vfs_file_t *file) {
    (void)file;
    return SUPERVISOR_FS_OK;
}

supervisor_fs_err_t supervisor_vfs_read_file(supervisor_vfs_file_t *file, void *buf, size_t len, size_t *bytes_read) {
    (void)file;
    (void)buf;
    (void)len;
    *bytes_read = 0;
    return SUPERVISOR_FS_IO;
}

supervisor_fs_err_t supervisor_vfs_write_file(supervisor_vfs_file_t *file, const void *buf, size_t len, size_t *bytes_written) {
    (void)file;
    (void)buf;
    (void)len;
    *bytes_written = 0;
    return SUPERVISOR_FS_IO;
}

supervisor_fs_err_t supervisor_vfs_seek_file(supervisor_vfs_file_t *file, size_t offset) {
    (void)file;
    (void)offset;
    return SUPERVISOR_FS_IO;
}

size_t supervisor_vfs_tell_file(supervisor_vfs_file_t *file) {
    (void)file;
    return 0;
}

size_t supervisor_vfs_file_size(supervisor_vfs_file_t *file) {
    (void)file;
    return 0;
}

supervisor_fs_err_t supervisor_vfs_truncate_file(supervisor_vfs_file_t *file) {
    (void)file;
    return SUPERVISOR_FS_IO;
}

supervisor_fs_err_t supervisor_vfs_stat(supervisor_vfs_t *vfs, const char *path, bool *is_dir, size_t *size, uint64_t *mtime_ns) {
    (void)vfs;
    (void)path;
    if (is_dir != NULL) {
        *is_dir = false;
    }
    if (size != NULL) {
        *size = 0;
    }
    if (mtime_ns != NULL) {
        *mtime_ns = 0;
    }
    return SUPERVISOR_FS_NO_FILE;
}

supervisor_fs_err_t supervisor_vfs_mkdir(supervisor_vfs_t *vfs, const char *path, uint64_t mtime_ns) {
    (void)vfs;
    (void)path;
    (void)mtime_ns;
    return SUPERVISOR_FS_IO;
}

supervisor_fs_err_t supervisor_vfs_rename(supervisor_vfs_t *vfs, const char *old_path, const char *new_path) {
    (void)vfs;
    (void)old_path;
    (void)new_path;
    return SUPERVISOR_FS_IO;
}

supervisor_fs_err_t supervisor_vfs_unlink(supervisor_vfs_t *vfs, const char *path) {
    (void)vfs;
    (void)path;
    return SUPERVISOR_FS_IO;
}

supervisor_fs_err_t supervisor_vfs_opendir(supervisor_vfs_t *vfs, const char *path, supervisor_vfs_dir_t *dir) {
    (void)vfs;
    (void)path;
    (void)dir;
    return SUPERVISOR_FS_NO_FILE;
}

supervisor_fs_err_t supervisor_vfs_readdir(supervisor_vfs_dir_t *dir, char *name, size_t name_len, bool *is_dir, size_t *size, uint64_t *mtime_ns) {
    (void)dir;
    (void)name_len;
    if (is_dir != NULL) {
        *is_dir = false;
    }
    if (size != NULL) {
        *size = 0;
    }
    if (mtime_ns != NULL) {
        *mtime_ns = 0;
    }
    name[0] = '\0';
    return SUPERVISOR_FS_OK;
}

supervisor_fs_err_t supervisor_vfs_rewinddir(supervisor_vfs_dir_t *dir) {
    (void)dir;
    return SUPERVISOR_FS_IO;
}

supervisor_fs_err_t supervisor_vfs_closedir(supervisor_vfs_dir_t *dir) {
    (void)dir;
    return SUPERVISOR_FS_OK;
}

supervisor_fs_err_t supervisor_vfs_statfs(supervisor_vfs_t *vfs, size_t *block_size, size_t *total_blocks, size_t *free_blocks) {
    (void)vfs;
    (void)block_size;
    (void)total_blocks;
    (void)free_blocks;
    return SUPERVISOR_FS_IO;
}
