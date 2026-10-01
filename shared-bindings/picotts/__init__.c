// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "shared-bindings/picotts/Engine.h"

//| """SVOX Pico text to speech
//|
//| The `picotts` module renders speech with the SVOX Pico engine (Apache-2.0) and its en-US
//| voice, both built into the firmware. Use it through the ``adafruit_svox_pico`` library,
//| which plays the audio through `audiomixer`."""
//|

static const mp_rom_map_elem_t picotts_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picotts) },
    { MP_ROM_QSTR(MP_QSTR_Engine), MP_ROM_PTR(&picotts_engine_type) },
};

static MP_DEFINE_CONST_DICT(picotts_module_globals, picotts_module_globals_table);

const mp_obj_module_t picotts_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&picotts_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_picotts, picotts_module);
