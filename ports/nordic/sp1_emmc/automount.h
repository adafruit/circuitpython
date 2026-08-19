// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#if defined(SP1_EMMC_AUTOMOUNT) && SP1_EMMC_AUTOMOUNT

#ifndef SP1_EMMC_AUTOMOUNT_PATH
#define SP1_EMMC_AUTOMOUNT_PATH "/sd"
#endif

// Why /sd is or is not there, so a boot that skipped the card can say so
// instead of leaving the user to guess. Reported by sp1emmc.automount_status().
typedef enum {
    SP1EMMC_AUTOMOUNT_NOT_TRIED = 0,
    SP1EMMC_AUTOMOUNT_OK,
    SP1EMMC_AUTOMOUNT_DISABLED,             // CIRCUITPY_EMMC_USB = 0
    SP1EMMC_AUTOMOUNT_SAFE_MODE,
    SP1EMMC_AUTOMOUNT_NO_CARD,              // bring-up failed or timed out
    SP1EMMC_AUTOMOUNT_NO_FILESYSTEM,        // card came up, f_mount refused it
    SP1EMMC_AUTOMOUNT_SKIPPED_AFTER_FAULT,  // last boot died in here
} sp1emmc_automount_status_t;

void sp1emmc_automount(void);

bool sp1emmc_is_automounted(void);

sp1emmc_automount_status_t sp1emmc_automount_get_status(void);

mp_obj_t sp1emmc_automount_construct(bool high_speed, bool write_enabled);

void sp1emmc_automount_abandon(void);

#else

#include <stdbool.h>

static inline bool sp1emmc_is_automounted(void) {
    return false;
}

#endif
