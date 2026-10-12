// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "shared/runtime/context_manager_helpers.h"
#include "shared-bindings/usb/core/Device.h"
#include "shared-bindings/usb_host_bulk/InStream.h"
#include "shared-bindings/util.h"

//| import usb.core
//|
//|
//| class InStream:
//|     """Continuously receive from a full-speed bulk IN endpoint in the background.
//|
//|     The USB host polls the endpoint every frame, independently of Python, and stores
//|     the payload in a ring of ``buffer_size`` bytes in internal SRAM.
//|
//|     Reads never block. To wait for data, wrap the stream with ``asyncio.StreamReader``
//|     or poll `in_waiting`.
//|
//|     One stream per endpoint can run. They share the bandwidth of
//|     their host port.
//|
//|     Synchronous example::
//|
//|         import time
//|         import usb.core
//|         import usb_host_bulk
//|
//|         # Replace these with your device's vendor and product IDs.
//|         device = usb.core.find(idVendor=0x0BDA, idProduct=0x2838)
//|         if device is None:
//|             raise RuntimeError("USB device not found")
//|         device.set_configuration()
//|         # Many devices only start sending after device-specific setup, such as
//|         # vendor requests made with device.ctrl_transfer(). Do that here.
//|
//|         buf = bytearray(8192)
//|         received = 0
//|         report_at = time.monotonic() + 1
//|         with usb_host_bulk.InStream(device, 0x81) as stream:
//|             while True:
//|                 n = stream.readinto(buf)  # None while nothing is waiting
//|                 if n == 0:
//|                     break  # the device stalled or was unplugged
//|                 if n:
//|                     received += n  # process buf[:n] here
//|                 if time.monotonic() >= report_at:
//|                     print("received", received, "bytes, lost packets:", stream.lost_packets)
//|                     report_at += 1
//|
//|     asyncio example::
//|
//|         import asyncio
//|         from asyncio import StreamReader
//|         import usb.core
//|         import usb_host_bulk
//|
//|         # Replace these with your device's vendor and product IDs.
//|         device = usb.core.find(idVendor=0x0BDA, idProduct=0x2838)
//|         if device is None:
//|             raise RuntimeError("USB device not found")
//|         device.set_configuration()
//|         # Many devices only start sending after device-specific setup, such as
//|         # vendor requests made with device.ctrl_transfer(). Do that here.
//|
//|         received = 0
//|
//|         async def receive(stream):
//|             global received
//|             reader = StreamReader(stream)
//|             buf = bytearray(8192)
//|             while True:
//|                 n = await reader.readinto(buf)
//|                 if not n:
//|                     break  # the device stalled or was unplugged
//|                 received += n  # process buf[:n] here
//|
//|         async def report(stream):
//|             while True:
//|                 await asyncio.sleep(1)
//|                 print("received", received, "bytes, lost packets:", stream.lost_packets)
//|
//|         async def main():
//|             with usb_host_bulk.InStream(device, 0x81) as stream:
//|                 reporter = asyncio.create_task(report(stream))
//|                 await receive(stream)
//|                 reporter.cancel()
//|
//|         asyncio.run(main())
//|
//|     ``asyncio.StreamReader(stream)`` is a CircuitPython and MicroPython idiom. Import
//|     ``StreamReader`` before starting the stream: asyncio loads it on first use, which
//|     can take long enough to overflow the ring. Use ``readinto`` rather than
//|     ``readexactly`` at high data rates, because ``readexactly`` allocates on every
//|     call.
//|     """
//|
//|     def __init__(
//|         self, device: usb.core.Device, endpoint: int, *, buffer_size: int = 32768
//|     ) -> None:
//|         """Open ``endpoint`` on ``device`` and start capturing immediately.
//|
//|         :param usb.core.Device device: a configured device (call ``set_configuration()`` first)
//|         :param int endpoint: bEndpointAddress of a bulk IN endpoint, 0x81 to 0x8F, with
//|           a maximum packet size of 64 bytes or less
//|         :param int buffer_size: ring size in bytes, a power of two from 4096 to 65536
//|         """
//|         ...
//|
static mp_obj_t usb_host_bulk_instream_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_device, ARG_endpoint, ARG_buffer_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_device, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_endpoint, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_buffer_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 32768} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t device_obj = mp_arg_validate_type(args[ARG_device].u_obj, &usb_core_device_type, MP_QSTR_device);
    usb_core_device_obj_t *device = MP_OBJ_TO_PTR(device_obj);
    if (common_hal_usb_core_device_deinited(device)) {
        raise_deinited_error();
    }
    // IN endpoint addresses 1-15; endpoint 0 is control only.
    mp_int_t endpoint = mp_arg_validate_int_range(args[ARG_endpoint].u_int, 0x81, 0x8F, MP_QSTR_endpoint);
    mp_int_t buffer_size = mp_arg_validate_int_range(args[ARG_buffer_size].u_int, 4096, 65536, MP_QSTR_buffer_size);
    if ((buffer_size & (buffer_size - 1)) != 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be power of 2"), MP_QSTR_buffer_size);
    }

    usb_host_bulk_instream_obj_t *self = mp_obj_malloc_with_finaliser(usb_host_bulk_instream_obj_t, &usb_host_bulk_instream_type);
    common_hal_usb_host_bulk_instream_construct(self, device, endpoint, buffer_size);
    return MP_OBJ_FROM_PTR(self);
}

