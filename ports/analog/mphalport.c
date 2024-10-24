// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc
//
// SPDX-License-Identifier: MIT

#include "mphalport.h"
#include "py/mphal.h"

// includes __enable/__disable interrupts
#include "mxc_sys.h"

#include "shared-bindings/microcontroller/__init__.h"

void mp_hal_delay_us(mp_uint_t delay) {
    common_hal_mcu_delay_us(delay);
}

void mp_hal_disable_all_interrupts(void) {
    __disable_irq();
}

void mp_hal_enable_all_interrupts(void) {
    __enable_irq();
}
