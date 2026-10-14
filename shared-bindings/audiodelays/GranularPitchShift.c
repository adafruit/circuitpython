// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared-bindings/audiodelays/GranularPitchShift.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-module/audiodelays/GranularPitchShift.h"

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-module/synthio/block.h"

//| class GranularPitchShift:
//|     """A granular-synthesis pitch shift effect"""
//|
//|     def __init__(
//|         self,
//|         semitones: synthio.BlockInput = 0.0,
//|         mix: synthio.BlockInput = 1.0,
//|         grain_size: int = 1024,
//|         density: int = 2,
//|         spread: float = 0.0,
//|         buffer_size: int = 512,
//|         sample_rate: int = 8000,
//|         bits_per_sample: int = 16,
//|         samples_signed: bool = True,
//|         channel_count: int = 1,
//|     ) -> None:
//|         """Create a pitch shift effect that shifts pitch using granular synthesis: a cloud of
//|            short, overlapping, individually-enveloped grains resampled from a capture buffer. Unlike
//|            `PitchShift` (a single crossfaded window), the overlapping grains tend to sound smoother
//|            and less "robotic" at large shifts. This effect introduces a slight delay in the output
//|            proportional to ``grain_size``.
//|
//|            The mix parameter allows you to change how much of the unchanged sample passes through to
//|            the output to how much of the effect audio you hear as the output.
//|
//|         :param synthio.BlockInput semitones: The amount of pitch shifting in semitones (1/12th of an octave)
//|         :param synthio.BlockInput mix: The mix as a ratio of the sample (0.0) to the effect (1.0)
//|         :param int grain_size: The length in samples of each grain
//|         :param int density: The number of overlapping grains (overlap factor). Must be between 1 and 8.
//|         :param float spread: The amount of random jitter applied to each grain's start position,
//|            from 0.0 (deterministic; every grain starts at the same offset) to 1.0 (maximum jitter).
//|            Higher values give the classic granular "cloud" texture. Jitter is always backward in
//|            time, so it never introduces additional latency beyond ``grain_size``.
//|         :param int buffer_size: The total size in bytes of each of the two playback buffers to use
//|         :param int sample_rate: The sample rate to be used
//|         :param int channel_count: The number of channels the source samples contain. 1 = mono; 2 = stereo.
//|         :param int bits_per_sample: The bits per sample of the effect
//|         :param bool samples_signed: Effect is signed (True) or unsigned (False)
//|
//|         Shifting the pitch of a synth by 5 semitones::
//|
//|           import time
//|           import board
//|           import audiobusio
//|           import synthio
//|           import audiodelays
//|           from pwmio import PWMOut
//|           import adafruit_tlv320
//|
//|           mclk_pwm = PWMOut(board.I2S_MCLK, frequency=15_000_000, duty_cycle=2**15)
//|           i2c = board.I2C()
//|           dac = adafruit_tlv320.TLV320DAC3100(i2c)
//|           dac.configure_clocks(sample_rate=44100, bit_depth=16, mclk_freq=15_000_000)
//|           dac.headphone_output = True
//|           audio = audiobusio.I2SOut(board.I2S_BCLK, board.I2S_WS, board.I2S_DIN)
//|           synth = synthio.Synthesizer(channel_count=1, sample_rate=44100)
//|           pitch_shift = audiodelays.GranularPitchShift(semitones=5.0, mix=1.0, spread=0.25, grain_size=2048, density=2, buffer_size=1024, channel_count=1, sample_rate=44100)
//|           pitch_shift.play(synth)
//|           audio.play(pitch_shift)
//|
//|           while True:
//|               for notenum in (60, 64, 67, 71):
//|                   synth.press(notenum)
//|                   time.sleep(0.25)
//|                   synth.release_all()"""
//|         ...
//|
static mp_obj_t audiodelays_granular_pitch_shift_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_semitones, ARG_mix, ARG_grain_size, ARG_density, ARG_spread, ARG_buffer_size, ARG_sample_rate, ARG_bits_per_sample, ARG_samples_signed, ARG_channel_count, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_semitones, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(0)} },
        { MP_QSTR_mix, MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_ROM_INT(1)} },
        { MP_QSTR_grain_size, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1024} },
        { MP_QSTR_density, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 2} },
        { MP_QSTR_spread, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_ROM_INT(0)} },
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
    mp_int_t grain_size = mp_arg_validate_int_min(args[ARG_grain_size].u_int, 2, MP_QSTR_grain_size);
    mp_int_t density = mp_arg_validate_int_range(args[ARG_density].u_int, 1, GRANULAR_MAX_GRAINS, MP_QSTR_density);
    mp_int_t bits_per_sample = args[ARG_bits_per_sample].u_int;
    if (bits_per_sample != 8 && bits_per_sample != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits_per_sample must be 8 or 16"));
    }

    audiodelays_granular_pitch_shift_obj_t *self =
        mp_obj_malloc_with_finaliser(audiodelays_granular_pitch_shift_obj_t, &audiodelays_granular_pitch_shift_type);
    common_hal_audiodelays_granular_pitch_shift_construct(self,
        args[ARG_semitones].u_obj,
        args[ARG_mix].u_obj,
        grain_size,
        density,
        mp_obj_get_float(args[ARG_spread].u_obj),
        args[ARG_buffer_size].u_int,
        bits_per_sample,
        args[ARG_samples_signed].u_bool,
        channel_count,
        sample_rate);

    return MP_OBJ_FROM_PTR(self);
}


