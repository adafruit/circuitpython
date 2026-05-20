// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "extmod/vfs_fat.h"
#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audioi2sin/I2SIn.h"
#include "shared-bindings/util.h"

//| class I2SIn:
//|     """Record an input I2S audio stream from an external I2S source such as a MEMS microphone."""
//|
//|     def __init__(
//|         self,
//|         bit_clock: microcontroller.Pin,
//|         word_select: microcontroller.Pin,
//|         data: microcontroller.Pin,
//|         *,
//|         main_clock: Optional[microcontroller.Pin] = None,
//|         sample_rate: int = 16000,
//|         bit_depth: int = 16,
//|         output_bit_depth: Optional[int] = None,
//|         mono: bool = True,
//|         left_justified: bool = False,
//|         samples_signed: bool = True,
//|     ) -> None:
//|         """Create an I2SIn object associated with the given pins. This allows you to
//|         record audio signals from an external I2S source (e.g. an I2S MEMS microphone
//|         like the SPH0645LM4H or INMP441).
//|
//|         The pin signature mirrors `audiobusio.I2SOut` so users can swap classes;
//|         recording parameters mirror `audiobusio.PDMIn`.
//|
//|         :param ~microcontroller.Pin bit_clock: The bit clock (or serial clock) pin
//|         :param ~microcontroller.Pin word_select: The word select (or left/right clock) pin
//|         :param ~microcontroller.Pin data: The data input pin
//|         :param ~microcontroller.Pin main_clock: The main clock pin. Not all ports support this.
//|         :param int sample_rate: Target sample rate of the resulting samples. Check `sample_rate` for actual value.
//|         :param int bit_depth: Number of bits per sample on the I2S bus. Must be 8, 16, 24, or
//|           32. 8-bit only supported on espressif. The destination buffer typecode is determined
//|           by ``output_bit_depth`` (or ``bit_depth`` when ``output_bit_depth`` is ``None``):
//|
//|           +----------------+------------------+----------------------+
//|           | samples_signed | output_bit_depth | Required typecode(s) |
//|           +================+==================+======================+
//|           | True           | 24 or 32         | ``'i'``              |
//|           +----------------+------------------+----------------------+
//|           | True           | 16               | ``'h'``              |
//|           +----------------+------------------+----------------------+
//|           | True           | 8                | ``'b'`` or BYTEARRAY |
//|           +----------------+------------------+----------------------+
//|           | False          | 24 or 32         | ``'I'``              |
//|           +----------------+------------------+----------------------+
//|           | False          | 16               | ``'H'``              |
//|           +----------------+------------------+----------------------+
//|           | False          | 8                | ``'B'`` or BYTEARRAY |
//|           +----------------+------------------+----------------------+
//|
//|           Note that 24-bit samples from mics like the SPH0645LM4H / INMP441 are
//|           transported in 32-bit slots, so use ``bit_depth=32`` and an ``'I'`` buffer.
//|         :param int output_bit_depth: If set, recorded samples are bit-shifted from
//|           ``bit_depth`` to this width before being written to the destination buffer
//|           (8, 16, 24, or 32). Widening pads the new LSBs with zero; narrowing arithmetic-
//|           shifts the value right (sign-preserving when ``samples_signed`` is True). When
//|           ``None`` (the default) the destination buffer holds samples at ``bit_depth``.
//|         :param bool mono: True when capturing a single channel of audio, captures two channels otherwise.
//|         :param bool left_justified: True when data bits are aligned with the word select clock. False
//|           when they are shifted by one to match classic I2S protocol. Set True for mics like the SPH0645LM4H.
//|         :param bool samples_signed: Samples are signed (True) or unsigned (False). I2S mics deliver signed
//|           two's-complement PCM natively; set False to have the recorded samples converted to unsigned PCM
//|           (the top/sign bit is flipped, matching the WAV convention for unsigned samples).
//|
//|         Example, recording 16-bit mono samples from an INMP441::
//|
//|           import array
//|           import audioi2sin
//|           import board
//|
//|           buf = array.array("h", [0] * 16000)
//|           with audioi2sin.I2SIn(board.D9, board.D10, board.D11,
//|                                  sample_rate=16000, bit_depth=16) as mic:
//|               mic.record(buf, len(buf))
//|
//|         """
//|         ...
//|
static mp_obj_t audioi2sin_i2sin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    #if !CIRCUITPY_AUDIOI2SIN
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_I2SIn);
    return NULL; // Not reachable.
    #else
    enum { ARG_bit_clock, ARG_word_select, ARG_data, ARG_main_clock,
           ARG_sample_rate, ARG_bit_depth, ARG_output_bit_depth,
           ARG_mono, ARG_left_justified, ARG_samples_signed };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bit_clock,        MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_word_select,      MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_data,             MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_main_clock,       MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_sample_rate,      MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 16000} },
        { MP_QSTR_bit_depth,        MP_ARG_KW_ONLY | MP_ARG_INT,  {.u_int = 16} },
        { MP_QSTR_output_bit_depth, MP_ARG_KW_ONLY | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_mono,             MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_left_justified,   MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_samples_signed,   MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *bit_clock = validate_obj_is_free_pin(args[ARG_bit_clock].u_obj, MP_QSTR_bit_clock);
    const mcu_pin_obj_t *word_select = validate_obj_is_free_pin(args[ARG_word_select].u_obj, MP_QSTR_word_select);
    const mcu_pin_obj_t *data = validate_obj_is_free_pin(args[ARG_data].u_obj, MP_QSTR_data);
    const mcu_pin_obj_t *main_clock = validate_obj_is_free_pin_or_none(args[ARG_main_clock].u_obj, MP_QSTR_main_clock);

    uint32_t sample_rate = args[ARG_sample_rate].u_int;
    uint8_t bit_depth = args[ARG_bit_depth].u_int;
    if (bit_depth != 8 && bit_depth != 16 && bit_depth != 24 && bit_depth != 32) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be 8, 16, 24, or 32"), MP_QSTR_bit_depth);
    }
    uint8_t output_bit_depth;
    mp_obj_t output_bit_depth_obj = args[ARG_output_bit_depth].u_obj;
    if (output_bit_depth_obj == mp_const_none) {
        output_bit_depth = bit_depth;
    } else {
        mp_int_t v = mp_obj_get_int(output_bit_depth_obj);
        if (v != 8 && v != 16 && v != 24 && v != 32) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be 8, 16, 24, or 32"), MP_QSTR_output_bit_depth);
        }
        output_bit_depth = (uint8_t)v;
    }
    bool mono = args[ARG_mono].u_bool;
    bool left_justified = args[ARG_left_justified].u_bool;
    bool samples_signed = args[ARG_samples_signed].u_bool;

    audioi2sin_i2sin_obj_t *self = mp_obj_malloc_with_finaliser(audioi2sin_i2sin_obj_t, &audioi2sin_i2sin_type);
    common_hal_audioi2sin_i2sin_construct(self, bit_clock, word_select, data, main_clock,
        sample_rate, bit_depth, output_bit_depth, mono, left_justified, samples_signed);

    return MP_OBJ_FROM_PTR(self);
    #endif
}

