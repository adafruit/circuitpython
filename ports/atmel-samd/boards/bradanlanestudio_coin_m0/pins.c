// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Neopixels
    { MP_ROM_QSTR(MP_QSTR_D9), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_PA07) },

    // discrete LEDs
    { MP_ROM_QSTR(MP_QSTR_D10), MP_ROM_PTR(&pin_PA18) },
    { MP_ROM_QSTR(MP_QSTR_D11), MP_ROM_PTR(&pin_PA16) },
    { MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_PA19) },

    // on-board LED
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_PA17) },
    { MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_PA17) },

    { MP_ROM_QSTR(MP_QSTR_A0), MP_ROM_PTR(&pin_PA02) }, // Analog only; no PWM
    { MP_ROM_QSTR(MP_QSTR_SPEAKER), MP_ROM_PTR(&pin_PA02) },

    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH2), MP_ROM_PTR(&pin_PB08) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH1), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_TOUCH3), MP_ROM_PTR(&pin_PB02) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
