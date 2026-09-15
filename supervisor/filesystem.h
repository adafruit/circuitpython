// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "extmod/vfs_fat.h"
#if CIRCUITPY_FILESYSTEM_LITTLEFS
#include "extmod/vfs_lfs.h"
#endif

// A supervisor-managed filesystem object, which is either a FAT fs_user_mount_t
// or, on littlefs builds, an mp_obj_vfs_lfs2_t. Both begin with the same
// mp_obj_base_t + mp_vfs_blockdev_t prefix, so a bare fs_user_mount_t (e.g. an
// SD card mount) can also be cast to supervisor_vfs_t and inspected through
// .common. The two kinds are told apart by base.type: &mp_fat_vfs_type vs
// &mp_type_vfs_lfs2.
typedef union _supervisor_vfs_t {
    struct {
        mp_obj_base_t base;
        mp_vfs_blockdev_t blockdev;
    } common;
    fs_user_mount_t fat;
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    mp_obj_vfs_lfs2_t lfs2;
    #endif
} supervisor_vfs_t;

extern volatile bool filesystem_flush_requested;

// Supervisor-level filesystem API. These functions work on both FAT and
// littlefs mounts so that supervisor code (workflows, settings) doesn't have to
// care which one is active.

// Error codes shared by the supervisor filesystem API. They mirror the subset
// of FatFS FRESULT values used by the workflows so call sites can be shared
// between FAT and littlefs.
typedef enum {
    SUPERVISOR_FS_OK = 0,
    SUPERVISOR_FS_NO_FILE,          // File or directory doesn't exist.
    SUPERVISOR_FS_NO_PATH,          // A path component is missing (or bad).
    SUPERVISOR_FS_EXIST,            // File or directory already exists.
    SUPERVISOR_FS_WRITE_PROTECTED,  // Filesystem or media is not writable.
    SUPERVISOR_FS_NO_SPACE,         // No space left on the filesystem.
    SUPERVISOR_FS_IO,               // Any other error.
} supervisor_fs_err_t;

// Open flags for supervisor_vfs_open_file().
#define SUPERVISOR_FS_OPEN_READ         0x01
#define SUPERVISOR_FS_OPEN_WRITE        0x02
// Create the file if it doesn't exist (with WRITE). An existing file keeps its
// contents.
#define SUPERVISOR_FS_OPEN_CREATE       0x04
// Empty an existing file on open (with WRITE|CREATE).
#define SUPERVISOR_FS_OPEN_TRUNCATE     0x08

// CIRCUITPY-CHANGE: littlefs mtime attribute id, matching extmod/vfs_lfs.c:
// 64-bit little endian, nanoseconds since 1970/1/1.
#ifndef LFS_ATTR_MTIME
#define LFS_ATTR_MTIME (1)
#endif

// Handle for a file opened with supervisor_vfs_open_file().
typedef struct _supervisor_vfs_file_t {
    bool open;
    bool lfs;
    supervisor_vfs_t *vfs;
    // FAT: timestamp to stamp the file with on close (0 = current time).
    DWORD fattime;
    union {
        FIL fat;
        #if CIRCUITPY_FILESYSTEM_LITTLEFS
        struct {
            lfs2_file_t lfs2;
            struct lfs2_file_config cfg;
            struct lfs2_attr attr;
            uint8_t mtime[8];
            uint8_t buffer[FILESYSTEM_BLOCK_SIZE];
        } lfs;
        #endif
    } file;
} supervisor_vfs_file_t;

// Directory handle for supervisor_vfs_opendir().
#define SUPERVISOR_VFS_DIR_PATH_MAX (256)
typedef struct _supervisor_vfs_dir_t {
    bool open;
    bool lfs;
    supervisor_vfs_t *vfs;
    // Directory path used to build per-entry paths for littlefs, which keeps
    // modification times in per-path attributes. Unused on FAT.
    char path[SUPERVISOR_VFS_DIR_PATH_MAX];
    union {
        FF_DIR fat;
        #if CIRCUITPY_FILESYSTEM_LITTLEFS
        lfs2_dir_t lfs2;
        #endif
    } dir;
} supervisor_vfs_dir_t;

// Open a file on the given mount. path is relative to the mount. mtime_ns is
// the modification time to use for a newly written file, in nanoseconds past
// 1970/1/1, or 0 to use the current RTC time (the same time get_fattime()
// stamps FAT files with, including its fixed fallback when there is no RTC). On success, file must be closed with
// supervisor_vfs_close_file().
supervisor_fs_err_t supervisor_vfs_open_file(supervisor_vfs_t *vfs, const char *path, uint32_t flags,
    uint64_t mtime_ns, supervisor_vfs_file_t *file);
