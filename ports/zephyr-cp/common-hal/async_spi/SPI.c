// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/async_spi/SPI.h"

#include <string.h>

#include "py/runtime.h"
#include "supervisor/port.h"
#include "bindings/zephyr_kernel/__init__.h"

#include <errno.h>
#include <iobroker/iobroker.h>

// Settings take the other config slot: drivers see a change by the pointer.
static void apply_config(async_spi_spi_obj_t *self, uint32_t frequency, uint16_t operation) {
    struct spi_config *current = &self->config[self->active_config];
    if (current->frequency == frequency && current->operation == operation) {
        return;
    }
    self->active_config = 1 - self->active_config;
    self->config[self->active_config].frequency = frequency;
    self->config[self->active_config].operation = operation;
}

static void transfer_done(const struct device *dev, int result, void *userdata) {
    ARG_UNUSED(dev);
    ARG_UNUSED(result);
    async_spi_spi_obj_t *self = userdata;
    CIRCUITPY_ASYNC_FLAG_SET(self->done);
    // A configure() that waited for this transfer.
    if (self->idle_done != NULL) {
        apply_config(self, self->pending_frequency, self->pending_operation);
        CIRCUITPY_ASYNC_FLAG_SET(self->idle_done);
        self->idle_done = NULL;
    }
    port_wake_main_task_from_isr();
}

// Zephyr cannot stop a transfer, so this waits for it; deinit() waits too. Does not allocate.
static void wait_done(async_spi_spi_obj_t *self) {
    if (self->done == NULL) {
        return;
    }
    while (!CIRCUITPY_ASYNC_FLAG_IS_SET(self->done)) {
        RUN_BACKGROUND_TASKS;
    }
    self->done = NULL;
}

void common_hal_async_spi_spi_construct(async_spi_spi_obj_t *self, const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso) {
    const struct device *dev = NULL;
    int ret = iobroker_spi_allocate(clock->package_pin,
        mosi != NULL ? mosi->package_pin : IOBROKER_NO_PIN,
        miso != NULL ? miso->package_pin : IOBROKER_NO_PIN, &dev);
    if (ret < 0) {
        if (ret == -ENODEV) {
            mp_raise_ValueError(MP_ERROR_TEXT("All SPI peripherals are in use"));
        }
        if (ret == -EBUSY) {
            mp_raise_ValueError(MP_ERROR_TEXT("Internal resource(s) in use"));
        }
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Use device tree to define %q devices"), MP_QSTR_SPI);
    }

    self->spi_device = dev;
    self->clock = clock;
    self->mosi = mosi;
    self->miso = miso;
    self->active_config = 0;
    for (size_t i = 0; i < 2; i++) {
        self->config[i].frequency = 100000;
        self->config[i].operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_LINES_SINGLE;
    }
    self->done = NULL;
    self->fill = NULL;
    self->idle_done = NULL;

    // A routed device is initialized now; a fixed one already was (-EALREADY).
    int init_ret = device_init(dev);
    if (init_ret < 0 && init_ret != -EALREADY) {
        common_hal_async_spi_spi_deinit(self);
        raise_zephyr_error(init_ret);
    }
}

bool common_hal_async_spi_spi_deinited(async_spi_spi_obj_t *self) {
    return self->spi_device == NULL;
}

void common_hal_async_spi_spi_deinit(async_spi_spi_obj_t *self) {
    if (common_hal_async_spi_spi_deinited(self)) {
        return;
    }
    wait_done(self);
    (void)iobroker_release(self->spi_device);
    self->spi_device = NULL;
    self->clock = NULL;
    self->mosi = NULL;
    self->miso = NULL;
}

static uint16_t operation_of(uint8_t polarity, uint8_t phase, uint8_t bits) {
    uint16_t operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(bits) | SPI_LINES_SINGLE;
    if (polarity) {
        operation |= SPI_MODE_CPOL;
    }
    if (phase) {
        operation |= SPI_MODE_CPHA;
    }
    return operation;
}

