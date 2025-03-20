// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 snkYmkrct
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_PC07)},
    {MP_ROM_QSTR(MP_QSTR_BOOT), MP_ROM_PTR(&pin_PG03)},
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
