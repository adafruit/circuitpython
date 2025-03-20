// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/filesystem.h"

#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"

#include "py/mpstate.h"

#include "supervisor/flash.h"
#include "supervisor/linker.h"

#if CIRCUITPY_SDCARDIO
#include "shared-module/sdcardio/__init__.h"
#endif

static mp_vfs_mount_t _circuitpy_vfs;
static fs_user_mount_t _circuitpy_usermount;

#if CIRCUITPY_SAVES_PARTITION_SIZE > 0
static mp_vfs_mount_t _saves_vfs;
static fs_user_mount_t _saves_usermount;
#endif

static volatile uint32_t filesystem_flush_interval_ms = CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS;
volatile bool filesystem_flush_requested = false;

void filesystem_background(void) {
    if (filesystem_flush_requested) {
        filesystem_flush_interval_ms = CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS;
        // Flush but keep caches
        supervisor_flash_flush();
        filesystem_flush_requested = false;
    }
}

inline void filesystem_tick(void) {
    if (filesystem_flush_interval_ms == 0) {
        // 0 means not turned on.
        return;
    }
    if (filesystem_flush_interval_ms == 1) {
        filesystem_flush_requested = true;
        filesystem_flush_interval_ms = CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS;
    } else {
        filesystem_flush_interval_ms--;
    }
}


__attribute__((unused)) // this function MAY be unused
static void make_empty_file(FATFS *fatfs, const char *path) {
    FIL fp;
    f_open(fatfs, &fp, path, FA_WRITE | FA_CREATE_ALWAYS);
    f_close(&fp);
}

#if CIRCUITPY_FULL_BUILD
#define MAKE_FILE_WITH_OPTIONAL_CONTENTS(fatfs, filename, string_literal) do { \
        const byte buffer[] = string_literal; \
        make_file_with_contents(fatfs, filename, buffer, sizeof(buffer) - 1); \
} while (0)

static void make_file_with_contents(FATFS *fatfs, const char *filename, const byte *content, UINT size) {
    FIL fs;
    // Create or modify existing code.py file
    f_open(fatfs, &fs, filename, FA_WRITE | FA_CREATE_ALWAYS);
    f_write(&fs, content, size, &size);
    f_close(&fs);
}
#else
#define MAKE_FILE_WITH_OPTIONAL_CONTENTS(fatfs, filename, string_literal) \
    make_empty_file(fatfs, filename)
#endif

