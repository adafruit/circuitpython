// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "lib/oofatfs/ff.h"
#include "supervisor/shared/tick.h"

// Global millisecond tick count
static inline mp_uint_t mp_hal_ticks_ms(void) {
    return supervisor_ticks_ms32();
}

void mp_hal_set_interrupt_char(int c);

void mp_hal_disable_all_interrupts(void);
void mp_hal_enable_all_interrupts(void);
