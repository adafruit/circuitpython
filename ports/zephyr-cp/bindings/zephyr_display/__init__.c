// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "bindings/zephyr_display/Display.h"

//| """Display access for Zephyr boards
//|
//| This module provides `Display`, a `displayio` display class for the device selected by
//| the ``zephyr,display`` devicetree chosen property.
//|
//| .. note:: This module is Zephyr-specific. On other ports the equivalent class is one of
//|     the `displayio` display classes, such as `busdisplay.BusDisplay`.
//| """
//
// The current implementation composites with LVGL, which draws from its own fixed-size heap
// (CONFIG_LV_Z_MEM_POOL_SIZE) instead of the Python heap and drives only one display. Both are
// implementation details rather than part of this class's contract, so they aren't documented
// above, but they explain why complex scenes can fail inside LVGL and why only one `Display`
// can exist.

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
