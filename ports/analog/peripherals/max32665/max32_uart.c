// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#include "peripherals/pins.h"

#include "common-hal/busio/UART.h"
#include "max32_uart.h"
#include "max32665.h"

#include "py/runtime.h"
#include "py/mperrno.h"

// Assuming the use of MAP_A in MSDK, since all documentation 
// states the GPIO maps are the same

const mxc_gpio_cfg_t uart_maps[NUM_UARTS] = {
    // UART 0A
    { MXC_GPIO0, (MXC_GPIO_PIN_9 | MXC_GPIO_PIN_10), MXC_GPIO_FUNC_ALT3,
      MXC_GPIO_PAD_WEAK_PULL_UP, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
    // UART 1A
    { MXC_GPIO0, (MXC_GPIO_PIN_20 | MXC_GPIO_PIN_21), MXC_GPIO_FUNC_ALT3,
      MXC_GPIO_PAD_WEAK_PULL_UP, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
    // UART 2A
    { MXC_GPIO0, (MXC_GPIO_PIN_1 | MXC_GPIO_PIN_2), MXC_GPIO_FUNC_ALT3,
      MXC_GPIO_PAD_WEAK_PULL_UP, MXC_GPIO_VSSEL_VDDIO, MXC_GPIO_DRVSTR_0 },
};

int pinsToUart(const mcu_pin_obj_t *rx, const mcu_pin_obj_t *tx) {
    for (int i = 0; i < NUM_UARTS; i++) {
        if ((uart_maps[i].port == (MXC_GPIO_GET_GPIO(tx->port)))
            && (uart_maps[i].mask == ((tx->mask) | (rx->mask)))) {
            return i;
        }
    }
    mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_pins);
    return -1;
}
