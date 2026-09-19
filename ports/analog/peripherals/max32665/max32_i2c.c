// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#include "peripherals/pins.h"

#include "common-hal/busio/I2C.h"
#include "max32_i2c.h"
#include "max32665.h"

#include "py/runtime.h"
#include "py/mperrno.h"

const mxc_gpio_cfg_t i2c_maps[NUM_I2C] = {
    // I2C0
    { MXC_GPIO0, (MXC_GPIO_PIN_6 | MXC_GPIO_PIN_7), MXC_GPIO_FUNC_ALT1,
      MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
    // I2C1
    { MXC_GPIO0, (MXC_GPIO_PIN_14 | MXC_GPIO_PIN_15), MXC_GPIO_FUNC_ALT1,
      MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
    // I2C2
    { MXC_GPIO1, (MXC_GPIO_PIN_14 | MXC_GPIO_PIN_15), MXC_GPIO_FUNC_ALT1,
      MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
};

int pinsToI2c(const mcu_pin_obj_t *sda, const mcu_pin_obj_t *scl) {
    for (int i = 0; i < NUM_I2C; i++) {
        if ((i2c_maps[i].port == (MXC_GPIO_GET_GPIO(sda->port)))
            && (i2c_maps[i].mask == ((sda->mask) | (scl->mask)))) {
            return i;
        }
    }

    mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_pins);
    return -1;
}
