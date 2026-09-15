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

#include "shared/timeutils/timeutils.h"
#include "supervisor/fatfs.h"
#include "supervisor/flash.h"
#include "supervisor/linker.h"

#if CIRCUITPY_FILESYSTEM_LITTLEFS
#include "extmod/vfs_lfs.h"
#include "lib/littlefs/lfs2.h"
#include "supervisor/port_heap.h"
#include "supervisor/shared/tick.h"
#endif

#if CIRCUITPY_SDCARDIO
#include "shared-module/sdcardio/__init__.h"
#endif

#if CIRCUITPY_EMMC_USB
#include "shared-module/emmcio/__init__.h"
#endif

static mp_vfs_mount_t _circuitpy_vfs;
static supervisor_vfs_t _circuitpy_mount;

#if CIRCUITPY_FILESYSTEM_LITTLEFS
static bool _lfs_freshly_formatted;
#endif

#if CIRCUITPY_SAVES_PARTITION_SIZE > 0
static mp_vfs_mount_t _saves_vfs;
static fs_user_mount_t _saves_usermount;
#endif

static volatile uint32_t filesystem_flush_interval_ms = CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS;
volatile bool filesystem_flush_requested = false;

// Mapping between FatFS and littlefs results and the supervisor filesystem
// error codes.
static supervisor_fs_err_t fat_error(FRESULT res) {
    switch (res) {
        case FR_OK:
            return SUPERVISOR_FS_OK;
        case FR_NO_FILE:
            return SUPERVISOR_FS_NO_FILE;
        case FR_NO_PATH:
        case FR_INVALID_NAME:
            return SUPERVISOR_FS_NO_PATH;
        case FR_EXIST:
            return SUPERVISOR_FS_EXIST;
        case FR_WRITE_PROTECTED:
            return SUPERVISOR_FS_WRITE_PROTECTED;
        case FR_DENIED:
            // FatFS uses FR_DENIED for read-only media and for no free
            // cluster. We can't tell them apart, so report no space.
            return SUPERVISOR_FS_NO_SPACE;
        default:
            return SUPERVISOR_FS_IO;
    }
}

// Convert nanoseconds past 1970/1/1 to the FAT timestamp format.
static DWORD fattime_from_ns(uint64_t ns) {
    timeutils_struct_time_t tm;
    uint64_t seconds = timeutils_seconds_since_epoch_from_nanoseconds_since_1970(ns);
    timeutils_seconds_since_epoch_to_struct_time(seconds, &tm);
    return ((tm.tm_year - 1980) << 25) | (tm.tm_mon << 21) | (tm.tm_mday << 16) |
           (tm.tm_hour << 11) | (tm.tm_min << 5) | (tm.tm_sec >> 1);
}

#if CIRCUITPY_FILESYSTEM_LITTLEFS
static supervisor_fs_err_t lfs_error(int err) {
    switch (err) {
        case LFS2_ERR_OK:
            return SUPERVISOR_FS_OK;
        case LFS2_ERR_NOENT:
            return SUPERVISOR_FS_NO_FILE;
        case LFS2_ERR_EXIST:
            return SUPERVISOR_FS_EXIST;
        case LFS2_ERR_NOTDIR:
        case LFS2_ERR_ISDIR:
            return SUPERVISOR_FS_NO_PATH;
        case LFS2_ERR_NOSPC:
            return SUPERVISOR_FS_NO_SPACE;
        default:
            return SUPERVISOR_FS_IO;
    }
}

// Store nanoseconds past the littlefs mtime epoch (1970/1/1) little-endian.
static void lfs_mtime_store(uint8_t buf[8], uint64_t ns) {
    ns = timeutils_nanoseconds_since_epoch_to_nanoseconds_since_1970(ns);
    for (size_t i = 0; i < 8; ++i) {
        buf[i] = ns;
        ns >>= 8;
    }
}

static uint64_t lfs_mtime_load(const uint8_t buf[8]) {
    uint64_t ns = 0;
    for (size_t i = 8; i > 0; --i) {
        ns = ns << 8 | buf[i - 1];
    }
    return timeutils_nanoseconds_since_epoch_to_nanoseconds_since_1970(ns);
}

// Read a file or directory modification time from the littlefs mtime attribute.
static uint64_t lfs_get_path_mtime(lfs2_t *lfs, const char *path) {
    uint8_t mtime[8];
    lfs2_ssize_t sz = lfs2_getattr(lfs, path, LFS_ATTR_MTIME, mtime, sizeof(mtime));
    if (sz != (lfs2_ssize_t)sizeof(mtime)) {
        return 0;
    }
    return lfs_mtime_load(mtime);
}

static bool is_lfs(supervisor_vfs_t *vfs) {
    return vfs->common.base.type == &mp_type_vfs_lfs2;
}
#endif

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
static bool make_empty_file(supervisor_vfs_t *vfs, const char *path) {
    supervisor_vfs_file_t fp;
    supervisor_fs_err_t err = supervisor_vfs_open_file(vfs, path,
        SUPERVISOR_FS_OPEN_WRITE | SUPERVISOR_FS_OPEN_CREATE | SUPERVISOR_FS_OPEN_TRUNCATE, 0, &fp);
    if (err != SUPERVISOR_FS_OK) {
        return false;
    }
    return supervisor_vfs_close_file(&fp) == SUPERVISOR_FS_OK;
}

