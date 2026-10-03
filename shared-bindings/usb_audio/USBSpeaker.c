// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/usb_audio/USBSpeaker.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-bindings/util.h"
#include "shared-module/usb_audio/__init__.h"

//| class USBSpeaker:
//|     """Plays audio streamed from the host computer as a USB Audio Class speaker.
//|
//|     A ``USBSpeaker`` is a *source* of audio samples, exactly like
//|     `audiocore.RawSample` or `audiocore.WaveFile`. The host PC streams audio to
//|     the board over USB, and the ``USBSpeaker`` hands that audio to a consumer
//|     such as `audiobusio.I2SOut`, `audiopwmio.PWMAudioOut` or `audioio.AudioOut`
//|     (optionally through the effect modules), so the board appears as a speaker.
//|
//|     You cannot create an instance of `usb_audio.USBSpeaker`.
//|
//|     There is a single shared instance, available as ``usb_audio.usb_speaker``
//|     once ``usb_audio.enable()`` has configured an output (speaker) stream in
//|     ``boot.py``. Until then ``usb_audio.usb_speaker`` is ``None``.
//|
//|     .. code-block:: py
//|
//|         # boot.py
//|         import usb_audio
//|         usb_audio.enable(sample_rate=16000, channel_count=1,
//|                          microphone=False, speaker=True)
//|
//|     .. code-block:: py
//|
//|         # code.py
//|         import board
//|         import usb_audio
//|         import audiobusio
//|
//|         spk = usb_audio.usb_speaker
//|         out = audiobusio.I2SOut(board.I2S_BIT_CLOCK, board.I2S_WORD_SELECT, board.I2S_DATA)
//|         out.play(spk, loop=True)
//|         while True:
//|             pass
//|
//|     """

static mp_obj_t usb_audio_usbspeaker_deinit(mp_obj_t self_in) {
    usb_audio_usbspeaker_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_usb_audio_usbspeaker_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbspeaker_deinit_obj, usb_audio_usbspeaker_deinit);

static void check_for_deinit(usb_audio_usbspeaker_obj_t *self) {
    audiosample_check_for_deinit(&self->base);
}

//|     def read(self, destination: circuitpython_typing.WriteableBuffer, destination_length: int) -> int:
//|         """Copies up to ``destination_length`` of the most recent samples streamed
//|         from the host into ``destination``, for analysis such as an audio-reactive
//|         effect or VU meter. This does not block: it returns whatever the host has
//|         delivered so far, which may be fewer than ``destination_length`` samples
//|         (or zero when the host is not streaming).
//|
//|         ``destination`` must be an ``array.array`` of 16-bit signed samples
//|         (typecode ``"h"``), matching the negotiated speaker format.
//|
//|         Reading consumes the samples, so a ``USBSpeaker`` being read this way
//|         should not also be passed to an output backend's ``play()`` at the same
//|         time.
//|
//|         :return: The number of samples copied into ``destination``."""
//|         ...
//|
static mp_obj_t usb_audio_usbspeaker_obj_read(mp_obj_t self_in, mp_obj_t destination, mp_obj_t destination_length) {
    usb_audio_usbspeaker_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    uint32_t length = mp_arg_validate_type_int(destination_length, MP_QSTR_length);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(destination, &bufinfo, MP_BUFFER_WRITE);
    // The negotiated speaker format is 16-bit signed PCM, so require a matching
    // 'h' array. This keeps the copy a straight memcpy with no conversion.
    if (bufinfo.typecode != 'h') {
        mp_raise_TypeError(MP_ERROR_TEXT("destination must be an array of type 'h'"));
    }
    size_t capacity = bufinfo.len / sizeof(int16_t);
    if (capacity < length) {
        length = capacity;
    }

    uint32_t length_read = common_hal_usb_audio_usbspeaker_read(self, bufinfo.buf, length);
    return MP_OBJ_NEW_SMALL_INT(length_read);
}
static MP_DEFINE_CONST_FUN_OBJ_3(usb_audio_usbspeaker_read_obj, usb_audio_usbspeaker_obj_read);

//|     def __enter__(self) -> USBSpeaker:
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

