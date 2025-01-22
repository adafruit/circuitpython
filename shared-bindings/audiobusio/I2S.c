// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audiobusio/I2S.h"
#include "shared-bindings/util.h"

//| class I2S:
//|     """Connect with an I2S bus to input and/or output an audio stream"""
//|
//|     def __init__(
//|         self,
//|         bit_clock: microcontroller.Pin,
//|         word_select: microcontroller.Pin,
//|         *,
//|         data_out: Optional[microcontroller.Pin] = None,
//|         data_in: Optional[microcontroller.Pin] = None,
//|         main_clock: Optional[microcontroller.Pin] = None,
//|         left_justified: bool = False,
//|         buffer_size: int = 512,
//|         channel_count: int = 2,
//|         sample_rate: int = 8000,
//|         bits_per_sample: int = 16,
//|         samples_signed: bool = True
//|     ) -> None:
//|         """Create a I2S object associated with the given pins.
//|
//|         :param ~microcontroller.Pin bit_clock: The bit clock (or serial clock) pin
//|         :param ~microcontroller.Pin word_select: The word select (or left/right clock) pin
//|         :param ~microcontroller.Pin data_out: The data output pin
//|         :param ~microcontroller.Pin data_in: The data input pin
//|         :param ~microcontroller.Pin main_clock: The main clock pin
//|         :param bool left_justified: True when data bits are aligned with the word select clock. False
//|           when they are shifted by one to match classic I2S protocol.
//|         :param int buffer_size: The total size in bytes of the input buffer. Only used if handling
//|           input.
//|         :param int channel_count: The number of channels. 1 = mono; 2 = stereo. Only used if handling
//|           input.
//|         :param int sample_rate: The desired sample rate. Only used if handling input.
//|         :param int bits_per_sample: Number of bits per sample. Must be divisible by 8. Only used if
//|           handling input.
//|         :param bool samples_signed: Samples are signed (True) or unsigned (False). Only used if
//|           handling input.
//|
//|         Simple 8ksps 440 Hz sine wave on `Metro M0 Express <https://www.adafruit.com/product/3505>`_
//|         using `UDA1334 Breakout <https://www.adafruit.com/product/3678>`_::
//|
//|           import audiobusio
//|           import audiocore
//|           import board
//|           import array
//|           import time
//|           import math
//|
//|           # Generate one period of sine wave.
//|           length = 8000 // 440
//|           sine_wave = array.array("H", [0] * length)
//|           for i in range(length):
//|               sine_wave[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15) + 2 ** 15)
//|
//|           sine_wave = audiocore.RawSample(sine_wave, sample_rate=8000)
//|           i2s = audiobusio.I2S(board.D1, board.D0, data_out=board.D9)
//|           i2s.play(sine_wave, loop=True)
//|           time.sleep(1)
//|           i2s.stop()
//|
//|         Playing a wave file from flash::
//|
//|           import board
//|           import audiocore
//|           import audiobusio
//|           import digitalio
//|
//|           f = open("cplay-5.1-16bit-16khz.wav", "rb")
//|           wav = audiocore.WaveFile(f)
//|
//|           a = audiobusio.I2S(board.D1, board.D0, data_out=board.D9)
//|
//|           print("playing")
//|           a.play(wav)
//|           while a.playing:
//|             pass
//|           print("stopped")
//|
//|         Playing an I2S input signal to a PWMAudioOut::
//|
//|           import audiobusio
//|           import board
//|           import audiopwmio
//|
//|           mic = audiobusio.I2S(board.GP0, board.GP1, data_in=board.GP2, channel_count=1, sample_rate=16000)
//|           dac = audiopwmio.PWMAudioOut(board.GP3)
//|           dac.play(mic)
//|         """
//|     ...
static mp_obj_t audiobusio_i2s_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    #if !CIRCUITPY_AUDIOBUSIO_I2SOUT || !CIRCUITPY_AUDIOBUSIO_I2SIN
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_I2S);
    return NULL;                // Not reachable.
    #else
    enum { ARG_bit_clock, ARG_word_select, ARG_data_out, ARG_data_in, ARG_main_clock, ARG_left_justified, ARG_buffer_size, ARG_channel_count, ARG_sample_rate, ARG_bits_per_sample, ARG_samples_signed };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bit_clock,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_word_select,     MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_data_out,        MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_data_in,         MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_main_clock,      MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_left_justified,  MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_buffer_size,     MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 512} },
        { MP_QSTR_channel_count,   MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 2} },
        { MP_QSTR_sample_rate,     MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 8000} },
        { MP_QSTR_bits_per_sample, MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 16} },
        { MP_QSTR_samples_signed,  MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *bit_clock = validate_obj_is_free_pin(args[ARG_bit_clock].u_obj, MP_QSTR_bit_clock);
    const mcu_pin_obj_t *word_select = validate_obj_is_free_pin(args[ARG_word_select].u_obj, MP_QSTR_word_select);
    const mcu_pin_obj_t *data_out = validate_obj_is_free_pin_or_none(args[ARG_data_out].u_obj, MP_QSTR_data_out);
    const mcu_pin_obj_t *data_in = validate_obj_is_free_pin_or_none(args[ARG_data_in].u_obj, MP_QSTR_data_in);
    const mcu_pin_obj_t *main_clock = validate_obj_is_free_pin_or_none(args[ARG_main_clock].u_obj, MP_QSTR_main_clock);

    mp_int_t channel_count = mp_arg_validate_int_range(args[ARG_channel_count].u_int, 1, 2, MP_QSTR_channel_count);
    mp_int_t sample_rate = mp_arg_validate_int_min(args[ARG_sample_rate].u_int, 1, MP_QSTR_sample_rate);
    mp_int_t bits_per_sample = mp_arg_validate_int_range(args[ARG_bits_per_sample].u_int, 8, 32, MP_QSTR_bits_per_sample);
    if (bits_per_sample % 8 != 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be multiple of 8."), MP_QSTR_bits_per_sample);
    }

    audiobusio_i2s_obj_t *self = mp_obj_malloc(audiobusio_i2s_obj_t, &audiobusio_i2s_type);
    common_hal_audiobusio_i2s_construct(self, bit_clock, word_select, data_out, data_in, main_clock, args[ARG_left_justified].u_bool, args[ARG_buffer_size].u_int, channel_count, sample_rate, bits_per_sample, args[ARG_samples_signed].u_bool);

    return MP_OBJ_FROM_PTR(self);
    #endif
}

