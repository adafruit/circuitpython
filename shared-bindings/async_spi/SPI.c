// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "supervisor/shared/awaitable.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared/runtime/buffer_helper.h"
#include "shared/runtime/context_manager_helpers.h"
#include "shared-bindings/async_spi/SPI.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/util.h"

//| class SPI:
//|     """An SPI bus whose transfers are awaited
//|
//|     Configuration and locking work as in `busio.SPI`. ``write``, ``readinto`` and
//|     ``write_readinto`` return awaitables: the transfer runs while other asyncio tasks run.
//|     Short transfers, and transfers that cannot use DMA (for example from a buffer in flash),
//|     block other tasks until they finish. A transfer's arguments are checked when the result is
//|     awaited. To share the bus between tasks, hold an `asyncio.Lock` around each use."""
//|
//|     def __init__(
//|         self,
//|         clock: microcontroller.Pin,
//|         MOSI: Optional[microcontroller.Pin] = None,
//|         MISO: Optional[microcontroller.Pin] = None,
//|     ) -> None:
//|         """Construct an SPI object on the given pins.
//|
//|         :param ~microcontroller.Pin clock: the pin to use for the clock.
//|         :param ~microcontroller.Pin MOSI: the Main Out Selected In pin.
//|         :param ~microcontroller.Pin MISO: the Main In Selected Out pin."""
//|         ...
//|
//  Provided by busio.SPI.

static async_spi_spi_obj_t *locked_spi(mp_obj_t self_in) {
    async_spi_spi_obj_t *self = MP_OBJ_TO_PTR(mp_arg_validate_type(self_in, &async_spi_spi_type, MP_QSTR_self));
    if (common_hal_busio_spi_deinited(&self->spi)) {
        raise_deinited_error();
    }
    if (!common_hal_busio_spi_has_lock(&self->spi)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Function requires lock"));
    }
    return self;
}

// buffer[start:end], with start and end in elements as in busio.SPI. Returns the slice's address
// and sets *len to its length in bytes.
static uint8_t *buffer_slice(mp_obj_t buffer, int32_t start, int32_t end, mp_uint_t flags, size_t *len) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, flags);
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);
    size_t length = bufinfo.len / stride_in_bytes;
    normalize_buffer_bounds(&start, end, &length);
    *len = length * stride_in_bytes;
    return (uint8_t *)bufinfo.buf + start * stride_in_bytes;
}

//|     def deinit(self) -> None:
//|         """Turn off the SPI bus. A transfer still running is finished first."""
//|         ...
//|
//  Provided by busio.SPI.

//|     def __enter__(self) -> SPI:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
//  Provided by context manager helper.

//|     def configure(
//|         self, *, baudrate: int = 100000, polarity: int = 0, phase: int = 0, bits: int = 8
//|     ) -> None:
//|         """Configure the SPI bus, as `busio.SPI.configure`. The bus must be locked."""
//|         ...
//|
//  Provided by busio.SPI.

//|     def try_lock(self) -> bool:
//|         """Attempts to grab the SPI lock. Returns True on success."""
//|         ...
//|
//  Provided by busio.SPI.

//|     def unlock(self) -> None:
//|         """Releases the SPI lock."""
//|         ...
//|
//  Provided by busio.SPI.

