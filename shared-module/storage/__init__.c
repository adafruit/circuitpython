// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2015 Josef Gajdusek
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "extmod/vfs.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/os/__init__.h"
#include "shared-bindings/storage/__init__.h"
#include "supervisor/filesystem.h"
#include "supervisor/flash.h"
#include "supervisor/shared/reload.h"
#if CIRCUITPY_STORAGE_MAP_FILE
#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"
#include "py/objarray.h"
#include "py/objlist.h"
#include "py/stream.h"
#if CIRCUITPY_SETTINGS_TOML
#include "supervisor/shared/settings.h"
#endif
#endif

#if CIRCUITPY_USB_DEVICE
#include "supervisor/usb.h"
#endif

#if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
#include "tusb.h"

// Is the MSC device enabled?
static volatile bool storage_usb_is_enabled;
// CIRCUITPY's USB writability when the drive was disabled. It is restored on enable
// unless a remount() changed it in the meantime.
static bool usb_writable_before_disable;
static bool restore_usb_writable;

void storage_usb_set_defaults(void) {
    storage_usb_is_enabled = CIRCUITPY_USB_MSC_ENABLED_DEFAULT;
    restore_usb_writable = false;
}

bool storage_usb_enabled(void) {
    return storage_usb_is_enabled;
}

static bool usb_drive_set_enabled(bool enabled) {
    if (enabled) {
        // Set this before the LUN reports ready: the host reads write-protect status when it remounts.
        if (restore_usb_writable) {
            filesystem_set_internal_writable_by_usb(usb_writable_before_disable);
            restore_usb_writable = false;
        }
    } else if (storage_usb_is_enabled) {
        supervisor_vfs_t *circuitpy = filesystem_circuitpy();
        usb_writable_before_disable = circuitpy != NULL &&
            (circuitpy->common.blockdev.flags & MP_BLOCKDEV_FLAG_USB_WRITABLE) != 0;
        restore_usb_writable = true;
    }
    // We can't change the descriptors once we're connected, but we can make the LUN be ready or not ready.
    storage_usb_is_enabled = enabled;
    if (tud_connected()) {
        // The TEST UNIT READY callback in usb_msc_flash.c checks the value of storage_usb_is_enabled.
        // If it's false, TEST UNIT READY will report "not ready"
        // Linux and macOS send a TEST UNIT READY poll about every 1.1 seconds or faster.
        // Windows polls every 2.1 seconds or so.
        // So wait long enough for host to send a TEST UNIT READY and receive a reply.
        mp_hal_delay_ms(2500);
    }
    if (!enabled) {
        filesystem_set_internal_writable_by_usb(false);
        // USB MSC may still hold the lock from when it reported the drive writable.
        usb_msc_release_circuitpy_lock();
    }
    return true;
}

bool common_hal_storage_disable_usb_drive(void) {
    if (tud_connected()) {
        // Complain if already connected. Use `storage.unsafe_disable_usb_drive()` in that case.
        return false;
    }
    return usb_drive_set_enabled(false);
}

bool common_hal_storage_unsafe_disable_usb_drive(void) {
    return usb_drive_set_enabled(false);
}

bool common_hal_storage_enable_usb_drive(void) {
    // A file open for writing or a workflow transfer holds the filesystem lock.
    // The host would then see the drive as read-only, and keep it that way after the lock is released.
    supervisor_vfs_t *circuitpy = filesystem_circuitpy();
    if (!storage_usb_is_enabled && circuitpy != NULL &&
        (circuitpy->common.blockdev.flags & MP_BLOCKDEV_FLAG_LOCKED) != 0) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("File write in progress"));
    }
    return usb_drive_set_enabled(true);
}
#else
bool common_hal_storage_disable_usb_drive(void) {
    return false;
}

bool common_hal_storage_unsafe_disable_usb_drive(void) {
    return false;
}

bool common_hal_storage_enable_usb_drive(void) {
    return false;
}
#endif // CIRCUITPY_USB_MSC

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

