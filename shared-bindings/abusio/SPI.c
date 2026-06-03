// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Python type definition for abusio.SPI — an async-native SPI bus.
// Configuration and locking delegate directly to common_hal_busio_spi_*;
// the three transfer methods return circuitpy_awaitable_obj_t objects.

#include <string.h>
#include <stdint.h>

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/abusio/SPI.h"
#include "shared-bindings/util.h"

#include "shared/runtime/buffer_helper.h"
#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mperrno.h"
#include "py/objarray.h"
#include "py/objproperty.h"
#include "py/objtuple.h"
#include "py/runtime.h"
#include "py/circuitpy_objawaitable.h"

#if MICROPY_PY_ASYNC_AWAIT

// ---- helpers ----------------------------------------------------------------

static abusio_spi_obj_t *native_abusio_spi(mp_obj_t self_in) {
    return mp_arg_validate_type(self_in, &abusio_spi_type, MP_QSTR_self);
}

static void check_for_deinit(abusio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(&self->spi)) {
        raise_deinited_error();
    }
}

static void check_lock(abusio_spi_obj_t *self) {
    if (!common_hal_busio_spi_has_lock(&self->spi)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Function requires lock"));
    }
}

// ---- constructor / lifecycle ------------------------------------------------

//| class SPI:
//|     """Async-native SPI bus backed by DMA + IRQ completion.
//|
//|     All transfer methods (``write``, ``readinto``, ``write_readinto``) return
//|     awaitables and must be used with ``await`` inside an ``async def``.
//|     Use ``await spi.lock()`` to acquire the bus lock without blocking other
//|     tasks."""
//|
//|
//|     def __init__(
//|         self,
//|         clock: microcontroller.Pin,
//|         MOSI: Optional[microcontroller.Pin] = None,
//|         MISO: Optional[microcontroller.Pin] = None,
//|     ) -> None:
//|         """Construct an async SPI object on the given pins."""
//|         ...
//|
static mp_obj_t abusio_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    abusio_spi_obj_t *self = mp_obj_malloc_with_finaliser(abusio_spi_obj_t, &abusio_spi_type);
    enum { ARG_clock, ARG_MOSI, ARG_MISO };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_clock, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_MOSI, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_MISO, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj, MP_QSTR_clock);
    const mcu_pin_obj_t *mosi = validate_obj_is_free_pin_or_none(args[ARG_MOSI].u_obj, MP_QSTR_MOSI);
    const mcu_pin_obj_t *miso = validate_obj_is_free_pin_or_none(args[ARG_MISO].u_obj, MP_QSTR_MISO);

    // Initialise the embedded busio SPI object (sets .base.type to busio_spi_type
    // temporarily; we fix it up below so the object identity is abusio.SPI).
    self->spi.base.type = &busio_spi_type;
    common_hal_busio_spi_construct(&self->spi, clock, mosi, miso, false);
    self->spi.base.type = &abusio_spi_type;
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Release the SPI pins."""
//|         ...
//|
static mp_obj_t abusio_spi_deinit(mp_obj_t self_in) {
    abusio_spi_obj_t *self = native_abusio_spi(self_in);
    common_hal_busio_spi_deinit(&self->spi);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(abusio_spi_deinit_obj, abusio_spi_deinit);

// ---- configuration / locking ------------------------------------------------

//|     def configure(
//|         self,
//|         *,
//|         baudrate: int = 100000,
//|         polarity: int = 0,
//|         phase: int = 0,
//|         bits: int = 8,
//|     ) -> None:
//|         """Configure the SPI bus. The bus must be locked."""
//|         ...
//|
static mp_obj_t abusio_spi_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
    };
    abusio_spi_obj_t *self = native_abusio_spi(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    common_hal_busio_spi_configure(&self->spi,
        args[ARG_baudrate].u_int, args[ARG_polarity].u_int,
        args[ARG_phase].u_int, args[ARG_bits].u_int);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(abusio_spi_configure_obj, 1, abusio_spi_configure);

// ---- async lock -------------------------------------------------------------

//|     async def lock(self) -> None:
//|         """Acquire the SPI lock asynchronously.
//|
//|         Suspends the current task until the lock is acquired, yielding to
//|         other tasks on each failed attempt.
//|         """
//|         ...
//|

typedef struct {
    mp_obj_base_t base;
    abusio_spi_obj_t *spi;
} abusio_spi_lock_aw_obj_t;

#if MICROPY_PY_ASYNCIO
extern mp_obj_t mp_asyncio_context;
#endif

