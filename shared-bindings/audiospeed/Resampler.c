// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/audiospeed/Resampler.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-bindings/util.h"
#include "shared-module/audiospeed/Resampler.h"

//| class Resampler:
//|     """Wraps an audio sample to match it to the destination sample rate."""
//|
//|     def __init__(self) -> None:
//|         """Create a Resampler that wraps ``source``.
//|
//|         :param audiosample source: The audio source to resample.
//|
//|         Playing a wave file through a mixer with half the sample rate::
//|
//|           import board
//|           import audiocore
//|           import audiomixer
//|           import audiospeed
//|           import audioio
//|
//|           wav = audiocore.WaveFile("sample.wav")
//|           resampler = audiospeed.Resampler(wav)
//|           mixer = audiomixer.Mixer(
//|               channel_count=wav.channel_count,
//|               bits_per_sample=wav.bits_per_sample,
//|               sample_rate=wav.sample_rate // 2,
//|           )
//|           audio = audioio.AudioOut(board.A0)
//|           audio.play(mixer)
//|           mixer.play(resampler)
//|         """
//|         ...
//|
static mp_obj_t audiospeed_resampler_make_new(const mp_obj_type_t *type,
    size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_source };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_source, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Validate source implements audiosample protocol
    mp_obj_t source = args[ARG_source].u_obj;
    audiosample_check(source);

    audiospeed_resampler_obj_t *self = mp_obj_malloc_with_finaliser(audiospeed_resampler_obj_t, &audiospeed_resampler_type);
    common_hal_audiospeed_resampler_construct(self, source);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the Resampler and releases all memory resources for reuse."""
//|         ...
//|
static mp_obj_t audiospeed_resampler_deinit(mp_obj_t self_in) {
    audiospeed_resampler_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiospeed_resampler_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiospeed_resampler_deinit_obj, audiospeed_resampler_deinit);

//|     rate: float
//|     """Playback speed multiplier."""
//|
static mp_obj_t audiospeed_resampler_obj_get_rate(mp_obj_t self_in) {
    audiospeed_resampler_obj_t *self = MP_OBJ_TO_PTR(self_in);
    audiosample_check_for_deinit(&self->base.base);
    return common_hal_audiospeed_resampler_get_rate(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(audiospeed_resampler_get_rate_obj, audiospeed_resampler_obj_get_rate);

MP_PROPERTY_GETTER(audiospeed_resampler_rate_obj,
    (mp_obj_t)&audiospeed_resampler_get_rate_obj);

static const mp_rom_map_elem_t audiospeed_resampler_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiospeed_resampler_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&audiospeed_resampler_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_rate), MP_ROM_PTR(&audiospeed_resampler_rate_obj) },
    AUDIOSAMPLE_FIELDS,
};
static MP_DEFINE_CONST_DICT(audiospeed_resampler_locals_dict, audiospeed_resampler_locals_dict_table);

static const audiosample_p_t audiospeed_resampler_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_audiosample)
    .reset_buffer = (audiosample_reset_buffer_fun)audiospeed_resampler_reset_buffer,
    .get_buffer = (audiosample_get_buffer_fun)audiospeed_resampler_get_buffer,
};

MP_DEFINE_CONST_OBJ_TYPE(
    audiospeed_resampler_type,
    MP_QSTR_Resampler,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiospeed_resampler_make_new,
    locals_dict, &audiospeed_resampler_locals_dict,
    protocol, &audiospeed_resampler_proto
    );
