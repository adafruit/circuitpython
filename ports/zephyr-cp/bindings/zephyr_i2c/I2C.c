// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/zephyr_i2c/I2C.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "shared/runtime/buffer_helper.h"
#include "py/binary.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include <zephyr/device.h>
#include <zephyr/devicetree.h>

//| class I2C:
//|     """Two wire serial protocol
//|
//|     I2C is a two-wire protocol for communicating between devices. At the
//|     physical level it consists of 2 wires: SCL and SDA, the clock and data
//|     lines respectively.
//|
//|     .. class:: I2C()
//|
//|       Cannot be instantiated directly. Instead singletons are created using the
//|       ``board`` aliases that match the device tree labels. ``board`` may list multiple
//|       aliases for a single device. For example, ``board.I2C1`` and ``board.ARDUINO_I2C``
//|       may both refer to the same device.
//|     """
//|

static zephyr_i2c_i2c_obj_t *native_i2c(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &zephyr_i2c_i2c_type));
    return MP_OBJ_TO_PTR(self_in);
}

static void check_for_deinit(zephyr_i2c_i2c_obj_t *self) {
    if (common_hal_zephyr_i2c_i2c_deinited(self)) {
        raise_deinited_error();
    }
}

static void check_lock(zephyr_i2c_i2c_obj_t *self) {
    if (!common_hal_zephyr_i2c_i2c_has_lock(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Function requires lock"));
    }
}

//|     def deinit(self) -> None:
//|         """Releases control of the underlying hardware so other classes can use it."""
//|         ...
static mp_obj_t zephyr_i2c_i2c_obj_deinit(mp_obj_t self_in) {
    zephyr_i2c_i2c_obj_t *self = native_i2c(self_in);
    common_hal_zephyr_i2c_i2c_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_i2c_i2c_deinit_obj, zephyr_i2c_i2c_obj_deinit);

//|     def __enter__(self) -> I2C:
//|         """No-op used in Context Managers."""
//|         ...
static mp_obj_t zephyr_i2c_i2c_obj___enter__(mp_obj_t self_in) {
    zephyr_i2c_i2c_obj_t *self = native_i2c(self_in);
    check_for_deinit(self);
    return self_in;
}
static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_i2c_i2c___enter___obj, zephyr_i2c_i2c_obj___enter__);

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware on context exit."""
//|         ...
static mp_obj_t zephyr_i2c_i2c_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_zephyr_i2c_i2c_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_i2c_i2c___exit___obj, 4, 4, zephyr_i2c_i2c_obj___exit__);

//|     def probe(self, address: int) -> bool:
//|         """Check if a device responds at the address.
//|
//|         :param int address: The 7-bit device address
//|         :return: True if the device responds, False otherwise
//|         :rtype: bool
//|         """
//|         ...
static mp_obj_t zephyr_i2c_i2c_probe(mp_obj_t self_in, mp_obj_t addr_obj) {
    zephyr_i2c_i2c_obj_t *self = native_i2c(self_in);
    check_for_deinit(self);
    check_lock(self);

    mp_int_t addr = mp_obj_get_int(addr_obj);
    return mp_obj_new_bool(common_hal_zephyr_i2c_i2c_probe(self, addr));
}
static MP_DEFINE_CONST_FUN_OBJ_2(zephyr_i2c_i2c_probe_obj, zephyr_i2c_i2c_probe);

//|     def scan(self) -> List[int]:
//|         """Scan all I2C addresses between 0x08 and 0x77 inclusive and return a
//|         list of those that respond.
//|
//|         :return: List of device addresses
//|         :rtype: list
//|         """
//|         ...
static mp_obj_t zephyr_i2c_i2c_scan(mp_obj_t self_in) {
    zephyr_i2c_i2c_obj_t *self = native_i2c(self_in);
    check_for_deinit(self);
    check_lock(self);

    mp_obj_t list = mp_obj_new_list(0, NULL);
    // 7-bit addresses 0b0000xxx and 0b1111xxx are reserved
    for (uint8_t addr = 0x08; addr <= 0x77; addr++) {
        if (common_hal_zephyr_i2c_i2c_probe(self, addr)) {
            mp_obj_list_append(list, MP_OBJ_NEW_SMALL_INT(addr));
        }
    }
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_i2c_i2c_scan_obj, zephyr_i2c_i2c_scan);

