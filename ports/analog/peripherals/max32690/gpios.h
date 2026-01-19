// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/mphal.h"

// MSDK HAL includes
#include "gpio.h"
#include "gpio_regs.h"
#include "max32690.h"

int32_t gpio_init(uint32_t portmask);
