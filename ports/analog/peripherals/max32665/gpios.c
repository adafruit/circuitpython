// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#include "gpios.h"

volatile mxc_gpio_regs_t *gpio_ports[NUM_GPIO_PORTS] =
{MXC_GPIO0, MXC_GPIO1};

int32_t gpio_init(uint32_t port) {
    return MXC_GPIO_Init(1 << port);
}
