// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/async_spi/SPI.h"

#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"

void common_hal_async_spi_spi_construct(async_spi_spi_obj_t *self, const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso) {
    rp2_spi_error_t error = rp2_spi_construct(&self->spi, clock, mosi, miso);
    if (error == RP2_SPI_INVALID_PINS) {
        raise_ValueError_invalid_pins();
    }
    if (error == RP2_SPI_IN_USE) {
        mp_raise_ValueError(MP_ERROR_TEXT("SPI peripheral in use"));
    }
}

bool common_hal_async_spi_spi_deinited(async_spi_spi_obj_t *self) {
    return rp2_spi_deinited(&self->spi);
}

void common_hal_async_spi_spi_deinit(async_spi_spi_obj_t *self) {
    rp2_spi_deinit(&self->spi);
}

void common_hal_async_spi_spi_configure_start(async_spi_spi_obj_t *self, uint32_t baudrate,
    uint8_t polarity, uint8_t phase, uint8_t bits, circuitpy_async_flag_t *done) {
    rp2_spi_configure_start(&self->spi, baudrate, polarity, phase, bits, done);
}

mp_obj_t common_hal_async_spi_spi_configure_end(void *context, circuitpy_async_flag_t *done) {
    return mp_const_none;
}

void common_hal_async_spi_spi_configure_cancel(void *context, circuitpy_async_flag_t *done) {
    async_spi_spi_obj_t *self = context;
    rp2_spi_configure_cancel(&self->spi, done);
}

uint32_t common_hal_async_spi_spi_get_frequency(async_spi_spi_obj_t *self) {
    return self->spi.real_frequency;
}

void common_hal_async_spi_spi_write_start(async_spi_spi_obj_t *self, const uint8_t *data,
    size_t len, circuitpy_async_flag_t *done) {
    rp2_spi_write_start(&self->spi, data, len, done);
}

void common_hal_async_spi_spi_readinto_start(async_spi_spi_obj_t *self, uint8_t *data,
    size_t len, uint8_t write_value, circuitpy_async_flag_t *done) {
    rp2_spi_read_start(&self->spi, data, len, write_value, done);
}

void common_hal_async_spi_spi_write_readinto_start(async_spi_spi_obj_t *self,
    const uint8_t *data_out, uint8_t *data_in, size_t len, circuitpy_async_flag_t *done) {
    rp2_spi_transfer_start(&self->spi, data_out, data_in, len, done);
}

mp_obj_t common_hal_async_spi_spi_transfer_end(void *context, circuitpy_async_flag_t *done) {
    async_spi_spi_obj_t *self = context;
    rp2_spi_stop(&self->spi, done);
    return mp_const_none;
}

void common_hal_async_spi_spi_transfer_cancel(void *context, circuitpy_async_flag_t *done) {
    async_spi_spi_obj_t *self = context;
    rp2_spi_stop(&self->spi, done);
}
