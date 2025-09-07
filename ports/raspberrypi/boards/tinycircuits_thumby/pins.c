// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Link Cable (ASR00074)
    { MP_ROM_QSTR(MP_QSTR_EXT_TX), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_EXT), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_EXT_PU), MP_ROM_PTR(&pin_GPIO1) },

    // 0.42 inch OLED AST1042
    { MP_ROM_QSTR(MP_QSTR_OLED_CS), MP_ROM_PTR(CIRCUITPY_BOARD_OLED_CS) },
    { MP_ROM_QSTR(MP_QSTR_OLED_DC), MP_ROM_PTR(CIRCUITPY_BOARD_OLED_DC) },
    { MP_ROM_QSTR(MP_QSTR_OLED_RESET), MP_ROM_PTR(CIRCUITPY_BOARD_OLED_RESET) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(DEFAULT_SPI_BUS_SCK) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(DEFAULT_SPI_BUS_MOSI) },

    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)},

    // Buttons
    { MP_ROM_QSTR(MP_QSTR_BUTTON_LEFT), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_UP), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_RIGHT), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_DOWN), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_1), MP_ROM_PTR(&pin_GPIO24) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_2), MP_ROM_PTR(&pin_GPIO27) },

    // Mono PWM Speaker
    { MP_ROM_QSTR(MP_QSTR_SPEAKER), MP_ROM_PTR(&pin_GPIO28) },

    // Hardware revision ID pins
    { MP_OBJ_NEW_QSTR(MP_QSTR_ID3), MP_ROM_PTR(&pin_GPIO12) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ID2), MP_ROM_PTR(&pin_GPIO13) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ID1), MP_ROM_PTR(&pin_GPIO14) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ID0), MP_ROM_PTR(&pin_GPIO15) },

    // Power pins
    { MP_ROM_QSTR(MP_QSTR_VBUS_SENSE), MP_ROM_PTR(&pin_GPIO26) }
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
