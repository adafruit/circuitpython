// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/busio/UART.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "shared/runtime/interrupt_char.h"
#include "py/mpconfig.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "bindings/zephyr_kernel/__init__.h"
#include "supervisor/port.h"

#include <stdatomic.h>
#include <string.h>

#include <iobroker/iobroker.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(busio_uart);

// Turn RX back on after serial_cb() paused it for a full msgq.
static void resume_rx(busio_uart_obj_t *self) {
    if (self->rx_paused) {
        self->rx_paused = false;
        uart_irq_rx_enable(self->uart_device);
    }
}

/*
 * Feed pending TX bytes to the driver, then move received characters into
 * the message queue.
 */
static void serial_cb(const struct device *dev, void *user_data) {
    busio_uart_obj_t *self = (busio_uart_obj_t *)user_data;

    uint8_t c;

    uart_irq_update(dev);

    if (self->tx_len > 0 && uart_irq_tx_ready(dev)) {
        int sent = uart_fifo_fill(dev, self->tx_data, self->tx_len);
        if (sent > 0) {
            self->tx_data += sent;
            self->tx_len -= sent;
        }
        if (self->tx_len == 0) {
            uart_irq_tx_disable(dev);
            k_sem_give(&self->tx_done);
        }
    }

    if (!uart_irq_rx_ready(dev)) {
        return;
    }

    /* read until FIFO empty, or until msgq full if flow_control */
    while (true) {
        if (self->flow_control && k_msgq_num_free_get(&self->msgq) == 0) {
            // Leave the remaining bytes in the driver instead of dropping
            // them. USB CDC then stops accepting packets, so the host
            // waits. read() calls resume_rx() after taking bytes out of
            // the msgq.
            self->rx_paused = true;
            uart_irq_rx_disable(dev);
            // read() may have freed msgq space just before rx_paused was
            // set. Then read() saw rx_paused false and did not call
            // resume_rx(), so check for space again.
            if (k_msgq_num_free_get(&self->msgq) == 0) {
                break;
            }
            resume_rx(self);
        }
        if (uart_fifo_read(dev, &c, 1) != 1) {
            break;
        }
        if (self->sigint_enabled && mp_interrupt_char == c) {
            common_hal_busio_uart_clear_rx_buffer(self);
            mp_sched_keyboard_interrupt();
            port_wake_main_task_from_isr();
        } else if (k_msgq_put(&self->msgq, &c, K_NO_WAIT) == 0) {
            // Wake the main task so it can service the new RX data
            // instead of sleeping out its full timeout.
            port_wake_main_task_from_isr();
        }
    }
}

// Helper function for Zephyr-specific initialization from device tree
mp_obj_t common_hal_busio_uart_construct_from_device(busio_uart_obj_t *self, const struct device *uart_device, uint16_t receiver_buffer_size, byte *receiver_buffer, bool sigint_enabled) {
    self->base.type = &busio_uart_type;
    self->uart_device = uart_device;
    self->dynamic = false;
    self->receiver_buffer = NULL;
    self->tx = NULL;
    self->rx = NULL;
    self->rts = NULL;
    self->cts = NULL;
    int ret = uart_irq_callback_user_data_set(uart_device, serial_cb, self);

    if (ret < 0) {
        LOG_ERR("Failed to set UART IRQ callback: %d", ret);
    }

    k_msgq_init(&self->msgq, receiver_buffer, 1, receiver_buffer_size);

    self->timeout = K_FOREVER;
    self->write_timeout = K_FOREVER;
    self->tx_len = 0;
    k_sem_init(&self->tx_done, 0, 1);
    self->rx_paused = false;
    self->flow_control = false;
    self->sigint_enabled = sigint_enabled;
    uart_irq_rx_enable(uart_device);

    return MP_OBJ_FROM_PTR(self);
}

