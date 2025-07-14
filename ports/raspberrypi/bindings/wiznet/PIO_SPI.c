// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

//TODO: busio.WIZNET_PIO_SPI class.
// This file contains all of the Python API definitions for the
// busio.WIZNET_PIO_SPI class.

#include <string.h>

#include "shared-bindings/microcontroller/Pin.h"
#include "bindings/wiznet/PIO_SPI.h"
#include "shared-bindings/util.h"

#include "shared/runtime/buffer_helper.h"
#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mperrno.h"
#include "py/objproperty.h"
#include "py/runtime.h"


//TODO: class WIZNET_PIO_SPI


static mp_obj_t wiznet_pio_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    #if CIRCUITPY_WIZNET
    wiznet_pio_spi_obj_t *self = mp_obj_malloc(wiznet_pio_spi_obj_t, &wiznet_pio_spi_type);
    enum { ARG_clock, ARG_MOSI, ARG_MISO, ARG_half_duplex };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_clock, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_MOSI, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_MISO, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_half_duplex, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj, MP_QSTR_clock);
    const mcu_pin_obj_t *mosi = validate_obj_is_free_pin_or_none(args[ARG_MOSI].u_obj, MP_QSTR_mosi);
    const mcu_pin_obj_t *miso = validate_obj_is_free_pin_or_none(args[ARG_MISO].u_obj, MP_QSTR_miso);

    if (!miso && !mosi) {
        mp_raise_ValueError(MP_ERROR_TEXT("Must provide MISO or MOSI pin"));
    }

    common_hal_wiznet_pio_spi_construct(self, clock, mosi, miso, args[ARG_half_duplex].u_bool);
    return MP_OBJ_FROM_PTR(self);
    #else
    mp_raise_NotImplementedError(NULL);
    #endif // CIRCUITPY_WIZNET
}

#if CIRCUITPY_WIZNET

// TODO: def deinit

static mp_obj_t wiznet_pio_spi_obj_deinit(mp_obj_t self_in) {
    wiznet_pio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_wiznet_pio_spi_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(wiznet_pio_spi_deinit_obj, wiznet_pio_spi_obj_deinit);

// TODO: def __enter__

// TODO: def __exit__

static void check_lock(wiznet_pio_spi_obj_t *self) {
    asm ("");
    if (!common_hal_wiznet_pio_spi_has_lock(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Function requires lock"));
    }
}

static void check_for_deinit(wiznet_pio_spi_obj_t *self) {
    if (common_hal_wiznet_pio_spi_deinited(self)) {
        raise_deinited_error();
    }
}

// TODO: def configure

static mp_obj_t wiznet_pio_spi_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
    };
    wiznet_pio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t polarity = (uint8_t)mp_arg_validate_int_range(args[ARG_polarity].u_int, 0, 1, MP_QSTR_polarity);
    uint8_t phase = (uint8_t)mp_arg_validate_int_range(args[ARG_phase].u_int, 0, 1, MP_QSTR_phase);
    uint8_t bits = (uint8_t)mp_arg_validate_int_range(args[ARG_bits].u_int, 8, 9, MP_QSTR_bits);

    if (!common_hal_wiznet_pio_spi_configure(self, args[ARG_baudrate].u_int,
        polarity, phase, bits)) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(wiznet_pio_spi_configure_obj, 1, wiznet_pio_spi_configure);

// TODO: def try_lock

static mp_obj_t wiznet_pio_spi_obj_try_lock(mp_obj_t self_in) {
    wiznet_pio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_wiznet_pio_spi_try_lock(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(wiznet_pio_spi_try_lock_obj, wiznet_pio_spi_obj_try_lock);

// TODO: def unlock

static mp_obj_t wiznet_pio_spi_obj_unlock(mp_obj_t self_in) {
    wiznet_pio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_wiznet_pio_spi_unlock(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(wiznet_pio_spi_unlock_obj, wiznet_pio_spi_obj_unlock);

// TODO: def write

static mp_obj_t wiznet_pio_spi_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };

    wiznet_pio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);
    // Compute bounds in terms of elements, not bytes.
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);
    int32_t start = args[ARG_start].u_int;
    size_t length = bufinfo.len / stride_in_bytes;
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);

    // Treat start and length in terms of bytes from now on.
    start *= stride_in_bytes;
    length *= stride_in_bytes;

    if (length == 0) {
        return mp_const_none;
    }

    bool ok = common_hal_wiznet_pio_spi_write(self, ((uint8_t *)bufinfo.buf) + start, length);

    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(wiznet_pio_spi_write_obj, 1, wiznet_pio_spi_write);

// TODO: def readinto

static mp_obj_t wiznet_pio_spi_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end, ARG_write_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_write_value, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    wiznet_pio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);
    // Compute bounds in terms of elements, not bytes.
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);
    int32_t start = args[ARG_start].u_int;
    size_t length = bufinfo.len / stride_in_bytes;
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);

    // Treat start and length in terms of bytes from now on.
    start *= stride_in_bytes;
    length *= stride_in_bytes;

    if (length == 0) {
        return mp_const_none;
    }

    bool ok = common_hal_wiznet_pio_spi_read(self, ((uint8_t *)bufinfo.buf) + start, length, args[ARG_write_value].u_int);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(wiznet_pio_spi_readinto_obj, 1, wiznet_pio_spi_readinto);


