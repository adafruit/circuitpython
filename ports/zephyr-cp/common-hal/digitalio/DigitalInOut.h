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
    digitalio_direction_t direction;
    bool value;
    digitalio_drive_mode_t drive_mode;
    digitalio_pull_t pull;
} digitalio_digitalinout_obj_t;
