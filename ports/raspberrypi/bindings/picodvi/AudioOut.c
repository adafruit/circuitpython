// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared/runtime/context_manager_helpers.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "bindings/picodvi/AudioOut.h"
#include "bindings/picodvi/Framebuffer.h"
#include "shared-bindings/util.h"

//| class AudioOut:
//|     """Plays audio through the DVI cable of a `Framebuffer`, on displays
//|     that have speakers or an audio output. RP2350 only: elsewhere the
//|     constructor raises `NotImplementedError`."""
//|
//|     def __init__(self, framebuffer: Framebuffer) -> None:
//|         """Create an AudioOut that sends audio with ``framebuffer``'s video.
//|
//|         The framebuffer must use a 640-pixel-wide output mode, such as
//|         640x480 or 320x240. This reserves about 120 KB (640 wide) or
//|         150 KB (320 or 160 wide) of internal RAM until `deinit`, so create
//|         the AudioOut soon after the Framebuffer. A 640x480 framebuffer with
//|         8-bit color leaves too little RAM. The display receives silence
//|         whenever nothing is playing.
//|
//|         :param Framebuffer framebuffer: The display output to add audio to
//|
//|         Playing a sine tone::
//|
//|           import array
//|           import math
//|           import time
//|           import audiocore
//|           import board
//|           import displayio
//|           import picodvi
//|
//|           displayio.release_displays()
//|           fb = picodvi.Framebuffer(320, 240, clk_dp=board.CKP, clk_dn=board.CKN,
//|                                    red_dp=board.D0P, red_dn=board.D0N,
//|                                    green_dp=board.D1P, green_dn=board.D1N,
//|                                    blue_dp=board.D2P, blue_dn=board.D2N)
//|           audio = picodvi.AudioOut(fb)
//|           sine = array.array("h", [int(8000 * math.sin(math.pi * 2 * i / 48)) for i in range(48)])
//|           tone = audiocore.RawSample(sine, sample_rate=48000)
//|           audio.play(tone, loop=True)
//|           time.sleep(1)
//|           audio.stop()"""
//|         ...
//|
static mp_obj_t picodvi_audioout_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    #if CIRCUITPY_PICODVI_AUDIOOUT
    enum { ARG_framebuffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_framebuffer, MP_ARG_OBJ | MP_ARG_REQUIRED },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    picodvi_framebuffer_obj_t *framebuffer = MP_OBJ_TO_PTR(
        mp_arg_validate_type(args[ARG_framebuffer].u_obj, &picodvi_framebuffer_type, MP_QSTR_framebuffer));

    picodvi_audioout_obj_t *self = mp_obj_malloc_with_finaliser(picodvi_audioout_obj_t, &picodvi_audioout_type);
    common_hal_picodvi_audioout_construct(self, framebuffer);
    return MP_OBJ_FROM_PTR(self);
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
}

#if CIRCUITPY_PICODVI_AUDIOOUT
//|     def deinit(self) -> None:
//|         """Stops playback and releases the framebuffer for another AudioOut."""
//|         ...
//|
static mp_obj_t picodvi_audioout_deinit(mp_obj_t self_in) {
    picodvi_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_picodvi_audioout_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(picodvi_audioout_deinit_obj, picodvi_audioout_deinit);

static void check_for_deinit(picodvi_audioout_obj_t *self) {
    if (common_hal_picodvi_audioout_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> AudioOut:
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
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         Sample must be an `audiocore.WaveFile`, `audiocore.RawSample`, `audiomixer.Mixer`,
//|         `synthio.Synthesizer` or `audiomp3.MP3Decoder` with a sample rate of 48000 Hz.
//|
//|         Mono samples will be converted to stereo by copying value to both the left channel and the right channel.
//|
//|         The sample itself should consist of 8 bit or 16 bit samples."""
//|         ...
//|
static mp_obj_t picodvi_audioout_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_loop, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    picodvi_audioout_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_picodvi_audioout_play(self, args[ARG_sample].u_obj, args[ARG_loop].u_bool);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picodvi_audioout_play_obj, 1, picodvi_audioout_obj_play);

//|     def stop(self) -> None:
//|         """Stops playback."""
//|         ...
//|
static mp_obj_t picodvi_audioout_obj_stop(mp_obj_t self_in) {
    picodvi_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_picodvi_audioout_stop(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(picodvi_audioout_stop_obj, picodvi_audioout_obj_stop);

//|     playing: bool
//|     """True when the audio sample is being output. (read-only)"""
//|
static mp_obj_t picodvi_audioout_obj_get_playing(mp_obj_t self_in) {
    picodvi_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_picodvi_audioout_get_playing(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(picodvi_audioout_get_playing_obj, picodvi_audioout_obj_get_playing);

MP_PROPERTY_GETTER(picodvi_audioout_playing_obj,
    (mp_obj_t)&picodvi_audioout_get_playing_obj);

//|     def pause(self) -> None:
//|         """Stops playback temporarily while remembering the position. Use `resume` to resume playback."""
//|         ...
//|
static mp_obj_t picodvi_audioout_obj_pause(mp_obj_t self_in) {
    picodvi_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (!common_hal_picodvi_audioout_get_playing(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Not playing"));
    }
    common_hal_picodvi_audioout_pause(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(picodvi_audioout_pause_obj, picodvi_audioout_obj_pause);

//|     def resume(self) -> None:
//|         """Resumes sample playback after :py:func:`pause`."""
//|         ...
//|
static mp_obj_t picodvi_audioout_obj_resume(mp_obj_t self_in) {
    picodvi_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (common_hal_picodvi_audioout_get_paused(self)) {
        common_hal_picodvi_audioout_resume(self);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(picodvi_audioout_resume_obj, picodvi_audioout_obj_resume);

//|     paused: bool
//|     """True when playback is paused. (read-only)"""
//|
//|
static mp_obj_t picodvi_audioout_obj_get_paused(mp_obj_t self_in) {
    picodvi_audioout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_picodvi_audioout_get_paused(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(picodvi_audioout_get_paused_obj, picodvi_audioout_obj_get_paused);

MP_PROPERTY_GETTER(picodvi_audioout_paused_obj,
    (mp_obj_t)&picodvi_audioout_get_paused_obj);
#endif // CIRCUITPY_PICODVI_AUDIOOUT

static const mp_rom_map_elem_t picodvi_audioout_locals_dict_table[] = {
    #if CIRCUITPY_PICODVI_AUDIOOUT
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&picodvi_audioout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&picodvi_audioout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&picodvi_audioout_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&picodvi_audioout_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&picodvi_audioout_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&picodvi_audioout_resume_obj) },

    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&picodvi_audioout_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_paused), MP_ROM_PTR(&picodvi_audioout_paused_obj) },
    #endif // CIRCUITPY_PICODVI_AUDIOOUT
};
static MP_DEFINE_CONST_DICT(picodvi_audioout_locals_dict, picodvi_audioout_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picodvi_audioout_type,
    MP_QSTR_AudioOut,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, picodvi_audioout_make_new,
    locals_dict, &picodvi_audioout_locals_dict
    );
