// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include <zephyr/device.h>

#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    const struct device *dac;
    uint8_t channel_id;
    uint8_t resolution;
} analogio_analogout_obj_t;
