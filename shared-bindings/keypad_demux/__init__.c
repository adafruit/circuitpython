// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 CDarius
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"

#include "shared-bindings/keypad_demux/DemuxKeyMatrix.h"
#include "shared-bindings/util.h"

//| """Support for scanning key matrices that use a demultiplexer
//|
//| The `keypad_demux` module provides native support to scan a matrix of keys or buttons
//| where either the row or column axis is controlled by a demultiplexer or decoder IC
//| such as the 74LS138 or 74LS238.  In this arrangement a binary input value
//| determines which column (or row) to select, thereby reducing the number of input pins.
//| For example the input 101 would select line 5 in the matrix.
//| Set ``columns_to_anodes`` to ``False`` with a non-inverting demultiplexer
//| which drives the selected line high.
//| Set ``transpose`` to ``True`` if columns are multiplexed rather than rows.
//|
//| .. jinja
//| """

static mp_rom_map_elem_t keypad_demux_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),          MP_OBJ_NEW_QSTR(MP_QSTR_keypad_demux) },
    { MP_ROM_QSTR(MP_QSTR_DemuxKeyMatrix),    MP_OBJ_FROM_PTR(&keypad_demux_demuxkeymatrix_type) },
};

static MP_DEFINE_CONST_DICT(keypad_demux_module_globals, keypad_demux_module_globals_table);

const mp_obj_module_t keypad_demux_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&keypad_demux_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_keypad_demux, keypad_demux_module);
