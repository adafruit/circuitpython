// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Support for a watchdog that was started by the board's bootloader, before
// CircuitPython's first instruction, and that cannot be stopped.
//
// This is not the `watchdog` module: that one owns the peripheral and can
// configure it. Here the WDT is already running and its configuration
// registers (CRV, RREN, CONFIG) are locked, so the only thing the application
// can do is reload it.
//
// A board opts in with CIRCUITPY_BOOTLOADER_ARMED_WDT in its mpconfigboard.h,
// and should normally also set CIRCUITPY_WATCHDOG = 0, since
// common_hal_watchdog_set_mode() would call nrfx_wdt_init() on the running
// peripheral, where the timeout write is silently ignored.

// mpconfigboard.h arrives via mpconfigport.h. Included here rather than left to
// the caller so that the feed can never be silently compiled out by an include
// order that omitted it.
#include "py/mpconfig.h"

#include "nrfx.h"

#ifndef CIRCUITPY_BOOTLOADER_ARMED_WDT
#define CIRCUITPY_BOOTLOADER_ARMED_WDT (0)
#endif

// Whether entering safe mode after a watchdog reset requires USB to be
// connected (port_init(), supervisor/port.c).
#ifndef CIRCUITPY_SAFE_MODE_ON_WATCHDOG_REQUIRES_USB
#define CIRCUITPY_SAFE_MODE_ON_WATCHDOG_REQUIRES_USB (!CIRCUITPY_BOOTLOADER_ARMED_WDT)
#endif

// Value that a reload request register must be written with, per the nRF52
// product specification.
#define NRF_WDT_RELOAD_REQUEST_VALUE (0x6E524635UL)

// Reload the bootloader's watchdog.
//
// Call this from the main loop, never from an interrupt handler. Feeding from
// an ISR would keep a wedged main loop "alive" indefinitely.
static inline void bootloader_wdt_feed(void) {
    #if CIRCUITPY_BOOTLOADER_ARMED_WDT
    for (size_t channel = 0; channel < 8; channel++) {
        NRF_WDT->RR[channel] = NRF_WDT_RELOAD_REQUEST_VALUE;
    }
    #endif
}