void common_hal_storage_mount(mp_obj_t vfs_obj, const char *mount_path, bool readonly) {
    const char *abs_mount_path = common_hal_os_path_abspath(mount_path);
    // create new object
    mp_vfs_mount_t *vfs = m_new_obj(mp_vfs_mount_t);
    vfs->str = abs_mount_path;
    vfs->len = strlen(abs_mount_path);
    vfs->obj = vfs_obj;
    vfs->next = NULL;

    mp_obj_t args[2];
    args[0] = readonly ? mp_const_true : mp_const_false;
    args[1] = mp_const_false; // Don't make the file system automatically when mounting.

    // Check that there is a directory with the same name as the mount point.
    // But it's ok to mount '/' in any case.
    if (strcmp(vfs->str, "/") != 0) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_obj_t mount_point_stat = common_hal_os_stat(abs_mount_path);
            nlr_pop();
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mount_point_stat);
            if ((MP_OBJ_SMALL_INT_VALUE(t->items[0]) & MP_S_IFDIR) == 0) {
                mp_raise_RuntimeError(MP_ERROR_TEXT("Mount point directory missing"));
            }
        } else {
            // Something with the same name doesn't exist.
            mp_raise_RuntimeError(MP_ERROR_TEXT("Mount point directory missing"));
        }
    }

    // check that the destination mount point is unused
    const char *path_out;
    mp_vfs_mount_t *existing_mount = mp_vfs_lookup_path(abs_mount_path, &path_out);
    if (existing_mount != MP_VFS_NONE && existing_mount != MP_VFS_ROOT) {
        if (vfs->len != 1 && existing_mount->len == 1) {
            // if root dir is mounted, still allow to mount something within a subdir of root
        } else {
            // mount point in use
            mp_raise_OSError(MP_EPERM);
        }
    }

    // call the underlying object to do any mounting operation
    mp_vfs_proxy_call(vfs, MP_QSTR_mount, 2, (mp_obj_t *)&args);

    fs_user_mount_t *vfs_fat = MP_OBJ_TO_PTR(vfs_obj);
    // Filesystem is read-only to USB if writable by CircuitPython, and vice versa.
    // The flag helpers take a supervisor_vfs_t; a mounted VfsFat is always a
    // fs_user_mount_t, whose base + blockdev prefix matches both vfs
    // kinds.
    filesystem_set_writable_by_usb((supervisor_vfs_t *)vfs_fat, readonly);
    filesystem_set_concurrent_write_protection((supervisor_vfs_t *)vfs_fat, true);

    // Insert the vfs into the mount table by pushing it onto the front of the
    // mount table.
    mp_vfs_mount_t **vfsp = &MP_STATE_VM(vfs_mount_table);
    vfs->next = *vfsp;
    *vfsp = vfs;
}

void common_hal_storage_umount_object(mp_obj_t vfs_obj) {
    // remove vfs from the mount table
    mp_vfs_mount_t *vfs = NULL;
    for (mp_vfs_mount_t **vfsp = &MP_STATE_VM(vfs_mount_table); *vfsp != NULL; vfsp = &(*vfsp)->next) {
        if ((*vfsp)->obj == vfs_obj) {
            vfs = *vfsp;
            *vfsp = (*vfsp)->next;
            break;
        }
    }

    if (vfs == NULL) {
        mp_raise_OSError(MP_EINVAL);
    }

    // if we unmounted the current device then set current to root
    if (MP_STATE_VM(vfs_cur) == vfs) {
        MP_STATE_VM(vfs_cur) = MP_VFS_ROOT;
    }

    // call the underlying object to do any unmounting operation
    mp_vfs_proxy_call(vfs, MP_QSTR_umount, 0, NULL);
}

static mp_obj_t storage_object_from_path(const char *mount_path) {
    const char *abs_mount_path = common_hal_os_path_abspath(mount_path);
    for (mp_vfs_mount_t **vfsp = &MP_STATE_VM(vfs_mount_table); *vfsp != NULL; vfsp = &(*vfsp)->next) {
        if (strcmp(abs_mount_path, (*vfsp)->str) == 0) {
            return (*vfsp)->obj;
        }
    }
    mp_raise_OSError(MP_EINVAL);
}

void common_hal_storage_umount_path(const char *mount_path) {
    common_hal_storage_umount_object(storage_object_from_path(mount_path));
}

mp_obj_t common_hal_storage_getmount(const char *mount_path) {
    return storage_object_from_path(mount_path);
}

static void remount_vfs(supervisor_vfs_t *fs_mount, bool readonly, bool disable_concurrent_write_protection) {
    #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
    if (!blockdev_lock(fs_mount)) {
        if (usb_msc_holds_lock(&fs_mount->fat)) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot remount path when visible via USB."));
        }
        // A file open for writing or a workflow transfer holds the lock.
        mp_raise_RuntimeError(MP_ERROR_TEXT("File write in progress"));
    }
    #endif

    filesystem_set_writable_by_usb(fs_mount, readonly);
    filesystem_set_concurrent_write_protection(fs_mount, !disable_concurrent_write_protection);
    blockdev_unlock(fs_mount);

    #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
    if (fs_mount == filesystem_circuitpy()) {
        // remount() wins over the state saved when the USB drive was disabled.
        restore_usb_writable = false;
    }
    usb_msc_remount(&fs_mount->fat);
    #endif
}

