// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared-bindings/audiofilters/Filter.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-module/audiofilters/Filter.h"

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-module/synthio/block.h"

//| class Filter:
//|     """A Filter effect"""
//|
//|     def __init__(
//|         self,
//|         filter: Optional[synthio.Biquad | Tuple[synthio.Biquad]] = None,
//|         mix: synthio.BlockInput = 1.0,
//|         buffer_size: int = 512,
//|         sample_rate: int = 8000,
//|         bits_per_sample: int = 16,
//|         samples_signed: bool = True,
//|         channel_count: int = 1,
//|     ) -> None:
//|         """Create a Filter effect where the original sample is processed through a biquad filter
//|            created by a synthio.Synthesizer object. This can be used to generate a low-pass,
//|            high-pass, or band-pass filter.
//|
//|            The mix parameter allows you to change how much of the unchanged sample passes through to
//|            the output to how much of the effect audio you hear as the output.
//|
//|         :param Optional[synthio.Biquad|Tuple[synthio.Biquad]] filter: A normalized biquad filter object or tuple of normalized biquad filter objects. The sample is processed sequentially by each filter to produce the output samples.
//|         :param synthio.BlockInput mix: The mix as a ratio of the sample (0.0) to the effect (1.0).
//|         :param int buffer_size: The total size in bytes of each of the two playback buffers to use
//|         :param int sample_rate: The sample rate to be used
//|         :param int channel_count: The number of channels the source samples contain. 1 = mono; 2 = stereo.
//|         :param int bits_per_sample: The bits per sample of the effect
//|         :param bool samples_signed: Effect is signed (True) or unsigned (False)
//|
//|         Playing adding a filter to a synth::
//|
//|           import time
//|           import board
//|           import audiobusio
//|           import synthio
//|           import audiofilters
//|
//|           audio = audiobusio.I2SOut(bit_clock=board.GP20, word_select=board.GP21, data=board.GP22)
//|           synth = synthio.Synthesizer(channel_count=1, sample_rate=44100)
//|           effect = audiofilters.Filter(buffer_size=1024, channel_count=1, sample_rate=44100, mix=1.0)
//|           effect.filter = synth.low_pass_filter(frequency=2000, Q=1.25)
//|           effect.play(synth)
//|           audio.play(effect)
//|
//|           note = synthio.Note(261)
//|           while True:
//|               synth.press(note)
//|               time.sleep(0.25)
//|               synth.release(note)
//|               time.sleep(5)"""
//|         ...
//|
static mp_obj_t audiofilters_filter_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_filter, ARG_mix, ARG_buffer_size, ARG_sample_rate, ARG_bits_per_sample, ARG_samples_signed, ARG_channel_count, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_filter, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_NONE } },
        { MP_QSTR_mix, MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_ROM_INT(1)} },
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
    mp_int_t bits_per_sample = args[ARG_bits_per_sample].u_int;
    if (bits_per_sample != 8 && bits_per_sample != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits_per_sample must be 8 or 16"));
    }

    audiofilters_filter_obj_t *self = mp_obj_malloc(audiofilters_filter_obj_t, &audiofilters_filter_type);
    common_hal_audiofilters_filter_construct(self, args[ARG_filter].u_obj, args[ARG_mix].u_obj, args[ARG_buffer_size].u_int, bits_per_sample, args[ARG_samples_signed].u_bool, channel_count, sample_rate);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the Filter."""
//|         ...
//|
static mp_obj_t audiofilters_filter_deinit(mp_obj_t self_in) {
    audiofilters_filter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiofilters_filter_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiofilters_filter_deinit_obj, audiofilters_filter_deinit);

static void check_for_deinit(audiofilters_filter_obj_t *self) {
    audiosample_check_for_deinit(&self->base);
}

//|     def __enter__(self) -> Filter:
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


//|     filter: synthio.Biquad | Tuple[synthio.Biquad] | None
//|     """A normalized biquad filter object or tuple of normalized biquad filter objects. The sample is processed sequentially by each filter to produce the output samples."""
//|
static mp_obj_t audiofilters_filter_obj_get_filter(mp_obj_t self_in) {
    audiofilters_filter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return common_hal_audiofilters_filter_get_filter(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiofilters_filter_get_filter_obj, audiofilters_filter_obj_get_filter);

static mp_obj_t audiofilters_filter_obj_set_filter(mp_obj_t self_in, mp_obj_t filter_in) {
    audiofilters_filter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiofilters_filter_set_filter(self, filter_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiofilters_filter_set_filter_obj, audiofilters_filter_obj_set_filter);

MP_PROPERTY_GETSET(audiofilters_filter_filter_obj,
    (mp_obj_t)&audiofilters_filter_get_filter_obj,
    (mp_obj_t)&audiofilters_filter_set_filter_obj);


//|     mix: synthio.BlockInput
//|     """The rate the filtered signal mix between 0 and 1 where 0 is only sample and 1 is all effect."""
static mp_obj_t audiofilters_filter_obj_get_mix(mp_obj_t self_in) {
    return common_hal_audiofilters_filter_get_mix(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiofilters_filter_get_mix_obj, audiofilters_filter_obj_get_mix);

static mp_obj_t audiofilters_filter_obj_set_mix(mp_obj_t self_in, mp_obj_t mix_in) {
    audiofilters_filter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiofilters_filter_set_mix(self, mix_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiofilters_filter_set_mix_obj, audiofilters_filter_obj_set_mix);

MP_PROPERTY_GETSET(audiofilters_filter_mix_obj,
    (mp_obj_t)&audiofilters_filter_get_mix_obj,
    (mp_obj_t)&audiofilters_filter_set_mix_obj);


//|     playing: bool
//|     """True when the effect is playing a sample. (read-only)"""
//|
static mp_obj_t audiofilters_filter_obj_get_playing(mp_obj_t self_in) {
    audiofilters_filter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiofilters_filter_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiofilters_filter_get_playing_obj, audiofilters_filter_obj_get_playing);

MP_PROPERTY_GETTER(audiofilters_filter_playing_obj,
    (mp_obj_t)&audiofilters_filter_get_playing_obj);

//|     def play(self, sample: circuitpython_typing.AudioSample, *, loop: bool = False) -> None:
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         The sample must match the encoding settings given in the constructor."""
//|         ...
//|
static mp_obj_t audiofilters_filter_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audiofilters_filter_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audiofilters_filter_play(self, sample, args[ARG_loop].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiofilters_filter_play_obj, 1, audiofilters_filter_obj_play);

//|     def stop(self) -> None:
//|         """Stops playback of the sample."""
//|         ...
//|
//|
static mp_obj_t audiofilters_filter_obj_stop(mp_obj_t self_in) {
    audiofilters_filter_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_audiofilters_filter_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiofilters_filter_stop_obj, audiofilters_filter_obj_stop);

static const mp_rom_map_elem_t audiofilters_filter_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiofilters_filter_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiofilters_filter_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audiofilters_filter_stop_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiofilters_filter_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_filter), MP_ROM_PTR(&audiofilters_filter_filter_obj) },
    { MP_ROM_QSTR(MP_QSTR_mix), MP_ROM_PTR(&audiofilters_filter_mix_obj) },
    AUDIOSAMPLE_FIELDS,
};
static MP_DEFINE_CONST_DICT(audiofilters_filter_locals_dict, audiofilters_filter_locals_dict_table);

static const audiosample_p_t audiofilters_filter_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .reset_buffer = (audiosample_reset_buffer_fun)audiofilters_filter_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiofilters_filter_get_buffer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    audiofilters_filter_type,
    MP_QSTR_Filter,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiofilters_filter_make_new,
    locals_dict, &audiofilters_filter_locals_dict,
    protocol, &audiofilters_filter_proto
    );
