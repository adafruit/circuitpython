// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tod Kurt
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/mcp4822/MCP4822.h"
#include "shared-bindings/util.h"

//| class MCP4822:
//|     """Output audio to an MCP4822 dual-channel 12-bit SPI DAC."""
//|
//|     def __init__(
//|         self,
//|         clock: microcontroller.Pin,
//|         mosi: microcontroller.Pin,
//|         cs: microcontroller.Pin,
//|         *,
//|         gain: int = 1,
//|     ) -> None:
//|         """Create an MCP4822 object associated with the given SPI pins.
//|
//|         :param ~microcontroller.Pin clock: The SPI clock (SCK) pin
//|         :param ~microcontroller.Pin mosi: The SPI data (SDI/MOSI) pin
//|         :param ~microcontroller.Pin cs: The chip select (CS) pin
//|         :param int gain: DAC output gain, 1 for 1x (0-2.048V) or 2 for 2x (0-4.096V). Default 1.
//|
//|         Simple 8ksps 440 Hz sine wave::
//|
//|           import mcp4822
//|           import audiocore
//|           import board
//|           import array
//|           import time
//|           import math
//|
//|           length = 8000 // 440
//|           sine_wave = array.array("H", [0] * length)
//|           for i in range(length):
//|               sine_wave[i] = int(math.sin(math.pi * 2 * i / length) * (2 ** 15) + 2 ** 15)
//|
//|           sine_wave = audiocore.RawSample(sine_wave, sample_rate=8000)
//|           dac = mcp4822.MCP4822(clock=board.GP18, mosi=board.GP19, cs=board.GP21)
//|           dac.play(sine_wave, loop=True)
//|           time.sleep(1)
//|           dac.stop()
//|
//|         Playing a wave file from flash::
//|
//|           import board
//|           import audiocore
//|           import mcp4822
//|
//|           f = open("sound.wav", "rb")
//|           wav = audiocore.WaveFile(f)
//|
//|           dac = mcp4822.MCP4822(clock=board.GP18, mosi=board.GP19, cs=board.GP21)
//|           dac.play(wav)
//|           while dac.playing:
//|               pass"""
//|         ...
//|
static mp_obj_t mcp4822_mcp4822_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_clock, ARG_mosi, ARG_cs, ARG_gain };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_clock, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_mosi,  MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_cs,    MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_gain,  MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj, MP_QSTR_clock);
    const mcu_pin_obj_t *mosi = validate_obj_is_free_pin(args[ARG_mosi].u_obj, MP_QSTR_mosi);
    const mcu_pin_obj_t *cs = validate_obj_is_free_pin(args[ARG_cs].u_obj, MP_QSTR_cs);
    const mp_int_t gain = mp_arg_validate_int_range(args[ARG_gain].u_int, 1, 2, MP_QSTR_gain);

    mcp4822_mcp4822_obj_t *self = mp_obj_malloc_with_finaliser(mcp4822_mcp4822_obj_t, &mcp4822_mcp4822_type);
    common_hal_mcp4822_mcp4822_construct(self, clock, mosi, cs, (uint8_t)gain);

    return MP_OBJ_FROM_PTR(self);
}

