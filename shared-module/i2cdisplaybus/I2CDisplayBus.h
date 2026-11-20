// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/lifetime.h"

#include "common-hal/busio/I2C.h"
#include "common-hal/digitalio/DigitalInOut.h"

typedef struct {
    mp_obj_base_t base;
    displayio_bus_lifetime_t bus_lifetime;
    busio_i2c_obj_t *bus;
    busio_i2c_obj_t inline_bus;
    digitalio_digitalinout_obj_t reset;
    uint16_t address;
} i2cdisplaybus_i2cdisplaybus_obj_t;

mp_obj_t i2cdisplaybus_i2cdisplaybus_start_vm(i2cdisplaybus_i2cdisplaybus_obj_t *self);