#if CIRCUITPY_AUDIOBUSIO_I2SOUT && CIRCUITPY_AUDIOBUSIO_I2SIN

//|     def deinit(self) -> None:
//|         """Deinitialises the I2S and releases any hardware resources for reuse."""
//|         ...
static mp_obj_t audiobusio_i2s_deinit(mp_obj_t self_in) {
    audiobusio_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiobusio_i2s_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2s_deinit_obj, audiobusio_i2s_deinit);

static void check_for_deinit(audiobusio_i2s_obj_t *self) {
    if (common_hal_audiobusio_i2s_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> I2S:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
static mp_obj_t audiobusio_i2s_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audiobusio_i2s_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audiobusio_i2s___exit___obj, 4, 4, audiobusio_i2s_obj___exit__);

//|     def record(self, destination: WriteableBuffer, destination_length: int) -> int:
//|         """Records destination_length bytes of samples to destination. This is
//|         blocking.
//|
//|         An IOError may be raised when the destination is too slow to record the
//|         audio at the given rate. For internal flash, writing all 1s to the file
//|         before recording is recommended to speed up writes.
//|
//|         :return: The number of samples recorded. If this is less than ``destination_length``,
//|           some samples were missed due to processing time."""
//|         ...
static mp_obj_t audiobusio_i2s_obj_record(mp_obj_t self_obj, mp_obj_t destination, mp_obj_t destination_length) {
    audiobusio_i2s_obj_t *self = MP_OBJ_TO_PTR(self_obj);
    check_for_deinit(self);
    uint32_t length = mp_arg_validate_type_int(destination_length, MP_QSTR_length);
    mp_arg_validate_length_min(length, 0, MP_QSTR_length);

    mp_buffer_info_t bufinfo;
    if (mp_obj_is_type(destination, &mp_type_fileio)) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Cannot record to a file"));
    } else if (mp_get_buffer(destination, &bufinfo, MP_BUFFER_WRITE)) {
        if (bufinfo.len / mp_binary_get_size('@', bufinfo.typecode, NULL) < length) {
            mp_raise_ValueError(MP_ERROR_TEXT("Destination capacity is smaller than destination_length."));
        }
        uint8_t bit_depth = common_hal_audiobusio_i2s_get_bits_per_sample(self);
        if (bufinfo.typecode != 'h' && bit_depth == 16) {
            mp_raise_ValueError(MP_ERROR_TEXT("destination buffer must be an array of type 'h' for bit_depth = 16"));
        } else if (bufinfo.typecode != 'B' && bufinfo.typecode != BYTEARRAY_TYPECODE && bit_depth == 8) {
            mp_raise_ValueError(MP_ERROR_TEXT("destination buffer must be a bytearray or array of type 'B' for bit_depth = 8"));
        }
        // length is the buffer length in slots, not bytes.
        uint32_t length_written =
            common_hal_audiobusio_i2s_record_to_buffer(self, bufinfo.buf, length);
        return MP_OBJ_NEW_SMALL_INT(length_written);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(audiobusio_i2s_record_obj, audiobusio_i2s_obj_record);

//|     def play(self, sample: circuitpython_typing.AudioSample, *, loop: bool = False) -> None:
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         Sample must be an `audiocore.WaveFile`, `audiocore.RawSample`, `audiomixer.Mixer` or `audiomp3.MP3Decoder`.
//|
//|         The sample itself should consist of 8 bit or 16 bit samples."""
//|         ...
static mp_obj_t audiobusio_i2s_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample,    MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_loop,      MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    audiobusio_i2s_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_audiobusio_i2s_play(self, sample, args[ARG_loop].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(audiobusio_i2s_play_obj, 1, audiobusio_i2s_obj_play);

//|     def stop(self) -> None:
//|         """Stops playback."""
//|         ...
static mp_obj_t audiobusio_i2s_obj_stop(mp_obj_t self_in) {
    audiobusio_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_audiobusio_i2s_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2s_stop_obj, audiobusio_i2s_obj_stop);

//|     playing: bool
//|     """True when the audio sample is being output. (read-only)"""
static mp_obj_t audiobusio_i2s_obj_get_playing(mp_obj_t self_in) {
    audiobusio_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiobusio_i2s_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2s_get_playing_obj, audiobusio_i2s_obj_get_playing);

MP_PROPERTY_GETTER(audiobusio_i2s_playing_obj,
    (mp_obj_t)&audiobusio_i2s_get_playing_obj);

//|     def pause(self) -> None:
//|         """Stops playback temporarily while remembering the position. Use `resume` to resume playback."""
//|         ...
static mp_obj_t audiobusio_i2s_obj_pause(mp_obj_t self_in) {
    audiobusio_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    if (!common_hal_audiobusio_i2s_get_playing(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Not playing"));
    }
    common_hal_audiobusio_i2s_pause(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2s_pause_obj, audiobusio_i2s_obj_pause);

//|     def resume(self) -> None:
//|         """Resumes sample playback after :py:func:`pause`."""
//|         ...
static mp_obj_t audiobusio_i2s_obj_resume(mp_obj_t self_in) {
    audiobusio_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    if (common_hal_audiobusio_i2s_get_paused(self)) {
        common_hal_audiobusio_i2s_resume(self);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2s_resume_obj, audiobusio_i2s_obj_resume);

//|     paused: bool
//|     """True when playback is paused. (read-only)"""
//|
static mp_obj_t audiobusio_i2s_obj_get_paused(mp_obj_t self_in) {
    audiobusio_i2s_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiobusio_i2s_get_paused(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2s_get_paused_obj, audiobusio_i2s_obj_get_paused);

MP_PROPERTY_GETTER(audiobusio_i2s_paused_obj,
    (mp_obj_t)&audiobusio_i2s_get_paused_obj);

#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT && CIRCUITPY_AUDIOBUSIO_I2SIN

static const mp_rom_map_elem_t audiobusio_i2s_locals_dict_table[] = {
    #if CIRCUITPY_AUDIOBUSIO_I2SOUT && CIRCUITPY_AUDIOBUSIO_I2SIN
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiobusio_i2s_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audiobusio_i2s___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_record), MP_ROM_PTR(&audiobusio_i2s_record_obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiobusio_i2s_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audiobusio_i2s_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause), MP_ROM_PTR(&audiobusio_i2s_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume), MP_ROM_PTR(&audiobusio_i2s_resume_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiobusio_i2s_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_paused), MP_ROM_PTR(&audiobusio_i2s_paused_obj) },
    #endif // CIRCUITPY_AUDIOBUSIO_I2SOUT && CIRCUITPY_AUDIOBUSIO_I2SIN
};
static MP_DEFINE_CONST_DICT(audiobusio_i2s_locals_dict, audiobusio_i2s_locals_dict_table);

static const audiosample_p_t audiobusio_i2s_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    #if CIRCUITPY_AUDIOBUSIO_I2SOUT && CIRCUITPY_AUDIOBUSIO_I2SIN
    .sample_rate = (audiosample_sample_rate_fun)common_hal_audiobusio_i2s_get_sample_rate,
    .bits_per_sample = (audiosample_bits_per_sample_fun)common_hal_audiobusio_i2s_get_bits_per_sample,
    .channel_count = (audiosample_channel_count_fun)common_hal_audiobusio_i2s_get_channel_count,
    .reset_buffer = (audiosample_reset_buffer_fun)audiobusio_i2s_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiobusio_i2s_get_buffer,
    .get_buffer_structure = (audiosample_get_buffer_structure_fun)audiobusio_i2s_get_buffer_structure,
    #endif // CIRCUITPY_AUDIOBUSIO_I2SOUT && CIRCUITPY_AUDIOBUSIO_I2SIN
};

MP_DEFINE_CONST_OBJ_TYPE(
    audiobusio_i2s_type,
    MP_QSTR_I2S,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiobusio_i2s_make_new,
    locals_dict, &audiobusio_i2s_locals_dict,
    protocol, &audiobusio_i2s_proto
    );