//|     connected: bool
//|     """True while the host is streaming audio to this speaker. (read-only)"""
//|
//|
static mp_obj_t usb_audio_usbspeaker_obj_get_connected(mp_obj_t self_in) {
    usb_audio_usbspeaker_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_usb_audio_usbspeaker_get_connected(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbspeaker_get_connected_obj, usb_audio_usbspeaker_obj_get_connected);

MP_PROPERTY_GETTER(usb_audio_usbspeaker_connected_obj,
    (mp_obj_t)&usb_audio_usbspeaker_get_connected_obj);

//|     host_mute: bool
//|     """True when the host has muted this speaker. (read-only)"""
//|
static mp_obj_t usb_audio_usbspeaker_obj_get_host_mute(mp_obj_t self_in) {
    usb_audio_usbspeaker_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(usb_audio_host_mute(USB_AUDIO_DIR_SPEAKER));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbspeaker_get_host_mute_obj, usb_audio_usbspeaker_obj_get_host_mute);

MP_PROPERTY_GETTER(usb_audio_usbspeaker_host_mute_obj,
    (mp_obj_t)&usb_audio_usbspeaker_get_host_mute_obj);

//|     host_volume: float
//|     """The volume the host has set, in decibels from ``-60.0`` to ``0.0``. (read-only)"""
//|
static mp_obj_t usb_audio_usbspeaker_obj_get_host_volume(mp_obj_t self_in) {
    usb_audio_usbspeaker_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(usb_audio_host_volume(USB_AUDIO_DIR_SPEAKER));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbspeaker_get_host_volume_obj, usb_audio_usbspeaker_obj_get_host_volume);

MP_PROPERTY_GETTER(usb_audio_usbspeaker_host_volume_obj,
    (mp_obj_t)&usb_audio_usbspeaker_get_host_volume_obj);

//|     host_gain: float
//|     """`host_volume` and `host_mute` as a linear factor from ``0.0`` to ``1.0``. (read-only)"""
//|
static mp_obj_t usb_audio_usbspeaker_obj_get_host_gain(mp_obj_t self_in) {
    usb_audio_usbspeaker_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(usb_audio_host_gain(USB_AUDIO_DIR_SPEAKER));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbspeaker_get_host_gain_obj, usb_audio_usbspeaker_obj_get_host_gain);

MP_PROPERTY_GETTER(usb_audio_usbspeaker_host_gain_obj,
    (mp_obj_t)&usb_audio_usbspeaker_get_host_gain_obj);

//|     apply_host_volume: bool
//|     """Whether the board scales the samples it receives by `host_gain` itself.
//|     `False` by default, leaving the level to your code; `True` makes the host's
//|     volume slider and mute work on their own."""
//|
static mp_obj_t usb_audio_usbspeaker_obj_get_apply_host_volume(mp_obj_t self_in) {
    usb_audio_usbspeaker_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(usb_audio_apply_host_volume(USB_AUDIO_DIR_SPEAKER));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbspeaker_get_apply_host_volume_obj,
    usb_audio_usbspeaker_obj_get_apply_host_volume);

static mp_obj_t usb_audio_usbspeaker_obj_set_apply_host_volume(mp_obj_t self_in, mp_obj_t value) {
    usb_audio_usbspeaker_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    usb_audio_set_apply_host_volume(USB_AUDIO_DIR_SPEAKER, mp_obj_is_true(value));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(usb_audio_usbspeaker_set_apply_host_volume_obj,
    usb_audio_usbspeaker_obj_set_apply_host_volume);

MP_PROPERTY_GETSET(usb_audio_usbspeaker_apply_host_volume_obj,
    (mp_obj_t)&usb_audio_usbspeaker_get_apply_host_volume_obj,
    (mp_obj_t)&usb_audio_usbspeaker_set_apply_host_volume_obj);

//|     sample_rate: int
//|     """The sample rate negotiated with the host in ``boot.py``. (read-only)"""
//|
//|     bits_per_sample: int
//|     """The bit depth negotiated with the host in ``boot.py``. (read-only)"""
//|
//|     channel_count: int
//|     """The number of channels negotiated with the host in ``boot.py``. (read-only)"""
//|
//|
static const mp_rom_map_elem_t usb_audio_usbspeaker_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&usb_audio_usbspeaker_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&usb_audio_usbspeaker_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&usb_audio_usbspeaker_read_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_connected), MP_ROM_PTR(&usb_audio_usbspeaker_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_host_mute), MP_ROM_PTR(&usb_audio_usbspeaker_host_mute_obj) },
    { MP_ROM_QSTR(MP_QSTR_host_volume), MP_ROM_PTR(&usb_audio_usbspeaker_host_volume_obj) },
    { MP_ROM_QSTR(MP_QSTR_host_gain), MP_ROM_PTR(&usb_audio_usbspeaker_host_gain_obj) },
    { MP_ROM_QSTR(MP_QSTR_apply_host_volume), MP_ROM_PTR(&usb_audio_usbspeaker_apply_host_volume_obj) },
    AUDIOSAMPLE_FIELDS,
};
static MP_DEFINE_CONST_DICT(usb_audio_usbspeaker_locals_dict, usb_audio_usbspeaker_locals_dict_table);

static const audiosample_p_t usb_audio_usbspeaker_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .reset_buffer = (audiosample_reset_buffer_fun)usb_audio_usbspeaker_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)usb_audio_usbspeaker_get_buffer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    usb_audio_USBSpeaker_type,
    MP_QSTR_USBSpeaker,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &usb_audio_usbspeaker_locals_dict,
    protocol, &usb_audio_usbspeaker_proto
    );