void common_hal_storage_remount(const char *mount_path, bool readonly, bool disable_concurrent_write_protection) {
    const char *path_under_mount;
    const char *abs_mount_path = common_hal_os_path_abspath(mount_path);
    supervisor_vfs_t *fs_mount = filesystem_for_path(abs_mount_path, &path_under_mount);
    if (path_under_mount[0] != 0 && strcmp(abs_mount_path, "/") != 0) {
        mp_raise_OSError(MP_EINVAL);
    }
    if (fs_mount == NULL) {
        // Nothing is mounted there (or there is no filesystem at all).
        mp_raise_OSError(MP_EINVAL);
    }

    remount_vfs(fs_mount, readonly, disable_concurrent_write_protection);
}

void common_hal_storage_erase_filesystem(bool extended) {
    #if CIRCUITPY_USB_DEVICE
    usb_disconnect();
    #endif
    mp_hal_delay_ms(1000);
    #if CIRCUITPY_STORAGE_EXTEND
    supervisor_flash_set_extended(extended);
    #endif
    (void)filesystem_init(false, true);  // Force a re-format. Ignore failure.
    common_hal_mcu_on_next_reset(RUNMODE_NORMAL);
    common_hal_mcu_reset();
    // We won't actually get here, since we're resetting.
}

#if CIRCUITPY_STORAGE_MAP_FILE
// Flash mappings can outlive the Python VM, so read this setting only at boot.
static bool storage_map_file_enabled = true;

void storage_map_file_init(void) {
    #if CIRCUITPY_SETTINGS_TOML
    settings_get_bool("CIRCUITPY_STORAGE_MAP_FILE", &storage_map_file_enabled);
    #endif
}

// Start clusters of the files mapped since the VM started; a write to one of them is refused
// until reload.
MP_REGISTER_ROOT_POINTER(mp_obj_t storage_mapped_files);

#if CIRCUITPY_STORAGE_MAP_IMPORT
// Readers of the .mpy files imported in place, linked through next_reader. They stay
// until reload: their runs are in use.
MP_REGISTER_ROOT_POINTER(void *storage_mapped_imports);
#endif

void storage_map_file_reset(void) {
    MP_STATE_VM(storage_mapped_files) = MP_OBJ_NULL;
    // A held workflow write may ask for a reload after the VM has already stopped. Drop
    // that request: left pending, it makes the BLE workflow drop its replies.
    MP_STATE_THREAD(mp_pending_exception) = MP_OBJ_NULL;
    #if CIRCUITPY_STORAGE_MAP_IMPORT
    common_hal_mcu_disable_interrupts();        // the USB task may be checking the list
    MP_STATE_VM(storage_mapped_imports) = NULL;
    common_hal_mcu_enable_interrupts();
    #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
    usb_msc_finish_deferred_write();            // the code it would have overwritten is gone
    #endif
    #endif
}

static bool mapped_files_contain(DWORD sclust) {
    mp_obj_list_t *mapped = MP_OBJ_TO_PTR(MP_STATE_VM(storage_mapped_files));
    for (size_t i = 0; i < mapped->len; i++) {
        if (MP_OBJ_SMALL_INT_VALUE(mapped->items[i]) == (mp_int_t)sclust) {
            return true;
        }
    }
    return false;
}

static void mapped_files_add(DWORD sclust) {
    if (MP_STATE_VM(storage_mapped_files) == MP_OBJ_NULL) {
        MP_STATE_VM(storage_mapped_files) = mp_obj_new_list(0, NULL);
    }
    if (!mapped_files_contain(sclust)) {
        mp_obj_list_append(MP_STATE_VM(storage_mapped_files), MP_OBJ_NEW_SMALL_INT(sclust));
    }
}

// Not inlined into its raising wrapper: the supervisor file writers call it too.
MP_NOINLINE bool storage_map_file_in_use(fs_user_mount_t *vfs, const char *path) {
    // A bare fs_user_mount_t (e.g. an SD card mount) shares the supervisor_vfs_t
    // prefix, so the cast is safe. If CIRCUITPY is littlefs, the FAT mount can
    // never be the CIRCUITPY mount, so no mapped file can match either.
    if (MP_STATE_VM(storage_mapped_files) == MP_OBJ_NULL ||
        (supervisor_vfs_t *)vfs != filesystem_circuitpy()) {
        return false;
    }
    FIL fp;
    if (f_open(&vfs->fatfs, &fp, path, FA_READ) != FR_OK) {
        return false;                           // no such file yet
    }
    DWORD sclust = fp.obj.sclust;
    f_close(&fp);
    return mapped_files_contain(sclust);
}