#if CIRCUITPY_FULL_BUILD
#define MAKE_FILE_WITH_OPTIONAL_CONTENTS(vfs, filename, string_literal) do { \
        const byte buffer[] = string_literal; \
        make_file_with_contents(vfs, filename, buffer, sizeof(buffer) - 1); \
} while (0)

static bool make_file_with_contents(supervisor_vfs_t *vfs, const char *filename, const byte *content, UINT size) {
    supervisor_vfs_file_t fp;
    if (supervisor_vfs_open_file(vfs, filename,
        SUPERVISOR_FS_OPEN_WRITE | SUPERVISOR_FS_OPEN_CREATE | SUPERVISOR_FS_OPEN_TRUNCATE, 0, &fp) != SUPERVISOR_FS_OK) {
        return false;
    }
    size_t written;
    supervisor_vfs_write_file(&fp, content, size, &written);
    return supervisor_vfs_close_file(&fp) == SUPERVISOR_FS_OK && written == size;
}
#else
#define MAKE_FILE_WITH_OPTIONAL_CONTENTS(vfs, filename, string_literal) \
    (void)make_empty_file(vfs, filename)
#endif

#if CIRCUITPY_FILESYSTEM_LITTLEFS

// littlefs block-device adapter for the supervisor flash block device. The
// block size is FILESYSTEM_BLOCK_SIZE so lfs2 blocks map 1:1 onto supervisor
// flash blocks. Reads/writes that are not block-aligned are served through a
// read-modify-write because the supervisor block device is block granular.

static int lfs_flash_read(const struct lfs2_config *c, lfs2_block_t block, lfs2_off_t off, void *buffer, lfs2_size_t size) {
    if (off == 0 && size == FILESYSTEM_BLOCK_SIZE) {
        return supervisor_flash_read_blocks(buffer, block, 1) == 0 ? LFS2_ERR_OK : LFS2_ERR_IO;
    }
    uint8_t tmp[FILESYSTEM_BLOCK_SIZE];
    if (supervisor_flash_read_blocks(tmp, block, 1) != 0) {
        return LFS2_ERR_IO;
    }
    memcpy(buffer, tmp + off, size);
    return LFS2_ERR_OK;
}

static int lfs_flash_prog(const struct lfs2_config *c, lfs2_block_t block, lfs2_off_t off, const void *buffer, lfs2_size_t size) {
    supervisor_flash_mark_dirty();
    if (off == 0 && size == FILESYSTEM_BLOCK_SIZE) {
        return supervisor_flash_write_blocks(buffer, block, 1) == 0 ? LFS2_ERR_OK : LFS2_ERR_IO;
    }
    uint8_t tmp[FILESYSTEM_BLOCK_SIZE];
    if (supervisor_flash_read_blocks(tmp, block, 1) != 0) {
        return LFS2_ERR_IO;
    }
    memcpy(tmp + off, buffer, size);
    if (supervisor_flash_write_blocks(tmp, block, 1) != 0) {
        return LFS2_ERR_IO;
    }
    return LFS2_ERR_OK;
}

// The supervisor block device erases as part of its write path, so there is
// nothing to do here.
static int lfs_flash_erase(const struct lfs2_config *c, lfs2_block_t block) {
    return LFS2_ERR_OK;
}

static int lfs_flash_sync(const struct lfs2_config *c) {
    supervisor_flash_flush();
    return LFS2_ERR_OK;
}

static void *lfs_supervisor_alloc(size_t size) {
    return port_malloc(size, false);
}

static bool lfs_write_boot_file(const char *path, const char *contents, size_t size) {
    supervisor_vfs_file_t file;
    if (supervisor_vfs_open_file(&_circuitpy_mount, path,
        SUPERVISOR_FS_OPEN_WRITE | SUPERVISOR_FS_OPEN_CREATE | SUPERVISOR_FS_OPEN_TRUNCATE, 0, &file) != SUPERVISOR_FS_OK) {
        return false;
    }
    size_t written = 0;
    supervisor_vfs_write_file(&file, contents, size, &written);
    supervisor_fs_err_t err = supervisor_vfs_close_file(&file);
    return written == size && err == SUPERVISOR_FS_OK;
}

#endif  // CIRCUITPY_FILESYSTEM_LITTLEFS

