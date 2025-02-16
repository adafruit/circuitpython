// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/audiocore/__init__.h"

//| class RawSample:
//|     """A raw audio sample buffer in memory"""
//|
//|     def __init__(
//|         self,
//|         buffer: ReadableBuffer,
//|         *,
//|         channel_count: int = 1,
//|         sample_rate: int = 8000,
//|         single_buffer: bool = True,
//|     ) -> None:
//|         """Create a RawSample based on the given buffer of values. If channel_count is more than
//|         1 then each channel's samples should alternate. In other words, for a two channel buffer, the
//|         first sample will be for channel 1, the second sample will be for channel two, the third for
//|         channel 1 and so on.
//|
//|         :param ~circuitpython_typing.ReadableBuffer buffer: A buffer with samples
//|         :param int channel_count: The number of channels in the buffer
//|         :param int sample_rate: The desired playback sample rate
//|         :param bool single_buffer: Selects single buffered or double buffered transfer mode.  This affects
//|                                    what happens if the sample buffer is changed while the sample is playing.
//|                                    In single buffered transfers, a change in buffer contents will not affect active playback.
//|                                    In double buffered transfers, changed buffer contents will
//|                                    be played back when the transfer reaches the next half-buffer point.
//|
//|         Playing 8ksps 440 Hz and 880 Hz sine waves::
//|
//|           import analogbufio
//|           import array
//|           import audiocore
//|           import audiopwmio
//|           import board
//|           import math
//|           import time
//|
//|           # Generate one period of sine wave.
//|           length = 8000 // 440
//|           sine_wave = array.array("h", [0] * length)
//|           for i in range(length):
//|               sine_wave[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15))
//|           pwm = audiopwmio.PWMAudioOut(left_channel=board.D12, right_channel=board.D13)
//|
//|           # Play single-buffered
//|           sample = audiocore.RawSample(sine_wave)
//|           pwm.play(sample, loop=True)
//|           time.sleep(3)
//|           # changing the wave has no effect
//|           for i in range(length):
//|                sine_wave[i] = int(math.sin(math.pi * 4 * i / length) * (2 ** 15))
//|           time.sleep(3)
//|           pwm.stop()
//|           time.sleep(1)
//|
//|           # Play double-buffered
//|           sample = audiocore.RawSample(sine_wave, single_buffer=False)
//|           pwm.play(sample, loop=True)
//|           time.sleep(3)
//|           # changing the wave takes effect almost immediately
//|           for i in range(length):
//|               sine_wave[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15))
//|           time.sleep(3)
//|           pwm.stop()
//|           pwm.deinit()"""
//|         ...
//|
static mp_obj_t audioio_rawsample_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_buffer, ARG_channel_count, ARG_sample_rate, ARG_single_buffer };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL } },
        { MP_QSTR_channel_count, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1 } },
        { MP_QSTR_sample_rate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 8000} },
        { MP_QSTR_single_buffer, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    audioio_rawsample_obj_t *self = mp_obj_malloc(audioio_rawsample_obj_t, &audioio_rawsample_type);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);
    uint8_t bytes_per_sample = 1;
    bool signed_samples = bufinfo.typecode == 'b' || bufinfo.typecode == 'h';
    if (bufinfo.typecode == 'h' || bufinfo.typecode == 'H') {
        bytes_per_sample = 2;
    } else if (bufinfo.typecode != 'b' && bufinfo.typecode != 'B' && bufinfo.typecode != BYTEARRAY_TYPECODE) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be a bytearray or array of type 'h', 'H', 'b', or 'B'"), MP_QSTR_buffer);
    }
    if (!args[ARG_single_buffer].u_bool && bufinfo.len % (bytes_per_sample * args[ARG_channel_count].u_int * 2) != 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Length of %q must be an even multiple of channel_count * type_size"), MP_QSTR_buffer);
    }
    common_hal_audioio_rawsample_construct(self, ((uint8_t *)bufinfo.buf), bufinfo.len,
        bytes_per_sample, signed_samples, args[ARG_channel_count].u_int,
        args[ARG_sample_rate].u_int, args[ARG_single_buffer].u_bool);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the RawSample and releases any hardware resources for reuse."""
//|         ...
//|
static mp_obj_t audioio_rawsample_deinit(mp_obj_t self_in) {
    audioio_rawsample_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audioio_rawsample_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audioio_rawsample_deinit_obj, audioio_rawsample_deinit);

//|     def __enter__(self) -> RawSample:
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

//|     sample_rate: Optional[int]
//|     """32 bit value that dictates how quickly samples are played in Hertz (cycles per second).
//|     When the sample is looped, this can change the pitch output without changing the underlying
//|     sample. This will not change the sample rate of any active playback. Call ``play`` again to
//|     change it."""
//|
//|

static const mp_rom_map_elem_t audioio_rawsample_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audioio_rawsample_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },

    // Properties
    AUDIOSAMPLE_FIELDS,
};
static MP_DEFINE_CONST_DICT(audioio_rawsample_locals_dict, audioio_rawsample_locals_dict_table);

static const audiosample_p_t audioio_rawsample_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .reset_buffer = (audiosample_reset_buffer_fun)audioio_rawsample_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audioio_rawsample_get_buffer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    audioio_rawsample_type,
    MP_QSTR_RawSample,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audioio_rawsample_make_new,
    locals_dict, &audioio_rawsample_locals_dict,
    protocol, &audioio_rawsample_proto
    );