#if CIRCUITPY_AUDIOI2SIN
//|     def deinit(self) -> None:
//|         """Deinitialises the I2SIn and releases any hardware resources for reuse."""
//|         ...
//|
static mp_obj_t audioi2sin_i2sin_deinit(mp_obj_t self_in) {
    audioi2sin_i2sin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audioi2sin_i2sin_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audioi2sin_i2sin_deinit_obj, audioi2sin_i2sin_deinit);

static void check_for_deinit(audioi2sin_i2sin_obj_t *self) {
    if (common_hal_audioi2sin_i2sin_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> I2SIn:
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

//|     def record(self, destination: WriteableBuffer, destination_length: int) -> int:
//|         """Records destination_length samples to destination. This is blocking.
//|
//|         :return: The number of samples recorded. If this is less than ``destination_length``,
//|           some samples were missed due to processing time."""
//|         ...
//|
static mp_obj_t audioi2sin_i2sin_obj_record(mp_obj_t self_obj, mp_obj_t destination, mp_obj_t destination_length) {
    audioi2sin_i2sin_obj_t *self = MP_OBJ_TO_PTR(self_obj);
    check_for_deinit(self);
    uint32_t length = mp_arg_validate_type_int(destination_length, MP_QSTR_length);
    mp_arg_validate_length_min(length, 0, MP_QSTR_length);

    mp_buffer_info_t bufinfo;
    if (mp_obj_is_type(destination, &mp_type_fileio)) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Cannot record to a file"));
    }
    mp_get_buffer_raise(destination, &bufinfo, MP_BUFFER_WRITE);
    if (bufinfo.len / mp_binary_get_size('@', bufinfo.typecode, NULL) < length) {
        mp_raise_ValueError(MP_ERROR_TEXT("Destination capacity is smaller than destination_length."));
    }
    uint8_t output_bit_depth = common_hal_audioi2sin_i2sin_get_output_bit_depth(self);
    char error_type = ' ';
    bool samples_signed = common_hal_audioi2sin_i2sin_get_samples_signed(self);
    if (samples_signed) {
        if ((output_bit_depth == 24 || output_bit_depth == 32) && bufinfo.typecode != 'i') {
            error_type = 'i';
        } else if (output_bit_depth == 16 && bufinfo.typecode != 'h') {
            error_type = 'h';
        } else if (output_bit_depth == 8 && bufinfo.typecode != 'b' && bufinfo.typecode != BYTEARRAY_TYPECODE) {
            error_type = 'b'; // NOTE: Not identifying as bytearray
        }
    } else {
        if ((output_bit_depth == 24 || output_bit_depth == 32) && bufinfo.typecode != 'I') {
            error_type = 'I';
        } else if (output_bit_depth == 16 && bufinfo.typecode != 'H') {
            error_type = 'H';
        } else if (output_bit_depth == 8 && bufinfo.typecode != 'B' && bufinfo.typecode != BYTEARRAY_TYPECODE) {
            error_type = 'B';
        }
    }
    if (error_type != ' ') {
        mp_raise_TypeError_varg(
            MP_ERROR_TEXT("invalid destination buffer, must be an array of type: %c"),
            error_type
            );
    }
    uint32_t length_written =
        common_hal_audioi2sin_i2sin_record_to_buffer(self, bufinfo.buf, length);
    return MP_OBJ_NEW_SMALL_INT(length_written);
}
MP_DEFINE_CONST_FUN_OBJ_3(audioi2sin_i2sin_record_obj, audioi2sin_i2sin_obj_record);

//|     sample_rate: int
//|     """The actual sample rate of the recording. This may not match the constructed
//|     sample rate due to internal clock limitations."""
//|
static mp_obj_t audioi2sin_i2sin_obj_get_sample_rate(mp_obj_t self_in) {
    audioi2sin_i2sin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audioi2sin_i2sin_get_sample_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioi2sin_i2sin_get_sample_rate_obj, audioi2sin_i2sin_obj_get_sample_rate);

MP_PROPERTY_GETTER(audioi2sin_i2sin_sample_rate_obj,
    (mp_obj_t)&audioi2sin_i2sin_get_sample_rate_obj);

//|     bit_depth: int
//|     """The actual bit depth of the recording. (read-only)"""
//|
//|
static mp_obj_t audioi2sin_i2sin_obj_get_bit_depth(mp_obj_t self_in) {
    audioi2sin_i2sin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audioi2sin_i2sin_get_bit_depth(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioi2sin_i2sin_get_bit_depth_obj, audioi2sin_i2sin_obj_get_bit_depth);

MP_PROPERTY_GETTER(audioi2sin_i2sin_bit_depth_obj,
    (mp_obj_t)&audioi2sin_i2sin_get_bit_depth_obj);

//|     output_bit_depth: int
//|     """The bit depth of samples written to the destination buffer. Equals ``bit_depth`` when
//|     ``output_bit_depth`` was not supplied (or was ``None``) at construction time. (read-only)"""
//|
//|
static mp_obj_t audioi2sin_i2sin_obj_get_output_bit_depth(mp_obj_t self_in) {
    audioi2sin_i2sin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_audioi2sin_i2sin_get_output_bit_depth(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioi2sin_i2sin_get_output_bit_depth_obj, audioi2sin_i2sin_obj_get_output_bit_depth);

MP_PROPERTY_GETTER(audioi2sin_i2sin_output_bit_depth_obj,
    (mp_obj_t)&audioi2sin_i2sin_get_output_bit_depth_obj);

//|     samples_signed: bool
//|     """True if recorded samples are signed PCM, False for unsigned. (read-only)"""
//|
//|
static mp_obj_t audioi2sin_i2sin_obj_get_samples_signed(mp_obj_t self_in) {
    audioi2sin_i2sin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audioi2sin_i2sin_get_samples_signed(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audioi2sin_i2sin_get_samples_signed_obj, audioi2sin_i2sin_obj_get_samples_signed);

MP_PROPERTY_GETTER(audioi2sin_i2sin_samples_signed_obj,
    (mp_obj_t)&audioi2sin_i2sin_get_samples_signed_obj);

static const mp_rom_map_elem_t audioi2sin_i2sin_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&audioi2sin_i2sin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audioi2sin_i2sin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_record), MP_ROM_PTR(&audioi2sin_i2sin_record_obj) },
    { MP_ROM_QSTR(MP_QSTR_sample_rate), MP_ROM_PTR(&audioi2sin_i2sin_sample_rate_obj) },
    { MP_ROM_QSTR(MP_QSTR_bit_depth), MP_ROM_PTR(&audioi2sin_i2sin_bit_depth_obj) },
    { MP_ROM_QSTR(MP_QSTR_output_bit_depth), MP_ROM_PTR(&audioi2sin_i2sin_output_bit_depth_obj) },
    { MP_ROM_QSTR(MP_QSTR_samples_signed), MP_ROM_PTR(&audioi2sin_i2sin_samples_signed_obj) },
};
static MP_DEFINE_CONST_DICT(audioi2sin_i2sin_locals_dict, audioi2sin_i2sin_locals_dict_table);
#endif // CIRCUITPY_AUDIOI2SIN

MP_DEFINE_CONST_OBJ_TYPE(
    audioi2sin_i2sin_type,
    MP_QSTR_I2SIn,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audioi2sin_i2sin_make_new
    #if CIRCUITPY_AUDIOI2SIN
    , locals_dict, &audioi2sin_i2sin_locals_dict
    #endif
    );
