// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tod Kurt
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/audiospeed/SpeedChanger.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-bindings/util.h"
#include "shared-module/audiospeed/SpeedChanger.h"

// Convert a Python float to 16.16 fixed-point rate
static uint32_t rate_to_fp(mp_obj_t rate_obj) {
    mp_float_t rate = mp_arg_validate_obj_float_range(rate_obj, 0.001, 1000.0, MP_QSTR_rate);
    return (uint32_t)(rate * (1 << 16));
}

// Convert 16.16 fixed-point rate to Python float
static mp_obj_t fp_to_rate(uint32_t rate_fp) {
    return mp_obj_new_float((mp_float_t)rate_fp / (1 << 16));
}

//| class SpeedChanger:
//|     """Wraps an audio sample to play it back at a different speed.
//|
//|     Uses nearest-neighbor resampling with a fixed-point phase accumulator
//|     for CPU-efficient variable-speed playback."""
//|
//|     def __init__(self, source: circuitpython_typing.AudioSample, rate: float = 1.0) -> None:
//|         """Create a SpeedChanger that wraps ``source``.
//|
//|         :param audiosample source: The audio source to resample.
//|         :param float rate: Playback speed multiplier. 1.0 = normal, 2.0 = double speed,
//|           0.5 = half speed. Must be positive.
//|
//|         Playing a wave file at 1.5x speed::
//|
//|           import board
//|           import audiocore
//|           import audiospeed
//|           import audioio
//|
//|           wav = audiocore.WaveFile("drum.wav")
//|           fast = audiospeed.SpeedChanger(wav, rate=1.5)
//|           audio = audioio.AudioOut(board.A0)
//|           audio.play(fast)
//|
//|           # Change speed during playback:
//|           fast.rate = 2.0   # double speed
//|           fast.rate = 0.5   # half speed
//|         """
//|         ...
//|
static mp_obj_t audiospeed_speedchanger_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_source, ARG_rate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_source, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_rate, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Validate source implements audiosample protocol
    mp_obj_t source = args[ARG_source].u_obj;
    audiosample_check(source);

    uint32_t rate_fp = 1 << 16; // default 1.0
    if (args[ARG_rate].u_obj != mp_const_none) {
        rate_fp = rate_to_fp(args[ARG_rate].u_obj);
    }

    audiospeed_speedchanger_obj_t *self = mp_obj_malloc(audiospeed_speedchanger_obj_t, &audiospeed_speedchanger_type);
    common_hal_audiospeed_speedchanger_construct(self, source, rate_fp);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the SpeedChanger and releases all memory resources for reuse."""
//|         ...
//|
static mp_obj_t audiospeed_speedchanger_deinit(mp_obj_t self_in) {
    audiospeed_speedchanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiospeed_speedchanger_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiospeed_speedchanger_deinit_obj, audiospeed_speedchanger_deinit);

//|     rate: float
//|     """Playback speed multiplier. Can be changed during playback."""
//|
static mp_obj_t audiospeed_speedchanger_obj_get_rate(mp_obj_t self_in) {
    audiospeed_speedchanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    audiosample_check_for_deinit(&self->base);
    return fp_to_rate(common_hal_audiospeed_speedchanger_get_rate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiospeed_speedchanger_get_rate_obj, audiospeed_speedchanger_obj_get_rate);

static mp_obj_t audiospeed_speedchanger_obj_set_rate(mp_obj_t self_in, mp_obj_t rate_obj) {
    audiospeed_speedchanger_obj_t *self = MP_OBJ_TO_PTR(self_in);
    audiosample_check_for_deinit(&self->base);
    common_hal_audiospeed_speedchanger_set_rate(self, rate_to_fp(rate_obj));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(audiospeed_speedchanger_set_rate_obj, audiospeed_speedchanger_obj_set_rate);

MP_PROPERTY_GETSET(audiospeed_speedchanger_rate_obj,
    (mp_obj_t)&audiospeed_speedchanger_get_rate_obj,
    (mp_obj_t)&audiospeed_speedchanger_set_rate_obj);

static const mp_rom_map_elem_t audiospeed_speedchanger_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiospeed_speedchanger_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_rate), MP_ROM_PTR(&audiospeed_speedchanger_rate_obj) },
    AUDIOSAMPLE_FIELDS,
};
static MP_DEFINE_CONST_DICT(audiospeed_speedchanger_locals_dict, audiospeed_speedchanger_locals_dict_table);

static const audiosample_p_t audiospeed_speedchanger_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .reset_buffer = (audiosample_reset_buffer_fun)audiospeed_speedchanger_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiospeed_speedchanger_get_buffer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    audiospeed_speedchanger_type,
    MP_QSTR_SpeedChanger,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiospeed_speedchanger_make_new,
    locals_dict, &audiospeed_speedchanger_locals_dict,
    protocol, &audiospeed_speedchanger_proto
    );
