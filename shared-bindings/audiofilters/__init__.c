// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/audiofilters/__init__.h"
#include "shared-bindings/audiofilters/Distortion.h"
#include "shared-bindings/audiofilters/Filter.h"
#include "shared-bindings/audiofilters/Phaser.h"

//| """Support for audio filter effects
//|
//| The `audiofilters` module contains classes to provide access to audio filter effects.
//|
//| """

static const mp_rom_map_elem_t audiofilters_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audiofilters) },
    { MP_ROM_QSTR(MP_QSTR_Filter), MP_ROM_PTR(&audiofilters_filter_type) },
    { MP_ROM_QSTR(MP_QSTR_Distortion), MP_ROM_PTR(&audiofilters_distortion_type) },
    { MP_ROM_QSTR(MP_QSTR_Phaser), MP_ROM_PTR(&audiofilters_phaser_type) },

    // Enum-like Classes.
    { MP_ROM_QSTR(MP_QSTR_DistortionMode), MP_ROM_PTR(&audiofilters_distortion_mode_type) },
};

static MP_DEFINE_CONST_DICT(audiofilters_module_globals, audiofilters_module_globals_table);

const mp_obj_module_t audiofilters_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audiofilters_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audiofilters, audiofilters_module);
