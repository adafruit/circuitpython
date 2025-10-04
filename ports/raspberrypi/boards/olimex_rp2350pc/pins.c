// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    //  UEXT2 HEADER  Pin 1 - 3.3V, 2 - Gnd
    { MP_OBJ_NEW_QSTR(MP_QSTR_D36), MP_ROM_PTR(&pin_GPIO36) }, // PIN 6
    { MP_OBJ_NEW_QSTR(MP_QSTR_D37), MP_ROM_PTR(&pin_GPIO37) }, // 5
    { MP_OBJ_NEW_QSTR(MP_QSTR_D38), MP_ROM_PTR(&pin_GPIO38) }, // 3
    { MP_OBJ_NEW_QSTR(MP_QSTR_D39), MP_ROM_PTR(&pin_GPIO39) }, // 4
    { MP_OBJ_NEW_QSTR(MP_QSTR_D42), MP_ROM_PTR(&pin_GPIO42) }, // 9
    { MP_OBJ_NEW_QSTR(MP_QSTR_D43), MP_ROM_PTR(&pin_GPIO43) }, // 8
    { MP_OBJ_NEW_QSTR(MP_QSTR_D44), MP_ROM_PTR(&pin_GPIO44) }, // 7
    { MP_OBJ_NEW_QSTR(MP_QSTR_D45), MP_ROM_PTR(&pin_GPIO45) }, // 10
     
//    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO32) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA0), MP_ROM_PTR(&pin_GPIO36) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D36), MP_ROM_PTR(&pin_GPIO36) },
//    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO33) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL0), MP_ROM_PTR(&pin_GPIO37) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D37), MP_ROM_PTR(&pin_GPIO37) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SDA1), MP_ROM_PTR(&pin_GPIO2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_GPIO2) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SCL1), MP_ROM_PTR(&pin_GPIO3) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_GPIO3) },

    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO4) },

    { MP_ROM_QSTR(MP_QSTR_SD_CS), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_SD_SCK), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_SD_MOSI), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_SD_MISO), MP_ROM_PTR(&pin_GPIO24) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SD_CARD_DETECT), MP_ROM_PTR(&pin_GPIO41) },

    { MP_ROM_QSTR(MP_QSTR_CKN), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_CKP), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_D0N), MP_ROM_PTR(&pin_GPIO13) }, // Red
    { MP_ROM_QSTR(MP_QSTR_D0P), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_D1N), MP_ROM_PTR(&pin_GPIO19) }, // Green
    { MP_ROM_QSTR(MP_QSTR_D1P), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_D2N), MP_ROM_PTR(&pin_GPIO17) }, // Blue
    { MP_ROM_QSTR(MP_QSTR_D2P), MP_ROM_PTR(&pin_GPIO16) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO25) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_D25), MP_ROM_PTR(&pin_GPIO25) },

    { MP_ROM_QSTR(MP_QSTR_PWM_SPK_L), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_PWM_SPK_R), MP_ROM_PTR(&pin_GPIO27) },

    { MP_ROM_QSTR(MP_QSTR_I2S_PWR), MP_ROM_PTR(&pin_GPIO22) },

    { MP_ROM_QSTR(MP_QSTR_I2S_DIN), MP_ROM_PTR(&pin_GPIO31) },
    { MP_ROM_QSTR(MP_QSTR_I2S_MCLK), MP_ROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_I2S_BCLK), MP_ROM_PTR(&pin_GPIO30) },
    { MP_ROM_QSTR(MP_QSTR_I2S_WS), MP_ROM_PTR(&pin_GPIO29) },
    { MP_ROM_QSTR(MP_QSTR_I2S_ASDOUT), MP_ROM_PTR(&pin_GPIO28) },

    // AMP_VOL must be set high and AMP_SHDN set low to enable the PAM8003 amplifier
    // There is an unpopulated potentiometer but without it AMP_VOL is only off/on
    { MP_ROM_QSTR(MP_QSTR_AMP_VOL), MP_ROM_PTR(&pin_GPIO34) },
    { MP_ROM_QSTR(MP_QSTR_AMP_SHDN), MP_ROM_PTR(&pin_GPIO35) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
