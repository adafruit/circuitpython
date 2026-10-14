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

void board_init(void) {
    power_on();
}
