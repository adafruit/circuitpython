// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "bindings/zephyr_serial/UART.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

#include "shared/runtime/context_manager_helpers.h"
#include "shared/runtime/interrupt_char.h"

#include "py/stream.h"
#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "py/stream.h"

#define STREAM_DEBUG(...) (void)0
// #define STREAM_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)

//| class UART:
//|     """A bidirectional serial protocol. Already initialized for Zephyr defined
//|        busses in :py:mod:`board`.
//|
//|     .. raw:: html
//|
//|         <p>
//|         <details>
//|         <summary>Available on these boards</summary>
//|         <ul>
//|         {% for board in support_matrix_reverse["zephyr_serial.UART"] %}
//|         <li> {{ board }}
//|         {% endfor %}
//|         </ul>
//|         </details>
//|         </p>
//|
//|     """
//|

static void validate_timeout(mp_float_t timeout) {
    mp_arg_validate_int_range((int)timeout, 0, 100, MP_QSTR_timeout);
}

// Helper to ensure we have the native super class instead of a subclass.
static zephyr_serial_uart_obj_t *native_uart(mp_obj_t uart_obj) {
    mp_obj_t native_uart = mp_obj_cast_to_native_base(uart_obj, MP_OBJ_FROM_PTR(&zephyr_serial_uart_type));
    if (native_uart == MP_OBJ_NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Must be a %q subclass."), MP_QSTR_UART);
    }
    mp_obj_assert_native_inited(native_uart);
    return MP_OBJ_TO_PTR(native_uart);
}