// we don't make this function static because it needs a lot of stack and we
// want it to be executed without using stack within main() function
bool filesystem_init(bool create_allowed, bool force_create) {
    // init the vfs object
    fs_user_mount_t *circuitpy = &_circuitpy_usermount;
    circuitpy->blockdev.flags = 0;
    supervisor_flash_init_vfs(circuitpy);

    #if CIRCUITPY_SAVES_PARTITION_SIZE > 0
    // SAVES is placed before CIRCUITPY so that CIRCUITPY takes up the remaining space.
    circuitpy->blockdev.offset = CIRCUITPY_SAVES_PARTITION_SIZE;
    circuitpy->blockdev.size = -1;

    fs_user_mount_t *saves = &_saves_usermount;
    saves->blockdev.flags = 0;
    saves->blockdev.offset = 0;
    saves->blockdev.size = CIRCUITPY_SAVES_PARTITION_SIZE;
    supervisor_flash_init_vfs(saves);
    filesystem_set_concurrent_write_protection(saves, true);
    filesystem_set_writable_by_usb(saves, false);
    #endif

    mp_vfs_mount_t *circuitpy_vfs = &_circuitpy_vfs;
    circuitpy_vfs->len = 0;

    // try to mount the flash
    FRESULT res = f_mount(&circuitpy->fatfs);
    if ((res == FR_NO_FILESYSTEM && create_allowed) || force_create) {
        // No filesystem so create a fresh one, or reformat has been requested.
        uint8_t working_buf[FF_MAX_SS];
        BYTE formats = FM_FAT;
        #if FF_FS_EXFAT
        formats |= FM_EXFAT | FM_FAT32;
        #endif
        res = f_mkfs(&circuitpy->fatfs, formats, 0, working_buf, sizeof(working_buf));
        if (res != FR_OK) {
            return false;
        }
        // Flush the new file system to make sure it's repaired immediately.
        supervisor_flash_flush();

        // set label
        #ifdef CIRCUITPY_DRIVE_LABEL
        res = f_setlabel(&circuitpy->fatfs, CIRCUITPY_DRIVE_LABEL);
        #else
        res = f_setlabel(&circuitpy->fatfs, "CIRCUITPY");
        #endif
        if (res != FR_OK) {
            return false;
        }

        #if CIRCUITPY_USB_DEVICE
        // inhibit file indexing on MacOS
        res = f_mkdir(&circuitpy->fatfs, "/.fseventsd");
        if (res != FR_OK) {
            return false;
        }
        make_empty_file(&circuitpy->fatfs, "/.fseventsd/no_log");
        make_empty_file(&circuitpy->fatfs, "/.metadata_never_index");

        // Prevent storing trash on all OSes.
        make_empty_file(&circuitpy->fatfs, "/.Trashes"); // MacOS
        make_empty_file(&circuitpy->fatfs, "/.Trash-1000"); // Linux, XDG trash spec:
        // https://specifications.freedesktop.org/trash-spec/trashspec-latest.html
        #endif

        #if CIRCUITPY_SDCARDIO || CIRCUITPY_SDIOIO
        res = f_mkdir(&circuitpy->fatfs, "/sd");
        #if CIRCUITPY_FULL_BUILD
        MAKE_FILE_WITH_OPTIONAL_CONTENTS(&circuitpy->fatfs, "/sd/placeholder.txt",
            "SD cards mounted at /sd will hide this file from Python."
            " SD cards are not visible via USB CIRCUITPY.\n");
        #endif
        #endif

        #if CIRCUITPY_SAVES_PARTITION_SIZE > 0
        res = f_mkfs(&saves->fatfs, formats, 0, working_buf, sizeof(working_buf));
        if (res == FR_OK) {
            // Flush the new file system to make sure it's repaired immediately.
            supervisor_flash_flush();
            res = f_setlabel(&saves->fatfs, "CPSAVES");
        }

        if (res == FR_OK) {
            res = f_mkdir(&circuitpy->fatfs, "/saves");
        }
        #if CIRCUITPY_FULL_BUILD
        if (res == FR_OK) {
            MAKE_FILE_WITH_OPTIONAL_CONTENTS(&circuitpy->fatfs, "/saves/placeholder.txt",
                "A separate filesystem mounted at /saves will hide this file from Python."
                " Saves are visible via USB CPSAVES.\n");
        }
        #endif
        #endif

        #if CIRCUITPY_OS_GETENV
        make_empty_file(&circuitpy->fatfs, "/settings.toml");
        #endif
        // make a sample code.py file
        MAKE_FILE_WITH_OPTIONAL_CONTENTS(&circuitpy->fatfs, "/code.py", "print(\"Hello World!\")\n");

        // create empty lib directory
        res = f_mkdir(&circuitpy->fatfs, "/lib");
        if (res != FR_OK) {
            return false;
        }

        // and ensure everything is flushed
        supervisor_flash_flush();
    } else if (res != FR_OK) {
        return false;
    }

    circuitpy_vfs->str = "/";
    circuitpy_vfs->len = 1;
    circuitpy_vfs->obj = MP_OBJ_FROM_PTR(circuitpy);
    circuitpy_vfs->next = NULL;

    MP_STATE_VM(vfs_mount_table) = circuitpy_vfs;

    #if CIRCUITPY_SAVES_PARTITION_SIZE > 0
    res = f_mount(&saves->fatfs);
    if (res == FR_OK) {
        mp_vfs_mount_t *saves_vfs = &_saves_vfs;
        saves_vfs->str = "/saves";
        saves_vfs->len = 6;
        saves_vfs->obj = MP_OBJ_FROM_PTR(&_saves_usermount);
        saves_vfs->next = MP_STATE_VM(vfs_mount_table);
        MP_STATE_VM(vfs_mount_table) = saves_vfs;
    }
    #endif

    // The current directory is used as the boot up directory.
    // It is set to the internal flash filesystem by default.
    MP_STATE_PORT(vfs_cur) = circuitpy_vfs;

    #if CIRCUITPY_STORAGE_EXTEND
    supervisor_flash_update_extended();
    #endif

    #if CIRCUITPY_SDCARDIO
    sdcardio_init();
    #endif

    return true;
}

void PLACE_IN_ITCM(filesystem_flush)(void) {
    // Reset interval before next flush.
    filesystem_flush_interval_ms = CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS;
    supervisor_flash_flush();
    // Don't keep caches because this is called when starting or stopping the VM.
    supervisor_flash_release_cache();
}