// we don't make this function static because it needs a lot of stack and we
// want it to be executed without using stack within main() function
bool filesystem_init(bool create_allowed, bool force_create) {
    mp_vfs_mount_t *circuitpy_vfs = &_circuitpy_vfs;
    circuitpy_vfs->len = 0;

    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    // init the vfs object. The VM (and its GC) is not running yet, so
    // everything here uses static storage or the port allocator.
    mp_obj_vfs_lfs2_t *circuitpy = &_circuitpy_mount.lfs2;
    memset(circuitpy, 0, sizeof(*circuitpy));
    circuitpy->base.type = &mp_type_vfs_lfs2;
    // cur_dir is used to join relative paths (make_path). It cannot be
    // allocated on the GC heap because the VM is not running yet, so give it
    // a static buffer. It is truncated back to its previous length after each
    // join, so it stays short in practice.
    static char lfs_cur_dir_buf[128];
    vstr_init_fixed_buf(&circuitpy->cur_dir, sizeof(lfs_cur_dir_buf), lfs_cur_dir_buf);
    vstr_add_str(&circuitpy->cur_dir, "/");
    circuitpy->enable_mtime = true;

    struct lfs2_config *config = &circuitpy->config;
    memset(config, 0, sizeof(*config));
    config->context = circuitpy;
    config->read = lfs_flash_read;
    config->prog = lfs_flash_prog;
    config->erase = lfs_flash_erase;
    config->sync = lfs_flash_sync;
    config->block_size = supervisor_flash_get_block_size();
    // Initialize the block device (opens the partition) before asking for its
    // geometry; the FAT path does the same via the blockdev INIT ioctl.
    supervisor_flash_init();
    config->block_count = supervisor_flash_get_block_count();
    config->read_size = config->block_size;
    config->prog_size = config->block_size;

    _lfs_freshly_formatted = false;
    if (force_create) {
        // Reformat requested (e.g. from safe mode).
        if (lfs2_format(&circuitpy->lfs, config) != LFS2_ERR_OK) {
            return false;
        }
        _lfs_freshly_formatted = true;
    }
    int mount_err;
    mp_obj_t mounted = mp_vfs_lfs2_mount_supervisor(circuitpy, lfs_supervisor_alloc,
        create_allowed && !force_create, &_lfs_freshly_formatted, &mount_err);
    if (mounted == MP_OBJ_NULL) {
        return false;
    }
    (void)mount_err;

    if (_lfs_freshly_formatted) {
        #if CIRCUITPY_SDCARDIO
        lfs2_mkdir(&circuitpy->lfs, "/sd");
        #endif

        #if CIRCUITPY_SETTINGS_TOML
        // settings.toml is not read yet on littlefs filesystems; see
        // supervisor/shared/settings.c. Create it empty for forward
        // compatibility.
        lfs_write_boot_file("/settings.toml", "", 0);
        #endif
        // make a sample code.py file
        lfs_write_boot_file("/code.py", "print(\"Hello World!\")\n", sizeof("print(\"Hello World!\")\n") - 1);

        // create empty lib directory
        lfs2_mkdir(&circuitpy->lfs, "/lib");

        // and ensure everything is flushed
        supervisor_flash_flush();
    }
    #else
    // init the vfs object
    fs_user_mount_t *circuitpy = &_circuitpy_mount.fat;
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
        make_empty_file(&_circuitpy_mount, "/.fseventsd/no_log");
        make_empty_file(&_circuitpy_mount, "/.metadata_never_index");

        // Prevent storing trash on all OSes.
        make_empty_file(&_circuitpy_mount, "/.Trashes"); // MacOS
        make_empty_file(&_circuitpy_mount, "/.Trash-1000"); // Linux, XDG trash spec:
        // https://specifications.freedesktop.org/trash-spec/trashspec-latest.html
        #endif

        #if CIRCUITPY_SDCARDIO || CIRCUITPY_SDIOIO
        res = f_mkdir(&circuitpy->fatfs, "/sd");
        #if CIRCUITPY_FULL_BUILD
        MAKE_FILE_WITH_OPTIONAL_CONTENTS(&_circuitpy_mount, "/sd/placeholder.txt",
            "SD cards mounted at /sd will hide this file from Python.\n");
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
            MAKE_FILE_WITH_OPTIONAL_CONTENTS(&_circuitpy_mount, "/saves/placeholder.txt",
                "A separate filesystem mounted at /saves will hide this file from Python."
                " Saves are visible via USB CPSAVES.\n");
        }
        #endif
        #endif

        #if CIRCUITPY_SETTINGS_TOML
        make_empty_file(&_circuitpy_mount, "/settings.toml");
        #endif
        // make a sample code.py file
        MAKE_FILE_WITH_OPTIONAL_CONTENTS(&_circuitpy_mount, "/code.py", "print(\"Hello World!\")\n");

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
    #endif  // CIRCUITPY_FILESYSTEM_LITTLEFS

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
    #if defined(DEFAULT_SD_CARD_DETECT) && CIRCUITPY_SDCARD_USB
    // Mount the SD card now so it's ready when USB enumerates.
    // Lazy mount from tud_msc_test_unit_ready_cb can lose races with
    // macOS's probe timing. Gated on CIRCUITPY_SDCARD_USB to match the
    // existing call site in usb_msc_flash.c (guarded by SDCARD_LUN).
    // automount_sd_card() itself honors the runtime CIRCUITPY_SDCARD_USB
    // setting and is a no-op when it is disabled.
    automount_sd_card();
    #endif
    #endif

    // Same reason as the SD card above, mount it before USB enumerates rather than
    // lazily from tud_msc_test_unit_ready_cb(). Also the same requirement
    // that settings.toml is readable.
    #if CIRCUITPY_EMMC_USB
    automount_emmc();
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
    filesystem_set_writable_by_usb(&_circuitpy_mount, writable);
}

