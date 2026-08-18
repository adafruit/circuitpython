// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "supervisor/shared/safe_mode.h"

// Returns true if the user initiates safe mode in a board specific way.
// Also add BOARD_USER_SAFE_MODE in mpconfigboard.h to explain the board specific
// way.
bool board_requests_safe_mode(void);

// Called from port_init() before any peripheral is initialized, for boards
// whose bootloader hands over live peripherals and enabled interrupts. Nothing
// is available yet: no heap, no filesystem, no serial. Keep it to register
// writes.
void board_early_init(void);

// Initializes board related state once on start up.
void board_init(void);

// Re-apply the board's quiescent pin configuration.
void board_reset_pin_defaults(void);

// Reset the state of off MCU components such as neopixels.
void reset_board(void);

// Deinit the board. This should put the board in deep sleep durable, low power
// state. It should not prevent the user access method from working (such as
// disabling USB, BLE or flash) because CircuitPython may continue to run.
void board_deinit(void);
