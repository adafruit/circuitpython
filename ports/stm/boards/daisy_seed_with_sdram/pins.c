// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 snkYmkrct
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

// See pinout on Daisy Seed product page
// https://electro-smith.com/products/daisy-seed?variant=45234245108004
static const mp_rom_map_elem_t board_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_PC07)},
    {MP_ROM_QSTR(MP_QSTR_BOOT), MP_ROM_PTR(&pin_PG03)},
    {MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PB12)},
    {MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PC11)},
    {MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_PC10)},
    {MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_PC09)},
    {MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_PC08)},
    {MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PD02)},
    {MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PC12)},
    {MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_PG10)},
    {MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_PG11)},
    {MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PB04)},
    {MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PB05)},
    {MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PB08)},
    {MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_PB09)},
    {MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PB06)},
    {MP_ROM_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_PB07)},

    {MP_ROM_QSTR(MP_QSTR_D15), MP_ROM_PTR(&pin_PC00)},
    {MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PC00)},
    {MP_ROM_QSTR(MP_QSTR_D16), MP_ROM_PTR(&pin_PA03)},
    {MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA03)},
    {MP_ROM_QSTR(MP_QSTR_D17), MP_ROM_PTR(&pin_PB01)},
    {MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PB01)},
    {MP_ROM_QSTR(MP_QSTR_D18), MP_ROM_PTR(&pin_PA07)},
    {MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PA07)},
    {MP_ROM_QSTR(MP_QSTR_D19), MP_ROM_PTR(&pin_PA06)},
    {MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PA06)},
    {MP_ROM_QSTR(MP_QSTR_D20), MP_ROM_PTR(&pin_PC01)},
    {MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PC01)},
    {MP_ROM_QSTR(MP_QSTR_D21), MP_ROM_PTR(&pin_PC04)},
    {MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_PC04)},
    {MP_ROM_QSTR(MP_QSTR_D22), MP_ROM_PTR(&pin_PA05)},
    {MP_ROM_QSTR(MP_QSTR_A7), MP_ROM_PTR(&pin_PA05)},
    {MP_ROM_QSTR(MP_QSTR_D23), MP_ROM_PTR(&pin_PA04)},
    {MP_ROM_QSTR(MP_QSTR_A8), MP_ROM_PTR(&pin_PA04)},
    {MP_ROM_QSTR(MP_QSTR_D24), MP_ROM_PTR(&pin_PA01)},
    {MP_ROM_QSTR(MP_QSTR_A9), MP_ROM_PTR(&pin_PA01)},
    {MP_ROM_QSTR(MP_QSTR_D25), MP_ROM_PTR(&pin_PA00)},
    {MP_ROM_QSTR(MP_QSTR_A10), MP_ROM_PTR(&pin_PA00)},
    {MP_ROM_QSTR(MP_QSTR_D26), MP_ROM_PTR(&pin_PD11)},
    {MP_ROM_QSTR(MP_QSTR_D27), MP_ROM_PTR(&pin_PG09)},
    {MP_ROM_QSTR(MP_QSTR_D28), MP_ROM_PTR(&pin_PA02)},
    {MP_ROM_QSTR(MP_QSTR_A11), MP_ROM_PTR(&pin_PA02)},
    {MP_ROM_QSTR(MP_QSTR_D29), MP_ROM_PTR(&pin_PB14)},
    {MP_ROM_QSTR(MP_QSTR_D30), MP_ROM_PTR(&pin_PB15)},

};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
