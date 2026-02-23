// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/mipidsi/Bus.h"
#include "shared-bindings/mipidsi/Display.h"

//| """Low-level routines for interacting with MIPI DSI"""

static const mp_rom_map_elem_t mipidsi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mipidsi) },
    { MP_ROM_QSTR(MP_QSTR_Bus), MP_ROM_PTR(&mipidsi_bus_type) },
    { MP_ROM_QSTR(MP_QSTR_Display), MP_ROM_PTR(&mipidsi_display_type) },
};

static MP_DEFINE_CONST_DICT(mipidsi_module_globals, mipidsi_module_globals_table);

const mp_obj_module_t mipidsi_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mipidsi_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_mipidsi, mipidsi_module);
