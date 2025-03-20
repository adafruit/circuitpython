// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/__init__.h"
#include "bindings/zephyr_serial/UART.h"

#include "shared/runtime/interrupt_char.h"
#include "py/mpconfig.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"

#include <stdatomic.h>
#include <string.h>

#include <zephyr/drivers/uart.h>

/*
 * Read characters from UART until line end is detected. Afterwards push the
 * data to the message queue.
 */
static void serial_cb(const struct device *dev, void *user_data) {
    zephyr_serial_uart_obj_t *self = (zephyr_serial_uart_obj_t *)user_data;

    uint8_t c;

    if (!uart_irq_update(dev)) {
        return;
    }

    if (!uart_irq_rx_ready(dev)) {
        return;
    }

    /* read until FIFO empty */
    while (uart_fifo_read(dev, &c, 1) == 1) {
        if (mp_interrupt_char == c) {
            zephyr_serial_uart_clear_rx_buffer(self);
            mp_sched_keyboard_interrupt();
        } else if (!self->rx_paused) {
            if (k_msgq_put(&self->msgq, &c, K_NO_WAIT) != 0) {
                self->rx_paused = true;
            }
        }
    }
}

void zephyr_serial_uart_never_reset(zephyr_serial_uart_obj_t *self) {
}


void zephyr_serial_uart_construct(zephyr_serial_uart_obj_t *self, const struct device *const uart_device, uint16_t receiver_buffer_size, byte *receiver_buffer) {
    self->uart_device = uart_device;
    int ret = uart_irq_callback_user_data_set(uart_device, serial_cb, self);


    k_msgq_init(&self->msgq, receiver_buffer, 1, receiver_buffer_size);

    if (ret < 0) {
        if (ret == -ENOTSUP) {
            printk("Interrupt-driven UART API support not enabled\n");
        } else if (ret == -ENOSYS) {
            printk("UART device does not support interrupt-driven API\n");
        } else {
            printk("Error setting UART callback: %d\n", ret);
        }
        return;
    }
    self->timeout = K_USEC(100);
    uart_irq_rx_enable(uart_device);
}

bool zephyr_serial_uart_deinited(zephyr_serial_uart_obj_t *self) {
    return !device_is_ready(self->uart_device);
}

void zephyr_serial_uart_deinit(zephyr_serial_uart_obj_t *self) {
}

// Read characters.
size_t zephyr_serial_uart_read(zephyr_serial_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    size_t count = 0;
    while (count < len && k_msgq_get(&self->msgq, data + count, self->timeout) == 0) {
        count++;
    }
    if (count > 0) {
        self->rx_paused = false;
    }

    return count;
}

// Write characters.
size_t zephyr_serial_uart_write(zephyr_serial_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    for (int i = 0; i < len; i++) {
        uart_poll_out(self->uart_device, data[i]);
    }

    return len;
}

uint32_t zephyr_serial_uart_get_baudrate(zephyr_serial_uart_obj_t *self) {
    return 0;
}

void zephyr_serial_uart_set_baudrate(zephyr_serial_uart_obj_t *self, uint32_t baudrate) {
}

mp_float_t zephyr_serial_uart_get_timeout(zephyr_serial_uart_obj_t *self) {
    return 0;
}

void zephyr_serial_uart_set_timeout(zephyr_serial_uart_obj_t *self, mp_float_t timeout) {
}

uint32_t zephyr_serial_uart_rx_characters_available(zephyr_serial_uart_obj_t *self) {
    return k_msgq_num_used_get(&self->msgq);
}

void zephyr_serial_uart_clear_rx_buffer(zephyr_serial_uart_obj_t *self) {
    k_msgq_purge(&self->msgq);
}

bool zephyr_serial_uart_ready_to_tx(zephyr_serial_uart_obj_t *self) {
    return true;
}