//|     def deinit(self) -> None:
//|         """Deinitialises the GranularPitchShift."""
//|         ...
//|
static mp_obj_t audiodelays_granular_pitch_shift_deinit(mp_obj_t self_in) {
    audiodelays_granular_pitch_shift_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_granular_pitch_shift_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_granular_pitch_shift_deinit_obj, audiodelays_granular_pitch_shift_deinit);

static void check_for_deinit(audiodelays_granular_pitch_shift_obj_t *self) {
    audiosample_check_for_deinit(&self->base);
}


//|     def __enter__(self) -> GranularPitchShift:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
static mp_obj_t audiodelays_granular_pitch_shift_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audiodelays_granular_pitch_shift_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audiodelays_granular_pitch_shift___exit___obj, 4, 4, audiodelays_granular_pitch_shift_obj___exit__);


//|     semitones: synthio.BlockInput
//|     """The amount of pitch shifting in semitones (1/12th of an octave)."""
//|
static mp_obj_t audiodelays_granular_pitch_shift_obj_get_semitones(mp_obj_t self_in) {
    audiodelays_granular_pitch_shift_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_audiodelays_granular_pitch_shift_get_semitones(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_granular_pitch_shift_get_semitones_obj, audiodelays_granular_pitch_shift_obj_get_semitones);

static mp_obj_t audiodelays_granular_pitch_shift_obj_set_semitones(mp_obj_t self_in, mp_obj_t semitones_in) {
    audiodelays_granular_pitch_shift_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_granular_pitch_shift_set_semitones(self, semitones_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_granular_pitch_shift_set_semitones_obj, audiodelays_granular_pitch_shift_obj_set_semitones);

MP_PROPERTY_GETSET(audiodelays_granular_pitch_shift_semitones_obj,
    (mp_obj_t)&audiodelays_granular_pitch_shift_get_semitones_obj,
    (mp_obj_t)&audiodelays_granular_pitch_shift_set_semitones_obj);


//|     mix: synthio.BlockInput
//|     """The output mix between 0 and 1 where 0 is only sample and 1 is all effect."""
static mp_obj_t audiodelays_granular_pitch_shift_obj_get_mix(mp_obj_t self_in) {
    return common_hal_audiodelays_granular_pitch_shift_get_mix(self_in);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_granular_pitch_shift_get_mix_obj, audiodelays_granular_pitch_shift_obj_get_mix);

static mp_obj_t audiodelays_granular_pitch_shift_obj_set_mix(mp_obj_t self_in, mp_obj_t mix_in) {
    audiodelays_granular_pitch_shift_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_granular_pitch_shift_set_mix(self, mix_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_granular_pitch_shift_set_mix_obj, audiodelays_granular_pitch_shift_obj_set_mix);

MP_PROPERTY_GETSET(audiodelays_granular_pitch_shift_mix_obj,
    (mp_obj_t)&audiodelays_granular_pitch_shift_get_mix_obj,
    (mp_obj_t)&audiodelays_granular_pitch_shift_set_mix_obj);


//|     spread: float
//|     """The amount of random jitter applied to each grain's start position, from 0.0
//|     (deterministic) to 1.0 (maximum jitter). Higher values give a thicker granular texture."""
static mp_obj_t audiodelays_granular_pitch_shift_obj_get_spread(mp_obj_t self_in) {
    audiodelays_granular_pitch_shift_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_audiodelays_granular_pitch_shift_get_spread(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_granular_pitch_shift_get_spread_obj, audiodelays_granular_pitch_shift_obj_get_spread);

static mp_obj_t audiodelays_granular_pitch_shift_obj_set_spread(mp_obj_t self_in, mp_obj_t spread_in) {
    audiodelays_granular_pitch_shift_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_granular_pitch_shift_set_spread(self, mp_obj_get_float(spread_in));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiodelays_granular_pitch_shift_set_spread_obj, audiodelays_granular_pitch_shift_obj_set_spread);

MP_PROPERTY_GETSET(audiodelays_granular_pitch_shift_spread_obj,
    (mp_obj_t)&audiodelays_granular_pitch_shift_get_spread_obj,
    (mp_obj_t)&audiodelays_granular_pitch_shift_set_spread_obj);


//|     playing: bool
//|     """True when the effect is playing a sample. (read-only)"""
//|
static mp_obj_t audiodelays_granular_pitch_shift_obj_get_playing(mp_obj_t self_in) {
    audiodelays_granular_pitch_shift_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiodelays_granular_pitch_shift_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_granular_pitch_shift_get_playing_obj, audiodelays_granular_pitch_shift_obj_get_playing);

MP_PROPERTY_GETTER(audiodelays_granular_pitch_shift_playing_obj,
    (mp_obj_t)&audiodelays_granular_pitch_shift_get_playing_obj);


//|     def play(self, sample: circuitpython_typing.AudioSample, *, loop: bool = False) -> GranularPitchShift:
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         The sample must match the encoding settings given in the constructor.
//|
//|         :return: The effect object itself. Can be used for chaining, ie:
//|           ``audio.play(effect.play(sample))``.
//|         :rtype: GranularPitchShift"""
//|         ...
//|
static mp_obj_t audiodelays_granular_pitch_shift_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audiodelays_granular_pitch_shift_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audiodelays_granular_pitch_shift_play(self, sample, args[ARG_loop].u_bool);

    return MP_OBJ_FROM_PTR(self);
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiodelays_granular_pitch_shift_play_obj, 1, audiodelays_granular_pitch_shift_obj_play);


//|     def stop(self) -> None:
//|         """Stops playback of the sample."""
//|         ...
//|
//|
static mp_obj_t audiodelays_granular_pitch_shift_obj_stop(mp_obj_t self_in) {
    audiodelays_granular_pitch_shift_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiodelays_granular_pitch_shift_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiodelays_granular_pitch_shift_stop_obj, audiodelays_granular_pitch_shift_obj_stop);


static const mp_rom_map_elem_t audiodelays_granular_pitch_shift_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiodelays_granular_pitch_shift_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&audiodelays_granular_pitch_shift_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audiodelays_granular_pitch_shift___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiodelays_granular_pitch_shift_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audiodelays_granular_pitch_shift_stop_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiodelays_granular_pitch_shift_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_semitones), MP_ROM_PTR(&audiodelays_granular_pitch_shift_semitones_obj) },
    { MP_ROM_QSTR(MP_QSTR_mix), MP_ROM_PTR(&audiodelays_granular_pitch_shift_mix_obj) },
    { MP_ROM_QSTR(MP_QSTR_spread), MP_ROM_PTR(&audiodelays_granular_pitch_shift_spread_obj) },
    AUDIOSAMPLE_FIELDS,
};
static MP_DEFINE_CONST_DICT(audiodelays_granular_pitch_shift_locals_dict, audiodelays_granular_pitch_shift_locals_dict_table);

static const audiosample_p_t audiodelays_granular_pitch_shift_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .reset_buffer = (audiosample_reset_buffer_fun)audiodelays_granular_pitch_shift_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiodelays_granular_pitch_shift_get_buffer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    audiodelays_granular_pitch_shift_type,
    MP_QSTR_GranularPitchShift,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiodelays_granular_pitch_shift_make_new,
    locals_dict, &audiodelays_granular_pitch_shift_locals_dict,
    protocol, &audiodelays_granular_pitch_shift_proto
    );
