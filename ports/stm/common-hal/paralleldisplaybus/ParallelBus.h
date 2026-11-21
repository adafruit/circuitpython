// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vegard Wærp
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/digitalio/DigitalInOut.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *data_pins[8];
    const mcu_pin_obj_t *write;
    const mcu_pin_obj_t *read;
    digitalio_digitalinout_obj_t command;
    digitalio_digitalinout_obj_t chip_select;
    digitalio_digitalinout_obj_t reset;
} paralleldisplaybus_parallelbus_obj_t;
