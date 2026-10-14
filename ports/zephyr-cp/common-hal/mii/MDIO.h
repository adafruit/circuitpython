// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
} mii_mdio_obj_t;

// Zephyr pulls its MDIO wiring from the devicetree; constructing the bus is
// not supported and raises NotImplementedError.
void common_hal_mii_mdio_construct(mii_mdio_obj_t *self,
    const mcu_pin_obj_t *mdc, const mcu_pin_obj_t *mdio);