static void check_for_deinit(mcp4822_mcp4822_obj_t *self) {
    if (common_hal_mcp4822_mcp4822_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def deinit(self) -> None:
//|         """Deinitialises the MCP4822 and releases any hardware resources for reuse."""
//|         ...
//|
static mp_obj_t mcp4822_mcp4822_deinit(mp_obj_t self_in) {
    mcp4822_mcp4822_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_mcp4822_mcp4822_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mcp4822_mcp4822_deinit_obj, mcp4822_mcp4822_deinit);

//|     def __enter__(self) -> MCP4822:
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

//|     def play(self, sample: circuitpython_typing.AudioSample, *, loop: bool = False) -> None:
//|         """Plays the sample once when loop=False and continuously when loop=True.
//|         Does not block. Use `playing` to block.
//|
//|         Sample must be an `audiocore.WaveFile`, `audiocore.RawSample`, `audiomixer.Mixer` or `audiomp3.MP3Decoder`.
//|
//|         The sample itself should consist of 8 bit or 16 bit samples."""
//|         ...
//|
static mp_obj_t mcp4822_mcp4822_obj_play(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample, ARG_loop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_loop,   MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    mcp4822_mcp4822_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sample = args[ARG_sample].u_obj;
    common_hal_mcp4822_mcp4822_play(self, sample, args[ARG_loop].u_bool);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mcp4822_mcp4822_play_obj, 1, mcp4822_mcp4822_obj_play);

//|     def stop(self) -> None:
//|         """Stops playback."""
//|         ...
//|
static mp_obj_t mcp4822_mcp4822_obj_stop(mp_obj_t self_in) {
    mcp4822_mcp4822_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_mcp4822_mcp4822_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mcp4822_mcp4822_stop_obj, mcp4822_mcp4822_obj_stop);

//|     playing: bool
//|     """True when the audio sample is being output. (read-only)"""
//|
static mp_obj_t mcp4822_mcp4822_obj_get_playing(mp_obj_t self_in) {
    mcp4822_mcp4822_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_mcp4822_mcp4822_get_playing(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(mcp4822_mcp4822_get_playing_obj, mcp4822_mcp4822_obj_get_playing);

MP_PROPERTY_GETTER(mcp4822_mcp4822_playing_obj,
    (mp_obj_t)&mcp4822_mcp4822_get_playing_obj);

//|     def pause(self) -> None:
//|         """Stops playback temporarily while remembering the position. Use `resume` to resume playback."""
//|         ...
//|
static mp_obj_t mcp4822_mcp4822_obj_pause(mp_obj_t self_in) {
    mcp4822_mcp4822_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    if (!common_hal_mcp4822_mcp4822_get_playing(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Not playing"));
    }
    common_hal_mcp4822_mcp4822_pause(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mcp4822_mcp4822_pause_obj, mcp4822_mcp4822_obj_pause);

//|     def resume(self) -> None:
//|         """Resumes sample playback after :py:func:`pause`."""
//|         ...
//|
static mp_obj_t mcp4822_mcp4822_obj_resume(mp_obj_t self_in) {
    mcp4822_mcp4822_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    if (common_hal_mcp4822_mcp4822_get_paused(self)) {
        common_hal_mcp4822_mcp4822_resume(self);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mcp4822_mcp4822_resume_obj, mcp4822_mcp4822_obj_resume);

//|     paused: bool
//|     """True when playback is paused. (read-only)"""
//|
//|
static mp_obj_t mcp4822_mcp4822_obj_get_paused(mp_obj_t self_in) {
    mcp4822_mcp4822_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_mcp4822_mcp4822_get_paused(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(mcp4822_mcp4822_get_paused_obj, mcp4822_mcp4822_obj_get_paused);

MP_PROPERTY_GETTER(mcp4822_mcp4822_paused_obj,
    (mp_obj_t)&mcp4822_mcp4822_get_paused_obj);

static const mp_rom_map_elem_t mcp4822_mcp4822_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR___del__),   MP_ROM_PTR(&mcp4822_mcp4822_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),    MP_ROM_PTR(&mcp4822_mcp4822_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),  MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play),      MP_ROM_PTR(&mcp4822_mcp4822_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop),      MP_ROM_PTR(&mcp4822_mcp4822_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_pause),     MP_ROM_PTR(&mcp4822_mcp4822_pause_obj) },
    { MP_ROM_QSTR(MP_QSTR_resume),    MP_ROM_PTR(&mcp4822_mcp4822_resume_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing),   MP_ROM_PTR(&mcp4822_mcp4822_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_paused),    MP_ROM_PTR(&mcp4822_mcp4822_paused_obj) },
};
static MP_DEFINE_CONST_DICT(mcp4822_mcp4822_locals_dict, mcp4822_mcp4822_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mcp4822_mcp4822_type,
    MP_QSTR_MCP4822,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, mcp4822_mcp4822_make_new,
    locals_dict, &mcp4822_mcp4822_locals_dict
    );
