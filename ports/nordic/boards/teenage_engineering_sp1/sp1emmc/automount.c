// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "sp1emmc/automount.h"

#if defined(SP1EMMC_AUTOMOUNT) && SP1EMMC_AUTOMOUNT

#include "py/mpstate.h"

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"

#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/settings.h"

#include "sp1emmc/sp1emmc.h"


static mp_vfs_mount_t _emmc_vfs;
static fs_user_mount_t _emmc_usermount;

static bool _tried;
static sp1emmc_automount_status_t _status = SP1EMMC_AUTOMOUNT_NOT_TRIED;

#define AUTOMOUNT_BUDGET_US  5000000u

// One word of RAM that survives a reset but not a power cycle. If it is still
// set when we get here, the previous boot faulted. Skip the card for this
// boot so the board enumerates, and clear the crumb so the next boot tries again.
#define AUTOMOUNT_CRUMB_MAGIC  0x5350314du   // 'SP1M'

static struct {
    uint32_t magic;
    uint32_t in_progress;
} _crumb __attribute__((section(".uninitialized")));

static void automount_give_up(sp1emmc_automount_status_t status) {
    emmc_deadline_clear();
    // Leave the card powered down and the pins released
    sp1emmc_automount_abandon();
    _crumb.in_progress = 0;
    _status = status;
}

void sp1emmc_automount(void) {
    if (_tried) {
        return;
    }
    _tried = true;

    if (get_safe_mode() != SAFE_MODE_NONE) {
        _status = SP1EMMC_AUTOMOUNT_SAFE_MODE;
        return;
    }

    bool enabled = true;
    (void)settings_get_bool("CIRCUITPY_EMMC_USB", &enabled);
    if (!enabled) {
        _status = SP1EMMC_AUTOMOUNT_DISABLED;
        return;
    }

    if (_crumb.magic == AUTOMOUNT_CRUMB_MAGIC && _crumb.in_progress != 0) {
        _crumb.in_progress = 0;
        _status = SP1EMMC_AUTOMOUNT_SKIPPED_AFTER_FAULT;
        return;
    }
    _crumb.magic = AUTOMOUNT_CRUMB_MAGIC;
    _crumb.in_progress = 1;

    emmc_deadline_set(AUTOMOUNT_BUDGET_US);

    mp_obj_t dev = sp1emmc_automount_construct(true, true);
    if (dev == MP_OBJ_NULL) {
        automount_give_up(SP1EMMC_AUTOMOUNT_NO_CARD);
        return;
    }

    fs_user_mount_t *vfs = &_emmc_usermount;
    vfs->base.type = &mp_fat_vfs_type;
    vfs->fatfs.drv = vfs;
    // Initialise underlying block device.
    vfs->blockdev.block_size = FF_MIN_SS;
    mp_vfs_blockdev_init(&vfs->blockdev, dev);

    if (f_mount(&vfs->fatfs) != FR_OK) {
        automount_give_up(SP1EMMC_AUTOMOUNT_NO_FILESYSTEM);
        return;
    }

    // Same as CIRCUITPY: while a host has the drive, the host owns writing.
    filesystem_set_concurrent_write_protection(vfs, true);
    filesystem_set_writable_by_usb(vfs, true);

    mp_vfs_mount_t *emmc_vfs = &_emmc_vfs;
    emmc_vfs->str = SP1EMMC_AUTOMOUNT_PATH;
    emmc_vfs->len = sizeof(SP1EMMC_AUTOMOUNT_PATH) - 1;
    emmc_vfs->obj = MP_OBJ_FROM_PTR(&_emmc_usermount);
    emmc_vfs->next = MP_STATE_VM(vfs_mount_table);
    MP_STATE_VM(vfs_mount_table) = emmc_vfs;

    // The budget covers bring-up and the mount only
    emmc_deadline_clear();
    _crumb.in_progress = 0;
    _status = SP1EMMC_AUTOMOUNT_OK;
}

sp1emmc_automount_status_t sp1emmc_automount_get_status(void) {
    return _status;
}

#endif // SP1EMMC_AUTOMOUNT