// TODO: def write_readinto

static mp_obj_t wiznet_pio_spi_write_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_out_buffer, ARG_in_buffer, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_out_buffer,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_in_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_out_start,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_out_end,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_in_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_in_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    wiznet_pio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t buf_out_info;
    mp_get_buffer_raise(args[ARG_out_buffer].u_obj, &buf_out_info, MP_BUFFER_READ);
    int out_stride_in_bytes = mp_binary_get_size('@', buf_out_info.typecode, NULL);
    int32_t out_start = args[ARG_out_start].u_int;
    size_t out_length = buf_out_info.len / out_stride_in_bytes;
    normalize_buffer_bounds(&out_start, args[ARG_out_end].u_int, &out_length);

    mp_buffer_info_t buf_in_info;
    mp_get_buffer_raise(args[ARG_in_buffer].u_obj, &buf_in_info, MP_BUFFER_WRITE);
    int in_stride_in_bytes = mp_binary_get_size('@', buf_in_info.typecode, NULL);
    int32_t in_start = args[ARG_in_start].u_int;
    size_t in_length = buf_in_info.len / in_stride_in_bytes;
    normalize_buffer_bounds(&in_start, args[ARG_in_end].u_int, &in_length);

    // Treat start and length in terms of bytes from now on.
    out_start *= out_stride_in_bytes;
    out_length *= out_stride_in_bytes;
    in_start *= in_stride_in_bytes;
    in_length *= in_stride_in_bytes;

    if (out_length != in_length) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer slices must be of equal length"));
    }

    if (out_length == 0) {
        return mp_const_none;
    }

    bool ok = common_hal_wiznet_pio_spi_transfer(self,
        ((uint8_t *)buf_out_info.buf) + out_start,
        ((uint8_t *)buf_in_info.buf) + in_start,
        out_length);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(wiznet_pio_spi_write_readinto_obj, 1, wiznet_pio_spi_write_readinto);

#endif // CIRCUITPY_WIZNET

static const mp_rom_map_elem_t wiznet_pio_spi_locals_dict_table[] = {
    #if CIRCUITPY_WIZNET
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&wiznet_pio_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_configure), MP_ROM_PTR(&wiznet_pio_spi_configure_obj) },
    { MP_ROM_QSTR(MP_QSTR_try_lock), MP_ROM_PTR(&wiznet_pio_spi_try_lock_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlock), MP_ROM_PTR(&wiznet_pio_spi_unlock_obj) },

    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&wiznet_pio_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&wiznet_pio_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&wiznet_pio_spi_write_readinto_obj) },

    #endif // CIRCUITPY_WIZNET
};
static MP_DEFINE_CONST_DICT(wiznet_pio_spi_locals_dict, wiznet_pio_spi_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    wiznet_pio_spi_type,
    MP_QSTR_PIO_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, wiznet_pio_spi_make_new,
    locals_dict, &wiznet_pio_spi_locals_dict
);

wiznet_pio_spi_obj_t *validate_obj_is_wiznet_pio_spi_bus(mp_obj_t obj, qstr arg_name) {
    return mp_arg_validate_type(obj, &wiznet_pio_spi_type, arg_name);
}
