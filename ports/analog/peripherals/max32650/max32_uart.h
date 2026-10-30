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

// UART register compatibility macro
#define UART_F_STATUS_TX_BUSY       MXC_F_UART_STAT_TX_BUSY

int pinsToUart(const mcu_pin_obj_t *rx, const mcu_pin_obj_t *tx);

// UART wrappers for portability
int uart_init(mxc_uart_regs_t *uart, unsigned int baud);
int uart_set_flow_ctrl(mxc_uart_regs_t *uart, bool enable, int rtsThreshold);
