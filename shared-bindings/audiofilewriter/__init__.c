// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/audiofilewriter/__init__.h"
#include "shared-bindings/audiofilewriter/AudioFileWriter.h"

//| """Support for streaming audio to a WAV file"""

static const mp_rom_map_elem_t audiofilewriter_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audiofilewriter) },
    { MP_ROM_QSTR(MP_QSTR_AudioFileWriter), MP_ROM_PTR(&audiofilewriter_audiofilewriter_type) },
};

static MP_DEFINE_CONST_DICT(audiofilewriter_module_globals, audiofilewriter_module_globals_table);

const mp_obj_module_t audiofilewriter_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audiofilewriter_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audiofilewriter, audiofilewriter_module);