static mp_obj_t abusio_spi_lock_aw_iternext(mp_obj_t self_in) {
    abusio_spi_lock_aw_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (common_hal_busio_spi_try_lock(&self->spi->spi)) {
        MP_STATE_THREAD(stop_iteration_arg) = mp_const_none;
        return MP_OBJ_STOP_ITERATION;
    }
    // Re-schedule the current task so it is retried on the next event-loop
    // iteration (same pattern as circuitpy_objawaitable.c).
    #if MICROPY_PY_ASYNCIO
    if (mp_asyncio_context != MP_OBJ_NULL) {
        mp_obj_t _task_queue = mp_obj_dict_get(mp_asyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR__task_queue));
        mp_obj_t cur_task = mp_obj_dict_get(mp_asyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR_cur_task));
        if (_task_queue != MP_OBJ_NULL && cur_task != MP_OBJ_NULL && cur_task != mp_const_none) {
            mp_obj_t dest[3];
            mp_load_method(_task_queue, MP_QSTR_push, dest);
            dest[2] = cur_task;
            mp_call_method_n_kw(1, 0, dest);
        }
    }
    #endif
    return mp_const_none; // yield — lock not yet available
}

static mp_obj_t abusio_spi_lock_aw_await(mp_obj_t self_in) {
    return self_in;
}
static MP_DEFINE_CONST_FUN_OBJ_1(abusio_spi_lock_aw_await_obj, abusio_spi_lock_aw_await);

static const mp_rom_map_elem_t abusio_spi_lock_aw_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___await__), MP_ROM_PTR(&abusio_spi_lock_aw_await_obj) },
};
static MP_DEFINE_CONST_DICT(abusio_spi_lock_aw_locals_dict, abusio_spi_lock_aw_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    abusio_spi_lock_aw_type,
    MP_QSTR_SPI_lock_awaitable,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, abusio_spi_lock_aw_iternext,
    locals_dict, &abusio_spi_lock_aw_locals_dict
    );

static mp_obj_t abusio_spi_lock(mp_obj_t self_in) {
    abusio_spi_obj_t *self = native_abusio_spi(self_in);
    check_for_deinit(self);
    abusio_spi_lock_aw_obj_t *aw =
        mp_obj_malloc(abusio_spi_lock_aw_obj_t, &abusio_spi_lock_aw_type);
    aw->spi = self;
    return MP_OBJ_FROM_PTR(aw);
}
static MP_DEFINE_CONST_FUN_OBJ_1(abusio_spi_lock_obj, abusio_spi_lock);

//|     def unlock(self) -> None:
//|         """Release the lock."""
//|         ...
//|
static mp_obj_t abusio_spi_unlock(mp_obj_t self_in) {
    abusio_spi_obj_t *self = native_abusio_spi(self_in);
    check_for_deinit(self);
    common_hal_busio_spi_unlock(&self->spi);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(abusio_spi_unlock_obj, abusio_spi_unlock);

// ---- async transfers --------------------------------------------------------

//|     async def write(self, buffer: ReadableBuffer, *, start: int = 0, end: Optional[int] = None) -> None:
//|         """Write ``buffer[start:end]`` to the SPI bus asynchronously.
//|
//|         The bus must be locked before calling.
//|         """
//|         ...
//|
static mp_obj_t abusio_spi_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_start,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    abusio_spi_obj_t *self = native_abusio_spi(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);
    int32_t start = args[ARG_start].u_int;
    size_t length = bufinfo.len;
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);

    // Pack (self, sliced_memoryview) as a 2-tuple passed through the awaitable data field.
    mp_obj_t slice = mp_obj_new_memoryview('B', length,
        (uint8_t *)bufinfo.buf + start);
    mp_obj_t tuple_items[2] = { pos_args[0], slice };
    mp_obj_t data = mp_obj_new_tuple(2, tuple_items);

    return circuitpy_awaitable_new(
        common_hal_abusio_spi_write_start,
        common_hal_abusio_spi_write_end,
        common_hal_abusio_spi_write_cancel,
        data);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(abusio_spi_write_obj, 1, abusio_spi_write);

//|     async def readinto(
//|         self,
//|         buffer: WriteableBuffer,
//|         *,
//|         start: int = 0,
//|         end: Optional[int] = None,
//|         write_value: int = 0,
//|     ) -> None:
//|         """Read into ``buffer[start:end]`` asynchronously, writing ``write_value`` on MOSI.
//|
//|         The bus must be locked before calling.
//|         """
//|         ...
//|
static mp_obj_t abusio_spi_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end, ARG_write_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,      MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_start,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_write_value, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    abusio_spi_obj_t *self = native_abusio_spi(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);
    int32_t start = args[ARG_start].u_int;
    size_t length = bufinfo.len;
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);

    mp_obj_t slice = mp_obj_new_memoryview('B' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW, length,
        (uint8_t *)bufinfo.buf + start);
    mp_obj_t tuple_items[3] = {
        pos_args[0],
        slice,
        mp_obj_new_int(args[ARG_write_value].u_int),
    };
    mp_obj_t data = mp_obj_new_tuple(3, tuple_items);

    return circuitpy_awaitable_new(
        common_hal_abusio_spi_readinto_start,
        common_hal_abusio_spi_readinto_end,
        common_hal_abusio_spi_readinto_cancel,
        data);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(abusio_spi_readinto_obj, 1, abusio_spi_readinto);

