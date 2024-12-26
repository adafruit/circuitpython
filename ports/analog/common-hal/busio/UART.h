// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_EFR32_COMMON_HAL_BUSIO_UART_H
#define MICROPY_INCLUDED_EFR32_COMMON_HAL_BUSIO_UART_H

#include "common-hal/microcontroller/Pin.h"
#include "peripherals/periph.h"
#include "py/obj.h"
#include "py/ringbuf.h"

#include "uart.h"

// Define a struct for what BUSIO.UART should contain
typedef struct {
    mp_obj_base_t base;
    mxc_uart_regs_t* uart_regs;
    const mcu_pin_obj_t *rx;
    const mcu_pin_obj_t *tx;
    uint32_t baudrate;
    bool parity;
    uint8_t stop_bits;
    uint8_t bits;
} busio_uart_obj_t;

void uart_reset(void);

#endif  // MICROPY_INCLUDED_EFR32_COMMON_HAL_BUSIO_UART_H
