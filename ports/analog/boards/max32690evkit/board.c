// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "supervisor/port.h"
#include "mpconfigboard.h"
#include "max32_port.h"

// Board-level setup for MAX32690
mxc_gpio_regs_t *gpio_ports[NUM_GPIO_PORTS] =
{MXC_GPIO0, MXC_GPIO1, MXC_GPIO2, MXC_GPIO3, MXC_GPIO4};

// clang-format off
const mxc_gpio_cfg_t pb_pin[] = {
    { MXC_GPIO4, MXC_GPIO_PIN_0, MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH, MXC_GPIO_DRVSTR_0},
};
const int num_pbs = (sizeof(pb_pin) / sizeof(mxc_gpio_cfg_t));

const mxc_gpio_cfg_t led_pin[] = {
    { MXC_GPIO0, MXC_GPIO_PIN_14, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
    { MXC_GPIO2, MXC_GPIO_PIN_12, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
};
const int num_leds = (sizeof(led_pin) / sizeof(mxc_gpio_cfg_t));
// clang-format on

// DEFAULT:  Using the weak-defined supervisor/shared/board.c functions

/***** OPTIONAL BOARD-SPECIFIC FUNCTIONS from supervisor/board.h *****/
// Returns true if the user initiates safe mode in a board specific way.
// Also add BOARD_USER_SAFE_MODE in mpconfigboard.h to explain the board specific
// way.
// bool board_requests_safe_mode(void);

volatile uint32_t system_ticks = 0;

void SysTick_Handler(void) {
    system_ticks++;
}

uint32_t board_millis(void) {
    return system_ticks;
}

// Initializes board related state once on start up.
void board_init(void) {
}

// Reset the state of off MCU components such as neopixels.
// void reset_board(void);

// Deinit the board. This should put the board in deep sleep durable, low power
// state. It should not prevent the user access method from working (such as
// disabling USB, BLE or flash) because CircuitPython may continue to run.
// void board_deinit(void);

/*******************************************************************/
