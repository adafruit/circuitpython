// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mphal.h"
#include "py/obj.h"

#include <iobroker/iobroker.h>
#include <zephyr/drivers/gpio.h>

typedef struct {
    mp_obj_base_t base;
    // Package pin of the SoC package the pad is bonded to, resolved at
    // build time from the board's package pin map. IOBROKER_NO_PIN when the
    // pad has no entry in the map (or the SoC has no package pin map).
    package_pin_t package_pin;
} mcu_pin_obj_t;

#include "autogen-pins.h"
