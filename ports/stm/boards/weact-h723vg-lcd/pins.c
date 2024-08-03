// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA00) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PA01) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_PA06) },
    { MP_ROM_QSTR(MP_QSTR_A7), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_A8), MP_ROM_PTR(&pin_PA08) },
    { MP_ROM_QSTR(MP_QSTR_A9), MP_ROM_PTR(&pin_PA09) },
    { MP_ROM_QSTR(MP_QSTR_A10), MP_ROM_PTR(&pin_PA10) },
    { MP_ROM_QSTR(MP_QSTR_A11), MP_ROM_PTR(&pin_PA11) },
    { MP_ROM_QSTR(MP_QSTR_A12), MP_ROM_PTR(&pin_PA12) },
    { MP_ROM_QSTR(MP_QSTR_A15), MP_ROM_PTR(&pin_PA15) },

    { MP_ROM_QSTR(MP_QSTR_B0), MP_ROM_PTR(&pin_PB00) },
    { MP_ROM_QSTR(MP_QSTR_B1), MP_ROM_PTR(&pin_PB01) },
    { MP_ROM_QSTR(MP_QSTR_B2), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_B3), MP_ROM_PTR(&pin_PB03) },
    { MP_ROM_QSTR(MP_QSTR_B4), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_B5), MP_ROM_PTR(&pin_PB05) },
    { MP_ROM_QSTR(MP_QSTR_B6), MP_ROM_PTR(&pin_PB06) },
    { MP_ROM_QSTR(MP_QSTR_B7), MP_ROM_PTR(&pin_PB07) },
    { MP_ROM_QSTR(MP_QSTR_B8), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_B9), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_B10), MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_B11), MP_ROM_PTR(&pin_PB11) },
    { MP_ROM_QSTR(MP_QSTR_B12), MP_ROM_PTR(&pin_PB12) },
    { MP_ROM_QSTR(MP_QSTR_B13), MP_ROM_PTR(&pin_PB13) },
    { MP_ROM_QSTR(MP_QSTR_B14), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_B15), MP_ROM_PTR(&pin_PB15) },

    { MP_ROM_QSTR(MP_QSTR_C0), MP_ROM_PTR(&pin_PC00) },
    { MP_ROM_QSTR(MP_QSTR_C1), MP_ROM_PTR(&pin_PC01) },
    { MP_ROM_QSTR(MP_QSTR_C2), MP_ROM_PTR(&pin_PC02) },
    { MP_ROM_QSTR(MP_QSTR_C3), MP_ROM_PTR(&pin_PC03) },
    { MP_ROM_QSTR(MP_QSTR_C4), MP_ROM_PTR(&pin_PC04) },
    { MP_ROM_QSTR(MP_QSTR_C5), MP_ROM_PTR(&pin_PC05) },
    { MP_ROM_QSTR(MP_QSTR_C6), MP_ROM_PTR(&pin_PC06) },
    { MP_ROM_QSTR(MP_QSTR_C7), MP_ROM_PTR(&pin_PC07) },
    { MP_ROM_QSTR(MP_QSTR_C8), MP_ROM_PTR(&pin_PC08) },
    { MP_ROM_QSTR(MP_QSTR_C9), MP_ROM_PTR(&pin_PC09) },
    { MP_ROM_QSTR(MP_QSTR_C10), MP_ROM_PTR(&pin_PC10) },
    { MP_ROM_QSTR(MP_QSTR_C11), MP_ROM_PTR(&pin_PC11) },
    { MP_ROM_QSTR(MP_QSTR_C12), MP_ROM_PTR(&pin_PC12) },
    { MP_ROM_QSTR(MP_QSTR_C13), MP_ROM_PTR(&pin_PC13) },

    { MP_ROM_QSTR(MP_QSTR_D0), MP_ROM_PTR(&pin_PD00) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_PD01) },
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_PD02) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_PD03) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_PD04) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_PD05) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_PD06) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_PD07) },
    { MP_ROM_QSTR(MP_QSTR_D8), MP_ROM_PTR(&pin_PD08) },
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PD09) },
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PD10) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PD11) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_PD12) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PD13) },
    { MP_ROM_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_PD14) },
    { MP_ROM_QSTR(MP_QSTR_D15), MP_ROM_PTR(&pin_PD15) },

    { MP_ROM_QSTR(MP_QSTR_E0), MP_ROM_PTR(&pin_PE00) },
    { MP_ROM_QSTR(MP_QSTR_E1), MP_ROM_PTR(&pin_PE01) },
    { MP_ROM_QSTR(MP_QSTR_E2), MP_ROM_PTR(&pin_PE02) },
    { MP_ROM_QSTR(MP_QSTR_E3), MP_ROM_PTR(&pin_PE03) },
    { MP_ROM_QSTR(MP_QSTR_E4), MP_ROM_PTR(&pin_PE04) },
    { MP_ROM_QSTR(MP_QSTR_E5), MP_ROM_PTR(&pin_PE05) },
    { MP_ROM_QSTR(MP_QSTR_E6), MP_ROM_PTR(&pin_PE06) },
    { MP_ROM_QSTR(MP_QSTR_E7), MP_ROM_PTR(&pin_PE07) },
    { MP_ROM_QSTR(MP_QSTR_E8), MP_ROM_PTR(&pin_PE08) },
    { MP_ROM_QSTR(MP_QSTR_E9), MP_ROM_PTR(&pin_PE09) },
    { MP_ROM_QSTR(MP_QSTR_E10), MP_ROM_PTR(&pin_PE10) },
    { MP_ROM_QSTR(MP_QSTR_E11), MP_ROM_PTR(&pin_PE11) },
    { MP_ROM_QSTR(MP_QSTR_E12), MP_ROM_PTR(&pin_PE12) },
    { MP_ROM_QSTR(MP_QSTR_E13), MP_ROM_PTR(&pin_PE13) },
    { MP_ROM_QSTR(MP_QSTR_E14), MP_ROM_PTR(&pin_PE14) },
    { MP_ROM_QSTR(MP_QSTR_E15), MP_ROM_PTR(&pin_PE15) },

    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_PE03) },
    { MP_ROM_QSTR(MP_QSTR_SW), MP_ROM_PTR(&pin_PC13) },

    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
