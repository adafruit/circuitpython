// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared-bindings/audiodelays/Flanger.h"
#include "shared-module/audiodelays/Flanger.h"
#include "shared-bindings/audiocore/__init__.h"

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-module/synthio/block.h"

//| class Flanger:
//|     """A Flanger effect"""
//|
//|     def __init__(
//|         self,
//|         max_delay_ms: int = 10,
//|         min_delay_ms: synthio.BlockInput = 1.0,
//|         rate: synthio.BlockInput = 0.5,
//|         depth: synthio.BlockInput = 0.5,
//|         feedback: synthio.BlockInput = 0.5,
//|         mix: synthio.BlockInput = 0.5,
//|         invert: bool = False,
//|         buffer_size: int = 512,
//|         sample_rate: int = 8000,
//|         bits_per_sample: int = 16,
//|         samples_signed: bool = True,
//|         channel_count: int = 1,
//|     ) -> None:
//|         """Create a Flanger effect by mixing the sample with a copy of itself taken from a very
//|            short delay whose length is continuously swept up and down by an internal low frequency
//|            oscillator. The moving delay creates a comb filter whose notches sweep through the
//|            spectrum, and the ``feedback`` path routes the delayed signal back into the delay line
//|            to sharpen those notches into resonant peaks.
//|
//|            The delay is swept upwards from ``min_delay_ms`` towards ``max_delay_ms``::
//|
//|              sweep_top = min_delay_ms + depth * (max_delay_ms - min_delay_ms)
//|              current_delay = min_delay_ms + triangle_lfo() * (sweep_top - min_delay_ms)
//|
//|            where ``triangle_lfo()`` ranges from 0.0 to 1.0 at ``rate`` Hz. Because the sweep runs
//|            upwards from a floor rather than around a centre, no combination of ``min_delay_ms`` and
//|            ``depth`` can push the delay outside of the buffer.
//|
//|
//|         :param int max_delay_ms: The maximum delay the flanger can sweep to in milliseconds, valid range is 1-100.
//|         :param synthio.BlockInput min_delay_ms: The shortest delay of the sweep in milliseconds. Clamped between the length of one sample and max_delay_ms.
//|         :param synthio.BlockInput rate: The frequency of the sweep in hertz. Clamped between 0.0 and 20.0. A rate of 0.0 holds the delay still.
//|         :param synthio.BlockInput depth: How much of the range above min_delay_ms is swept, from 0.0 to 1.0.
//|         :param synthio.BlockInput feedback: How much of the delayed signal is fed back into the delay line, from -0.95 to 0.95. Negative values invert the polarity of the feedback.
//|         :param synthio.BlockInput mix: How much of the wet audio to include along with the original signal, from 0.0 to 1.0.
//|         :param bool invert: Subtract the wet signal from the dry signal instead of adding it, which flips which frequencies cancel.
//|         :param int buffer_size: The total size in bytes of each of the two playback buffers to use
//|         :param int sample_rate: The sample rate to be used
//|         :param int channel_count: The number of channels the source samples contain. 1 = mono; 2 = stereo.
//|         :param int bits_per_sample: The bits per sample of the effect
//|         :param bool samples_signed: Effect is signed (True) or unsigned (False)
//|
//|         Adding a flanger to a synth::
//|
//|           import time
//|           import board
//|           import audiobusio
//|           import synthio
//|           import audiodelays
//|
//|           audio = audiobusio.I2SOut(board.I2S_BIT_CLOCK, board.I2S_WS, board.I2S_DOUT)
//|           synth = synthio.Synthesizer(channel_count=1, sample_rate=44100)
//|           flanger = audiodelays.Flanger(max_delay_ms=10, min_delay_ms=1.0, rate=0.3, depth=0.8,
//|                                         feedback=0.7, mix=1.0, buffer_size=1024,
//|                                         channel_count=1, sample_rate=44100)
//|           audio.play(flanger.play(synth))
//|
//|           note = synthio.Note(261)
//|           while True:
//|               synth.press(note)
//|               time.sleep(3)
//|               synth.release(note)
//|               time.sleep(1)"""
//|         ...
//|
static mp_obj_t audiodelays_flanger_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_max_delay_ms, ARG_min_delay_ms, ARG_rate, ARG_depth, ARG_feedback, ARG_mix, ARG_invert, ARG_buffer_size, ARG_sample_rate, ARG_bits_per_sample, ARG_samples_signed, ARG_channel_count, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_max_delay_ms, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 10 } },
        { MP_QSTR_min_delay_ms, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_rate, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_depth, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_feedback, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mix, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_invert, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
        { MP_QSTR_buffer_size, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 512} },
        { MP_QSTR_sample_rate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 8000} },
        { MP_QSTR_bits_per_sample, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 16} },
        { MP_QSTR_samples_signed, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
        { MP_QSTR_channel_count, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1 } },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t max_delay_ms = mp_arg_validate_int_range(args[ARG_max_delay_ms].u_int, 1, 100, MP_QSTR_max_delay_ms);

    mp_int_t channel_count = mp_arg_validate_int_range(args[ARG_channel_count].u_int, 1, 2, MP_QSTR_channel_count);
    mp_int_t sample_rate = mp_arg_validate_int_min(args[ARG_sample_rate].u_int, 1, MP_QSTR_sample_rate);
    mp_int_t bits_per_sample = args[ARG_bits_per_sample].u_int;
    if (bits_per_sample != 8 && bits_per_sample != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits_per_sample must be 8 or 16"));
    }

    audiodelays_flanger_obj_t *self = mp_obj_malloc_with_finaliser(audiodelays_flanger_obj_t, &audiodelays_flanger_type);
    common_hal_audiodelays_flanger_construct(self, max_delay_ms, args[ARG_min_delay_ms].u_obj, args[ARG_rate].u_obj, args[ARG_depth].u_obj, args[ARG_feedback].u_obj, args[ARG_mix].u_obj, args[ARG_invert].u_bool, args[ARG_buffer_size].u_int, bits_per_sample, args[ARG_samples_signed].u_bool, channel_count, sample_rate);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the Flanger."""
//|         ...
//|
static mp_obj_t audiodelays_flanger_deinit(mp_obj_t self_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_flanger_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_flanger_deinit_obj, audiodelays_flanger_deinit);

static void check_for_deinit(audiodelays_flanger_obj_t *self) {
    audiosample_check_for_deinit(&self->base);
}

//|     def __enter__(self) -> Flanger:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
static mp_obj_t audiodelays_flanger_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audiodelays_flanger_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audiodelays_flanger___exit___obj, 4, 4, audiodelays_flanger_obj___exit__);


//|     min_delay_ms: synthio.BlockInput
//|     """The shortest delay of the sweep in milliseconds. This is the floor that the sweep runs
//|     upwards from. Clamped between the length of a single sample and ``max_delay_ms``."""
//|
static mp_obj_t audiodelays_flanger_obj_get_min_delay_ms(mp_obj_t self_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_audiodelays_flanger_get_min_delay_ms(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_flanger_get_min_delay_ms_obj, audiodelays_flanger_obj_get_min_delay_ms);

static mp_obj_t audiodelays_flanger_obj_set_min_delay_ms(mp_obj_t self_in, mp_obj_t min_delay_ms_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_flanger_set_min_delay_ms(self, min_delay_ms_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_flanger_set_min_delay_ms_obj, audiodelays_flanger_obj_set_min_delay_ms);

MP_PROPERTY_GETSET(audiodelays_flanger_min_delay_ms_obj,
    (mp_obj_t)&audiodelays_flanger_get_min_delay_ms_obj,
    (mp_obj_t)&audiodelays_flanger_set_min_delay_ms_obj);

//|     rate: synthio.BlockInput
//|     """The frequency of the delay sweep in hertz. Clamped between 0.0 and 20.0. A rate of 0.0
//|     holds the delay still, which leaves a fixed comb filter."""
//|
static mp_obj_t audiodelays_flanger_obj_get_rate(mp_obj_t self_in) {
    return common_hal_audiodelays_flanger_get_rate(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_flanger_get_rate_obj, audiodelays_flanger_obj_get_rate);

static mp_obj_t audiodelays_flanger_obj_set_rate(mp_obj_t self_in, mp_obj_t rate_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_flanger_set_rate(self, rate_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_flanger_set_rate_obj, audiodelays_flanger_obj_set_rate);

MP_PROPERTY_GETSET(audiodelays_flanger_rate_obj,
    (mp_obj_t)&audiodelays_flanger_get_rate_obj,
    (mp_obj_t)&audiodelays_flanger_set_rate_obj);

//|     depth: synthio.BlockInput
//|     """How much of the range between ``min_delay_ms`` and ``max_delay_ms`` the sweep covers, from
//|     0.0 to 1.0. A depth of 0.0 leaves a static short delay."""
//|
static mp_obj_t audiodelays_flanger_obj_get_depth(mp_obj_t self_in) {
    return common_hal_audiodelays_flanger_get_depth(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_flanger_get_depth_obj, audiodelays_flanger_obj_get_depth);

static mp_obj_t audiodelays_flanger_obj_set_depth(mp_obj_t self_in, mp_obj_t depth_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_flanger_set_depth(self, depth_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_flanger_set_depth_obj, audiodelays_flanger_obj_set_depth);

MP_PROPERTY_GETSET(audiodelays_flanger_depth_obj,
    (mp_obj_t)&audiodelays_flanger_get_depth_obj,
    (mp_obj_t)&audiodelays_flanger_set_depth_obj);

//|     feedback: synthio.BlockInput
//|     """How much of the delayed signal is fed back into the delay line, clamped between -0.95 and
//|     0.95. Larger magnitudes give a more resonant, ringing sweep. Negative values invert the
//|     polarity of the feedback for the hollower variant."""
//|
static mp_obj_t audiodelays_flanger_obj_get_feedback(mp_obj_t self_in) {
    return common_hal_audiodelays_flanger_get_feedback(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_flanger_get_feedback_obj, audiodelays_flanger_obj_get_feedback);

static mp_obj_t audiodelays_flanger_obj_set_feedback(mp_obj_t self_in, mp_obj_t feedback_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_flanger_set_feedback(self, feedback_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_flanger_set_feedback_obj, audiodelays_flanger_obj_set_feedback);

MP_PROPERTY_GETSET(audiodelays_flanger_feedback_obj,
    (mp_obj_t)&audiodelays_flanger_get_feedback_obj,
    (mp_obj_t)&audiodelays_flanger_set_feedback_obj);

//|     mix: synthio.BlockInput
//|     """How much of the wet audio to include along with the original signal, clamped between 0.0
//|     and 1.0 where 0.0 is only the sample and 1.0 is the full effect."""
//|
static mp_obj_t audiodelays_flanger_obj_get_mix(mp_obj_t self_in) {
    return common_hal_audiodelays_flanger_get_mix(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_flanger_get_mix_obj, audiodelays_flanger_obj_get_mix);

static mp_obj_t audiodelays_flanger_obj_set_mix(mp_obj_t self_in, mp_obj_t mix_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_flanger_set_mix(self, mix_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_flanger_set_mix_obj, audiodelays_flanger_obj_set_mix);

MP_PROPERTY_GETSET(audiodelays_flanger_mix_obj,
    (mp_obj_t)&audiodelays_flanger_get_mix_obj,
    (mp_obj_t)&audiodelays_flanger_set_mix_obj);

//|     invert: bool
//|     """Whether the wet signal is subtracted from the dry signal instead of added to it. This
//|     flips which frequencies the comb filter cancels, the same as the polarity switch on a
//|     flanger pedal."""
//|
static mp_obj_t audiodelays_flanger_obj_get_invert(mp_obj_t self_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_audiodelays_flanger_get_invert(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_flanger_get_invert_obj, audiodelays_flanger_obj_get_invert);

static mp_obj_t audiodelays_flanger_obj_set_invert(mp_obj_t self_in, mp_obj_t invert_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_flanger_set_invert(self, mp_obj_is_true(invert_in));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_flanger_set_invert_obj, audiodelays_flanger_obj_set_invert);

MP_PROPERTY_GETSET(audiodelays_flanger_invert_obj,
    (mp_obj_t)&audiodelays_flanger_get_invert_obj,
    (mp_obj_t)&audiodelays_flanger_set_invert_obj);

//|     lfo_value: float
//|     """The current value of the internal LFO that sweeps the delay time, from 0.0 at
//|     `min_delay_ms` to 1.0 at the top of the sweep. The LFO only advances while audio is being
//|     played. (read-only)"""
//|
static mp_obj_t audiodelays_flanger_obj_get_lfo_value(mp_obj_t self_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_audiodelays_flanger_get_lfo_value(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_flanger_get_lfo_value_obj, audiodelays_flanger_obj_get_lfo_value);

MP_PROPERTY_GETTER(audiodelays_flanger_lfo_value_obj,
    (mp_obj_t)&audiodelays_flanger_get_lfo_value_obj);

//|     playing: bool
//|     """True when the effect is playing a sample. (read-only)"""
//|
static mp_obj_t audiodelays_flanger_obj_get_playing(mp_obj_t self_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiodelays_flanger_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_flanger_get_playing_obj, audiodelays_flanger_obj_get_playing);

MP_PROPERTY_GETTER(audiodelays_flanger_playing_obj,
    (mp_obj_t)&audiodelays_flanger_get_playing_obj);

//|     def play(self, sample: circuitpython_typing.AudioSample, *, loop: bool = False) -> Flanger:
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         The sample must match the encoding settings given in the constructor.
//|
//|         :return: The effect object itself. Can be used for chaining, ie:
//|           ``audio.play(effect.play(sample))``.
//|         :rtype: Flanger"""
//|         ...
//|
static mp_obj_t audiodelays_flanger_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audiodelays_flanger_play(self, sample, args[ARG_loop].u_bool);

    return MP_OBJ_FROM_PTR(self);
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiodelays_flanger_play_obj, 1, audiodelays_flanger_obj_play);

//|     def stop(self) -> None:
//|         """Stops playback of the sample."""
//|         ...
//|
//|
static mp_obj_t audiodelays_flanger_obj_stop(mp_obj_t self_in) {
    audiodelays_flanger_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_audiodelays_flanger_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_flanger_stop_obj, audiodelays_flanger_obj_stop);

static const mp_rom_map_elem_t audiodelays_flanger_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiodelays_flanger_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&audiodelays_flanger_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audiodelays_flanger___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiodelays_flanger_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audiodelays_flanger_stop_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiodelays_flanger_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_min_delay_ms), MP_ROM_PTR(&audiodelays_flanger_min_delay_ms_obj) },
    { MP_ROM_QSTR(MP_QSTR_rate), MP_ROM_PTR(&audiodelays_flanger_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_depth), MP_ROM_PTR(&audiodelays_flanger_depth_obj) },
    { MP_ROM_QSTR(MP_QSTR_feedback), MP_ROM_PTR(&audiodelays_flanger_feedback_obj) },
    { MP_ROM_QSTR(MP_QSTR_mix), MP_ROM_PTR(&audiodelays_flanger_mix_obj) },
    { MP_ROM_QSTR(MP_QSTR_invert), MP_ROM_PTR(&audiodelays_flanger_invert_obj) },
    { MP_ROM_QSTR(MP_QSTR_lfo_value), MP_ROM_PTR(&audiodelays_flanger_lfo_value_obj) },
    AUDIOSAMPLE_FIELDS,
};
static MP_DEFINE_CONST_DICT(audiodelays_flanger_locals_dict, audiodelays_flanger_locals_dict_table);

static const audiosample_p_t audiodelays_flanger_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .reset_buffer = (audiosample_reset_buffer_fun)audiodelays_flanger_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiodelays_flanger_get_buffer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    audiodelays_flanger_type,
    MP_QSTR_Flanger,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiodelays_flanger_make_new,
    locals_dict, &audiodelays_flanger_locals_dict,
    protocol, &audiodelays_flanger_proto
    );
