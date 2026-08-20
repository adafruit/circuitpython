// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Support for the watchdog that this board's bootloader starts, before
// CircuitPython's first instruction, and that cannot be stopped.
//
// This is not the `watchdog` module: that one owns the peripheral and can
// configure it. Here the WDT is already running and its configuration
// registers (CRV, RREN, CONFIG) are locked, so the only thing the application
// can do is reload it.
//
// The board opts in with CIRCUITPY_BOOTLOADER_ARMED_WDT in its
// mpconfigboard.h. That flag, and the safe-mode policy derived from it,
// default to off for every other board in the port's mpconfigport.h, which is
// what keeps the generic feed sites (supervisor/port.c,
// peripherals/nrf/nvm.c) compiling without this header.

// mpconfigboard.h arrives via mpconfigport.h. Included here rather than left to
// the caller so that the feed can never be silently compiled out by an include
// order that omitted it.
#include "py/mpconfig.h"

#include "nrfx.h"

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
