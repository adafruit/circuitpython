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

void automount_emmc(void);

bool emmcio_is_automounted(void);

#else

#include <stdbool.h>

static inline bool emmcio_is_automounted(void) {
    return false;
}

#endif