//|     import sys
//|
//|     async def write(
//|         self, buffer: ReadableBuffer, *, start: int = 0, end: int = sys.maxsize
//|     ) -> None:
//|         """Write the data in ``buffer[start:end]``. The bus must be locked.
//|
//|         :param ~circuitpython_typing.ReadableBuffer buffer: write out the data in this buffer
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``"""
//|         ...
//|
static void *async_spi_spi_write(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    async_spi_spi_obj_t *self = locked_spi(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t len;
    uint8_t *data = buffer_slice(args[ARG_buffer].u_obj, args[ARG_start].u_int, args[ARG_end].u_int,
        MP_BUFFER_READ, &len);
    common_hal_busio_spi_write_start(&self->spi, data, len, done);
    return self;
}
static CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(async_spi_spi_write_obj, 1, async_spi_spi_write);

//|     async def readinto(
//|         self,
//|         buffer: WriteableBuffer,
//|         *,
//|         start: int = 0,
//|         end: int = sys.maxsize,
//|         write_value: int = 0,
//|     ) -> None:
//|         """Read into ``buffer[start:end]`` while writing ``write_value`` for each byte read.
//|         The bus must be locked.
//|
//|         :param ~circuitpython_typing.WriteableBuffer buffer: read data into this buffer
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``
//|         :param int write_value: value to write while reading"""
//|         ...
//|
static void *async_spi_spi_readinto(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end, ARG_write_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_write_value, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    async_spi_spi_obj_t *self = locked_spi(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t len;
    uint8_t *data = buffer_slice(args[ARG_buffer].u_obj, args[ARG_start].u_int, args[ARG_end].u_int,
        MP_BUFFER_WRITE, &len);
    common_hal_busio_spi_read_start(&self->spi, data, len, (uint8_t)args[ARG_write_value].u_int, done);
    return self;
}
static CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(async_spi_spi_readinto_obj, 1, async_spi_spi_readinto);

//|     async def write_readinto(
//|         self,
//|         out_buffer: ReadableBuffer,
//|         in_buffer: WriteableBuffer,
//|         *,
//|         out_start: int = 0,
//|         out_end: int = sys.maxsize,
//|         in_start: int = 0,
//|         in_end: int = sys.maxsize,
//|     ) -> None:
//|         """Write out the data in ``out_buffer`` while simultaneously reading data into ``in_buffer``.
//|         The two slices must have the same length. The bus must be locked.
//|
//|         :param ~circuitpython_typing.ReadableBuffer out_buffer: write out the data in this buffer
//|         :param ~circuitpython_typing.WriteableBuffer in_buffer: read data into this buffer
//|         :param int out_start: beginning of ``out_buffer`` slice
//|         :param int out_end: end of ``out_buffer`` slice; if not specified, use ``len(out_buffer)``
//|         :param int in_start: beginning of ``in_buffer`` slice
//|         :param int in_end: end of ``in_buffer`` slice; if not specified, use ``len(in_buffer)``"""
//|         ...
//|
static void *async_spi_spi_write_readinto(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_out_buffer, ARG_in_buffer, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_out_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_in_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_out_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_out_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_in_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_in_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    async_spi_spi_obj_t *self = locked_spi(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t out_len, in_len;
    const uint8_t *data_out = buffer_slice(args[ARG_out_buffer].u_obj, args[ARG_out_start].u_int,
        args[ARG_out_end].u_int, MP_BUFFER_READ, &out_len);
    uint8_t *data_in = buffer_slice(args[ARG_in_buffer].u_obj, args[ARG_in_start].u_int,
        args[ARG_in_end].u_int, MP_BUFFER_WRITE, &in_len);
    if (out_len != in_len) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer slices must be of equal length"));
    }
    common_hal_busio_spi_transfer_start(&self->spi, data_out, data_in, out_len, done);
    return self;
}
static CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(async_spi_spi_write_readinto_obj, 2, async_spi_spi_write_readinto);

//|     frequency: int
//|     """The actual SPI bus frequency. This may not match the frequency requested
//|     due to internal limitations."""
//|
//|
MP_PROPERTY_GETTER(async_spi_spi_frequency_obj,
    (mp_obj_t)&busio_spi_get_frequency_obj);

static const mp_rom_map_elem_t async_spi_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&busio_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&busio_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_configure), MP_ROM_PTR(&busio_spi_configure_obj) },
    { MP_ROM_QSTR(MP_QSTR_try_lock), MP_ROM_PTR(&busio_spi_try_lock_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlock), MP_ROM_PTR(&busio_spi_unlock_obj) },

    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&async_spi_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&async_spi_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&async_spi_spi_write_readinto_obj) },

    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&async_spi_spi_frequency_obj) },
};
static MP_DEFINE_CONST_DICT(async_spi_spi_locals_dict, async_spi_spi_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    async_spi_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, busio_spi_make_new,
    locals_dict, &async_spi_spi_locals_dict
    );
