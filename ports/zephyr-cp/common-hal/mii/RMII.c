// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/mii/RMII.h"
#include "shared-bindings/mii/RMII.h"

#include "py/runtime.h"

void common_hal_mii_rmii_construct(mii_rmii_obj_t *self,
    const mcu_pin_obj_t *mdc, const mcu_pin_obj_t *mdio,
    const mcu_pin_obj_t *rxd0, const mcu_pin_obj_t *rxd1, const mcu_pin_obj_t *rx_dv,
    const mcu_pin_obj_t *txd0, const mcu_pin_obj_t *txd1, const mcu_pin_obj_t *tx_en,
    const mcu_pin_obj_t *refclk, const mcu_pin_obj_t *reset) {
    (void)mdc;
    (void)mdio;
    (void)rxd0;
    (void)rxd1;
    (void)rx_dv;
    (void)txd0;
    (void)txd1;
    (void)tx_en;
    (void)refclk;
    (void)reset;
    mp_raise_NotImplementedError(NULL);
}
