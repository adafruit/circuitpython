// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#include "peripherals/pins.h"

#include "common-hal/busio/SPI.h"
#include "max32_spi.h"
#include "max32665.h"

#include "py/runtime.h"
#include "py/mperrno.h"

// Assuming the use of MAP_A in MSDK, since all documentation 
// states the GPIO maps are the same

const mxc_gpio_cfg_t spi_maps[NUM_SPI] = {
    // SPI0A
    { MXC_GPIO1,
      (MXC_GPIO_PIN_9 | MXC_GPIO_PIN_10 | MXC_GPIO_PIN_11, MXC_GPIO_PIN_12, MXC_GPIO_PIN_13),
      MXC_GPIO_FUNC_ALT1, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
    // SPI1
    { MXC_GPIO0,
      (MXC_GPIO_PIN_17 | MXC_GPIO_PIN_18 | MXC_GPIO_PIN_19, MXC_GPIO_PIN_20, MXC_GPIO_PIN_21),
      MXC_GPIO_FUNC_ALT2, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
    // SPI2
    { MXC_GPIO0,
      (MXC_GPIO_PIN_25 | MXC_GPIO_PIN_26 | MXC_GPIO_PIN_27 | MXC_GPIO_PIN_28 | MXC_GPIO_PIN_29),
      MXC_GPIO_FUNC_ALT2, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
};


int pinsToSpi(const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso,
    const mcu_pin_obj_t *sck) {
    for (int i = 0; i < NUM_SPI; i++) {
        if ((spi_maps[i].port == (MXC_GPIO_GET_GPIO(mosi->port)))
            && (spi_maps[i].mask == ((mosi->mask) | (miso->mask) | (sck->mask)))) {
            return i;
        }
    }
    mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_pins);
    return -1;
}
