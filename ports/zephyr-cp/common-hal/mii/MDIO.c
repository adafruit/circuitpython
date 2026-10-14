// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/mii/MDIO.h"
#include "shared-bindings/mii/MDIO.h"

#include "py/runtime.h"

void common_hal_mii_mdio_construct(mii_mdio_obj_t *self,
    const mcu_pin_obj_t *mdc, const mcu_pin_obj_t *mdio) {
    (void)mdc;
    (void)mdio;
    mp_raise_NotImplementedError(NULL);
}
