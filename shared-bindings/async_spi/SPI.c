// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "supervisor/shared/awaitable.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared/runtime/buffer_helper.h"
#include "shared/runtime/context_manager_helpers.h"
#include "shared-bindings/async_spi/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

//| class SPI:
//|     """An SPI bus whose transfers are awaited
//|
//|     ``write``, ``readinto`` and ``write_readinto`` return awaitables: the transfer runs while
//|     other asyncio tasks run. Short transfers, and transfers that cannot use DMA (for example
//|     from a buffer in flash), block other tasks until they finish. A transfer's arguments are
//|     checked when the result is awaited. To share the bus between tasks, hold an
//|     `asyncio.Lock` around each use."""
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
// The tables are busio.SPI's too, so they live here, where the arguments are documented.
const mp_arg_t async_spi_spi_make_new_args[] = {
    { MP_QSTR_clock, MP_ARG_REQUIRED | MP_ARG_OBJ },
    { MP_QSTR_MOSI, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_MISO, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_half_duplex, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },  // busio.SPI only
};
const mp_arg_t async_spi_spi_configure_args[] = {
    { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100000} },
    { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
};
const mp_arg_t async_spi_spi_write_args[] = {
    { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
    { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
};
const mp_arg_t async_spi_spi_readinto_args[] = {
    { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
    { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    { MP_QSTR_write_value, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
};
const mp_arg_t async_spi_spi_write_readinto_args[] = {
    { MP_QSTR_out_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
    { MP_QSTR_in_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
    { MP_QSTR_out_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_out_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    { MP_QSTR_in_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_in_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
};

static mp_obj_t async_spi_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_clock, ARG_MOSI, ARG_MISO };
    mp_arg_val_t args[3];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, 3, async_spi_spi_make_new_args, args);

    const mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj, MP_QSTR_clock);
    const mcu_pin_obj_t *mosi = validate_obj_is_free_pin_or_none(args[ARG_MOSI].u_obj, MP_QSTR_MOSI);
    const mcu_pin_obj_t *miso = validate_obj_is_free_pin_or_none(args[ARG_MISO].u_obj, MP_QSTR_MISO);
    if (!miso && !mosi) {
        mp_raise_ValueError(MP_ERROR_TEXT("Must provide MISO or MOSI pin"));
    }

    async_spi_spi_obj_t *self = mp_obj_malloc_with_finaliser(async_spi_spi_obj_t, &async_spi_spi_type);
    common_hal_async_spi_spi_construct(self, clock, mosi, miso);
    return MP_OBJ_FROM_PTR(self);
}

static MP_NOINLINE async_spi_spi_obj_t *checked_spi(mp_obj_t self_in) {
    async_spi_spi_obj_t *self = MP_OBJ_TO_PTR(mp_arg_validate_type(self_in, &async_spi_spi_type, MP_QSTR_self));
    if (common_hal_async_spi_spi_deinited(self)) {
        raise_deinited_error();
    }
    return self;
}

//|     def deinit(self) -> None:
//|         """Turn off the SPI bus. A transfer still running is stopped."""
//|         ...
//|
static mp_obj_t async_spi_spi_obj_deinit(mp_obj_t self_in) {
    async_spi_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_async_spi_spi_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(async_spi_spi_deinit_obj, async_spi_spi_obj_deinit);

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

//|     async def configure(
//|         self, *, baudrate: int = 100000, polarity: int = 0, phase: int = 0, bits: int = 8
//|     ) -> None:
//|         """Configure the SPI bus, as `busio.SPI.configure`. A transfer still running finishes
//|         first, with the old settings."""
//|         ...
//|
static void *async_spi_spi_configure(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits };
    async_spi_spi_obj_t *self = checked_spi(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(async_spi_spi_configure_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(async_spi_spi_configure_args), async_spi_spi_configure_args, args);

    uint8_t polarity = (uint8_t)mp_arg_validate_int_range(args[ARG_polarity].u_int, 0, 1, MP_QSTR_polarity);
    uint8_t phase = (uint8_t)mp_arg_validate_int_range(args[ARG_phase].u_int, 0, 1, MP_QSTR_phase);
    uint8_t bits = (uint8_t)mp_arg_validate_int_range(args[ARG_bits].u_int, 8, 9, MP_QSTR_bits);

    common_hal_async_spi_spi_configure_start(self, args[ARG_baudrate].u_int, polarity, phase, bits, done);
    return self;
}
static CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(async_spi_spi_configure_obj, 1, async_spi_spi_configure);

//|     import sys
//|
//|     async def write(
//|         self, buffer: ReadableBuffer, *, start: int = 0, end: int = sys.maxsize
//|     ) -> None:
//|         """Write the data in ``buffer[start:end]``.
//|
//|         :param ~circuitpython_typing.ReadableBuffer buffer: write out the data in this buffer
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``"""
//|         ...
//|
static void *async_spi_spi_write(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    async_spi_spi_obj_t *self = checked_spi(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(async_spi_spi_write_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(async_spi_spi_write_args), async_spi_spi_write_args, args);

    size_t len;
    uint8_t *data = buffer_slice(args[ARG_buffer].u_obj, args[ARG_start].u_int, args[ARG_end].u_int,
        MP_BUFFER_READ, &len);
    common_hal_async_spi_spi_write_start(self, data, len, done);
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
//|
//|         :param ~circuitpython_typing.WriteableBuffer buffer: read data into this buffer
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``
//|         :param int write_value: value to write while reading"""
//|         ...
//|
static void *async_spi_spi_readinto(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end, ARG_write_value };
    async_spi_spi_obj_t *self = checked_spi(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(async_spi_spi_readinto_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(async_spi_spi_readinto_args), async_spi_spi_readinto_args, args);

    size_t len;
    uint8_t *data = buffer_slice(args[ARG_buffer].u_obj, args[ARG_start].u_int, args[ARG_end].u_int,
        MP_BUFFER_WRITE, &len);
    common_hal_async_spi_spi_readinto_start(self, data, len, (uint8_t)args[ARG_write_value].u_int, done);
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
//|         The two slices must have the same length.
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
    async_spi_spi_obj_t *self = checked_spi(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(async_spi_spi_write_readinto_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(async_spi_spi_write_readinto_args), async_spi_spi_write_readinto_args, args);

    size_t out_len, in_len;
    const uint8_t *data_out = buffer_slice(args[ARG_out_buffer].u_obj, args[ARG_out_start].u_int,
        args[ARG_out_end].u_int, MP_BUFFER_READ, &out_len);
    uint8_t *data_in = buffer_slice(args[ARG_in_buffer].u_obj, args[ARG_in_start].u_int,
        args[ARG_in_end].u_int, MP_BUFFER_WRITE, &in_len);
    if (out_len != in_len) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer slices must be of equal length"));
    }
    common_hal_async_spi_spi_write_readinto_start(self, data_out, data_in, out_len, done);
    return self;
}
static CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(async_spi_spi_write_readinto_obj, 2, async_spi_spi_write_readinto);

//|     frequency: int
//|     """The actual SPI bus frequency. This may not match the frequency requested
//|     due to internal limitations."""
//|
//|
static mp_obj_t async_spi_spi_get_frequency(mp_obj_t self_in) {
    async_spi_spi_obj_t *self = checked_spi(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_async_spi_spi_get_frequency(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(async_spi_spi_get_frequency_obj, async_spi_spi_get_frequency);

MP_PROPERTY_GETTER(async_spi_spi_frequency_obj,
    (mp_obj_t)&async_spi_spi_get_frequency_obj);

static const mp_rom_map_elem_t async_spi_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&async_spi_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&async_spi_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_configure), MP_ROM_PTR(&async_spi_spi_configure_obj) },

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
    make_new, async_spi_spi_make_new,
    locals_dict, &async_spi_spi_locals_dict
    );
