// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/usb_audio/USBMicrophone.h"
#include "shared-bindings/util.h"
#include "shared-module/usb_audio/__init__.h"
#include "shared-module/usb_audio/usb_audio_descriptors.h"

//| class USBMicrophone:
//|     """Streams an audio sample to the host computer as a USB Audio Class microphone.
//|
//|     A ``USBMicrophone`` is a *consumer* of an audio sample, exactly like
//|     `audioio.AudioOut`, `audiobusio.I2SOut` and `audiopwmio.PWMAudioOut`. The
//|     samples it pulls are streamed to the host PC over USB rather than to a pin,
//|     so the board appears as a microphone.
//|
//|     You cannot create an instance of `usb_audio.USBMicrophone`.
//|
//|     There is a single shared instance, available as ``usb_audio.usb_microphone``
//|     once ``usb_audio.enable()`` has configured an input (microphone) stream in
//|     ``boot.py``. Until then ``usb_audio.usb_microphone`` is ``None``."""
//|



static mp_obj_t usb_audio_usbmicrophone_deinit(mp_obj_t self_in) {
    usb_audio_usbmicrophone_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_usb_audio_usbmicrophone_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbmicrophone_deinit_obj, usb_audio_usbmicrophone_deinit);

static void check_for_deinit(usb_audio_usbmicrophone_obj_t *self) {
    if (common_hal_usb_audio_usbmicrophone_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> USBMicrophone:
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

//|     def play(self, sample: circuitpython_typing.AudioSample, *, loop: bool = False) -> None:
//|         """Streams the sample to the host once when loop=False and continuously when
//|         loop=True. Does not block. Use `playing` to block.
//|
//|         Sample must be an `audiocore.WaveFile`, `audiocore.RawSample`, `audiomixer.Mixer`,
//|         `audiomp3.MP3Decoder` or `synthio.Synthesizer`."""
//|         ...
//|
static mp_obj_t usb_audio_usbmicrophone_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    usb_audio_usbmicrophone_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_usb_audio_usbmicrophone_play(self, args[ARG_sample].u_obj, args[ARG_loop].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(usb_audio_usbmicrophone_play_obj, 1, usb_audio_usbmicrophone_obj_play);

//|     def stop(self) -> None:
//|         """Stops streaming and resets to the start of the sample."""
//|         ...
//|
static mp_obj_t usb_audio_usbmicrophone_obj_stop(mp_obj_t self_in) {
    usb_audio_usbmicrophone_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_usb_audio_usbmicrophone_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbmicrophone_stop_obj, usb_audio_usbmicrophone_obj_stop);

//|     playing: bool
//|     """True when an audio sample is being streamed even if `paused`. (read-only)"""
//|
static mp_obj_t usb_audio_usbmicrophone_obj_get_playing(mp_obj_t self_in) {
    usb_audio_usbmicrophone_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_usb_audio_usbmicrophone_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbmicrophone_get_playing_obj, usb_audio_usbmicrophone_obj_get_playing);

MP_PROPERTY_GETTER(usb_audio_usbmicrophone_playing_obj,
    (mp_obj_t)&usb_audio_usbmicrophone_get_playing_obj);

//|     def pause(self) -> None:
//|         """Stops streaming temporarily while remembering the position. Use `resume` to resume."""
//|         ...
//|
static mp_obj_t usb_audio_usbmicrophone_obj_pause(mp_obj_t self_in) {
    usb_audio_usbmicrophone_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    if (!common_hal_usb_audio_usbmicrophone_get_playing(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Not playing"));
    }
    common_hal_usb_audio_usbmicrophone_pause(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbmicrophone_pause_obj, usb_audio_usbmicrophone_obj_pause);

//|     def resume(self) -> None:
//|         """Resumes streaming after :py:func:`pause`."""
//|         ...
//|
static mp_obj_t usb_audio_usbmicrophone_obj_resume(mp_obj_t self_in) {
    usb_audio_usbmicrophone_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    if (common_hal_usb_audio_usbmicrophone_get_paused(self)) {
        common_hal_usb_audio_usbmicrophone_resume(self);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbmicrophone_resume_obj, usb_audio_usbmicrophone_obj_resume);

//|     paused: bool
//|     """True when streaming is paused. (read-only)"""
//|
//|
static mp_obj_t usb_audio_usbmicrophone_obj_get_paused(mp_obj_t self_in) {
    usb_audio_usbmicrophone_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_usb_audio_usbmicrophone_get_paused(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_audio_usbmicrophone_get_paused_obj, usb_audio_usbmicrophone_obj_get_paused);

MP_PROPERTY_GETTER(usb_audio_usbmicrophone_paused_obj,
    (mp_obj_t)&usb_audio_usbmicrophone_get_paused_obj);

static const mp_rom_map_elem_t usb_audio_usbmicrophone_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&usb_audio_usbmicrophone_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&usb_audio_usbmicrophone_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&usb_audio_usbmicrophone_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&usb_audio_usbmicrophone_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&usb_audio_usbmicrophone_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&usb_audio_usbmicrophone_resume_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&usb_audio_usbmicrophone_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_paused), MP_ROM_PTR(&usb_audio_usbmicrophone_paused_obj) },
};
static MP_DEFINE_CONST_DICT(usb_audio_usbmicrophone_locals_dict, usb_audio_usbmicrophone_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    usb_audio_USBMicrophone_type,
    MP_QSTR_USBMicrophone,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &usb_audio_usbmicrophone_locals_dict
    );