// Standard busio construct: pick a free peripheral instance and route it to
// the requested pins at runtime (supported on nRF SoCs).
void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts, const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir, bool rs485_invert,
    uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
    mp_float_t timeout, uint16_t receiver_buffer_size, byte *receiver_buffer,
    bool sigint_enabled) {
    if (rs485_dir != NULL) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("RS485"));
    }
    // nRF UARTE only supports 8 data bits.
    mp_arg_validate_int(bits, 8, MP_QSTR_bits);

    const struct device *dev = NULL;
    int ret = iobroker_uart_allocate(tx != NULL ? tx->package_pin : IOBROKER_NO_PIN,
        rx != NULL ? rx->package_pin : IOBROKER_NO_PIN,
        rts != NULL ? rts->package_pin : IOBROKER_NO_PIN,
        cts != NULL ? cts->package_pin : IOBROKER_NO_PIN, &dev);
    if (ret < 0) {
        if (ret == -ENODEV) {
            mp_raise_ValueError(MP_ERROR_TEXT("All UART peripherals are in use"));
        }
        if (ret == -EBUSY) {
            mp_raise_ValueError(MP_ERROR_TEXT("Internal resource(s) in use"));
        }
        if (ret == -EINVAL) {
            raise_ValueError_invalid_pins();
        }
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Use device tree to define %q devices"), MP_QSTR_UART);
    }

    bool allocated_buffer = false;
    if (receiver_buffer == NULL) {
        receiver_buffer = m_malloc(receiver_buffer_size);
        allocated_buffer = true;
    }

    common_hal_busio_uart_construct_from_device(self, dev, receiver_buffer_size, receiver_buffer, sigint_enabled);
    self->dynamic = true;
    self->receiver_buffer = allocated_buffer ? receiver_buffer : NULL;
    self->tx = tx;
    self->rx = rx;
    self->rts = rts;
    self->cts = cts;

    // Initialize the deferred device now that it is routed to the requested
    // pins. Fixed devicetree instances are already initialized (-EALREADY).
    int init_ret = device_init(dev);
    if (init_ret < 0 && init_ret != -EALREADY) {
        // The failed init may have routed pins and left the device in a
        // partial state; deinit gives up the claim and resets the pins.
        common_hal_busio_uart_deinit(self);
        raise_zephyr_error(init_ret);
    }

    // Apply line configuration.
    struct uart_config config = {
        .baudrate = baudrate,
        .data_bits = UART_CFG_DATA_BITS_8,
        .parity = (parity == BUSIO_UART_PARITY_NONE) ? UART_CFG_PARITY_NONE :
            ((parity == BUSIO_UART_PARITY_EVEN) ? UART_CFG_PARITY_EVEN : UART_CFG_PARITY_ODD),
        .stop_bits = (stop == 1) ? UART_CFG_STOP_BITS_1 : UART_CFG_STOP_BITS_2,
        .flow_ctrl = (rts != NULL && cts != NULL) ? UART_CFG_FLOW_CTRL_RTS_CTS : UART_CFG_FLOW_CTRL_NONE,
    };
    int config_ret = uart_configure(self->uart_device, &config);
    if (config_ret < 0) {
        LOG_ERR("uart_configure failed: %d (baudrate=%u stop=%u flow=%u)",
            config_ret, baudrate, stop, (rts != NULL && cts != NULL));
        common_hal_busio_uart_deinit(self);
        raise_zephyr_error(config_ret);
    }

    self->timeout = K_USEC((uint64_t)(timeout * 1000000));
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->uart_device == NULL;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }
    if (self->dynamic) {
        // The device may not be fully initialized: construct de-inits this
        // object when device_init() fails partway through. Zephyr then
        // reports it not ready (init_res != 0), so only poke the driver
        // when it is really up and running. The iobroker claim and any
        // routed pins are still given up below.
        if (device_is_ready(self->uart_device)) {
            uart_irq_rx_disable(self->uart_device);
            uart_irq_callback_user_data_set(self->uart_device, NULL, NULL);
        }
        // The release de-inits the device, which applies its low-power
        // pinctrl state and leaves the routed pins disconnected.
        (void)iobroker_release(self->uart_device);
        self->tx = NULL;
        self->rx = NULL;
        self->rts = NULL;
        self->cts = NULL;
        if (self->receiver_buffer != NULL) {
            m_free(self->receiver_buffer);
            self->receiver_buffer = NULL;
        }
        self->uart_device = NULL;
    }
}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    size_t count = 0;
    while (count < len && k_msgq_get(&self->msgq, data + count, self->timeout) == 0) {
        count++;
    }
    if (count > 0) {
        resume_rx(self);
    }

    return count;
}

// Write characters. serial_cb() feeds the driver from the TX interrupt, so
// this thread only waits and can stop on Ctrl-C.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    if (len == 0) {
        return 0;
    }
    k_sem_reset(&self->tx_done);
    self->tx_data = data;
    self->tx_len = len;
    uart_irq_tx_enable(self->uart_device);
    while (k_sem_take(&self->tx_done, K_MSEC(10)) != 0) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }
    }
    uart_irq_tx_disable(self->uart_device);

    unsigned int key = irq_lock();
    size_t not_sent = self->tx_len;
    self->tx_len = 0;
    irq_unlock(key);

    return len - not_sent;
}

void zephyr_uart_write_blocking(busio_uart_obj_t *self, const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uart_poll_out(self->uart_device, data[i]);
    }
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    struct uart_config config;
    uart_config_get(self->uart_device, &config);
    return config.baudrate;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
    struct uart_config config;
    uart_config_get(self->uart_device, &config);
    config.baudrate = baudrate;
    uart_configure(self->uart_device, &config);
}

// K_FOREVER reads as -1, which usb_cdc reports as None.
static mp_float_t timeout_to_seconds(k_timeout_t timeout) {
    if (K_TIMEOUT_EQ(timeout, K_FOREVER)) {
        return -1;
    }
    return (mp_float_t)k_ticks_to_us_floor64(timeout.ticks) / MICROPY_FLOAT_CONST(1000000.0);
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return timeout_to_seconds(self->timeout);
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
    self->timeout = timeout < 0 ? K_FOREVER : K_USEC((uint64_t)(timeout * 1000000));
}

mp_float_t common_hal_busio_uart_get_write_timeout(busio_uart_obj_t *self) {
    return timeout_to_seconds(self->write_timeout);
}

void common_hal_busio_uart_set_write_timeout(busio_uart_obj_t *self, mp_float_t write_timeout) {
    self->write_timeout = write_timeout < 0 ? K_FOREVER : K_USEC((uint64_t)(write_timeout * 1000000));
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return k_msgq_num_used_get(&self->msgq);
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
    k_msgq_purge(&self->msgq);
    resume_rx(self);
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    return true;
}