static void check_for_deinit(usb_host_bulk_instream_obj_t *self) {
    if (common_hal_usb_host_bulk_instream_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def readinto(self, buf: WriteableBuffer, nbytes: Optional[int] = None) -> Optional[int]:
//|         """Copy waiting bytes into ``buf``, at most ``nbytes`` of them if given.
//|
//|         Capture ends when the stream is deinited, which drops any bytes not yet read.
//|         It also ends on its own when the device stalls the endpoint, is unplugged, or is
//|         reconfigured. Bytes captured before that can still be read.
//|
//|         :return: the number of bytes copied, ``None`` if nothing is waiting yet, or 0
//|           once capture has ended and nothing is left to read
//|         :rtype: int or None"""
//|         ...
//|
//|     def read(self, nbytes: Optional[int] = None) -> Optional[bytes]:
//|         """Same as `readinto`, but allocates and returns ``bytes``: ``None`` if nothing
//|         is waiting yet, ``b""`` once capture has ended and nothing is left to read."""
//|         ...
//|

// Standard stream methods, implemented in py/stream.c on top of this.
static mp_uint_t usb_host_bulk_instream_read_stream(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    usb_host_bulk_instream_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // Look at deinited first: a stream seen deinited here has all of its data
    // visible below, so an empty read means the end.
    bool deinited = common_hal_usb_host_bulk_instream_deinited(self);
    uint32_t count = common_hal_usb_host_bulk_instream_read(self, buf, size);
    if (count > 0 || deinited) {
        return count;
    }
    *errcode = MP_EAGAIN;
    return MP_STREAM_ERROR;
}

static mp_uint_t usb_host_bulk_instream_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    usb_host_bulk_instream_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (request == MP_STREAM_CLOSE) {
        common_hal_usb_host_bulk_instream_deinit(self);
        return 0;
    }
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        mp_uint_t ret = 0;
        // A deinited stream is readable (it returns EOF) so that waiters wake up.
        bool deinited = common_hal_usb_host_bulk_instream_deinited(self);
        if ((flags & MP_STREAM_POLL_RD) &&
            (deinited || common_hal_usb_host_bulk_instream_get_in_waiting(self) > 0)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if (deinited) {
            ret |= MP_STREAM_POLL_HUP;
        }
        return ret;
    }
    *errcode = MP_EINVAL;
    return MP_STREAM_ERROR;
}

//|     in_waiting: int
//|     """Bytes waiting to be read, including those left when capture ended on its own.
//|     (read-only)"""
//|
static mp_obj_t usb_host_bulk_instream_obj_get_in_waiting(mp_obj_t self_in) {
    usb_host_bulk_instream_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(common_hal_usb_host_bulk_instream_get_in_waiting(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_host_bulk_instream_get_in_waiting_obj, usb_host_bulk_instream_obj_get_in_waiting);

MP_PROPERTY_GETTER(usb_host_bulk_instream_in_waiting_obj,
    (mp_obj_t)&usb_host_bulk_instream_get_in_waiting_obj);

//|     lost_packets: int
//|     """Packets dropped because the ring was full, meaning reads were not frequent
//|     enough. Zero means the captured stream has no gaps. Readable after `deinit`.
//|     (read-only)"""
//|
static mp_obj_t usb_host_bulk_instream_obj_get_lost_packets(mp_obj_t self_in) {
    usb_host_bulk_instream_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(common_hal_usb_host_bulk_instream_get_lost_packets(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_host_bulk_instream_get_lost_packets_obj, usb_host_bulk_instream_obj_get_lost_packets);

MP_PROPERTY_GETTER(usb_host_bulk_instream_lost_packets_obj,
    (mp_obj_t)&usb_host_bulk_instream_get_lost_packets_obj);

//|     def reset_input_buffer(self) -> None:
//|         """Discard every byte waiting in the ring. Raises `ValueError` once the stream
//|         is deinited, because no more data will come."""
//|         ...
//|
static mp_obj_t usb_host_bulk_instream_obj_reset_input_buffer(mp_obj_t self_in) {
    usb_host_bulk_instream_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_usb_host_bulk_instream_reset_input_buffer(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_host_bulk_instream_reset_input_buffer_obj, usb_host_bulk_instream_obj_reset_input_buffer);

//|     def deinit(self) -> None:
//|         """Stop capture and free the ring, dropping any bytes not yet read."""
//|         ...
//|
static mp_obj_t usb_host_bulk_instream_obj_deinit(mp_obj_t self_in) {
    usb_host_bulk_instream_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_usb_host_bulk_instream_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_host_bulk_instream_deinit_obj, usb_host_bulk_instream_obj_deinit);

//|     def __enter__(self) -> InStream:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
//|
//  Provided by context manager helper.

static const mp_rom_map_elem_t usb_host_bulk_instream_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&usb_host_bulk_instream_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&usb_host_bulk_instream_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },

    // Standard stream methods.
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },

    { MP_ROM_QSTR(MP_QSTR_reset_input_buffer), MP_ROM_PTR(&usb_host_bulk_instream_reset_input_buffer_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_in_waiting), MP_ROM_PTR(&usb_host_bulk_instream_in_waiting_obj) },
    { MP_ROM_QSTR(MP_QSTR_lost_packets), MP_ROM_PTR(&usb_host_bulk_instream_lost_packets_obj) },
};
static MP_DEFINE_CONST_DICT(usb_host_bulk_instream_locals_dict, usb_host_bulk_instream_locals_dict_table);

static const mp_stream_p_t usb_host_bulk_instream_stream_p = {
    .read = usb_host_bulk_instream_read_stream,
    .ioctl = usb_host_bulk_instream_ioctl,
    .is_text = false,
};

MP_DEFINE_CONST_OBJ_TYPE(
    usb_host_bulk_instream_type,
    MP_QSTR_InStream,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, usb_host_bulk_instream_make_new,
    locals_dict, &usb_host_bulk_instream_locals_dict,
    protocol, &usb_host_bulk_instream_stream_p
    );
