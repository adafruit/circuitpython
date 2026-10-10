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
} mii_rmii_obj_t;

// Zephyr pulls its RMII wiring from the devicetree; constructing the bus is
// not supported and raises NotImplementedError.
void common_hal_mii_rmii_construct(mii_rmii_obj_t *self,
    const mcu_pin_obj_t *mdc, const mcu_pin_obj_t *mdio,
    const mcu_pin_obj_t *rxd0, const mcu_pin_obj_t *rxd1, const mcu_pin_obj_t *rx_dv,
    const mcu_pin_obj_t *txd0, const mcu_pin_obj_t *txd1, const mcu_pin_obj_t *tx_en,
    const mcu_pin_obj_t *refclk, const mcu_pin_obj_t *reset);
