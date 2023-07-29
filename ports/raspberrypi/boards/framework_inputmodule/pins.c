// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Daniel Schaefer for Framework Computer Inc
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

// Pinouts: https://github.com/FrameworkComputer/inputmodules/tree/main/Electrical
// Where modules use a pin differently, it has one name per module type.
static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // All modules: input from the EC, low when the system is asleep
    { MP_ROM_QSTR(MP_QSTR_SLEEP), MP_ROM_PTR(&pin_GPIO0) },

    // Keyboard: KSI rows are read through an analog mux into ANALOG_IN
    { MP_ROM_QSTR(MP_QSTR_MUX_A), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_MUX_B), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_MUX_C), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_MUX_ENABLE), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_BOOT_DONE), MP_ROM_PTR(&pin_GPIO5) },

    // Keyboard: KSO columns
    { MP_ROM_QSTR(MP_QSTR_KSO0), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_KSO1), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_KSO2), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_KSO3), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_KSO4), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_KSO5), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_KSO6), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_KSO7), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_KSO8), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_KSO9), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_KSO10), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_KSO11), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_KSO12), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_KSO13), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_KSO14), MP_ROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_KSO15), MP_ROM_PTR(&pin_GPIO22) },

    // RGB & white keyboard
    { MP_ROM_QSTR(MP_QSTR_CAPS_LED), MP_ROM_PTR(&pin_GPIO24) },
    // White keyboard & numpad
    { MP_ROM_QSTR(MP_QSTR_BACKLIGHT), MP_ROM_PTR(&pin_GPIO25) },
    // LED matrix
    { MP_ROM_QSTR(MP_QSTR_DIP1), MP_ROM_PTR(&pin_GPIO25) },

    // RGB keyboard, macropad & LED matrix: IS31FL3743A/IS31FL3741 LED controller
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_SDB), MP_ROM_PTR(&pin_GPIO29) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

    // Keyboard: KSI scan ADC input from the mux
    { MP_ROM_QSTR(MP_QSTR_ANALOG_IN), MP_ROM_PTR(&pin_GPIO28) },
    // LED matrix: IS31FL3741 interrupt
    { MP_ROM_QSTR(MP_QSTR_INTB), MP_ROM_PTR(&pin_GPIO28) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
