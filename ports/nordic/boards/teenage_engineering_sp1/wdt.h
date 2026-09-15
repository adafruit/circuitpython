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
// This is the device's escape hatch. With no reset pin and no way to remove the
// battery, a wedge that stops the main loop has to become a reset, because a
// reset is what runs the bootloader and re-opens the reflashing window. So the
// feed lives in board_background_task(), never in an interrupt handler, and
// mpconfigboard.mk keeps CIRCUITPY_WATCHDOG off so that user code cannot get
// at the peripheral.

#include <stddef.h>

#include "nrfx.h"

// Value that a reload request register must be written with, per the nRF52
// product specification.
#define NRF_WDT_RELOAD_REQUEST_VALUE (0x6E524635UL)

// Reload the bootloader's watchdog.
//
// Call this from the main loop, never from an interrupt handler. Feeding from
// an ISR would keep a wedged main loop "alive" indefinitely.
static inline void bootloader_wdt_feed(void) {
    for (size_t channel = 0; channel < 8; channel++) {
        NRF_WDT->RR[channel] = NRF_WDT_RELOAD_REQUEST_VALUE;
    }
}
