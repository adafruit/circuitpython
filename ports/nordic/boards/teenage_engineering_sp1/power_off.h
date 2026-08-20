// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// A supervisor-level power-off gesture, for a board whose only "off" is
// SYSTEM_OFF.
//
// A board opts in by defining BOARD_POWER_OFF_BUTTON_PIN in mpconfigboard.h to
// an active-low, switch-to-ground button that is also the wake source. Holding
// it for BOARD_POWER_OFF_HOLD_SECONDS then powers the board down; pressing it
// again wakes the chip through a reset, which on a board with a bootloader
// means the bootloader runs first.
//
// A board may also define BOARD_POWER_OFF_CONFIRM_LED_PIN to an active-high
// LED, which is flashed once (BOARD_POWER_OFF_CONFIRM_LED_MS, default 200) as
// soon as the hold completes. Without it the gesture is silent, which on a
// screen-less board leaves no way to tell a successful power-off from a hold
// that was a moment too short.
//
// The board does not have to configure that pin. If its input buffer is found
// disconnected the poll reconnects it with a pull-up. A board is still free to
// configure it, and anything already configured is left alone.


#include "py/mpconfig.h"

#ifdef BOARD_POWER_OFF_BUTTON_PIN

// Poll the button and, if it has been held long enough, power off (never
// returns). Called from the board's board_background_task(), i.e. from every
// RUN_BACKGROUND_TASKS.
void power_off_tick(void);

// Put the board's own hardware into its off state: rails down, resets
// asserted, anything that would drain a battery through SYSTEM_OFF switched
// off.
void board_power_off_prepare(void);

#endif