//|     def deinit(self) -> None:
//|         """Deinitialises the UART and releases any hardware resources for reuse."""
//|         ...
//|
static mp_obj_t _zephyr_serial_uart_obj_deinit(mp_obj_t self_in) {
    zephyr_serial_uart_obj_t *self = native_uart(self_in);
    zephyr_serial_uart_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(_zephyr_serial_uart_deinit_obj, _zephyr_serial_uart_obj_deinit);

static void check_for_deinit(zephyr_serial_uart_obj_t *self) {
    if (zephyr_serial_uart_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> UART:
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

// These are standard stream methods. Code is in py/stream.c.
//
//|     def read(self, nbytes: Optional[int] = None) -> Optional[bytes]:
//|         """Read bytes.  If ``nbytes`` is specified then read at most that many
//|         bytes. Otherwise, read everything that arrives until the connection
//|         times out. Providing the number of bytes expected is highly recommended
//|         because it will be faster. If no bytes are read, return ``None``.
//|
//|         .. note:: When no bytes are read due to a timeout, this function returns ``None``.
//|           This matches the behavior of `io.RawIOBase.read` in Python 3, but
//|           differs from pyserial which returns ``b''`` in that situation.
//|
//|         :return: Data read
//|         :rtype: bytes or None"""
//|         ...
//|

//|     def readinto(self, buf: WriteableBuffer) -> Optional[int]:
//|         """Read bytes into the ``buf``. Read at most ``len(buf)`` bytes.
//|
//|         :return: number of bytes read and stored into ``buf``
//|         :rtype: int or None (on a non-blocking error)
//|
//|         *New in CircuitPython 4.0:* No length parameter is permitted."""
//|         ...
//|

//|     def readline(self) -> bytes:
//|         """Read a line, ending in a newline character, or
//|         return ``None`` if a timeout occurs sooner, or
//|         return everything readable if no newline is found and
//|         ``timeout=0``
//|
//|         :return: the line read
//|         :rtype: bytes or None"""
//|         ...
//|

//|     def write(self, buf: ReadableBuffer) -> Optional[int]:
//|         """Write the buffer of bytes to the bus.
//|
//|         *New in CircuitPython 4.0:* ``buf`` must be bytes, not a string.
//|
//|           :return: the number of bytes written
//|           :rtype: int or None"""
//|         ...
//|

// These three methods are used by the shared stream methods.
static mp_uint_t _zephyr_serial_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    STREAM_DEBUG("_zephyr_serial_uart_read stream %d\n", size);
    zephyr_serial_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    byte *buf = buf_in;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    return zephyr_serial_uart_read(self, buf, size, errcode);
}

static mp_uint_t _zephyr_serial_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    zephyr_serial_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    const byte *buf = buf_in;

    return zephyr_serial_uart_write(self, buf, size, errcode);
}

static mp_uint_t _zephyr_serial_uart_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    zephyr_serial_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && zephyr_serial_uart_rx_characters_available(self) > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && zephyr_serial_uart_ready_to_tx(self)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

//|     baudrate: int
//|     """The current baudrate."""
static mp_obj_t _zephyr_serial_uart_obj_get_baudrate(mp_obj_t self_in) {
    zephyr_serial_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(zephyr_serial_uart_get_baudrate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(_zephyr_serial_uart_get_baudrate_obj, _zephyr_serial_uart_obj_get_baudrate);

static mp_obj_t _zephyr_serial_uart_obj_set_baudrate(mp_obj_t self_in, mp_obj_t baudrate) {
    zephyr_serial_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    zephyr_serial_uart_set_baudrate(self, mp_obj_get_int(baudrate));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(_zephyr_serial_uart_set_baudrate_obj, _zephyr_serial_uart_obj_set_baudrate);


MP_PROPERTY_GETSET(_zephyr_serial_uart_baudrate_obj,
    (mp_obj_t)&_zephyr_serial_uart_get_baudrate_obj,
    (mp_obj_t)&_zephyr_serial_uart_set_baudrate_obj);

//|     in_waiting: int
//|     """The number of bytes in the input buffer, available to be read"""
static mp_obj_t _zephyr_serial_uart_obj_get_in_waiting(mp_obj_t self_in) {
    zephyr_serial_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(zephyr_serial_uart_rx_characters_available(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(_zephyr_serial_uart_get_in_waiting_obj, _zephyr_serial_uart_obj_get_in_waiting);

MP_PROPERTY_GETTER(_zephyr_serial_uart_in_waiting_obj,
    (mp_obj_t)&_zephyr_serial_uart_get_in_waiting_obj);

//|     timeout: float
//|     """The current timeout, in seconds (float)."""
//|
static mp_obj_t _zephyr_serial_uart_obj_get_timeout(mp_obj_t self_in) {
    zephyr_serial_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(zephyr_serial_uart_get_timeout(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(_zephyr_serial_uart_get_timeout_obj, _zephyr_serial_uart_obj_get_timeout);

static mp_obj_t _zephyr_serial_uart_obj_set_timeout(mp_obj_t self_in, mp_obj_t timeout) {
    zephyr_serial_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    mp_float_t timeout_float = mp_obj_get_float(timeout);
    validate_timeout(timeout_float);
    zephyr_serial_uart_set_timeout(self, timeout_float);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(_zephyr_serial_uart_set_timeout_obj, _zephyr_serial_uart_obj_set_timeout);


MP_PROPERTY_GETSET(_zephyr_serial_uart_timeout_obj,
    (mp_obj_t)&_zephyr_serial_uart_get_timeout_obj,
    (mp_obj_t)&_zephyr_serial_uart_set_timeout_obj);

//|     def reset_input_buffer(self) -> None:
//|         """Discard any unread characters in the input buffer."""
//|         ...
//|
//|
static mp_obj_t _zephyr_serial_uart_obj_reset_input_buffer(mp_obj_t self_in) {
    zephyr_serial_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    zephyr_serial_uart_clear_rx_buffer(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(_zephyr_serial_uart_reset_input_buffer_obj, _zephyr_serial_uart_obj_reset_input_buffer);

static const mp_rom_map_elem_t _zephyr_serial_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__),      MP_ROM_PTR(&_zephyr_serial_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),       MP_ROM_PTR(&_zephyr_serial_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),    MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),     MP_ROM_PTR(&default___exit___obj) },

    // Standard stream methods.
    { MP_ROM_QSTR(MP_QSTR_read),     MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),    MP_ROM_PTR(&mp_stream_write_obj) },

    { MP_ROM_QSTR(MP_QSTR_reset_input_buffer), MP_ROM_PTR(&_zephyr_serial_uart_reset_input_buffer_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_baudrate),     MP_ROM_PTR(&_zephyr_serial_uart_baudrate_obj) },
    { MP_ROM_QSTR(MP_QSTR_in_waiting),   MP_ROM_PTR(&_zephyr_serial_uart_in_waiting_obj) },
    { MP_ROM_QSTR(MP_QSTR_timeout),      MP_ROM_PTR(&_zephyr_serial_uart_timeout_obj) },

};
static MP_DEFINE_CONST_DICT(_zephyr_serial_uart_locals_dict, _zephyr_serial_uart_locals_dict_table);

static const mp_stream_p_t uart_stream_p = {
    .read = _zephyr_serial_uart_read,
    .write = _zephyr_serial_uart_write,
    .ioctl = _zephyr_serial_uart_ioctl,
    .is_text = false,
    // Disallow optional length argument for .readinto()
    .pyserial_readinto_compatibility = true,
};

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_serial_uart_type,
    MP_QSTR_UART,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT | MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &_zephyr_serial_uart_locals_dict,
    iter, mp_stream_unbuffered_iter,
    protocol, &uart_stream_p
    );
