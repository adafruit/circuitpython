// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "shared-module/storage/__init__.h"

void common_hal_storage_mount(mp_obj_t vfs_obj, const char *path, bool readonly);
void common_hal_storage_umount_path(const char *path);
void common_hal_storage_umount_object(mp_obj_t vfs_obj);
void common_hal_storage_remount(const char *path, bool readonly, bool disable_concurrent_write_protection);
mp_obj_t common_hal_storage_getmount(const char *path);
MP_NORETURN void common_hal_storage_erase_filesystem(bool extended);

bool common_hal_storage_disable_usb_drive(void);
bool common_hal_storage_unsafe_disable_usb_drive(void);
bool common_hal_storage_enable_usb_drive(void);

mp_obj_t common_hal_storage_map_file(mp_obj_t file);
#if CIRCUITPY_STORAGE_MAP_FILE
struct _fs_user_mount_t;
// Raises OSError if path on vfs is a file mapped since the VM started: a write would change
// bytes in use.
void storage_map_file_check_writable(struct _fs_user_mount_t *vfs, const char *path);
// True if path on vfs is such a file. Does not raise: for the supervisor's file writers.
bool storage_map_file_in_use(struct _fs_user_mount_t *vfs, const char *path);
// Read the mapping setting once after mounting CIRCUITPY, before user code runs.
void storage_map_file_init(void);
void storage_map_file_reset(void);
// A run is a part of a file that lies at consecutive flash addresses. A fragmented file has
// several runs.
// Calls fn for each run of a CIRCUITPY file open for reading, in file order, with the run's first
// drive block. Returns 0, or
// MP_EINVAL (closed or writable), MP_EOPNOTSUPP (mapping disabled or not a mapped CIRCUITPY
// file), MP_ENOMEM (no cluster map) or MP_EIO (bad chain).
typedef void (*storage_map_run_fn_t)(void *ctx, const uint8_t *addr, size_t len, uint32_t block);
int storage_map_runs(mp_obj_t file, storage_map_run_fn_t fn, void *ctx);
#endif
#if CIRCUITPY_STORAGE_MAP_IMPORT
#include "py/reader.h"
// Points reader at filename's flash runs if it is a mapped CIRCUITPY file; false otherwise.
bool storage_map_file_reader(mp_reader_t *reader, qstr filename);
mp_uint_t storage_map_reader_readbyte(void *data);
// Data in flash for the next len bytes if they lie in one run, else NULL and nothing is consumed.
const uint8_t *storage_map_reader_try_read_rom(void *data, size_t len);
// True if writing CIRCUITPY blocks [lba, lba + count) would change an imported .mpy that may
// still run. Call with interrupts disabled: the USB task may run beside the VM.
bool storage_map_import_uses_blocks(uint32_t lba, uint32_t count);
#endif
