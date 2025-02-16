// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/zephyr_serial/UART.h"
#include "py/ringbuf.h"

#include <zephyr/device.h>

extern const mp_obj_type_t zephyr_serial_uart_type;

// Construct an underlying UART object.
extern void zephyr_serial_uart_construct(zephyr_serial_uart_obj_t *self,
    const struct device *const uart_device, uint16_t receiver_buffer_size, byte *receiver_buffer);

extern void zephyr_serial_uart_deinit(zephyr_serial_uart_obj_t *self);
extern bool zephyr_serial_uart_deinited(zephyr_serial_uart_obj_t *self);

// Read characters. len is in characters NOT bytes!
extern size_t zephyr_serial_uart_read(zephyr_serial_uart_obj_t *self,
    uint8_t *data, size_t len, int *errcode);

// Write characters. len is in characters NOT bytes!
extern size_t zephyr_serial_uart_write(zephyr_serial_uart_obj_t *self,
    const uint8_t *data, size_t len, int *errcode);

extern uint32_t zephyr_serial_uart_get_baudrate(zephyr_serial_uart_obj_t *self);
extern void zephyr_serial_uart_set_baudrate(zephyr_serial_uart_obj_t *self, uint32_t baudrate);
extern mp_float_t zephyr_serial_uart_get_timeout(zephyr_serial_uart_obj_t *self);
extern void zephyr_serial_uart_set_timeout(zephyr_serial_uart_obj_t *self, mp_float_t timeout);

extern uint32_t zephyr_serial_uart_rx_characters_available(zephyr_serial_uart_obj_t *self);
extern void zephyr_serial_uart_clear_rx_buffer(zephyr_serial_uart_obj_t *self);
extern bool zephyr_serial_uart_ready_to_tx(zephyr_serial_uart_obj_t *self);

extern void zephyr_serial_uart_never_reset(zephyr_serial_uart_obj_t *self);