void filesystem_set_writable_by_usb(supervisor_vfs_t *vfs, bool usb_writable) {
    if (usb_writable) {
        vfs->common.blockdev.flags |= MP_BLOCKDEV_FLAG_USB_WRITABLE;
    } else {
        vfs->common.blockdev.flags &= ~MP_BLOCKDEV_FLAG_USB_WRITABLE;
    }
}

bool filesystem_is_writable_by_python(supervisor_vfs_t *vfs) {
    return ((vfs->common.blockdev.flags & MP_BLOCKDEV_FLAG_CONCURRENT_WRITE_PROTECTED) == 0) ||
           ((vfs->common.blockdev.flags & MP_BLOCKDEV_FLAG_USB_WRITABLE) == 0) ||
           ((vfs->common.blockdev.flags & MP_BLOCKDEV_FLAG_IGNORE_WRITE_PROTECTION) != 0);
}

bool filesystem_is_writable_by_usb(fs_user_mount_t *vfs) {
    return ((vfs->blockdev.flags & MP_BLOCKDEV_FLAG_CONCURRENT_WRITE_PROTECTED) == 0) ||
           ((vfs->blockdev.flags & MP_BLOCKDEV_FLAG_USB_WRITABLE) != 0) ||
           ((vfs->blockdev.flags & MP_BLOCKDEV_FLAG_IGNORE_WRITE_PROTECTION) != 0);
}

void filesystem_set_internal_concurrent_write_protection(bool concurrent_write_protection) {
    filesystem_set_concurrent_write_protection(&_circuitpy_mount, concurrent_write_protection);
}

void filesystem_set_concurrent_write_protection(supervisor_vfs_t *vfs, bool concurrent_write_protection) {
    if (concurrent_write_protection) {
        vfs->common.blockdev.flags |= MP_BLOCKDEV_FLAG_CONCURRENT_WRITE_PROTECTED;
    } else {
        vfs->common.blockdev.flags &= ~MP_BLOCKDEV_FLAG_CONCURRENT_WRITE_PROTECTED;
    }
}

void filesystem_set_ignore_write_protection(fs_user_mount_t *vfs, bool ignore_write_protection) {
    if (ignore_write_protection) {
        vfs->blockdev.flags |= MP_BLOCKDEV_FLAG_IGNORE_WRITE_PROTECTION;
    } else {
        vfs->blockdev.flags &= ~MP_BLOCKDEV_FLAG_IGNORE_WRITE_PROTECTION;
    }
}

bool filesystem_present(void) {
    return _circuitpy_vfs.len > 0;
}

supervisor_vfs_t *filesystem_circuitpy(void) {
    if (!filesystem_present()) {
        return NULL;
    }
    return &_circuitpy_mount;
}