supervisor_fs_err_t supervisor_vfs_close_file(supervisor_vfs_file_t *file);
// Read at most len bytes into buf. On success *bytes_read holds the number of
// bytes read; it may be less than len, and 0 at end of file.
supervisor_fs_err_t supervisor_vfs_read_file(supervisor_vfs_file_t *file, void *buf, size_t len, size_t *bytes_read);
// Write len bytes. On success *bytes_written holds the number of bytes
// written; it is less than len when the filesystem is full.
supervisor_fs_err_t supervisor_vfs_write_file(supervisor_vfs_file_t *file, const void *buf, size_t len, size_t *bytes_written);
// Seek to an absolute offset from the start of the file.
supervisor_fs_err_t supervisor_vfs_seek_file(supervisor_vfs_file_t *file, size_t offset);
// Current absolute offset from the start of the file.
size_t supervisor_vfs_tell_file(supervisor_vfs_file_t *file);
// File size in bytes.
size_t supervisor_vfs_file_size(supervisor_vfs_file_t *file);
// Truncate the file to the current offset.
supervisor_fs_err_t supervisor_vfs_truncate_file(supervisor_vfs_file_t *file);

// Stat a file or directory. All output arguments are optional (may be NULL).
// size is 0 for directories. mtime_ns is nanoseconds past the same epoch used
// for the littlefs mtime attribute (1970/1/1) and is 0 when unknown.
supervisor_fs_err_t supervisor_vfs_stat(supervisor_vfs_t *vfs, const char *path, bool *is_dir, size_t *size, uint64_t *mtime_ns);
// Create a directory. mtime_ns is used to stamp it on FAT, 0 to use the
// current RTC time. littlefs directories have no modification time.
supervisor_fs_err_t supervisor_vfs_mkdir(supervisor_vfs_t *vfs, const char *path, uint64_t mtime_ns);
supervisor_fs_err_t supervisor_vfs_rename(supervisor_vfs_t *vfs, const char *old_path, const char *new_path);
supervisor_fs_err_t supervisor_vfs_unlink(supervisor_vfs_t *vfs, const char *path);

// Open a directory for listing. path is relative to the mount.
supervisor_fs_err_t supervisor_vfs_opendir(supervisor_vfs_t *vfs, const char *path, supervisor_vfs_dir_t *dir);
// Read the next directory entry. On success name holds a null-terminated entry
// name; at the end of the directory, name is empty. The other output arguments
// are optional (may be NULL).
supervisor_fs_err_t supervisor_vfs_readdir(supervisor_vfs_dir_t *dir, char *name, size_t name_len, bool *is_dir, size_t *size, uint64_t *mtime_ns);
supervisor_fs_err_t supervisor_vfs_rewinddir(supervisor_vfs_dir_t *dir);
supervisor_fs_err_t supervisor_vfs_closedir(supervisor_vfs_dir_t *dir);

// Filesystem geometry in blocks. For FAT these are clusters; for littlefs,
// blocks.
supervisor_fs_err_t supervisor_vfs_statfs(supervisor_vfs_t *vfs, size_t *block_size, size_t *total_blocks, size_t *free_blocks);

void filesystem_background(void);
void filesystem_tick(void);
bool filesystem_init(bool create_allowed, bool force_create);
void filesystem_flush(void);
bool filesystem_present(void);
void filesystem_set_internal_writable_by_usb(bool usb_writable);
void filesystem_set_internal_concurrent_write_protection(bool concurrent_write_protection);
void filesystem_set_writable_by_usb(supervisor_vfs_t *vfs, bool usb_writable);
void filesystem_set_concurrent_write_protection(supervisor_vfs_t *vfs, bool concurrent_write_protection);
void filesystem_set_ignore_write_protection(fs_user_mount_t *vfs, bool ignore_write_protection);

// Whether user code can modify the filesystem. It doesn't depend on the state
// of USB. Don't use this for a workflow. In workflows, grab the shared file
// system lock.
bool filesystem_is_writable_by_python(supervisor_vfs_t *vfs);

// This controls whether USB tries to grab the underlying block device lock
// during enumeration. If another workflow is modifying the filesystem when this
// happens, then USB will be readonly.
bool filesystem_is_writable_by_usb(fs_user_mount_t *vfs);

supervisor_vfs_t *filesystem_circuitpy(void);
supervisor_vfs_t *filesystem_for_path(const char *path_in, const char **path_under_mount);

// Whether the supervisor-level filesystem API (above) can operate on this
// mount: FAT mounts reachable through FatFS and supervisor littlefs mounts.
// Use it to reject mounts (e.g. non-native or remote filesystems) that the
// workflows cannot access.
bool supervisor_vfs_supported(supervisor_vfs_t *fs_mount);

// We have two levels of locking. filesystem_* calls grab a shared blockdev lock to allow
// CircuitPython's fatfs code to edit the blocks. blockdev_* calls grab a lock to mutate blocks
// directly, excluding any filesystem_* locks.

bool filesystem_lock(supervisor_vfs_t *fs_mount);
void filesystem_unlock(supervisor_vfs_t *fs_mount);

bool blockdev_lock(supervisor_vfs_t *fs_mount);
void blockdev_unlock(supervisor_vfs_t *fs_mount);
