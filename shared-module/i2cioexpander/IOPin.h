// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-bindings/digitalio/Direction.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/digitalio/Pull.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-module/i2cioexpander/IOExpander.h"

typedef struct {
    mp_obj_base_t base;
    i2cioexpander_ioexpander_obj_t *expander;
    uint8_t pin_number;
    digitalio_direction_t direction;
} i2cioexpander_iopin_obj_t;

extern const mp_obj_type_t i2cioexpander_iopin_type;
