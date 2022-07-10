/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audioio/AudioIn.h"
#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate/translate.h"

//| class AudioIn:
//|     """Output an analog audio signal"""
//|
//|     def __init__(self, left_channel: microcontroller.Pin, *, right_channel: Optional[microcontroller.Pin] = None, quiescent_value: int = 0x8000) -> None:
//|         """Create a AudioIn object associated with the given pin(s). This allows you to
//|         play audio signals out on the given pin(s).
//|
//|         :param ~microcontroller.Pin left_channel: The pin to output the left channel to
//|         :param ~microcontroller.Pin right_channel: The pin to output the right channel to
//|         :param int quiescent_value: The output value when no signal is present. Samples should start
//|             and end with this value to prevent audible popping.
//|
//|         Simple 8ksps 440 Hz sin wave::
//|
//|           import audiocore
//|           import audioio
//|           import board
//|           import array
//|           import time
//|           import math
//|
//|           # Generate one period of sine wav.
//|           length = 8000 // 440
//|           sine_wave = array.array("H", [0] * length)
//|           for i in range(length):
//|               sine_wave[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15) + 2 ** 15)
//|
//|           dac = audioio.AudioIn(board.SPEAKER)
//|           sine_wave = audiocore.RawSample(sine_wave, sample_rate=8000)
//|           dac.play(sine_wave, loop=True)
//|           time.sleep(1)
//|           dac.stop()
//|
//|         Playing a wave file from flash::
//|
//|           import board
//|           import audioio
//|           import digitalio
//|
//|           # Required for CircuitPlayground Express
//|           speaker_enable = digitalio.DigitalInOut(board.SPEAKER_ENABLE)
//|           speaker_enable.switch_to_output(value=True)
//|
//|           data = open("cplay-5.1-16bit-16khz.wav", "rb")
//|           wav = audiocore.WaveFile(data)
//|           a = audioio.AudioIn(board.A0)
//|
//|           print("playing")
//|           a.play(wav)
//|           while a.playing:
//|             pass
//|           print("stopped")"""
//|         ...
//|
STATIC mp_obj_t audioio_audioin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_left_channel, ARG_right_channel, ARG_quiescent_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_left_channel, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_right_channel, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = mp_const_none} },
        { MP_QSTR_quiescent_value, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0x8000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *left_channel_pin = validate_obj_is_free_pin(args[ARG_left_channel].u_obj);
    const mcu_pin_obj_t *right_channel_pin = validate_obj_is_free_pin_or_none(args[ARG_right_channel].u_obj);

    // create AudioIn object from the given pin
    audioio_audioin_obj_t *self = m_new_obj(audioio_audioin_obj_t);
    self->base.type = &audioio_audioin_type;
    common_hal_audioio_audioin_construct(self, left_channel_pin, right_channel_pin, args[ARG_quiescent_value].u_int);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the AudioIn and releases any hardware resources for reuse."""
//|         ...
//|
STATIC mp_obj_t audioio_audioin_deinit(mp_obj_t self_in) {
    audioio_audioin_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audioio_audioin_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(audioio_audioin_deinit_obj, audioio_audioin_deinit);

//|     def __enter__(self) -> AudioIn:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t audioio_audioin_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_audioio_audioin_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(audioio_audioin___exit___obj, 4, 4, audioio_audioin_obj___exit__);


STATIC const mp_rom_map_elem_t audioio_audioin_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audioio_audioin_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&audioio_audioin___exit___obj) },

    // Properties
};
STATIC MP_DEFINE_CONST_DICT(audioio_audioin_locals_dict, audioio_audioin_locals_dict_table);

const mp_obj_type_t audioio_audioin_type = {
    { &mp_type_type },
    .name = MP_QSTR_AudioIn,
    .make_new = audioio_audioin_make_new,
    .locals_dict = (mp_obj_dict_t *)&audioio_audioin_locals_dict,
};
