// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "bindings/zephyr_display/Display.h"

static const mp_rom_map_elem_t zephyr_display_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_zephyr_display) },
    { MP_ROM_QSTR(MP_QSTR_Display), MP_ROM_PTR(&zephyr_display_display_type) },
};

static MP_DEFINE_CONST_DICT(zephyr_display_module_globals, zephyr_display_module_globals_table);

const mp_obj_module_t zephyr_display_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&zephyr_display_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_zephyr_display, zephyr_display_module);
