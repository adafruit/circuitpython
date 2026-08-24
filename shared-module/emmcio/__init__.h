// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#if CIRCUITPY_EMMC_USB

#ifndef CIRCUITPY_EMMC_MOUNT_PATH
#define CIRCUITPY_EMMC_MOUNT_PATH "/sd"
#endif

typedef enum {
    EMMCIO_AUTOMOUNT_NOT_TRIED = 0,
    EMMCIO_AUTOMOUNT_OK,
    EMMCIO_AUTOMOUNT_DISABLED,             // CIRCUITPY_EMMC_USB = 0
    EMMCIO_AUTOMOUNT_SAFE_MODE,
    EMMCIO_AUTOMOUNT_NO_CARD,              // bring-up failed or timed out
    EMMCIO_AUTOMOUNT_NO_FILESYSTEM,        // card came up, f_mount refused it
    EMMCIO_AUTOMOUNT_SKIPPED_AFTER_FAULT,  // last boot died in here
} emmcio_automount_status_t;

void automount_emmc(void);

bool emmcio_is_automounted(void);

emmcio_automount_status_t emmcio_automount_get_status(void);

#else

#include <stdbool.h>

static inline bool emmcio_is_automounted(void) {
    return false;
}

#endif
