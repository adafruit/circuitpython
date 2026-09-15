// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/emmcio/__init__.h"

#if CIRCUITPY_EMMC_USB

#include "py/mpstate.h"

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"

#include "supervisor/filesystem.h"
#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/settings.h"

#include "shared-bindings/microcontroller/Pin.h"

#include "common-hal/emmcio/EMMC.h"

#if !defined(DEFAULT_EMMC_CLOCK) || !defined(DEFAULT_EMMC_COMMAND) || !defined(DEFAULT_EMMC_DATA)
#error "CIRCUITPY_EMMC_USB needs DEFAULT_EMMC_CLOCK, DEFAULT_EMMC_COMMAND and DEFAULT_EMMC_DATA in mpconfigboard.h"
#endif

// RST_n and the I/O rail gate are optional: a board that hard-wires either one
// simply does not define it.
#ifndef DEFAULT_EMMC_RESET
#define DEFAULT_EMMC_RESET NULL
#endif
#ifndef DEFAULT_EMMC_VCCQ
#define DEFAULT_EMMC_VCCQ NULL
#endif


static mp_vfs_mount_t _emmc_vfs;
static fs_user_mount_t _emmc_usermount;

static bool _tried;

#define AUTOMOUNT_BUDGET_US  5000000u

static void automount_give_up(void) {
    common_hal_emmcio_emmc_clear_deadline();
    // Leave the card powered down and the pins released
    emmcio_automount_abandon();
}

void automount_emmc(void) {
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

    common_hal_emmcio_emmc_set_deadline(AUTOMOUNT_BUDGET_US);

    mp_obj_t dev = emmcio_automount_construct(DEFAULT_EMMC_CLOCK, DEFAULT_EMMC_COMMAND,
        DEFAULT_EMMC_DATA, DEFAULT_EMMC_RESET, DEFAULT_EMMC_VCCQ, true, true);
    if (dev == MP_OBJ_NULL) {
        automount_give_up();
        return;
    }

    fs_user_mount_t *vfs = &_emmc_usermount;
    vfs->base.type = &mp_fat_vfs_type;
    vfs->fatfs.drv = vfs;
    // Initialise underlying block device.
    vfs->blockdev.block_size = FF_MIN_SS;
    mp_vfs_blockdev_init(&vfs->blockdev, dev);

    if (f_mount(&vfs->fatfs) != FR_OK) {
        automount_give_up();
        return;
    }

    // Same as CIRCUITPY: while a host has the drive, the host owns writing.
    filesystem_set_concurrent_write_protection(vfs, true);
    filesystem_set_writable_by_usb(vfs, true);

    mp_vfs_mount_t *emmc_vfs = &_emmc_vfs;
    emmc_vfs->str = CIRCUITPY_EMMC_MOUNT_PATH;
    emmc_vfs->len = sizeof(CIRCUITPY_EMMC_MOUNT_PATH) - 1;
    emmc_vfs->obj = MP_OBJ_FROM_PTR(&_emmc_usermount);
    emmc_vfs->next = MP_STATE_VM(vfs_mount_table);
    MP_STATE_VM(vfs_mount_table) = emmc_vfs;

    // The budget covers bring-up and the mount only
    common_hal_emmcio_emmc_clear_deadline();
}

#endif // CIRCUITPY_EMMC_USB
