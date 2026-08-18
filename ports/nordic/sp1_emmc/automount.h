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

void sp1emmc_automount(void);

bool sp1emmc_is_automounted(void);

mp_obj_t sp1emmc_automount_construct(bool high_speed, bool write_enabled);

#else

static inline bool sp1emmc_is_automounted(void) {
    return false;
}

#endif