void common_hal_async_spi_spi_configure_start(async_spi_spi_obj_t *self, uint32_t baudrate,
    uint8_t polarity, uint8_t phase, uint8_t bits, circuitpy_async_flag_t *done) {
    CIRCUITPY_ASYNC_FLAG_INIT(done);
    uint16_t operation = operation_of(polarity, phase, bits);
    if (self->done == NULL || CIRCUITPY_ASYNC_FLAG_IS_SET(self->done)) {
        apply_config(self, baudrate, operation);
        CIRCUITPY_ASYNC_FLAG_SET(done);
        return;
    }
    // A later configure() replaces one still waiting.
    if (self->idle_done != NULL) {
        CIRCUITPY_ASYNC_FLAG_SET(self->idle_done);
    }
    self->pending_frequency = baudrate;
    self->pending_operation = operation;
    self->idle_done = done;
}

mp_obj_t common_hal_async_spi_spi_configure_end(void *context, circuitpy_async_flag_t *done) {
    return mp_const_none;
}

void common_hal_async_spi_spi_configure_cancel(void *context, circuitpy_async_flag_t *done) {
    async_spi_spi_obj_t *self = context;
    if (self->idle_done == done) {
        self->idle_done = NULL;
    }
}

uint32_t common_hal_async_spi_spi_get_frequency(async_spi_spi_obj_t *self) {
    return self->config[self->active_config].frequency;
}

// Start a transfer; done is set from the driver's callback, or at once for an empty one.
static void start(async_spi_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in,
    size_t len, circuitpy_async_flag_t *done) {
    wait_done(self);
    CIRCUITPY_ASYNC_FLAG_INIT(done);
    self->done = done;
    if (len == 0) {
        CIRCUITPY_ASYNC_FLAG_SET(done);
        return;
    }
    self->tx_buf.buf = (void *)data_out;
    self->tx_buf.len = len;
    self->tx.buffers = &self->tx_buf;
    self->tx.count = data_out != NULL ? 1 : 0;
    self->rx_buf.buf = data_in;
    self->rx_buf.len = len;
    self->rx.buffers = &self->rx_buf;
    self->rx.count = data_in != NULL ? 1 : 0;

    int ret = -ENOTSUP;
    if (DEVICE_API_GET(spi, self->spi_device)->transceive_async != NULL) {
        ret = spi_transceive_cb(self->spi_device, &self->config[self->active_config],
            self->tx.count ? &self->tx : NULL, self->rx.count ? &self->rx : NULL,
            transfer_done, self);
    }
    if (ret < 0) {
        CIRCUITPY_ASYNC_FLAG_SET(done);
        self->done = NULL;
        raise_zephyr_error(ret);
    }
}

void common_hal_async_spi_spi_write_start(async_spi_spi_obj_t *self, const uint8_t *data,
    size_t len, circuitpy_async_flag_t *done) {
    start(self, data, NULL, len, done);
}

void common_hal_async_spi_spi_readinto_start(async_spi_spi_obj_t *self, uint8_t *data,
    size_t len, uint8_t write_value, circuitpy_async_flag_t *done) {
    wait_done(self);
    // Without a TX buffer Zephyr sends zeros; another value needs a buffer.
    uint8_t *fill = NULL;
    if (write_value != 0) {
        fill = m_malloc(len);
        memset(fill, write_value, len);
    }
    self->fill = fill;
    start(self, fill, data, len, done);
}

void common_hal_async_spi_spi_write_readinto_start(async_spi_spi_obj_t *self,
    const uint8_t *data_out, uint8_t *data_in, size_t len, circuitpy_async_flag_t *done) {
    start(self, data_out, data_in, len, done);
}

mp_obj_t common_hal_async_spi_spi_transfer_end(void *context, circuitpy_async_flag_t *done) {
    async_spi_spi_obj_t *self = context;
    if (self->done == done) {
        wait_done(self);
    }
    return mp_const_none;
}

void common_hal_async_spi_spi_transfer_cancel(void *context, circuitpy_async_flag_t *done) {
    common_hal_async_spi_spi_transfer_end(context, done);
}
