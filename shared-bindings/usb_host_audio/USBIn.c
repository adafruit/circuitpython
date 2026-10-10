// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "extmod/vfs_fat.h"
#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-bindings/usb/core/Device.h"
#include "shared-bindings/usb_host/Port.h"
#include "shared-bindings/usb_host_audio/USBIn.h"
#include "shared-bindings/util.h"
//| import usb.core
//|
//|
//| class USBIn:
//|     """Stream or record the audio of a USB microphone on a USB host port."""
//|
//|     def __init__(
//|         self,
//|         device: usb.core.Device,
//|         endpoint: int,
//|         *,
//|         sample_rate: int,
//|         channel_count: int = 1,
//|         bit_depth: int = 16,
//|         samples_signed: bool = True,
//|         max_packet_size: int = 0,
//|         buffer_frames: int = 256,
//|     ) -> None:
//|         """Receive the PCM audio that a USB Audio Class microphone sends on an
//|         isochronous IN endpoint. The object can be played like any other audio
//|         sample, which makes the microphone the source of an audio effects
//|         chain, and it can `record` to a buffer.
//|
//|         The constructor does no control transfers. Before calling it, select
//|         the alternate setting of the streaming interface that has ``endpoint``
//|         and set the sample rate; the arguments describe the format chosen
//|         that way. To stop, select alternate setting 0 and then call `deinit`.
//|         Do not create a `USBIn` for an interface that is in alternate setting 0.
//|
//|         The host controller buffers the stream between reads, in a buffer
//|         sized by ``CIRCUITPY_USB_HOST_ISO_BUFFER_SIZE`` in ``settings.toml``.
//|         Without that setting the constructor raises ``RuntimeError``.
//|
//|         Only one isochronous IN endpoint is received at a time, so only one
//|         `USBIn` can stream, and a USB camera cannot stream video meanwhile.
//|         Creating a second one raises ``RuntimeError`` while the first is
//|         `streaming`, and deinitializes the first when its stream has ended.
//|
//|         The microphone's clock is independent of the clock of the audio
//|         output. While playing, the drift between them is taken up by
//|         stretching a buffer by one frame now and then, which cannot be heard.
//|         Only when playback stalls is audio dropped, which sets `overflow`, or a
//|         moment of silence played. About ``3 * buffer_frames`` frames of
//|         latency are added.
//|
//|         :param ~usb.core.Device device: The microphone. Its configuration must be set.
//|         :param int endpoint: The address of the isochronous IN endpoint
//|         :param int sample_rate: The sample rate that the microphone was set to, in Hz
//|         :param int channel_count: The number of channels in the stream
//|         :param int bit_depth: The bits per sample in the stream: 8, 16, 24 or 32. 24
//|           and 32 bit samples are narrowed to 16 bit; see `bits_per_sample`. For
//|           samples narrower than their bytes, such as 24 bits in 4 bytes, give the
//|           size of the bytes.
//|         :param bool samples_signed: Samples are signed (True) or unsigned (False). USB
//|           audio is signed, except for 8 bit PCM.
//|         :param int max_packet_size: The endpoint's ``wMaxPacketSize``
//|         :param int buffer_frames: The number of frames handed to the audio pipeline at a time
//|
//|         Example, recording a second from the microphone of a Logitech C270
//|         webcam. It streams 16 bit mono on interface 3, alternate setting 1,
//|         endpoint 0x86. For another microphone, read these from its
//|         configuration descriptor::
//|
//|           import array
//|           import usb.core
//|           import usb_host_audio
//|
//|           device = usb.core.find(idVendor=0x046D, idProduct=0x0825)
//|           device.set_configuration()
//|           # SET_INTERFACE to alternate setting 1 starts the stream, then
//|           # SET_CUR of the endpoint's sampling frequency picks 16 kHz.
//|           device.ctrl_transfer(0x01, 11, 1, 3, None)
//|           device.ctrl_transfer(0x22, 0x01, 0x0100, 0x86, (16000).to_bytes(3, "little"))
//|
//|           samples = array.array("h", [0] * 16000)
//|           with usb_host_audio.USBIn(device, 0x86, sample_rate=16000, max_packet_size=68) as mic:
//|               count = mic.record(samples, len(samples))
//|               # Alternate setting 0 stops the stream.
//|               device.ctrl_transfer(0x01, 11, 0, 3, None)
//|           print("recorded", count, "samples, peak", max(abs(s) for s in samples))
//|
//|         """
//|         ...
//|
static mp_obj_t usb_host_audio_usbin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_device, ARG_endpoint, ARG_sample_rate, ARG_channel_count, ARG_bit_depth,
           ARG_samples_signed, ARG_max_packet_size, ARG_buffer_frames };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_device,          MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_endpoint,        MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sample_rate,     MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_channel_count,   MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 1} },
        { MP_QSTR_bit_depth,       MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 16} },
        { MP_QSTR_samples_signed,  MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_max_packet_size, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 0} },
        { MP_QSTR_buffer_frames,   MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 256} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    usb_core_device_obj_t *device = mp_arg_validate_type(args[ARG_device].u_obj, &usb_core_device_type, MP_QSTR_device);
    if (common_hal_usb_core_device_deinited(device)) {
        raise_deinited_error();
    }
    mp_int_t endpoint = args[ARG_endpoint].u_int;
    if ((endpoint & 0x80) == 0 || (endpoint & 0x7f) == 0 || endpoint > 0x8f) {
        mp_arg_error_invalid(MP_QSTR_endpoint);
    }
    mp_int_t sample_rate = mp_arg_validate_int_min(args[ARG_sample_rate].u_int, 1, MP_QSTR_sample_rate);
    mp_int_t channel_count = mp_arg_validate_int_range(args[ARG_channel_count].u_int, 1, 2, MP_QSTR_channel_count);
    mp_int_t bit_depth = args[ARG_bit_depth].u_int;
    if (bit_depth != 8 && bit_depth != 16 && bit_depth != 24 && bit_depth != 32) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be 8, 16, 24, or 32"), MP_QSTR_bit_depth);
    }
    mp_int_t max_packet_size = mp_arg_validate_int_range(args[ARG_max_packet_size].u_int, 0, 1024, MP_QSTR_max_packet_size);
    mp_int_t buffer_frames = mp_arg_validate_int_range(args[ARG_buffer_frames].u_int, 32, 4096, MP_QSTR_buffer_frames);
    if (!common_hal_usb_host_port_iso_buffered()) {
        // Without the ring the host controller hands over bare packets, not
        // the length prefixed records that USBIn reads.
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q not set in settings.toml"),
            MP_QSTR_CIRCUITPY_USB_HOST_ISO_BUFFER_SIZE);
    }

    usb_host_audio_usbin_obj_t *self = mp_obj_malloc_with_finaliser(usb_host_audio_usbin_obj_t, &usb_host_audio_usbin_type);
    common_hal_usb_host_audio_usbin_construct(self, device, endpoint, sample_rate, channel_count,
        bit_depth, args[ARG_samples_signed].u_bool, max_packet_size, buffer_frames);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Stops receiving the stream and releases the buffers."""
//|         ...
//|
static mp_obj_t usb_host_audio_usbin_deinit(mp_obj_t self_in) {
    usb_host_audio_usbin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_usb_host_audio_usbin_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_host_audio_usbin_deinit_obj, usb_host_audio_usbin_deinit);

static void check_for_deinit(usb_host_audio_usbin_obj_t *self) {
    if (common_hal_usb_host_audio_usbin_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> USBIn:
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

//|     def record(self, destination: WriteableBuffer, destination_length: int) -> int:
//|         """Records destination_length samples to destination. This is blocking.
//|         A stereo frame is two samples, left then right.
//|
//|         ``destination`` is an ``array.array`` of typecode ``'h'`` for
//|         `bits_per_sample` 16 (``'H'`` when unsigned), and of typecode ``'b'``
//|         (``'B'`` when unsigned) or a ``bytearray`` for 8.
//|
//|         Audio is received between calls too, so that consecutive recordings
//|         join up. When more arrived than is buffered, the recording starts
//|         with the audio that arrives after the call.
//|
//|         :return: The number of samples recorded. If this is less than
//|           ``destination_length``, the microphone stopped sending audio."""
//|         ...
//|
static mp_obj_t usb_host_audio_usbin_obj_record(mp_obj_t self_obj, mp_obj_t destination, mp_obj_t destination_length) {
    usb_host_audio_usbin_obj_t *self = MP_OBJ_TO_PTR(self_obj);
    check_for_deinit(self);
    uint32_t length = mp_arg_validate_type_int(destination_length, MP_QSTR_length);
    mp_arg_validate_length_min(length, 0, MP_QSTR_length);

    mp_buffer_info_t bufinfo;
    if (mp_obj_is_type(destination, &mp_type_fileio)) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Cannot record to a file"));
    }
    mp_get_buffer_raise(destination, &bufinfo, MP_BUFFER_WRITE);
    if (bufinfo.len / mp_binary_get_size('@', bufinfo.typecode, NULL) < length) {
        mp_raise_ValueError(MP_ERROR_TEXT("Destination capacity is smaller than destination_length."));
    }
    char error_type = ' ';
    if (self->base.bits_per_sample == 16) {
        char typecode = self->base.samples_signed ? 'h' : 'H';
        if (bufinfo.typecode != typecode) {
            error_type = typecode;
        }
    } else {
        char typecode = self->base.samples_signed ? 'b' : 'B';
        if (bufinfo.typecode != typecode && bufinfo.typecode != BYTEARRAY_TYPECODE) {
            error_type = typecode; // NOTE: Not identifying as bytearray
        }
    }
    if (error_type != ' ') {
        mp_raise_TypeError_varg(
            MP_ERROR_TEXT("invalid destination buffer, must be an array of type: %c"),
            error_type
            );
    }
    uint32_t length_written =
        common_hal_usb_host_audio_usbin_record_to_buffer(self, bufinfo.buf, length);
    return MP_OBJ_NEW_SMALL_INT(length_written);
}
MP_DEFINE_CONST_FUN_OBJ_3(usb_host_audio_usbin_record_obj, usb_host_audio_usbin_obj_record);

