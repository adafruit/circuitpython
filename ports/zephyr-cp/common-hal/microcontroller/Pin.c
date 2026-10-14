// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/Pin.h"

#include "py/mphal.h"

#include <iobroker/iobroker.h>

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return true;
    }
    return !iobroker_pin_in_use(pin->package_pin);
}