void storage_map_file_check_writable(fs_user_mount_t *vfs, const char *path) {
    if (storage_map_file_in_use(vfs, path)) {
        mp_raise_OSError(MP_EACCES);            // mapped: its flash bytes are in use
    }
}
#endif

#if CIRCUITPY_STORAGE_MAP_FILE
int storage_map_runs(mp_obj_t file_in, storage_map_run_fn_t fn, void *ctx) {
    if (!storage_map_file_enabled) {
        return MP_EOPNOTSUPP;
    }
    if (!mp_obj_is_type(file_in, &mp_type_vfs_fat_fileio)) {
        return MP_EOPNOTSUPP;                   // only a FAT volume stores a file as flash bytes
    }
    pyb_file_obj_t *file = MP_OBJ_TO_PTR(file_in);
    FATFS *fatfs = file->fp.obj.fs;
    if (fatfs == NULL || (file->fp.flag & FA_WRITE)) {
        return MP_EINVAL;                       // closed, or not open for reading only
    }
    supervisor_vfs_t *fs_mount = filesystem_circuitpy();
    // A littlefs CIRCUITPY or another mount (an SD card) doesn't store files as mapped flash bytes.
    if (fs_mount == NULL || fs_mount->common.base.type != &mp_fat_vfs_type ||
        fatfs != &fs_mount->fat.fatfs) {
        return MP_EOPNOTSUPP;
    }
    DWORD *tbl = file->fp.cltbl;
    if (tbl == NULL) {
        return MP_ENOMEM;                       // open() could not allocate the cluster map
    }
    FSIZE_t left = f_size(&file->fp);
    if (left == 0) {
        return 0;
    }
    supervisor_flash_flush();                   // write back any RAM sector cache
    // Walk the cluster runs of the link map open() built and split a run wherever the port's
    // mapping is not contiguous, e.g. at the seam of a drive that spans two flash partitions.
    size_t nruns = (size_t)((tbl[0] - 2) / 2);
    for (size_t i = 0; i < nruns && left > 0; i++) {
        DWORD sector = fatfs->database + (tbl[2 + 2 * i] - 2) * fatfs->csize;
        FSIZE_t span = (FSIZE_t)tbl[1 + 2 * i] * fatfs->csize * FF_MIN_SS;
        if (span > left) {
            span = left;                        // the last run is clipped to the dir-entry size
        }
        while (span > 0) {
            uint32_t contiguous;
            const uint8_t *addr = supervisor_flash_xip_address(sector, &contiguous);
            if (addr == NULL) {
                return MP_EOPNOTSUPP;           // this build cannot map the drive here
            }
            FSIZE_t piece = (FSIZE_t)contiguous * FF_MIN_SS;
            if (piece > span) {
                piece = span;
            }
            fn(ctx, addr, (size_t)piece, sector);
            span -= piece;
            left -= piece;
            sector += contiguous;
        }
    }
    return left != 0 ? MP_EIO : 0;              // chain shorter than the dir-entry size: corrupt
}

static void append_view(void *views, const uint8_t *addr, size_t len, uint32_t block) {
    // read-only: a stray write raises instead of silently hitting the flash window
    mp_obj_list_append(MP_OBJ_FROM_PTR(views), mp_obj_new_memoryview('B', len, (void *)addr));
}
#endif

mp_obj_t common_hal_storage_map_file(mp_obj_t file_in) {
    #if CIRCUITPY_STORAGE_MAP_FILE
    mp_obj_list_t *views = MP_OBJ_TO_PTR(mp_obj_new_list(0, NULL));
    int err = storage_map_runs(file_in, append_view, views);
    if (err == MP_ENOMEM) {
        mp_raise_type(&mp_type_MemoryError);
    }
    if (err != 0) {
        mp_raise_OSError(err);
    }
    if (views->len == 0) {
        return mp_const_empty_tuple;            // an empty file has no clusters to protect
    }
    pyb_file_obj_t *file = MP_OBJ_TO_PTR(file_in);
    mapped_files_add(file->fp.obj.sclust);
    return mp_obj_new_tuple(views->len, views->items);
    #else
    (void)file_in;
    mp_raise_type(&mp_type_NotImplementedError);    // this port does not map its drive
    #endif
}

