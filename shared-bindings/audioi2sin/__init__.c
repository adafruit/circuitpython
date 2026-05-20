// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/audioi2sin/__init__.h"
#include "shared-bindings/audioi2sin/I2SIn.h"

//| """Support for recording audio from an I2S input source.
//|
//| The `audioi2sin` module contains the `I2SIn` class for recording audio
//| from an external I2S source such as a MEMS microphone (e.g. SPH0645LM4H
//| or INMP441).
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed. To do so, either call :py:meth:`!deinit` or use a
//| context manager."""

static const mp_rom_map_elem_t audioi2sin_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audioi2sin) },
    { MP_ROM_QSTR(MP_QSTR_I2SIn), MP_ROM_PTR(&audioi2sin_i2sin_type) },
};

static MP_DEFINE_CONST_DICT(audioi2sin_module_globals, audioi2sin_module_globals_table);

const mp_obj_module_t audioi2sin_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audioi2sin_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audioi2sin, audioi2sin_module);
