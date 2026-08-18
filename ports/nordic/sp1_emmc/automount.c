// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "sp1_emmc/automount.h"

#if defined(SP1_EMMC_AUTOMOUNT) && SP1_EMMC_AUTOMOUNT

#include "py/mpstate.h"

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"

#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/settings.h"


static mp_vfs_mount_t _emmc_vfs;
static fs_user_mount_t _emmc_usermount;

static bool _tried;

void sp1emmc_automount(void) {
    if (_tried) {
        return;
    }
    _tried = true;

    if (get_safe_mode() != SAFE_MODE_NONE) {
        return;
    }

    bool enabled = true;
    (void)settings_get_bool("CIRCUITPY_EMMC_USB", &enabled);
    if (!enabled) {
        return;
    }

    mp_obj_t dev = sp1emmc_automount_construct(true, true);
    if (dev == MP_OBJ_NULL) {
        return;
    }

    fs_user_mount_t *vfs = &_emmc_usermount;
    vfs->base.type = &mp_fat_vfs_type;
    vfs->fatfs.drv = vfs;
    // Initialise underlying block device.
    vfs->blockdev.block_size = FF_MIN_SS;
    mp_vfs_blockdev_init(&vfs->blockdev, dev);

    if (f_mount(&vfs->fatfs) != FR_OK) {
        return;
    }

    // Same as CIRCUITPY: while a host has the drive, the host owns writing.
    filesystem_set_concurrent_write_protection(vfs, true);
    filesystem_set_writable_by_usb(vfs, true);

    mp_vfs_mount_t *emmc_vfs = &_emmc_vfs;
    emmc_vfs->str = SP1_EMMC_AUTOMOUNT_PATH;
    emmc_vfs->len = sizeof(SP1_EMMC_AUTOMOUNT_PATH) - 1;
    emmc_vfs->obj = MP_OBJ_FROM_PTR(&_emmc_usermount);
    emmc_vfs->next = MP_STATE_VM(vfs_mount_table);
    MP_STATE_VM(vfs_mount_table) = emmc_vfs;
}

#endif // SP1_EMMC_AUTOMOUNT
