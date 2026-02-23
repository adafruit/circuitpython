// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/i2cioexpander/__init__.h"
#include "shared-bindings/i2cioexpander/IOExpander.h"

//| """Support for I2C-based GPIO expanders
//|
//| The `i2cioexpander` module contains classes to support I2C-based GPIO expanders
//| that can be controlled via simple register reads and writes.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| Example::
//|
//|   import board
//|   import busio
//|   import i2cioexpander
//|
//|   i2c = busio.I2C(board.SCL, board.SDA)
//|   expander = i2cioexpander.IOExpander(i2c, 0x20, 8, 0x01, 0x00, 0x03)
//|   pin0 = expander.pins[0]
//|   pin0.switch_to_output(value=True)
//| """

static const mp_rom_map_elem_t i2cioexpander_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_i2cioexpander) },
    { MP_ROM_QSTR(MP_QSTR_IOExpander), MP_ROM_PTR(&i2cioexpander_ioexpander_type) },
};

static MP_DEFINE_CONST_DICT(i2cioexpander_module_globals, i2cioexpander_module_globals_table);

const mp_obj_module_t i2cioexpander_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&i2cioexpander_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_i2cioexpander, i2cioexpander_module);
