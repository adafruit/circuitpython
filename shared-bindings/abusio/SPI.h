// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/circuitpy_objawaitable.h"

#include "common-hal/busio/SPI.h"
#include "shared-bindings/busio/SPI.h"

// abusio_spi_obj_t embeds busio_spi_obj_t as its first member so that all
// common_hal_busio_spi_* functions can be called directly by casting self.
typedef struct {
    busio_spi_obj_t spi;  // MUST be first — aliased to busio_spi_obj_t *
} abusio_spi_obj_t;

extern const mp_obj_type_t abusio_spi_type;

// Async write — data is mp_obj_tuple (self_mp_obj, buf_mp_obj)
void *common_hal_abusio_spi_write_start(circuitpy_async_flag_t *flag, mp_obj_t data);
mp_obj_t common_hal_abusio_spi_write_end(void *ctx);
void common_hal_abusio_spi_write_cancel(void *ctx);

// Async readinto — data is mp_obj_tuple (self_mp_obj, buf_mp_obj, write_value_mp_obj)
void *common_hal_abusio_spi_readinto_start(circuitpy_async_flag_t *flag, mp_obj_t data);
mp_obj_t common_hal_abusio_spi_readinto_end(void *ctx);
void common_hal_abusio_spi_readinto_cancel(void *ctx);

// Async write_readinto — data is mp_obj_tuple (self_mp_obj, out_mp_obj, in_mp_obj)
void *common_hal_abusio_spi_write_readinto_start(circuitpy_async_flag_t *flag, mp_obj_t data);
mp_obj_t common_hal_abusio_spi_write_readinto_end(void *ctx);
void common_hal_abusio_spi_write_readinto_cancel(void *ctx);
