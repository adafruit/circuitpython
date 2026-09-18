// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "supervisor/shared/board.h"
#include "mpconfigboard.h"

static void power_on(void) {
    // turn on internal battery
    nrf_gpio_cfg(POWER_SWITCH_PIN->number,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_write(POWER_SWITCH_PIN->number, true);
}

static void preserve_and_release_battery_pin(void) {
    // Preserve the battery state. The battery is enabled by default in factory bootloader.
    // Reset claimed_pins so user can control pin's state in the vm.
    // The code below doesn't actually reset the pin's state, but only set the flags.
}

void board_init(void) {
    // As of cpy 8.1.0, board_init() runs after reset_ports() on first run. That means
    // shutdown.
    // So if we need to run on battery, we must enable the battery here.
    power_on();
    preserve_and_release_battery_pin();
}

void reset_board(void) {
    preserve_and_release_battery_pin();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
