// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#pragma once

#include "uart_regs.h"
#include "mxc_sys.h"
#include "uart.h"
#include "peripherals/pins.h"

#define NUM_UARTS 3

int pinsToUart(const mcu_pin_obj_t *rx, const mcu_pin_obj_t *tx);
