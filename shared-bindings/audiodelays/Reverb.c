// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Mark Komus
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared-bindings/audiodelays/Reverb.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-module/audiodelays/Reverb.h"

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-module/synthio/block.h"

//| class Reverb:
//|     """An Reverb effect"""
//|
//|     def __init__(
//|         self,
//|         roomsize: synthio.BlockInput = 0.5,
//|         damp: synthio.BlockInput = 0.5,
//|         mix: synthio.BlockInput = 0.5,
//|         buffer_size: int = 512,
//|         sample_rate: int = 8000,
//|         bits_per_sample: int = 16,
//|         samples_signed: bool = True,
//|         channel_count: int = 1,
//|     ) -> None:
//|         """Create a Reverb effect simulating the audio taking place in a large room where you get echos
//|            off of various surfaces at various times. The size of the room can be adjusted as well as how
//|            much the higher frequencies get absorbed by the walls.
//|
//|            The mix parameter allows you to change how much of the unchanged sample passes through to
//|            the output to how much of the effect audio you hear as the output.
//|
//|         :param synthio.BlockInput roomsize: The size of the room. 0.0 = smallest; 1.0 = largest.
//|         :param synthio.BlockInput damp: How much the walls absorb. 0.0 = least; 1.0 = most.
//|         :param synthio.BlockInput mix: The mix as a ratio of the sample (0.0) to the effect (1.0).
//|         :param int buffer_size: The total size in bytes of each of the two playback buffers to use
//|         :param int sample_rate: The sample rate to be used
//|         :param int channel_count: The number of channels the source samples contain. 1 = mono; 2 = stereo.
//|         :param int bits_per_sample: The bits per sample of the effect. Reverb requires 16 bits.
//|         :param bool samples_signed: Effect is signed (True) or unsigned (False). Reverb requires signed (True).
//|
//|         Playing adding an reverb to a synth::
//|
//|           import time
//|           import board
//|           import audiobusio
//|           import synthio
//|           import audiodelays
//|
//|           audio = audiobusio.I2SOut(bit_clock=board.GP20, word_select=board.GP21, data=board.GP22)
//|           synth = synthio.Synthesizer(channel_count=1, sample_rate=44100)
//|           reverb = audiodelays.Reverb(roomsize=0.7, damp=0.3, buffer_size=1024, channel_count=1, sample_rate=44100, mix=0.7)
//|           reverb.play(synth)
//|           audio.play(reverb)
//|
//|           note = synthio.Note(261)
//|           while True:
//|               synth.press(note)
//|               time.sleep(0.55)
//|               synth.release(note)
//|               time.sleep(5)"""
//|         ...
//|
static mp_obj_t audiodelays_reverb_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_roomsize, ARG_damp, ARG_mix, ARG_buffer_size, ARG_sample_rate, ARG_bits_per_sample, ARG_samples_signed, ARG_channel_count, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_roomsize, MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_damp, MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mix, MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_buffer_size, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 512} },
        { MP_QSTR_sample_rate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 8000} },
        { MP_QSTR_bits_per_sample, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 16} },
        { MP_QSTR_samples_signed, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
        { MP_QSTR_channel_count, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t channel_count = mp_arg_validate_int_range(args[ARG_channel_count].u_int, 1, 2, MP_QSTR_channel_count);
    mp_int_t sample_rate = mp_arg_validate_int_min(args[ARG_sample_rate].u_int, 1, MP_QSTR_sample_rate);
    if (args[ARG_samples_signed].u_bool != true) {
        mp_raise_ValueError(MP_ERROR_TEXT("samples_signed must be true"));
    }
    mp_int_t bits_per_sample = args[ARG_bits_per_sample].u_int;
    if (bits_per_sample != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits_per_sample must be 16"));
    }

    audiodelays_reverb_obj_t *self = mp_obj_malloc(audiodelays_reverb_obj_t, &audiodelays_reverb_type);
    common_hal_audiodelays_reverb_construct(self, args[ARG_roomsize].u_obj, args[ARG_damp].u_obj, args[ARG_mix].u_obj, args[ARG_buffer_size].u_int, bits_per_sample, args[ARG_samples_signed].u_bool, channel_count, sample_rate);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the Reverb."""
//|         ...
//|
static mp_obj_t audiodelays_reverb_deinit(mp_obj_t self_in) {
    audiodelays_reverb_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_reverb_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_reverb_deinit_obj, audiodelays_reverb_deinit);

static void check_for_deinit(audiodelays_reverb_obj_t *self) {
    audiosample_check_for_deinit(&self->base);
}

//|     def __enter__(self) -> Reverb:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
//  Provided by context manager helper.

//|     roomsize: synthio.BlockInput
//|     """Apparent size of the room 0.0-1.0"""
static mp_obj_t audiodelays_reverb_obj_get_roomsize(mp_obj_t self_in) {
    return common_hal_audiodelays_reverb_get_roomsize(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_reverb_get_roomsize_obj, audiodelays_reverb_obj_get_roomsize);

static mp_obj_t audiodelays_reverb_obj_set_roomsize(mp_obj_t self_in, mp_obj_t roomsize) {
    audiodelays_reverb_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_reverb_set_roomsize(self, roomsize);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_reverb_set_roomsize_obj, audiodelays_reverb_obj_set_roomsize);

MP_PROPERTY_GETSET(audiodelays_reverb_roomsize_obj,
    (mp_obj_t)&audiodelays_reverb_get_roomsize_obj,
    (mp_obj_t)&audiodelays_reverb_set_roomsize_obj);

//|     damp: synthio.BlockInput
//|     """How reverbrent the area is. 0.0-1.0"""
static mp_obj_t audiodelays_reverb_obj_get_damp(mp_obj_t self_in) {
    return common_hal_audiodelays_reverb_get_damp(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_reverb_get_damp_obj, audiodelays_reverb_obj_get_damp);

static mp_obj_t audiodelays_reverb_obj_set_damp(mp_obj_t self_in, mp_obj_t damp) {
    audiodelays_reverb_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_reverb_set_damp(self, damp);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_reverb_set_damp_obj, audiodelays_reverb_obj_set_damp);

MP_PROPERTY_GETSET(audiodelays_reverb_damp_obj,
    (mp_obj_t)&audiodelays_reverb_get_damp_obj,
    (mp_obj_t)&audiodelays_reverb_set_damp_obj);

//|     mix: synthio.BlockInput
//|     """The rate the reverb mix between 0 and 1 where 0 is only sample and 1 is all effect."""
static mp_obj_t audiodelays_reverb_obj_get_mix(mp_obj_t self_in) {
    return common_hal_audiodelays_reverb_get_mix(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_reverb_get_mix_obj, audiodelays_reverb_obj_get_mix);

static mp_obj_t audiodelays_reverb_obj_set_mix(mp_obj_t self_in, mp_obj_t mix_in) {
    audiodelays_reverb_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_reverb_set_mix(self, mix_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_reverb_set_mix_obj, audiodelays_reverb_obj_set_mix);

MP_PROPERTY_GETSET(audiodelays_reverb_mix_obj,
    (mp_obj_t)&audiodelays_reverb_get_mix_obj,
    (mp_obj_t)&audiodelays_reverb_set_mix_obj);

//|     playing: bool
//|     """True when the effect is playing a sample. (read-only)"""
//|
static mp_obj_t audiodelays_reverb_obj_get_playing(mp_obj_t self_in) {
    audiodelays_reverb_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiodelays_reverb_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_reverb_get_playing_obj, audiodelays_reverb_obj_get_playing);

MP_PROPERTY_GETTER(audiodelays_reverb_playing_obj,
    (mp_obj_t)&audiodelays_reverb_get_playing_obj);

//|     def play(self, sample: circuitpython_typing.AudioSample, *, loop: bool = False) -> None:
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         The sample must match the encoding settings given in the constructor."""
//|         ...
//|
static mp_obj_t audiodelays_reverb_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audiodelays_reverb_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audiodelays_reverb_play(self, sample, args[ARG_loop].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiodelays_reverb_play_obj, 1, audiodelays_reverb_obj_play);

//|     def stop(self) -> None:
//|         """Stops playback of the sample. The reverb continues playing."""
//|         ...
//|
//|
static mp_obj_t audiodelays_reverb_obj_stop(mp_obj_t self_in) {
    audiodelays_reverb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_audiodelays_reverb_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_reverb_stop_obj, audiodelays_reverb_obj_stop);

static const mp_rom_map_elem_t audiodelays_reverb_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiodelays_reverb_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiodelays_reverb_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audiodelays_reverb_stop_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiodelays_reverb_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_roomsize), MP_ROM_PTR(&audiodelays_reverb_roomsize_obj) },
    { MP_ROM_QSTR(MP_QSTR_damp), MP_ROM_PTR(&audiodelays_reverb_damp_obj) },
    { MP_ROM_QSTR(MP_QSTR_mix), MP_ROM_PTR(&audiodelays_reverb_mix_obj) },
    AUDIOSAMPLE_FIELDS,
};
static MP_DEFINE_CONST_DICT(audiodelays_reverb_locals_dict, audiodelays_reverb_locals_dict_table);

static const audiosample_p_t audiodelays_reverb_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .reset_buffer = (audiosample_reset_buffer_fun)audiodelays_reverb_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiodelays_reverb_get_buffer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    audiodelays_reverb_type,
    MP_QSTR_Reverb,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiodelays_reverb_make_new,
    locals_dict, &audiodelays_reverb_locals_dict,
    protocol, &audiodelays_reverb_proto
    );
