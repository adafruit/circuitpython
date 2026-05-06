// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tod Kurt
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/mcp4822/__init__.h"
#include "shared-bindings/mcp4822/MCP4822.h"

//| """Audio output via MCP4822 dual-channel 12-bit SPI DAC.
//|
//| The `mcp4822` module provides the `MCP4822` class for non-blocking
//| audio playback through the Microchip MCP4822 SPI DAC using PIO and DMA.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed. To do so, either call :py:meth:`!deinit` or use a
//| context manager."""

static const mp_rom_map_elem_t mcp4822_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mcp4822) },
    { MP_ROM_QSTR(MP_QSTR_MCP4822), MP_ROM_PTR(&mcp4822_mcp4822_type) },
};

static MP_DEFINE_CONST_DICT(mcp4822_module_globals, mcp4822_module_globals_table);

const mp_obj_module_t mcp4822_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mcp4822_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_mcp4822, mcp4822_module);
