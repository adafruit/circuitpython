// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "bindings/esphmac/__init__.h"
#include "bindings/esphmac/HMACKey.h"

//| """Application-triggered HMAC-SHA256 using the on-chip HMAC peripheral
//| against a pre-burned, read-protected eFuse key block.
//|
//| This module is compute-only by design: it has no API to read a key's raw
//| bytes, and no API to burn or write eFuse keys -- use ``espefuse.py`` at
//| manufacturing time for that. Once a key block's ``RD_DIS`` eFuse bit is
//| set, the eFuse controller enforces the block-out in hardware; there is no
//| way to read the raw key back through this module, or any other,
//| regardless of API design.
//| """
//|

static const mp_rom_map_elem_t esphmac_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_esphmac) },
    { MP_ROM_QSTR(MP_QSTR_HMACKey), MP_ROM_PTR(&esphmac_hmackey_type) },
};
static MP_DEFINE_CONST_DICT(esphmac_module_globals, esphmac_module_globals_table);

const mp_obj_module_t esphmac_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&esphmac_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_esphmac, esphmac_module);
