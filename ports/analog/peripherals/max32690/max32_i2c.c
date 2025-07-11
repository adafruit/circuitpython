// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#include "peripherals/pins.h"

#include "common-hal/busio/I2C.h"
#include "max32_i2c.h"
#include "max32690.h"

#include "py/runtime.h"
#include "py/mperrno.h"


/* Note: The MAX32690 assigns the same alternate function to multiple sets
     * of pins.  The drivers will enable both sets so that either can be used.
     * Users should ensure the unused set is left unconnected.
     *
     * See MAX32690 Rev A2 Errata #16:
     * https://www.analog.com/media/en/technical-documentation/data-sheets/max32690_a2_errata_rev2.pdf
     *
     * Additionally, note that the TQFN package does not expose some of the duplicate pins.  For this package,
     * enabling the un-routed GPIOs has been shown to cause initialization issues with the I2C block.
     * To work around this, "MAX32690GTK_PACKAGE_TQFN" can be defined by the build system.  The recommend place
     * to do it is in the "board.mk" file of the BSP.  This will prevent the inaccessible pins from being configured.
     */

const mxc_gpio_cfg_t i2c_maps[NUM_I2C] = {
    // I2C0
    { MXC_GPIO2, (MXC_GPIO_PIN_7 | MXC_GPIO_PIN_8), MXC_GPIO_FUNC_ALT1,
      MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
    // I2C1
    { MXC_GPIO0, (MXC_GPIO_PIN_11 | MXC_GPIO_PIN_12), MXC_GPIO_FUNC_ALT1,
      MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
    // I2C2
    { MXC_GPIO1, (MXC_GPIO_PIN_7 | MXC_GPIO_PIN_8), MXC_GPIO_FUNC_ALT3,
      MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH, MXC_GPIO_DRVSTR_0 },
};
#ifndef MAX32690GTK_PACKAGE_TQFN
const mxc_gpio_cfg_t i2c_maps_extra[NUM_I2C] = {
    // I2C0A
    { MXC_GPIO0, (MXC_GPIO_PIN_30 | MXC_GPIO_PIN_31), MXC_GPIO_FUNC_ALT1,
      MXC_GPIO_PAD_PULL_UP, MXC_GPIO_VSSEL_VDDIOH, MXC_GPIO_DRVSTR_0 },
    // I2C1A
    { MXC_GPIO2, (MXC_GPIO_PIN_17 | MXC_GPIO_PIN_18), MXC_GPIO_FUNC_ALT1,
      MXC_GPIO_PAD_PULL_UP, MXC_GPIO_VSSEL_VDDIOH, MXC_GPIO_DRVSTR_0 },
    // I2C2C
    { MXC_GPIO0, (MXC_GPIO_PIN_13 | MXC_GPIO_PIN_14), MXC_GPIO_FUNC_ALT3,
      MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
};
#endif

int pinsToI2c(const mcu_pin_obj_t *sda, const mcu_pin_obj_t *scl) {
    for (int i = 0; i < NUM_I2C; i++) {
        if ((i2c_maps[i].port == (MXC_GPIO_GET_GPIO(sda->port)))
            && (i2c_maps[i].mask == ((sda->mask) | (scl->mask)))) {
            return i;
        }
    }

    // Additional for loop to cover alternate potential I2C maps
    #ifndef MAX32690GTK_PACKAGE_TQFN
    for (int i = 0; i < NUM_I2C; i++) {
        if ((i2c_maps_extra[i].port == (MXC_GPIO_GET_GPIO(sda->port)))
            && (i2c_maps_extra[i].mask == ((sda->mask) | (scl->mask)))) {
            return i;
        }
    }
    #endif

    mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_pins);
    return -1;
}
