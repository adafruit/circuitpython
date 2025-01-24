// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 n0xa, 2025 bootc
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

CIRCUITPY_BOARD_BUS_SINGLETON(grove_i2c, i2c, 1)

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Pin port on the top
    { MP_ROM_QSTR(MP_QSTR_G26), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_G36), MP_ROM_PTR(&pin_GPIO36) }, // G36/G25 pin
    { MP_ROM_QSTR(MP_QSTR_G25), MP_ROM_PTR(&pin_GPIO25) }, // G36/G25 pin
    { MP_ROM_QSTR(MP_QSTR_G0),  MP_ROM_PTR(&pin_GPIO0) },  // also PDM_MIC_CLK

    // Grove port on the bottom
    { MP_ROM_QSTR(MP_QSTR_G32), MP_ROM_PTR(&pin_GPIO32) },
    { MP_ROM_QSTR(MP_QSTR_GROVE_SDA), MP_ROM_PTR(&pin_GPIO32) },
    { MP_ROM_QSTR(MP_QSTR_G33), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_GROVE_SCL), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_GROVE_I2C), MP_ROM_PTR(&board_grove_i2c_obj) },

    // Buttons
    { MP_ROM_QSTR(MP_QSTR_G37), MP_ROM_PTR(&pin_GPIO37) },
    { MP_ROM_QSTR(MP_QSTR_BTN_A), MP_ROM_PTR(&pin_GPIO37) },
    { MP_ROM_QSTR(MP_QSTR_G39), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_BTN_B), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_G35), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_BTN_C), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_BTN_PWR), MP_ROM_PTR(&pin_GPIO35) },  // also WAKE

    // Buzzer / Speaker
    { MP_ROM_QSTR(MP_QSTR_G2), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_SPEAKER), MP_ROM_PTR(&pin_GPIO2) },

    // Red and IR LED (single pin)
    { MP_ROM_QSTR(MP_QSTR_G19), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_IR_LED), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO19) },

    // LCD display
    { MP_ROM_QSTR(MP_QSTR_LCD_MOSI), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CLK), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_LCD_DC), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_LCD_RST), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CS), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_LCD_BL), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)},

    // Battery voltage sense
    { MP_ROM_QSTR(MP_QSTR_G38), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_BAT_ADC), MP_ROM_PTR(&pin_GPIO38) },

    // Microphone
    { MP_ROM_QSTR(MP_QSTR_PDM_MIC_CLK), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_G34), MP_ROM_PTR(&pin_GPIO34) },
    { MP_ROM_QSTR(MP_QSTR_PDM_MIC_DATA), MP_ROM_PTR(&pin_GPIO34) },

    // Internal I2C (IMU and RTC)
    { MP_ROM_QSTR(MP_QSTR_G21), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_SYS_SDA), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_G22), MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_SYS_SCL), MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

    // Sleep/Wake signal
    { MP_ROM_QSTR(MP_QSTR_WAKE), MP_ROM_PTR(&pin_GPIO35) },

    // Power hold
    { MP_ROM_QSTR(MP_QSTR_G4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_HOLD), MP_ROM_PTR(&pin_GPIO4) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
