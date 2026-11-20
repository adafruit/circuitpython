// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vegard Wærp
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

// Pin assignments come from the board config stored in the device's MakeCode
// bootloader.
static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_PTR(&pin_PC00) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_PTR(&pin_PC01) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_LEFT), MP_ROM_PTR(&pin_PA15) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_RIGHT), MP_ROM_PTR(&pin_PC09) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_UP), MP_ROM_PTR(&pin_PB05) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_DOWN), MP_ROM_PTR(&pin_PB02) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_MENU), MP_ROM_PTR(&pin_PC13) },

    { MP_ROM_QSTR(MP_QSTR_TFT_CS), MP_ROM_PTR(&pin_PB12) },
    { MP_ROM_QSTR(MP_QSTR_TFT_DC), MP_ROM_PTR(&pin_PC03) },
    { MP_ROM_QSTR(MP_QSTR_TFT_RESET), MP_ROM_PTR(&pin_PB07) },
    { MP_ROM_QSTR(MP_QSTR_TFT_BACKLIGHT), MP_ROM_PTR(&pin_PB09) },
    { MP_ROM_QSTR(MP_QSTR_TFT_WR), MP_ROM_PTR(&pin_PC02) },
    { MP_ROM_QSTR(MP_QSTR_TFT_RD), MP_ROM_PTR(&pin_PC05) },
    { MP_ROM_QSTR(MP_QSTR_TFT_D0), MP_ROM_PTR(&pin_PB14) },
    { MP_ROM_QSTR(MP_QSTR_TFT_D1), MP_ROM_PTR(&pin_PC06) },
    { MP_ROM_QSTR(MP_QSTR_TFT_D2), MP_ROM_PTR(&pin_PC11) },
    { MP_ROM_QSTR(MP_QSTR_TFT_D3), MP_ROM_PTR(&pin_PC12) },
    { MP_ROM_QSTR(MP_QSTR_TFT_D4), MP_ROM_PTR(&pin_PA02) },
    { MP_ROM_QSTR(MP_QSTR_TFT_D5), MP_ROM_PTR(&pin_PA03) },
    { MP_ROM_QSTR(MP_QSTR_TFT_D6), MP_ROM_PTR(&pin_PA04) },
    { MP_ROM_QSTR(MP_QSTR_TFT_D7), MP_ROM_PTR(&pin_PA05) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display) },

    { MP_ROM_QSTR(MP_QSTR_ACCELEROMETER_INTERRUPT), MP_ROM_PTR(&pin_PB04) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_PB10) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_PB03) },

    { MP_ROM_QSTR(MP_QSTR_LIGHT), MP_ROM_PTR(&pin_PA07) },
    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_MONITOR), MP_ROM_PTR(&pin_PC04) },
    { MP_ROM_QSTR(MP_QSTR_VIBRATION), MP_ROM_PTR(&pin_PC08) },
    { MP_ROM_QSTR(MP_QSTR_SPEAKER), MP_ROM_PTR(&pin_PA08) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
