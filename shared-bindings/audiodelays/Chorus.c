// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Mark Komus
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared-bindings/audiodelays/Chorus.h"
#include "shared-module/audiodelays/Chorus.h"
#include "shared-bindings/audiocore/__init__.h"

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-module/synthio/block.h"

//| class Chorus:
//|     """An Chorus effect"""
//|
//|     def __init__(
//|         self,
//|         max_delay_ms: int = 50,
//|         delay_ms: synthio.BlockInput = 50.0,
//|         voices: synthio.BlockInput = 1.0,
//|         buffer_size: int = 512,
//|         sample_rate: int = 8000,
//|         bits_per_sample: int = 16,
//|         samples_signed: bool = True,
//|         channel_count: int = 1,
//|     ) -> None:
//|         """Create a Chorus effect by playing the current sample along with one or more samples
//|            (the voices) from the delay buffer. The voices played are evenly spaced across the delay
//|            buffer. So for 2 voices you would hear the current sample and the one delay milliseconds back.
//|            The delay timing of the chorus can be changed at runtime with the delay_ms parameter but the delay
//|            can never exceed the max_delay_ms parameter. The maximum delay you can set is limited by available
//|            memory.
//|
//|         :param int max_delay_ms: The maximum time the chorus can be in milliseconds
//|         :param synthio.BlockInput delay_ms: The current time of the chorus delay in milliseconds. Must be less the max_delay_ms.
//|         :param synthio.BlockInput voices: The number of voices playing split evenly over the delay buffer.
//|         :param int buffer_size: The total size in bytes of each of the two playback buffers to use
//|         :param int sample_rate: The sample rate to be used
//|         :param int channel_count: The number of channels the source samples contain. 1 = mono; 2 = stereo.
//|         :param int bits_per_sample: The bits per sample of the effect
//|         :param bool samples_signed: Effect is signed (True) or unsigned (False)
//|
//|         Playing adding an chorus to a synth::
//|
//|           import time
//|           import board
//|           import audiobusio
//|           import synthio
//|           import audiodelays
//|
//|           audio = audiobusio.I2SOut(bit_clock=board.GP20, word_select=board.GP21, data=board.GP22)
//|           synth = synthio.Synthesizer(channel_count=1, sample_rate=44100)
//|           chorus = audiodelays.Chorus(max_delay_ms=50, delay_ms=5, buffer_size=1024, channel_count=1, sample_rate=44100)
//|           chorus.play(synth)
//|           audio.play(chorus)
//|
//|           note = synthio.Note(261)
//|           while True:
//|               synth.press(note)
//|               time.sleep(0.25)
//|               synth.release(note)
//|               time.sleep(5)"""
//|         ...
//|
static mp_obj_t audiodelays_chorus_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_max_delay_ms, ARG_delay_ms, ARG_voices, ARG_buffer_size, ARG_sample_rate, ARG_bits_per_sample, ARG_samples_signed, ARG_channel_count, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_max_delay_ms, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 50 } },
        { MP_QSTR_delay_ms, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_voices, MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_buffer_size, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 512} },
        { MP_QSTR_sample_rate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 8000} },
        { MP_QSTR_bits_per_sample, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 16} },
        { MP_QSTR_samples_signed, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
        { MP_QSTR_channel_count, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t max_delay_ms = mp_arg_validate_int_range(args[ARG_max_delay_ms].u_int, 1, 4000, MP_QSTR_max_delay_ms);

    mp_int_t channel_count = mp_arg_validate_int_range(args[ARG_channel_count].u_int, 1, 2, MP_QSTR_channel_count);
    mp_int_t sample_rate = mp_arg_validate_int_min(args[ARG_sample_rate].u_int, 1, MP_QSTR_sample_rate);
    mp_int_t bits_per_sample = args[ARG_bits_per_sample].u_int;
    if (bits_per_sample != 8 && bits_per_sample != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits_per_sample must be 8 or 16"));
    }

    audiodelays_chorus_obj_t *self = mp_obj_malloc(audiodelays_chorus_obj_t, &audiodelays_chorus_type);
    common_hal_audiodelays_chorus_construct(self, max_delay_ms, args[ARG_delay_ms].u_obj, args[ARG_voices].u_obj, args[ARG_buffer_size].u_int, bits_per_sample, args[ARG_samples_signed].u_bool, channel_count, sample_rate);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the Chorus."""
//|         ...
//|
static mp_obj_t audiodelays_chorus_deinit(mp_obj_t self_in) {
    audiodelays_chorus_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_chorus_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_chorus_deinit_obj, audiodelays_chorus_deinit);

static void check_for_deinit(audiodelays_chorus_obj_t *self) {
    audiosample_check_for_deinit(&self->base);
}

//|     def __enter__(self) -> Chorus:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
static mp_obj_t audiodelays_chorus_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audiodelays_chorus_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audiodelays_chorus___exit___obj, 4, 4, audiodelays_chorus_obj___exit__);


//|     delay_ms: synthio.BlockInput
//|     """The current time of the chorus delay in milliseconds. Must be less the max_delay_ms."""
//|
static mp_obj_t audiodelays_chorus_obj_get_delay_ms(mp_obj_t self_in) {
    audiodelays_chorus_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_audiodelays_chorus_get_delay_ms(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_chorus_get_delay_ms_obj, audiodelays_chorus_obj_get_delay_ms);

static mp_obj_t audiodelays_chorus_obj_set_delay_ms(mp_obj_t self_in, mp_obj_t delay_ms_in) {
    audiodelays_chorus_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_chorus_set_delay_ms(self, delay_ms_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_chorus_set_delay_ms_obj, audiodelays_chorus_obj_set_delay_ms);

MP_PROPERTY_GETSET(audiodelays_chorus_delay_ms_obj,
    (mp_obj_t)&audiodelays_chorus_get_delay_ms_obj,
    (mp_obj_t)&audiodelays_chorus_set_delay_ms_obj);

//|     voices: synthio.BlockInput
//|     """The number of voices playing split evenly over the delay buffer."""
static mp_obj_t audiodelays_chorus_obj_get_voices(mp_obj_t self_in) {
    return common_hal_audiodelays_chorus_get_voices(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_chorus_get_voices_obj, audiodelays_chorus_obj_get_voices);

static mp_obj_t audiodelays_chorus_obj_set_voices(mp_obj_t self_in, mp_obj_t voices_in) {
    audiodelays_chorus_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_chorus_set_voices(self, voices_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_chorus_set_voices_obj, audiodelays_chorus_obj_set_voices);

MP_PROPERTY_GETSET(audiodelays_chorus_voices_obj,
    (mp_obj_t)&audiodelays_chorus_get_voices_obj,
    (mp_obj_t)&audiodelays_chorus_set_voices_obj);

//|     playing: bool
//|     """True when the effect is playing a sample. (read-only)"""
//|
static mp_obj_t audiodelays_chorus_obj_get_playing(mp_obj_t self_in) {
    audiodelays_chorus_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiodelays_chorus_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_chorus_get_playing_obj, audiodelays_chorus_obj_get_playing);

MP_PROPERTY_GETTER(audiodelays_chorus_playing_obj,
    (mp_obj_t)&audiodelays_chorus_get_playing_obj);

//|     def play(self, sample: circuitpython_typing.AudioSample, *, loop: bool = False) -> None:
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         The sample must match the encoding settings given in the constructor."""
//|         ...
//|
static mp_obj_t audiodelays_chorus_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audiodelays_chorus_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audiodelays_chorus_play(self, sample, args[ARG_loop].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiodelays_chorus_play_obj, 1, audiodelays_chorus_obj_play);

//|     def stop(self) -> None:
//|         """Stops playback of the sample."""
//|         ...
//|
//|
static mp_obj_t audiodelays_chorus_obj_stop(mp_obj_t self_in) {
    audiodelays_chorus_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_audiodelays_chorus_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_chorus_stop_obj, audiodelays_chorus_obj_stop);

static const mp_rom_map_elem_t audiodelays_chorus_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiodelays_chorus_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audiodelays_chorus___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiodelays_chorus_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audiodelays_chorus_stop_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiodelays_chorus_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_delay_ms), MP_ROM_PTR(&audiodelays_chorus_delay_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_voices), MP_ROM_PTR(&audiodelays_chorus_voices_obj) },
    AUDIOSAMPLE_FIELDS,
};
static MP_DEFINE_CONST_DICT(audiodelays_chorus_locals_dict, audiodelays_chorus_locals_dict_table);

static const audiosample_p_t audiodelays_chorus_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .reset_buffer = (audiosample_reset_buffer_fun)audiodelays_chorus_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiodelays_chorus_get_buffer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    audiodelays_chorus_type,
    MP_QSTR_Chorus,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiodelays_chorus_make_new,
    locals_dict, &audiodelays_chorus_locals_dict,
    protocol, &audiodelays_chorus_proto
    );
