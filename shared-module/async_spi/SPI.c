// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/async_spi/SPI.h"
#include "shared-bindings/busio/SPI.h"

mp_obj_t common_hal_async_spi_spi_transfer_end(void *context, circuitpy_async_flag_t *done) {
    async_spi_spi_obj_t *self = context;
    common_hal_busio_spi_stop(&self->spi, done);
    return mp_const_none;
}

void common_hal_async_spi_spi_transfer_cancel(void *context, circuitpy_async_flag_t *done) {
    async_spi_spi_obj_t *self = context;
    common_hal_busio_spi_stop(&self->spi, done);
}
