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
    // Bytes not yet handed to the driver by serial_cb(), and a semaphore it
    // gives when they are all sent.
    const uint8_t *tx_data;
    size_t tx_len;
    struct k_sem tx_done;
    byte *receiver_buffer;
    const mcu_pin_obj_t *tx;
    const mcu_pin_obj_t *rx;
    const mcu_pin_obj_t *rts;
    const mcu_pin_obj_t *cts;

    bool rx_paused;     // set by irq if no space in rbuf
    // Set when the sender waits for unread data (USB CDC), so RX can
    // pause on a full msgq instead of dropping bytes.
    bool flow_control;
    // Raise KeyboardInterrupt on mp_interrupt_char.
    bool sigint_enabled;

    // True when the underlying Zephyr device was dynamically routed to the
    // pins above at construction time. Such objects own their receiver
    // buffer and deinitialize the device and release their pins.
    bool dynamic;
} busio_uart_obj_t;

// Helper function for Zephyr-specific initialization from device tree
mp_obj_t common_hal_busio_uart_construct_from_device(busio_uart_obj_t *self, const struct device *uart_device, uint16_t receiver_buffer_size, byte *receiver_buffer, bool sigint_enabled);

// Internal helper for clearing buffer
void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self);

// Write all of data with uart_poll_out(). Used by the console and usb_cdc
// instead of common_hal_busio_uart_write(), which stops early on a pending
// Ctrl-C or reload and would cut console output short. With the host port
// closed, the CDC ACM driver drops this output instead of blocking, as on
// TinyUSB ports.
void zephyr_uart_write_blocking(busio_uart_obj_t *self, const uint8_t *data, size_t len);

// Zephyr-port-specific write-timeout accessors, used by usb_cdc/Serial.c.
mp_float_t common_hal_busio_uart_get_write_timeout(busio_uart_obj_t *self);
void common_hal_busio_uart_set_write_timeout(busio_uart_obj_t *self, mp_float_t write_timeout);
