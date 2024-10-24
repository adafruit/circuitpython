// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Mark Komus
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/audiodelays/__init__.h"
#include "shared-bindings/audiodelays/Echo.h"

//| """Support for audio delay effects
//|
//| The `audiodelays` module contains classes to provide access to audio delay effects.
//|
//| """

static const mp_rom_map_elem_t audiodelays_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audiodelays) },
    { MP_ROM_QSTR(MP_QSTR_Echo), MP_ROM_PTR(&audiodelays_echo_type) },
};

static MP_DEFINE_CONST_DICT(audiodelays_module_globals, audiodelays_module_globals_table);

const mp_obj_module_t audiodelays_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audiodelays_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audiodelays, audiodelays_module);
