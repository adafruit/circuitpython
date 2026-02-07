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
#include "shared-bindings/audiobusio/I2SIn.h"
#include "shared-bindings/util.h"

//| class I2SIn:
//|     """Record an input I2S audio stream"""
//|
//|     def __init__(
//|         self,
//|         bit_clock: microcontroller.Pin,
//|         word_select: microcontroller.Pin,
//|         data: microcontroller.Pin,
//|         *,
//|         buffer_size: int = 512,
//|         channel_count: int = 2,
//|         sample_rate: int = 8000,
//|         bits_per_sample: int = 16,
//|         samples_signed: bool = True
//|     ) -> None:
//|         """Create a I2SIn object associated with the given pins. This allows you to
//|         record audio signals from the given pins. Individual ports may put further
//|         restrictions on the recording parameters.
//|
//|         :param ~microcontroller.Pin bit_clock: The bit clock (or serial clock) pin
//|         :param ~microcontroller.Pin word_select: The word select (or left/right clock) pin
//|         :param ~microcontroller.Pin data: The data pin
//|         :param int buffer_size: The total size in bytes of the input buffer
//|         :param int channel_count: The number of channels. 1 = mono; 2 = stereo.
//|         :param int sample_rate: The desired sample rate
//|         :param int bits_per_sample: Number of bits per sample. Must be divisible by 8
//|         :param bool samples_signed: Samples are signed (True) or unsigned (False)
//|
//|         Playing an I2SIn signal to a PWMAudioOut::
//|
//|           import audiobusio
//|           import board
//|           import audiopwmio
//|
//|           mic = audiobusio.I2SIn(board.GP0, board.GP1, board.GP2, channel_count=1, sample_rate=16000)
//|           dac = audiopwmio.PWMAudioOut(board.GP3)
//|           dac.play(mic)
//|         """
//|     ...
static mp_obj_t audiobusio_i2sin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    #if !CIRCUITPY_AUDIOBUSIO_I2SIN || CIRCUITPY_AUDIOBUSIO_I2SOUT
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_I2SIn);
    return NULL;                // Not reachable.
    #else
    enum { ARG_bit_clock, ARG_word_select, ARG_data, ARG_buffer_size, ARG_channel_count, ARG_sample_rate, ARG_bits_per_sample, ARG_samples_signed };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bit_clock,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_word_select,     MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_data,            MP_ARG_REQUIRED | MP_ARG_OBJ },
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
    const mcu_pin_obj_t *data = validate_obj_is_free_pin(args[ARG_data].u_obj, MP_QSTR_data);

    mp_int_t channel_count = mp_arg_validate_int_range(args[ARG_channel_count].u_int, 1, 2, MP_QSTR_channel_count);
    mp_int_t sample_rate = mp_arg_validate_int_min(args[ARG_sample_rate].u_int, 1, MP_QSTR_sample_rate);
    mp_int_t bits_per_sample = mp_arg_validate_int_range(args[ARG_bits_per_sample].u_int, 8, 32, MP_QSTR_bits_per_sample);
    if (bits_per_sample % 8 != 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be multiple of 8."), MP_QSTR_bits_per_sample);
    }

    audiobusio_i2sin_obj_t *self = mp_obj_malloc(audiobusio_i2sin_obj_t, &audiobusio_i2sin_type);
    common_hal_audiobusio_i2sin_construct(self, bit_clock, word_select, data, args[ARG_buffer_size].u_int, channel_count, sample_rate, bits_per_sample, args[ARG_samples_signed].u_bool);

    return MP_OBJ_FROM_PTR(self);
    #endif
}

#if CIRCUITPY_AUDIOBUSIO_I2SIN && !CIRCUITPY_AUDIOBUSIO_I2SOUT

//|     def deinit(self) -> None:
//|         """Deinitialises the I2SIn and releases any hardware resources for reuse."""
//|         ...
static mp_obj_t audiobusio_i2sin_deinit(mp_obj_t self_in) {
    audiobusio_i2sin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiobusio_i2sin_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiobusio_i2sin_deinit_obj, audiobusio_i2sin_deinit);

//|     def __enter__(self) -> I2SIn:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context."""
//|         ...
//|
static mp_obj_t audiobusio_i2sin_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audiobusio_i2sin_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audiobusio_i2sin___exit___obj, 4, 4, audiobusio_i2sin_obj___exit__);

#endif // CIRCUITPY_AUDIOBUSIO_I2SIN

static const mp_rom_map_elem_t audiobusio_i2sin_locals_dict_table[] = {
    #if CIRCUITPY_AUDIOBUSIO_I2SIN && !CIRCUITPY_AUDIOBUSIO_I2SOUT
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiobusio_i2sin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audiobusio_i2sin___exit___obj) },
    #endif // CIRCUITPY_AUDIOBUSIO_I2SIN
};
static MP_DEFINE_CONST_DICT(audiobusio_i2sin_locals_dict, audiobusio_i2sin_locals_dict_table);

static const audiosample_p_t audiobusio_i2sin_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    #if CIRCUITPY_AUDIOBUSIO_I2SIN && !CIRCUITPY_AUDIOBUSIO_I2SOUT
    .sample_rate = (audiosample_sample_rate_fun)common_hal_audiobusio_i2sin_get_sample_rate,
    .bits_per_sample = (audiosample_bits_per_sample_fun)common_hal_audiobusio_i2sin_get_bits_per_sample,
    .channel_count = (audiosample_channel_count_fun)common_hal_audiobusio_i2sin_get_channel_count,
    .reset_buffer = (audiosample_reset_buffer_fun)audiobusio_i2sin_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiobusio_i2sin_get_buffer,
    .get_buffer_structure = (audiosample_get_buffer_structure_fun)audiobusio_i2sin_get_buffer_structure,
    #endif // CIRCUITPY_AUDIOBUSIO_I2SIN
};

MP_DEFINE_CONST_OBJ_TYPE(
    audiobusio_i2sin_type,
    MP_QSTR_I2SIn,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiobusio_i2sin_make_new,
    locals_dict, &audiobusio_i2sin_locals_dict,
    protocol, &audiobusio_i2sin_proto
    );
