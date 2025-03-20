// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Tim Cocks
//
// SPDX-License-Identifier: MIT
#include "py/obj.h"
#include "shared-bindings/tilepalettemapper/TilePaletteMapper.h"


static const mp_rom_map_elem_t tilepalettemapper_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_tilepalettemapper) },
    { MP_ROM_QSTR(MP_QSTR_TilePaletteMapper), MP_ROM_PTR(&tilepalettemapper_tilepalettemapper_type) },
};
static MP_DEFINE_CONST_DICT(tilepalettemapper_module_globals, tilepalettemapper_module_globals_table);

const mp_obj_module_t tilepalettemapper_module = {
    .base = {&mp_type_module },
    .globals = (mp_obj_dict_t *)&tilepalettemapper_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_tilepalettemapper, tilepalettemapper_module);
