// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Port-agnostic helpers for unpacking the mp_obj_t tuple that
// shared-bindings/abusio/SPI.c packs into each awaitable's data field.

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/objarray.h"
#include "py/runtime.h"
#include "shared-bindings/abusio/SPI.h"

#if MICROPY_PY_ASYNC_AWAIT

// data = (self, read-memoryview)
static inline void abusio_spi_unpack_write(mp_obj_t data,
    abusio_spi_obj_t **self_out,
    const uint8_t **buf_out, size_t *len_out) {
    mp_obj_t *items;
    size_t n;
    mp_obj_tuple_get(data, &n, &items);
    *self_out = MP_OBJ_TO_PTR(items[0]);
    mp_buffer_info_t bi;
    mp_get_buffer_raise(items[1], &bi, MP_BUFFER_READ);
    *buf_out = bi.buf;
    *len_out = bi.len;
}

// data = (self, write-memoryview, write_value_int)
static inline void abusio_spi_unpack_readinto(mp_obj_t data,
    abusio_spi_obj_t **self_out,
    uint8_t **buf_out, size_t *len_out, uint8_t *write_value_out) {
    mp_obj_t *items;
    size_t n;
    mp_obj_tuple_get(data, &n, &items);
    *self_out = MP_OBJ_TO_PTR(items[0]);
    mp_buffer_info_t bi;
    mp_get_buffer_raise(items[1], &bi, MP_BUFFER_WRITE);
    *buf_out = bi.buf;
    *len_out = bi.len;
    *write_value_out = (uint8_t)mp_obj_get_int(items[2]);
}

// data = (self, read-memoryview, write-memoryview)
static inline void abusio_spi_unpack_write_readinto(mp_obj_t data,
    abusio_spi_obj_t **self_out,
    const uint8_t **out_out, uint8_t **in_out, size_t *len_out) {
    mp_obj_t *items;
    size_t n;
    mp_obj_tuple_get(data, &n, &items);
    *self_out = MP_OBJ_TO_PTR(items[0]);
    mp_buffer_info_t obi, ibi;
    mp_get_buffer_raise(items[1], &obi, MP_BUFFER_READ);
    mp_get_buffer_raise(items[2], &ibi, MP_BUFFER_WRITE);
    *out_out = obi.buf;
    *in_out = ibi.buf;
    *len_out = obi.len;
}

#endif // MICROPY_PY_ASYNC_AWAIT