//|     async def write_readinto(
//|         self,
//|         out_buffer: ReadableBuffer,
//|         in_buffer: WriteableBuffer,
//|         *,
//|         out_start: int = 0,
//|         out_end: Optional[int] = None,
//|         in_start: int = 0,
//|         in_end: Optional[int] = None,
//|     ) -> None:
//|         """Full-duplex SPI transfer asynchronously.
//|
//|         ``out_buffer[out_start:out_end]`` and ``in_buffer[in_start:in_end]``
//|         must be the same length. The bus must be locked before calling. Sharing the same buffer for in and out is not supported.
//|         """
//|         ...
//|
static mp_obj_t abusio_spi_write_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_out_buffer, ARG_in_buffer, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_out_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_in_buffer,  MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_out_start,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_out_end,    MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_in_start,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_in_end,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    abusio_spi_obj_t *self = native_abusio_spi(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t out_bufinfo;
    mp_get_buffer_raise(args[ARG_out_buffer].u_obj, &out_bufinfo, MP_BUFFER_READ);
    mp_buffer_info_t in_bufinfo;
    mp_get_buffer_raise(args[ARG_in_buffer].u_obj, &in_bufinfo, MP_BUFFER_WRITE);

    int32_t out_start = args[ARG_out_start].u_int;
    size_t out_length = out_bufinfo.len;
    normalize_buffer_bounds(&out_start, args[ARG_out_end].u_int, &out_length);
    int32_t in_start = args[ARG_in_start].u_int;
    size_t in_length = in_bufinfo.len;
    normalize_buffer_bounds(&in_start, args[ARG_in_end].u_int, &in_length);

    if (out_length != in_length) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer slices must be same length"));
    }

    mp_obj_t out_slice = mp_obj_new_memoryview('B', out_length,
        (uint8_t *)out_bufinfo.buf + out_start);
    mp_obj_t in_slice = mp_obj_new_memoryview('B' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW, in_length,
        (uint8_t *)in_bufinfo.buf + in_start);
    mp_obj_t tuple_items[3] = { pos_args[0], out_slice, in_slice };
    mp_obj_t data = mp_obj_new_tuple(3, tuple_items);

    return circuitpy_awaitable_new(
        common_hal_abusio_spi_write_readinto_start,
        common_hal_abusio_spi_write_readinto_end,
        common_hal_abusio_spi_write_readinto_cancel,
        data);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(abusio_spi_write_readinto_obj, 2, abusio_spi_write_readinto);

// ---- frequency property -----------------------------------------------------

//|     frequency: int
//|     """The actual SPI bus frequency (read-only)."""
//|
static mp_obj_t abusio_spi_frequency_get(mp_obj_t self_in) {
    abusio_spi_obj_t *self = native_abusio_spi(self_in);
    check_for_deinit(self);
    return mp_obj_new_int_from_uint(common_hal_busio_spi_get_frequency(&self->spi));
}
MP_DEFINE_CONST_FUN_OBJ_1(abusio_spi_frequency_get_obj, abusio_spi_frequency_get);
MP_PROPERTY_GETTER(abusio_spi_frequency_obj, (mp_obj_t)&abusio_spi_frequency_get_obj);

// ---- type definition --------------------------------------------------------

static const mp_rom_map_elem_t abusio_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),        MP_ROM_PTR(&abusio_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__),       MP_ROM_PTR(&abusio_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),     MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),      MP_ROM_PTR(&default___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_configure),     MP_ROM_PTR(&abusio_spi_configure_obj) },
    { MP_ROM_QSTR(MP_QSTR_lock),          MP_ROM_PTR(&abusio_spi_lock_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlock),        MP_ROM_PTR(&abusio_spi_unlock_obj) },

    { MP_ROM_QSTR(MP_QSTR_write),         MP_ROM_PTR(&abusio_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),      MP_ROM_PTR(&abusio_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&abusio_spi_write_readinto_obj) },

    { MP_ROM_QSTR(MP_QSTR_frequency),     MP_ROM_PTR(&abusio_spi_frequency_obj) },
};
static MP_DEFINE_CONST_DICT(abusio_spi_locals_dict, abusio_spi_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    abusio_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, abusio_spi_make_new,
    locals_dict, &abusio_spi_locals_dict
    );

#endif // MICROPY_PY_ASYNC_AWAIT