//|     sample_rate: int
//|     """The sample rate given to the constructor, in Hz. This is the rate
//|     reported to the audio pipeline. The microphone's own clock decides the
//|     true rate, which is usually a fraction of a percent off."""
//|
//|     bits_per_sample: int
//|     """The number of bits per sample as it is streamed through the audio pipeline
//|     and recorded: 8 or 16. (read-only)"""
//|
//|     channel_count: int
//|     """The number of channels (1 for mono, 2 for stereo). (read-only)"""
//|

//|     bit_depth: int
//|     """The number of bits per sample that the microphone sends. (read-only)"""
//|
static mp_obj_t usb_host_audio_usbin_obj_get_bit_depth(mp_obj_t self_in) {
    usb_host_audio_usbin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_usb_host_audio_usbin_get_bit_depth(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_host_audio_usbin_get_bit_depth_obj, usb_host_audio_usbin_obj_get_bit_depth);

MP_PROPERTY_GETTER(usb_host_audio_usbin_bit_depth_obj,
    (mp_obj_t)&usb_host_audio_usbin_get_bit_depth_obj);

//|     samples_signed: bool
//|     """True if the samples are signed PCM, False for unsigned. (read-only)"""
//|
static mp_obj_t usb_host_audio_usbin_obj_get_samples_signed(mp_obj_t self_in) {
    usb_host_audio_usbin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_usb_host_audio_usbin_get_samples_signed(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_host_audio_usbin_get_samples_signed_obj, usb_host_audio_usbin_obj_get_samples_signed);

MP_PROPERTY_GETTER(usb_host_audio_usbin_samples_signed_obj,
    (mp_obj_t)&usb_host_audio_usbin_get_samples_signed_obj);

//|     streaming: bool
//|     """True while the stream is received. It turns False for good when the
//|     microphone stops sending: an alternate setting of the device was
//|     selected, its configuration was set again, it was unplugged, or another
//|     isochronous stream was started. The object then plays silence and records
//|     only what it received before;
//|     make a new one after starting the stream again. (read-only)
//|
//|     Receiving is not retried, because some devices stop answering control
//|     requests when an endpoint of an idle interface is polled."""
//|
static mp_obj_t usb_host_audio_usbin_obj_get_streaming(mp_obj_t self_in) {
    usb_host_audio_usbin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_usb_host_audio_usbin_get_streaming(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_host_audio_usbin_get_streaming_obj, usb_host_audio_usbin_obj_get_streaming);

MP_PROPERTY_GETTER(usb_host_audio_usbin_streaming_obj,
    (mp_obj_t)&usb_host_audio_usbin_get_streaming_obj);

//|     overflow: bool
//|     """True if audio was dropped since the last time this was checked: it
//|     was not played or recorded fast enough, or the host controller lost
//|     packets. (read-only)
//|
//|     Reading this clears the flag. Audio that arrives while nothing plays or
//|     records also overflows, except that starting playback discards it without
//|     setting the flag."""
//|
//|
static mp_obj_t usb_host_audio_usbin_obj_get_overflow(mp_obj_t self_in) {
    usb_host_audio_usbin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_usb_host_audio_usbin_get_overflow(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_host_audio_usbin_get_overflow_obj, usb_host_audio_usbin_obj_get_overflow);

MP_PROPERTY_GETTER(usb_host_audio_usbin_overflow_obj,
    (mp_obj_t)&usb_host_audio_usbin_get_overflow_obj);

static const mp_rom_map_elem_t usb_host_audio_usbin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&usb_host_audio_usbin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&usb_host_audio_usbin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_record), MP_ROM_PTR(&usb_host_audio_usbin_record_obj) },
    // sample_rate / bits_per_sample / channel_count come from the audiosample
    // protocol's shared property getters.
    AUDIOSAMPLE_FIELDS,
    { MP_ROM_QSTR(MP_QSTR_bit_depth), MP_ROM_PTR(&usb_host_audio_usbin_bit_depth_obj) },
    { MP_ROM_QSTR(MP_QSTR_samples_signed), MP_ROM_PTR(&usb_host_audio_usbin_samples_signed_obj) },
    { MP_ROM_QSTR(MP_QSTR_streaming), MP_ROM_PTR(&usb_host_audio_usbin_streaming_obj) },
    { MP_ROM_QSTR(MP_QSTR_overflow), MP_ROM_PTR(&usb_host_audio_usbin_overflow_obj) },
};
static MP_DEFINE_CONST_DICT(usb_host_audio_usbin_locals_dict, usb_host_audio_usbin_locals_dict_table);

static const audiosample_p_t usb_host_audio_usbin_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .reset_buffer = (audiosample_reset_buffer_fun)common_hal_usb_host_audio_usbin_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)common_hal_usb_host_audio_usbin_get_buffer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    usb_host_audio_usbin_type,
    MP_QSTR_USBIn,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, usb_host_audio_usbin_make_new,
    locals_dict, &usb_host_audio_usbin_locals_dict,
    protocol, &usb_host_audio_usbin_proto
    );
