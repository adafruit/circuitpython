// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/lifetime.h"

#include "common-hal/digitalio/DigitalInOut.h"
#include "bindings/rp2pio/StateMachine.h"
#include "common-hal/rp2pio/StateMachine.h"

typedef struct {
    mp_obj_base_t base;
    displayio_bus_lifetime_t bus_lifetime;
    digitalio_digitalinout_obj_t command;
    digitalio_digitalinout_obj_t chip_select;
    digitalio_digitalinout_obj_t reset;
    digitalio_digitalinout_obj_t read;
    uint8_t write;
    uint8_t data0_pin;
    rp2pio_statemachine_obj_t state_machine;
} paralleldisplaybus_parallelbus_obj_t;