#if CIRCUITPY_STORAGE_MAP_IMPORT
#ifndef CIRCUITPY_STORAGE_MAP_GUARD_BLOCKS
#define CIRCUITPY_STORAGE_MAP_GUARD_BLOCKS (1)
#endif

// Reads a .mpy straight from its flash runs. Data inside one run is used in place; the
// loader copies anything that spans two runs.
typedef struct {
    const uint8_t *addr;
    size_t len;
    uint32_t block;             // first drive block, for the USB write guard
} map_run_t;

typedef struct _map_reader_t {
    struct _map_reader_t *next_reader;  // the import before this one
    const uint8_t *cur;
    const uint8_t *end;         // end of the current run
    size_t next_run;            // index of the run after the current one
    size_t run_count;
    map_run_t runs[];
} map_reader_t;

static void count_run(void *ctx, const uint8_t *addr, size_t len, uint32_t block) {
    (*(size_t *)ctx)++;
}

static void add_run(void *ctx, const uint8_t *addr, size_t len, uint32_t block) {
    map_reader_t *r = ctx;
    r->runs[r->run_count] = (map_run_t) {addr, len, block};
    r->run_count++;
}

// Steps to the next run when the current one is used up. False at the end of the file.
static bool map_reader_fill(map_reader_t *r) {
    while (r->cur == r->end) {
        if (r->next_run == r->run_count) {
            return false;
        }
        r->cur = r->runs[r->next_run].addr;
        r->end = r->cur + r->runs[r->next_run].len;
        r->next_run++;
    }
    return true;
}

mp_uint_t storage_map_reader_readbyte(void *data) {
    map_reader_t *r = data;
    if (!map_reader_fill(r)) {
        return MP_READER_EOF;
    }
    return *r->cur++;
}

const uint8_t *storage_map_reader_try_read_rom(void *data, size_t len) {
    map_reader_t *r = data;
    map_reader_fill(r);
    // A str in flash needs its NUL after it, so data may fill a run exactly only at the end
    // of the file, where no str ends.
    size_t avail = r->end - r->cur;
    if (len > avail || (len == avail && r->next_run != r->run_count)) {
        return NULL;
    }
    const uint8_t *addr = r->cur;
    r->cur += len;
    return addr;
}

bool storage_map_import_uses_blocks(uint32_t lba, uint32_t count) {
    // A port that erases a whole sector while the VM can run rewrites the neighbours too.
    const uint32_t sector = CIRCUITPY_STORAGE_MAP_GUARD_BLOCKS;
    uint32_t end = (lba + count + sector - 1) / sector * sector;
    lba -= lba % sector;
    for (map_reader_t *r = MP_STATE_VM(storage_mapped_imports); r != NULL; r = r->next_reader) {
        for (size_t j = 0; j < r->run_count; j++) {
            const map_run_t *run = &r->runs[j];
            if (run->block < end && lba < run->block + (run->len + FF_MIN_SS - 1) / FF_MIN_SS) {
                return true;
            }
        }
    }
    return false;
}

static void map_reader_close(void *data) {
    // Kept in storage_mapped_imports until reload.
}

bool storage_map_file_reader(mp_reader_t *reader, qstr filename) {
    // The REPL can't be stopped while it waits for input, so a host write could not be held
    // off until its imports are gone. Load them into RAM instead.
    if (autoreload_is_suspended(AUTORELOAD_SUSPEND_REPL)) {
        return false;
    }
    mp_obj_t args[2] = {
        MP_OBJ_NEW_QSTR(filename),
        MP_OBJ_NEW_QSTR(MP_QSTR_rb),
    };
    mp_obj_t file = mp_vfs_open(MP_ARRAY_SIZE(args), &args[0], (mp_map_t *)&mp_const_empty_map);
    map_reader_t *r = NULL;
    size_t n = 0;
    if (storage_map_runs(file, count_run, &n) == 0 && n > 0) {
        r = m_new_obj_var(map_reader_t, runs, map_run_t, n);
        r->cur = r->end = NULL;
        r->next_run = r->run_count = 0;
        storage_map_runs(file, add_run, r);
        mapped_files_add(((pyb_file_obj_t *)MP_OBJ_TO_PTR(file))->fp.obj.sclust);
        r->next_reader = MP_STATE_VM(storage_mapped_imports);
        MP_STATE_VM(storage_mapped_imports) = r;
    }
    mp_stream_close(file);
    if (r == NULL) {
        return false;
    }
    reader->data = r;
    reader->readbyte = storage_map_reader_readbyte;
    reader->close = map_reader_close;
    return true;
}
#endif