supervisor_vfs_t *filesystem_for_path(const char *path_in, const char **path_under_mount) {
    mp_vfs_mount_t *vfs = mp_vfs_lookup_path(path_in, path_under_mount);
    if (vfs == MP_VFS_NONE) {
        return NULL;
    }
    supervisor_vfs_t *fs_mount;
    *path_under_mount = path_in;
    if (vfs == MP_VFS_ROOT) {
        fs_mount = filesystem_circuitpy();
    } else {
        fs_mount = (supervisor_vfs_t *)MP_OBJ_TO_PTR(vfs->obj);
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

static bool filesystem_native_fatfs(supervisor_vfs_t *fs_mount) {
    return fs_mount->common.base.type == &mp_fat_vfs_type &&
           (fs_mount->common.blockdev.flags & MP_BLOCKDEV_FLAG_NATIVE) != 0;
}

bool filesystem_lock(supervisor_vfs_t *fs_mount) {
    if (fs_mount->common.base.type != &mp_fat_vfs_type) {
        // littlefs mounts are never exposed through USB MSC, so there is no
        // STA_PROTECT to bypass and no lock_count to track. The blockdev lock
        // alone excludes other writers.
        return blockdev_lock(fs_mount);
    }
    if (fs_mount->fat.lock_count == 0 && !blockdev_lock(fs_mount)) {
        return false;
    }
    fs_mount->fat.lock_count += 1;
    // CIRCUITPY-CHANGE: while a non-USB-MSC writer (BLE file transfer, web
    // workflow, storage.remount) holds the filesystem lock, allow the
    // FatFS f_open(FA_WRITE) path to bypass STA_PROTECT. Without this, the
    // disk_ioctl(IOCTL_STATUS) -> filesystem_is_writable_by_python() check
    // added by #10659 always sets STA_PROTECT on USB-device-capable boards,
    // so even after the lock is held, f_open returns FR_WRITE_PROTECTED.
    // USB MSC takes the lock via blockdev_lock() directly, NOT via
    // filesystem_lock(), so this flag is never set on its behalf.
    fs_mount->common.blockdev.flags |= MP_BLOCKDEV_FLAG_IGNORE_WRITE_PROTECTION;
    return true;
}

void filesystem_unlock(supervisor_vfs_t *fs_mount) {
    if (fs_mount->common.base.type != &mp_fat_vfs_type) {
        blockdev_unlock(fs_mount);
        return;
    }
    fs_mount->fat.lock_count -= 1;
    if (fs_mount->fat.lock_count == 0) {
        // CIRCUITPY-CHANGE: clear the bypass when releasing the lock.
        fs_mount->common.blockdev.flags &= ~MP_BLOCKDEV_FLAG_IGNORE_WRITE_PROTECTION;
        blockdev_unlock(fs_mount);
    }
}

bool blockdev_lock(supervisor_vfs_t *fs_mount) {
    if ((fs_mount->common.blockdev.flags & MP_BLOCKDEV_FLAG_LOCKED) != 0) {
        return false;
    }
    fs_mount->common.blockdev.flags |= MP_BLOCKDEV_FLAG_LOCKED;
    return true;
}

void blockdev_unlock(supervisor_vfs_t *fs_mount) {
    fs_mount->common.blockdev.flags &= ~MP_BLOCKDEV_FLAG_LOCKED;
}

// Supervisor-level file, directory and filesystem helpers shared between FAT
// and littlefs mounts. All of them take or return supervisor_fs_err_t; paths
// are relative to the given mount. See supervisor/filesystem.h for details.

bool supervisor_vfs_supported(supervisor_vfs_t *vfs) {
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (is_lfs(vfs)) {
        return true;
    }
    #endif
    return filesystem_native_fatfs(vfs);
}

supervisor_fs_err_t supervisor_vfs_open_file(supervisor_vfs_t *vfs, const char *path, uint32_t flags,
    uint64_t mtime_ns, supervisor_vfs_file_t *file) {
    if (vfs == NULL) {
        return SUPERVISOR_FS_NO_FILE;
    }
    memset(file, 0, sizeof(*file));
    file->vfs = vfs;
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (is_lfs(vfs)) {
        struct lfs2_file_config *cfg = &file->file.lfs.cfg;
        memset(cfg, 0, sizeof(*cfg));
        cfg->buffer = file->file.lfs.buffer;
        // Modification time attribute, committed when a modified file is
        // closed. littlefs writes file attributes at close.
        if (mtime_ns == 0) {
            // No timestamp was requested. Use the same RTC-based time that FAT
            // stamps files with via get_fattime().
            mtime_ns = get_fattime_ns();
        }
        lfs_mtime_store(file->file.lfs.mtime, mtime_ns);
        struct lfs2_attr *attr = &file->file.lfs.attr;
        attr->type = LFS_ATTR_MTIME;
        attr->buffer = file->file.lfs.mtime;
        attr->size = sizeof(file->file.lfs.mtime);
        cfg->attrs = attr;
        cfg->attr_count = 1;
        int lfs_flags;
        if ((flags & SUPERVISOR_FS_OPEN_WRITE) != 0) {
            lfs_flags = LFS2_O_WRONLY;
        } else {
            lfs_flags = LFS2_O_RDONLY;
        }
        if ((flags & SUPERVISOR_FS_OPEN_CREATE) != 0) {
            lfs_flags |= LFS2_O_CREAT;
        }
        if ((flags & SUPERVISOR_FS_OPEN_TRUNCATE) != 0) {
            lfs_flags |= LFS2_O_TRUNC;
        }
        int err = lfs2_file_opencfg(&vfs->lfs2.lfs, &file->file.lfs.lfs2, path, lfs_flags, cfg);
        if (err < 0) {
            return lfs_error(err);
        }
        file->open = true;
        file->lfs = true;
        return SUPERVISOR_FS_OK;
    }
    #endif
    if (!filesystem_native_fatfs(vfs)) {
        return SUPERVISOR_FS_IO;
    }
    DWORD fattime = mtime_ns != 0 ? fattime_from_ns(mtime_ns) : 0;
    if (fattime != 0) {
        // FatFS stamps files when they're closed, so remember the timestamp
        // and apply it again in supervisor_vfs_close_file().
        override_fattime(fattime);
    }
    BYTE fa = 0;
    if ((flags & SUPERVISOR_FS_OPEN_READ) != 0) {
        fa |= FA_READ;
    }
    if ((flags & SUPERVISOR_FS_OPEN_WRITE) != 0) {
        fa |= FA_WRITE;
    }
    if ((flags & SUPERVISOR_FS_OPEN_TRUNCATE) != 0) {
        fa |= FA_CREATE_ALWAYS;
    } else if ((flags & SUPERVISOR_FS_OPEN_CREATE) != 0) {
        fa |= FA_OPEN_ALWAYS;
    }
    FRESULT res = f_open(&vfs->fat.fatfs, &file->file.fat, path, fa);
    if (fattime != 0) {
        override_fattime(0);
    }
    if (res != FR_OK) {
        return fat_error(res);
    }
    file->open = true;
    file->lfs = false;
    file->fattime = fattime;
    return SUPERVISOR_FS_OK;
}

supervisor_fs_err_t supervisor_vfs_close_file(supervisor_vfs_file_t *file) {
    if (!file->open) {
        return SUPERVISOR_FS_OK;
    }
    file->open = false;
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (file->lfs) {
        // The mtime attribute filled in at open is written out here for a
        // modified file.
        int err = lfs2_file_close(&file->vfs->lfs2.lfs, &file->file.lfs.lfs2);
        return err < 0 ? lfs_error(err) : SUPERVISOR_FS_OK;
    }
    #endif
    if (file->fattime != 0) {
        override_fattime(file->fattime);
    }
    FRESULT res = f_close(&file->file.fat);
    if (file->fattime != 0) {
        override_fattime(0);
    }
    return fat_error(res);
}

supervisor_fs_err_t supervisor_vfs_read_file(supervisor_vfs_file_t *file, void *buf, size_t len, size_t *bytes_read) {
    *bytes_read = 0;
    if (!file->open) {
        return SUPERVISOR_FS_IO;
    }
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (file->lfs) {
        lfs2_ssize_t read = lfs2_file_read(&file->vfs->lfs2.lfs, &file->file.lfs.lfs2, buf, len);
        if (read < 0) {
            return lfs_error(read);
        }
        *bytes_read = read;
        return SUPERVISOR_FS_OK;
    }
    #endif
    UINT read;
    FRESULT res = f_read(&file->file.fat, buf, len, &read);
    *bytes_read = read;
    return fat_error(res);
}

supervisor_fs_err_t supervisor_vfs_write_file(supervisor_vfs_file_t *file, const void *buf, size_t len, size_t *bytes_written) {
    *bytes_written = 0;
    if (!file->open) {
        return SUPERVISOR_FS_IO;
    }
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (file->lfs) {
        lfs2_ssize_t written = lfs2_file_write(&file->vfs->lfs2.lfs, &file->file.lfs.lfs2, buf, len);
        if (written < 0) {
            return lfs_error(written);
        }
        *bytes_written = written;
        return SUPERVISOR_FS_OK;
    }
    #endif
    UINT written;
    FRESULT res = f_write(&file->file.fat, buf, len, &written);
    *bytes_written = written;
    return fat_error(res);
}

supervisor_fs_err_t supervisor_vfs_seek_file(supervisor_vfs_file_t *file, size_t offset) {
    if (!file->open) {
        return SUPERVISOR_FS_IO;
    }
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (file->lfs) {
        lfs2_soff_t result = lfs2_file_seek(&file->vfs->lfs2.lfs, &file->file.lfs.lfs2, offset, LFS2_SEEK_SET);
        return result < 0 ? lfs_error(result) : SUPERVISOR_FS_OK;
    }
    #endif
    return fat_error(f_lseek(&file->file.fat, offset));
}

size_t supervisor_vfs_tell_file(supervisor_vfs_file_t *file) {
    if (!file->open) {
        return 0;
    }
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (file->lfs) {
        return lfs2_file_tell(&file->vfs->lfs2.lfs, &file->file.lfs.lfs2);
    }
    #endif
    return f_tell(&file->file.fat);
}

size_t supervisor_vfs_file_size(supervisor_vfs_file_t *file) {
    if (!file->open) {
        return 0;
    }
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (file->lfs) {
        return lfs2_file_size(&file->vfs->lfs2.lfs, &file->file.lfs.lfs2);
    }
    #endif
    return f_size(&file->file.fat);
}

supervisor_fs_err_t supervisor_vfs_truncate_file(supervisor_vfs_file_t *file) {
    if (!file->open) {
        return SUPERVISOR_FS_IO;
    }
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (file->lfs) {
        int err = lfs2_file_truncate(&file->vfs->lfs2.lfs, &file->file.lfs.lfs2,
            lfs2_file_tell(&file->vfs->lfs2.lfs, &file->file.lfs.lfs2));
        return err < 0 ? lfs_error(err) : SUPERVISOR_FS_OK;
    }
    #endif
    return fat_error(f_truncate(&file->file.fat));
}

supervisor_fs_err_t supervisor_vfs_stat(supervisor_vfs_t *vfs, const char *path, bool *is_dir, size_t *size, uint64_t *mtime_ns) {
    if (is_dir != NULL) {
        *is_dir = false;
    }
    if (size != NULL) {
        *size = 0;
    }
    if (mtime_ns != NULL) {
        *mtime_ns = 0;
    }
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (is_lfs(vfs)) {
        struct lfs2_info info;
        int err = lfs2_stat(&vfs->lfs2.lfs, path, &info);
        if (err < 0) {
            return lfs_error(err);
        }
        bool directory = info.type == LFS2_TYPE_DIR;
        if (is_dir != NULL) {
            *is_dir = directory;
        }
        if (size != NULL && !directory) {
            *size = info.size;
        }
        if (mtime_ns != NULL) {
            *mtime_ns = lfs_get_path_mtime(&vfs->lfs2.lfs, path);
        }
        return SUPERVISOR_FS_OK;
    }
    #endif
    FILINFO file_info;
    FRESULT res = f_stat(&vfs->fat.fatfs, path, &file_info);
    if (res != FR_OK) {
        return fat_error(res);
    }
    bool directory = (file_info.fattrib & AM_DIR) != 0;
    if (is_dir != NULL) {
        *is_dir = directory;
    }
    if (size != NULL && !directory) {
        *size = file_info.fsize;
    }
    if (mtime_ns != NULL) {
        *mtime_ns = (uint64_t)timeutils_mktime_1970(1980 + (file_info.fdate >> 9),
            (file_info.fdate >> 5) & 0xf,
            file_info.fdate & 0x1f,
            file_info.ftime >> 11,
            (file_info.ftime >> 5) & 0x3f,
            (file_info.ftime & 0x1f) * 2) * 1000000000ULL;
    }
    return SUPERVISOR_FS_OK;
}

supervisor_fs_err_t supervisor_vfs_mkdir(supervisor_vfs_t *vfs, const char *path, uint64_t mtime_ns) {
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (is_lfs(vfs)) {
        int err = lfs2_mkdir(&vfs->lfs2.lfs, path);
        return err < 0 ? lfs_error(err) : SUPERVISOR_FS_OK;
    }
    #endif
    if (!filesystem_native_fatfs(vfs)) {
        return SUPERVISOR_FS_IO;
    }
    DWORD fattime = mtime_ns != 0 ? fattime_from_ns(mtime_ns) : 0;
    if (fattime != 0) {
        override_fattime(fattime);
    }
    FRESULT res = f_mkdir(&vfs->fat.fatfs, path);
    if (fattime != 0) {
        override_fattime(0);
    }
    return fat_error(res);
}

supervisor_fs_err_t supervisor_vfs_rename(supervisor_vfs_t *vfs, const char *old_path, const char *new_path) {
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (is_lfs(vfs)) {
        int err = lfs2_rename(&vfs->lfs2.lfs, old_path, new_path);
        return err < 0 ? lfs_error(err) : SUPERVISOR_FS_OK;
    }
    #endif
    if (!filesystem_native_fatfs(vfs)) {
        return SUPERVISOR_FS_IO;
    }
    return fat_error(f_rename(&vfs->fat.fatfs, old_path, new_path));
}

supervisor_fs_err_t supervisor_vfs_unlink(supervisor_vfs_t *vfs, const char *path) {
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (is_lfs(vfs)) {
        int err = lfs2_remove(&vfs->lfs2.lfs, path);
        return err < 0 ? lfs_error(err) : SUPERVISOR_FS_OK;
    }
    #endif
    if (!filesystem_native_fatfs(vfs)) {
        return SUPERVISOR_FS_IO;
    }
    return fat_error(f_unlink(&vfs->fat.fatfs, path));
}

supervisor_fs_err_t supervisor_vfs_opendir(supervisor_vfs_t *vfs, const char *path, supervisor_vfs_dir_t *dir) {
    memset(dir, 0, sizeof(*dir));
    dir->vfs = vfs;
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (is_lfs(vfs)) {
        int err = lfs2_dir_open(&vfs->lfs2.lfs, &dir->dir.lfs2, path);
        if (err < 0) {
            return lfs_error(err);
        }
        dir->open = true;
        dir->lfs = true;
        // Store the path (without a trailing slash) for per-entry mtime
        // lookups. If it doesn't fit, mtimes will simply read as 0.
        size_t len = strlen(path);
        while (len > 0 && path[len - 1] == '/') {
            len--;
        }
        if (len > sizeof(dir->path) - 1) {
            len = sizeof(dir->path) - 1;
        }
        memcpy(dir->path, path, len);
        dir->path[len] = '\0';
        return SUPERVISOR_FS_OK;
    }
    #endif
    if (!filesystem_native_fatfs(vfs)) {
        return SUPERVISOR_FS_IO;
    }
    FRESULT res = f_opendir(&vfs->fat.fatfs, &dir->dir.fat, path);
    if (res != FR_OK) {
        return fat_error(res);
    }
    dir->open = true;
    dir->lfs = false;
    return SUPERVISOR_FS_OK;
}

supervisor_fs_err_t supervisor_vfs_readdir(supervisor_vfs_dir_t *dir, char *name, size_t name_len, bool *is_dir, size_t *size, uint64_t *mtime_ns) {
    if (is_dir != NULL) {
        *is_dir = false;
    }
    if (size != NULL) {
        *size = 0;
    }
    if (mtime_ns != NULL) {
        *mtime_ns = 0;
    }
    if (!dir->open) {
        return SUPERVISOR_FS_IO;
    }
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (dir->lfs) {
        struct lfs2_info info;
        int res;
        do {
            res = lfs2_dir_read(&dir->vfs->lfs2.lfs, &dir->dir.lfs2, &info);
            if (res < 0) {
                return lfs_error(res);
            }
            if (res == 0) {
                // End of the directory.
                name[0] = '\0';
                return SUPERVISOR_FS_OK;
            }
            // littlefs lists the "." and ".." entries; FatFS doesn't, so skip
            // them for parity.
        } while (info.name[0] == '.' &&
            (info.name[1] == '\0' || (info.name[1] == '.' && info.name[2] == '\0')));
        size_t name_chars = strlen(info.name);
        if (name_chars > name_len - 1) {
            name_chars = name_len - 1;
        }
        memcpy(name, info.name, name_chars);
        name[name_chars] = '\0';
        bool directory = info.type == LFS2_TYPE_DIR;
        if (is_dir != NULL) {
            *is_dir = directory;
        }
        if (size != NULL && !directory) {
            *size = info.size;
        }
        if (mtime_ns != NULL && !directory) {
            // littlefs keeps modification times as per-path attributes, so
            // build the entry's full path. The stored directory path is
            // temporarily extended to hold the entry name.
            size_t len = strlen(dir->path);
            if (len + 1 + name_chars + 1 <= sizeof(dir->path)) {
                dir->path[len] = '/';
                memcpy(dir->path + len + 1, info.name, name_chars);
                dir->path[len + 1 + name_chars] = '\0';
                *mtime_ns = lfs_get_path_mtime(&dir->vfs->lfs2.lfs, dir->path);
                dir->path[len] = '\0';
            }
        }
        return SUPERVISOR_FS_OK;
    }
    #endif
    FILINFO file_info;
    FRESULT res = f_readdir(&dir->dir.fat, &file_info);
    if (res != FR_OK) {
        return fat_error(res);
    }
    if (file_info.fname[0] == '\0') {
        // End of the directory.
        name[0] = '\0';
        return SUPERVISOR_FS_OK;
    }
    size_t name_chars = strlen(file_info.fname);
    if (name_chars > name_len - 1) {
        name_chars = name_len - 1;
    }
    memcpy(name, file_info.fname, name_chars);
    name[name_chars] = '\0';
    bool directory = (file_info.fattrib & AM_DIR) != 0;
    if (is_dir != NULL) {
        *is_dir = directory;
    }
    if (size != NULL && !directory) {
        *size = file_info.fsize;
    }
    if (mtime_ns != NULL) {
        *mtime_ns = (uint64_t)timeutils_mktime_1970(1980 + (file_info.fdate >> 9),
            (file_info.fdate >> 5) & 0xf,
            file_info.fdate & 0x1f,
            file_info.ftime >> 11,
            (file_info.ftime >> 5) & 0x3f,
            (file_info.ftime & 0x1f) * 2) * 1000000000ULL;
    }
    return SUPERVISOR_FS_OK;
}

supervisor_fs_err_t supervisor_vfs_rewinddir(supervisor_vfs_dir_t *dir) {
    if (!dir->open) {
        return SUPERVISOR_FS_IO;
    }
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (dir->lfs) {
        int err = lfs2_dir_rewind(&dir->vfs->lfs2.lfs, &dir->dir.lfs2);
        return err < 0 ? lfs_error(err) : SUPERVISOR_FS_OK;
    }
    #endif
    f_readdir(&dir->dir.fat, NULL);
    return SUPERVISOR_FS_OK;
}

supervisor_fs_err_t supervisor_vfs_closedir(supervisor_vfs_dir_t *dir) {
    if (!dir->open) {
        return SUPERVISOR_FS_OK;
    }
    dir->open = false;
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (dir->lfs) {
        int err = lfs2_dir_close(&dir->vfs->lfs2.lfs, &dir->dir.lfs2);
        return err < 0 ? lfs_error(err) : SUPERVISOR_FS_OK;
    }
    #endif
    return fat_error(f_closedir(&dir->dir.fat));
}

supervisor_fs_err_t supervisor_vfs_statfs(supervisor_vfs_t *vfs, size_t *block_size, size_t *total_blocks, size_t *free_blocks) {
    #if CIRCUITPY_FILESYSTEM_LITTLEFS
    if (is_lfs(vfs)) {
        struct lfs2_config *config = &vfs->lfs2.config;
        if (block_size != NULL) {
            *block_size = config->block_size;
        }
        if (total_blocks != NULL) {
            *total_blocks = config->block_count;
        }
        if (free_blocks != NULL) {
            lfs2_ssize_t used = lfs2_fs_size(&vfs->lfs2.lfs);
            *free_blocks = used < 0 ? 0 : config->block_count - used;
        }
        return SUPERVISOR_FS_OK;
    }
    #endif
    if (!filesystem_native_fatfs(vfs)) {
        return SUPERVISOR_FS_IO;
    }
    FATFS *fatfs = &vfs->fat.fatfs;
    DWORD free_clusters = 0;
    FRESULT res = f_getfree(fatfs, &free_clusters);
    if (res != FR_OK) {
        return fat_error(res);
    }
    size_t ssize;
    #if FF_MAX_SS != FF_MIN_SS
    ssize = fatfs->ssize;
    #else
    ssize = FF_MIN_SS;
    #endif
    if (block_size != NULL) {
        *block_size = fatfs->csize * ssize;
    }
    if (total_blocks != NULL) {
        *total_blocks = fatfs->n_fatent - 2;
    }
    if (free_blocks != NULL) {
        *free_blocks = free_clusters;
    }
    return SUPERVISOR_FS_OK;
}
