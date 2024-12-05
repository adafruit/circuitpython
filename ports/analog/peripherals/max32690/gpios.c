// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#include "gpios.h"

volatile mxc_gpio_regs_t *gpio_ports[NUM_GPIO_PORTS] =
{MXC_GPIO0, MXC_GPIO1, MXC_GPIO2, MXC_GPIO3, MXC_GPIO4};
