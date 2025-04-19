// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Mark Komus
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/audiofreeverb/__init__.h"
#include "shared-bindings/audiofreeverb/Freeverb.h"


//| """Support for audio freeverb effect
//|
//| The `audiofreeverb` module contains classes to provide access to audio freeverb effects.
//|
//| """

static const mp_rom_map_elem_t audiofreeverb_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audiofreeverb) },
    { MP_ROM_QSTR(MP_QSTR_Freeverb), MP_ROM_PTR(&audiofreeverb_freeverb_type) },
};

static MP_DEFINE_CONST_DICT(audiofreeverb_module_globals, audiofreeverb_module_globals_table);

const mp_obj_module_t audiofreeverb_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audiofreeverb_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audiofreeverb, audiofreeverb_module);