//|     def try_lock(self) -> bool:
//|         """Attempts to grab the I2C lock. Returns True on success.
//|
//|         :return: True when lock has been grabbed
//|         :rtype: bool
//|         """
//|         ...
static mp_obj_t zephyr_i2c_i2c_obj_try_lock(mp_obj_t self_in) {
    zephyr_i2c_i2c_obj_t *self = native_i2c(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_zephyr_i2c_i2c_try_lock(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_i2c_i2c_try_lock_obj, zephyr_i2c_i2c_obj_try_lock);

//|     def unlock(self) -> None:
//|         """Releases the I2C lock."""
//|         ...
static mp_obj_t zephyr_i2c_i2c_obj_unlock(mp_obj_t self_in) {
    zephyr_i2c_i2c_obj_t *self = native_i2c(self_in);
    check_for_deinit(self);
    common_hal_zephyr_i2c_i2c_unlock(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_i2c_i2c_unlock_obj, zephyr_i2c_i2c_obj_unlock);

//|     import sys
//|
//|     def readfrom_into(
//|         self, address: int, buffer: WriteableBuffer, *, start: int = 0, end: int = sys.maxsize
//|     ) -> None:
//|         """Read into ``buffer`` from the device selected by ``address``.
//|         At least one byte must be read.
//|
//|         If ``start`` or ``end`` is provided, then the buffer will be sliced
//|         as if ``buffer[start:end]`` were passed, but without copying the data.
//|         The number of bytes read will be the length of ``buffer[start:end]``.
//|
//|         :param int address: 7-bit device address
//|         :param ~circuitpython_typing.WriteableBuffer buffer: buffer to write into
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``
//|         """
//|         ...
static mp_obj_t zephyr_i2c_i2c_readfrom_into(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_address, ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    zephyr_i2c_i2c_obj_t *self = native_i2c(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);

    // Compute bounds in terms of elements, not bytes.
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);
    size_t length = bufinfo.len / stride_in_bytes;
    int32_t start = args[ARG_start].u_int;
    const int32_t end = args[ARG_end].u_int;
    normalize_buffer_bounds(&start, end, &length);
    mp_arg_validate_length_min(length, 1, MP_QSTR_buffer);

    // Treat start and length in terms of bytes from now on.
    start *= stride_in_bytes;
    length *= stride_in_bytes;

    uint8_t status = common_hal_zephyr_i2c_i2c_read(self, args[ARG_address].u_int,
        ((uint8_t *)bufinfo.buf) + start, length);

    if (status != 0) {
        mp_raise_OSError(status);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(zephyr_i2c_i2c_readfrom_into_obj, 3, zephyr_i2c_i2c_readfrom_into);

//|     import sys
//|
//|     def writeto(
//|         self, address: int, buffer: ReadableBuffer, *, start: int = 0, end: int = sys.maxsize
//|     ) -> None:
//|         """Write the bytes from ``buffer`` to the device selected by ``address`` and
//|         then transmit a stop bit.
//|
//|         If ``start`` or ``end`` is provided, then the buffer will be sliced
//|         as if ``buffer[start:end]`` were passed, but without copying the data.
//|         The number of bytes written will be the length of ``buffer[start:end]``.
//|
//|         Writing a buffer or slice of length zero is permitted, as it can be used
//|         to poll for the existence of a device.
//|
//|         :param int address: 7-bit device address
//|         :param ~circuitpython_typing.ReadableBuffer buffer: buffer containing the bytes to write
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``
//|         """
//|         ...
static mp_obj_t zephyr_i2c_i2c_writeto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_address, ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    zephyr_i2c_i2c_obj_t *self = native_i2c(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to write the data from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);

    // Compute bounds in terms of elements, not bytes.
    size_t length = bufinfo.len / stride_in_bytes;
    int32_t start = args[ARG_start].u_int;
    const int32_t end = args[ARG_end].u_int;
    normalize_buffer_bounds(&start, end, &length);

    // Treat start and length in terms of bytes from now on.
    start *= stride_in_bytes;
    length *= stride_in_bytes;

    uint8_t status = common_hal_zephyr_i2c_i2c_write(self, args[ARG_address].u_int,
        ((uint8_t *)bufinfo.buf) + start, length);

    if (status != 0) {
        mp_raise_OSError(status);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(zephyr_i2c_i2c_writeto_obj, 3, zephyr_i2c_i2c_writeto);

//|     import sys
//|
//|     def writeto_then_readfrom(
//|         self,
//|         address: int,
//|         out_buffer: ReadableBuffer,
//|         in_buffer: WriteableBuffer,
//|         *,
//|         out_start: int = 0,
//|         out_end: int = None,
//|         in_start: int = 0,
//|         in_end: int = None
//|     ) -> None:
//|         """Write the bytes from ``out_buffer`` to the device selected by ``address``, generate
//|         no stop bit, generate a repeated start and read into ``in_buffer``. ``out_buffer`` and
//|         ``in_buffer`` can be the same buffer because they are used sequentially.
//|
//|         If ``out_start`` or ``out_end`` is provided, then the buffer will be sliced
//|         as if ``out_buffer[out_start:out_end]`` were passed, but without copying the data.
//|         The number of bytes written will be the length of ``out_buffer[start:end]``.
//|
//|         If ``in_start`` or ``in_end`` is provided, then the input buffer will be sliced
//|         as if ``in_buffer[in_start:in_end]`` were passed,
//|         The number of bytes read will be the length of ``out_buffer[in_start:in_end]``.
//|
//|         :param int address: 7-bit device address
//|         :param ~circuitpython_typing.ReadableBuffer out_buffer: buffer containing the bytes to write
//|         :param ~circuitpython_typing.WriteableBuffer in_buffer: buffer to write into
//|         :param int out_start: beginning of ``out_buffer`` slice
//|         :param int out_end: end of ``out_buffer`` slice; if not specified, use ``len(out_buffer)``
//|         :param int in_start: beginning of ``in_buffer`` slice
//|         :param int in_end: end of ``in_buffer`` slice; if not specified, use ``len(in_buffer)``
//|         """
//|         ...
static mp_obj_t zephyr_i2c_i2c_writeto_then_readfrom(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_address, ARG_out_buffer, ARG_in_buffer, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_address, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_out_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_in_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_out_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_out_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_in_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_in_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    zephyr_i2c_i2c_obj_t *self = native_i2c(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t out_bufinfo;
    mp_get_buffer_raise(args[ARG_out_buffer].u_obj, &out_bufinfo, MP_BUFFER_READ);
    int out_stride_in_bytes = mp_binary_get_size('@', out_bufinfo.typecode, NULL);
    size_t out_length = out_bufinfo.len / out_stride_in_bytes;
    int32_t out_start = args[ARG_out_start].u_int;
    const int32_t out_end = args[ARG_out_end].u_int;
    normalize_buffer_bounds(&out_start, out_end, &out_length);

    mp_buffer_info_t in_bufinfo;
    mp_get_buffer_raise(args[ARG_in_buffer].u_obj, &in_bufinfo, MP_BUFFER_WRITE);
    int in_stride_in_bytes = mp_binary_get_size('@', in_bufinfo.typecode, NULL);
    size_t in_length = in_bufinfo.len / in_stride_in_bytes;
    int32_t in_start = args[ARG_in_start].u_int;
    const int32_t in_end = args[ARG_in_end].u_int;
    normalize_buffer_bounds(&in_start, in_end, &in_length);
    mp_arg_validate_length_min(in_length, 1, MP_QSTR_out_buffer);

    // Treat start and length in terms of bytes from now on.
    out_start *= out_stride_in_bytes;
    out_length *= out_stride_in_bytes;
    in_start *= in_stride_in_bytes;
    in_length *= in_stride_in_bytes;

    uint8_t status = common_hal_zephyr_i2c_i2c_write_read(self, args[ARG_address].u_int,
        ((uint8_t *)out_bufinfo.buf) + out_start, out_length,
        ((uint8_t *)in_bufinfo.buf) + in_start, in_length);

    if (status != 0) {
        mp_raise_OSError(status);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(zephyr_i2c_i2c_writeto_then_readfrom_obj, 4, zephyr_i2c_i2c_writeto_then_readfrom);

static const mp_rom_map_elem_t zephyr_i2c_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&zephyr_i2c_i2c_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&zephyr_i2c_i2c_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&zephyr_i2c_i2c___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&zephyr_i2c_i2c___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_probe), MP_ROM_PTR(&zephyr_i2c_i2c_probe_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&zephyr_i2c_i2c_scan_obj) },

    { MP_ROM_QSTR(MP_QSTR_try_lock), MP_ROM_PTR(&zephyr_i2c_i2c_try_lock_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlock), MP_ROM_PTR(&zephyr_i2c_i2c_unlock_obj) },

    { MP_ROM_QSTR(MP_QSTR_readfrom_into), MP_ROM_PTR(&zephyr_i2c_i2c_readfrom_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto), MP_ROM_PTR(&zephyr_i2c_i2c_writeto_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto_then_readfrom), MP_ROM_PTR(&zephyr_i2c_i2c_writeto_then_readfrom_obj) },
};
static MP_DEFINE_CONST_DICT(zephyr_i2c_i2c_locals_dict, zephyr_i2c_i2c_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_i2c_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    locals_dict, &zephyr_i2c_i2c_locals_dict
    );
