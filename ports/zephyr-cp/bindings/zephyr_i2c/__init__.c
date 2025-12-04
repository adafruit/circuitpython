// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"
#include "bindings/zephyr_i2c/I2C.h"

//| """Zephyr I2C driver for fixed I2C busses.
//|
//| This module provides access to I2C busses using Zephyr device labels."""

static const mp_rom_map_elem_t zephyr_i2c_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_zephyr_i2c) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&zephyr_i2c_i2c_type) },
};

static MP_DEFINE_CONST_DICT(zephyr_i2c_module_globals, zephyr_i2c_module_globals_table);

const mp_obj_module_t zephyr_i2c_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&zephyr_i2c_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_zephyr_i2c, zephyr_i2c_module);
