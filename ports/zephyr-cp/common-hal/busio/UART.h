// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"

#include <zephyr/kernel.h>

typedef struct {
    mp_obj_base_t base;

    const struct device *uart_device;
    struct k_msgq msgq;

    k_timeout_t timeout;
    k_timeout_t write_timeout;

    bool rx_paused;     // set by irq if no space in rbuf

    // True when the underlying Zephyr device was dynamically routed to the
    // pins below at construction time. Such objects own their receiver
    // buffer and deinitialize the device and release their pins.
    bool dynamic;
    byte *receiver_buffer;
    const mcu_pin_obj_t *tx;
    const mcu_pin_obj_t *rx;
    const mcu_pin_obj_t *rts;
    const mcu_pin_obj_t *cts;
} busio_uart_obj_t;

// Helper function for Zephyr-specific initialization from device tree
mp_obj_t common_hal_busio_uart_construct_from_device(busio_uart_obj_t *self, const struct device *uart_device, uint16_t receiver_buffer_size, byte *receiver_buffer);

// Internal helper for clearing buffer
void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self);

// Zephyr-port-specific write-timeout accessors, used by usb_cdc/Serial.c.
mp_float_t common_hal_busio_uart_get_write_timeout(busio_uart_obj_t *self);
void common_hal_busio_uart_set_write_timeout(busio_uart_obj_t *self, mp_float_t write_timeout);
