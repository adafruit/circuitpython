// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/ringbuf.h"

#include "hardware/uart.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t tx_pin;
    uint8_t rx_pin;
    uint8_t cts_pin;
    uint8_t rs485_dir_pin;
    bool rs485_invert;
    uint8_t rts_pin;
    uint8_t uart_id;
    uint8_t uart_irq_id;
    uint32_t baudrate;
    uint32_t timeout_ms;
    uart_inst_t *uart;
    ringbuf_t ringbuf;
} busio_uart_obj_t;

typedef struct uart_transfer_state uart_transfer_state;

uart_transfer_state *common_hal_busio_uart_start_write(busio_uart_obj_t *uart, const uint8_t *data, size_t len);
bool common_hal_busio_uart_write_isbusy(uart_transfer_state *state);

extern void reset_uart(void);
extern void never_reset_uart(uint8_t num);