void filesystem_set_internal_writable_by_usb(bool writable) {
    fs_user_mount_t *vfs = &_circuitpy_usermount;

    filesystem_set_writable_by_usb(vfs, writable);
}

void filesystem_set_writable_by_usb(fs_user_mount_t *vfs, bool usb_writable) {
    if (usb_writable) {
        vfs->blockdev.flags |= MP_BLOCKDEV_FLAG_USB_WRITABLE;
    } else {
        vfs->blockdev.flags &= ~MP_BLOCKDEV_FLAG_USB_WRITABLE;
    }
}

bool filesystem_is_writable_by_python(fs_user_mount_t *vfs) {
    return (vfs->blockdev.flags & MP_BLOCKDEV_FLAG_CONCURRENT_WRITE_PROTECTED) == 0 ||
           (vfs->blockdev.flags & MP_BLOCKDEV_FLAG_USB_WRITABLE) == 0;
}

bool filesystem_is_writable_by_usb(fs_user_mount_t *vfs) {
    return (vfs->blockdev.flags & MP_BLOCKDEV_FLAG_CONCURRENT_WRITE_PROTECTED) == 0 ||
           (vfs->blockdev.flags & MP_BLOCKDEV_FLAG_USB_WRITABLE) != 0;
}

void filesystem_set_internal_concurrent_write_protection(bool concurrent_write_protection) {
    filesystem_set_concurrent_write_protection(&_circuitpy_usermount, concurrent_write_protection);
}

void filesystem_set_concurrent_write_protection(fs_user_mount_t *vfs, bool concurrent_write_protection) {
    if (concurrent_write_protection) {
        vfs->blockdev.flags |= MP_BLOCKDEV_FLAG_CONCURRENT_WRITE_PROTECTED;
    } else {
        vfs->blockdev.flags &= ~MP_BLOCKDEV_FLAG_CONCURRENT_WRITE_PROTECTED;
    }
}

bool filesystem_present(void) {
    return _circuitpy_vfs.len > 0;
}

fs_user_mount_t *filesystem_circuitpy(void) {
    if (!filesystem_present()) {
        return NULL;
    }
    return &_circuitpy_usermount;
}

fs_user_mount_t *filesystem_for_path(const char *path_in, const char **path_under_mount) {
    mp_vfs_mount_t *vfs = mp_vfs_lookup_path(path_in, path_under_mount);
    if (vfs == MP_VFS_NONE) {
        return NULL;
    }
    fs_user_mount_t *fs_mount;
    *path_under_mount = path_in;
    if (vfs == MP_VFS_ROOT) {
        fs_mount = filesystem_circuitpy();
    } else {
        fs_mount = MP_OBJ_TO_PTR(vfs->obj);
        // Check if the vfs name is one character long: it must be "/" in that case.
        // If so don't remove the mount point name. We must use an absolute path
        // because otherwise the path will be adjusted by os.getcwd() when it's looked up.
        if (strlen(vfs->str) != 1) {
            // Remove the mount point directory name, such as "/sd".
            *path_under_mount += strlen(vfs->str);
        }
    }
    return fs_mount;
}

bool filesystem_native_fatfs(fs_user_mount_t *fs_mount) {
    return fs_mount->base.type == &mp_fat_vfs_type && (fs_mount->blockdev.flags & MP_BLOCKDEV_FLAG_NATIVE) != 0;
}

bool filesystem_lock(fs_user_mount_t *fs_mount) {
    if (fs_mount->lock_count == 0 && !blockdev_lock(fs_mount)) {
        return false;
    }
    fs_mount->lock_count += 1;
    return true;
}

void filesystem_unlock(fs_user_mount_t *fs_mount) {
    fs_mount->lock_count -= 1;
    if (fs_mount->lock_count == 0) {
        blockdev_unlock(fs_mount);
    }
}

bool blockdev_lock(fs_user_mount_t *fs_mount) {
    if ((fs_mount->blockdev.flags & MP_BLOCKDEV_FLAG_LOCKED) != 0) {
        return false;
    }
    fs_mount->blockdev.flags |= MP_BLOCKDEV_FLAG_LOCKED;
    return true;
}

void blockdev_unlock(fs_user_mount_t *fs_mount) {
    fs_mount->blockdev.flags &= ~MP_BLOCKDEV_FLAG_LOCKED;
}
