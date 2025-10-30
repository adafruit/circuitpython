// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "supervisor/filesystem.h"
#include "supervisor/usb.h"
#include "supervisor/shared/stack.h"

#include "max32_port.h"

// From boards/$(BOARD)/board.c
extern const mxc_gpio_cfg_t pb_pin[];
extern const int num_pbs;
extern const mxc_gpio_cfg_t led_pin[];
extern const int num_leds;

/** NOTE: ALL "ticks" refer to a 1/1024 s period */
static int status_led_ticks = 0;

// This function is where port-specific background
// tasks should be performed
// Execute port specific actions during background tick. Only if ticks are enabled.
void port_background_tick(void) {
    status_led_ticks++;

    // Set an LED approx. 1/s
    if (status_led_ticks > 1024) {
        MXC_GPIO_OutToggle(led_pin[2].port, led_pin[2].mask);
        status_led_ticks = 0;
    }
}

// Execute port specific actions during background tasks. This is before the
// background callback system and happens *very* often. Use
// port_background_tick() when possible.
void port_background_task(void) {
}

// Take port specific actions at the beginning and end of background ticks.
// This is used e.g., to set a monitoring pin for debug purposes.  "Actual
// work" should be done in port_background_tick() instead.
void port_start_background_tick(void) {
}
void port_finish_background_tick(void) {
}
