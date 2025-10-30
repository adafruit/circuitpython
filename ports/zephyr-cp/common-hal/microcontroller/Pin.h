// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mphal.h"

#include <zephyr/drivers/gpio.h>

typedef struct {
    mp_obj_base_t base;
    const struct device *port;
    gpio_pin_t number;
} mcu_pin_obj_t;

#include "autogen-pins.h"

void reset_all_pins(void);
void reset_pin(const mcu_pin_obj_t *pin);
void claim_pin(const mcu_pin_obj_t *pin);
