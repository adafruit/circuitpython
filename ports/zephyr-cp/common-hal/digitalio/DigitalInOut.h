// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/digitalio/Direction.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/digitalio/Pull.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    // GPIO controller device and pin number within it, resolved from the
    // pin's global number by the gpio allocate call at construct time.
    const struct device *port;
    gpio_pin_t number;
    digitalio_direction_t direction;
    bool value;
    digitalio_drive_mode_t drive_mode;
    digitalio_pull_t pull;
} digitalio_digitalinout_obj_t;
