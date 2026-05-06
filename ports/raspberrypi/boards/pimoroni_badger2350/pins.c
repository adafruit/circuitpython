// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Bob Abeles
//
// SPDX-License-Identifier: MIT

#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "badger2350-shared.h"


// LUT manipulation
static const mp_rom_map_elem_t lut_update_table[] = {
    { MP_ROM_QSTR(MP_QSTR_SET_UPDATE_SPEED), (mp_obj_t)&set_update_speed_obj },
    { MP_ROM_QSTR(MP_QSTR_SPEED_SLOW), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_FAST), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_FASTER), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_SPEED_FASTEST), MP_ROM_INT(3) },
};
MP_DEFINE_CONST_DICT(lut_update_dict, lut_update_table);

const mp_obj_module_t display_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&lut_update_dict,
};

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    { MP_ROM_QSTR(MP_QSTR_display), MP_ROM_PTR(&display_module) },

    { MP_ROM_QSTR(MP_QSTR_GP0), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_LED), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_LED0), MP_ROM_PTR(&pin_GPIO0) },

    { MP_ROM_QSTR(MP_QSTR_GP1), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_LED1), MP_ROM_PTR(&pin_GPIO1) },

    { MP_ROM_QSTR(MP_QSTR_GP2), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_LED2), MP_ROM_PTR(&pin_GPIO2) },

    { MP_ROM_QSTR(MP_QSTR_GP3), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_LED3), MP_ROM_PTR(&pin_GPIO3) },

    { MP_ROM_QSTR(MP_QSTR_GP4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO4) },

    { MP_ROM_QSTR(MP_QSTR_GP5), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO5) },

    { MP_ROM_QSTR(MP_QSTR_GP6), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_SW_DOWN), MP_ROM_PTR(&pin_GPIO6) },

    { MP_ROM_QSTR(MP_QSTR_GP7), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_SW_A), MP_ROM_PTR(&pin_GPIO7) },

    // GP8 is reserved for PSRAM chip select

    { MP_ROM_QSTR(MP_QSTR_GP9), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_SW_B), MP_ROM_PTR(&pin_GPIO9) },

    { MP_ROM_QSTR(MP_QSTR_GP10), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_SW_C), MP_ROM_PTR(&pin_GPIO10) },

    { MP_ROM_QSTR(MP_QSTR_GP11), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_SW_UP), MP_ROM_PTR(&pin_GPIO11) },

    { MP_ROM_QSTR(MP_QSTR_GP12), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_VBUS_SENSE), MP_ROM_PTR(&pin_GPIO12) },

    { MP_ROM_QSTR(MP_QSTR_GP13), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_RTC_ALARM), MP_ROM_PTR(&pin_GPIO13) },

    { MP_ROM_QSTR(MP_QSTR_GP14), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_SW_RESET), MP_ROM_PTR(&pin_GPIO14) },

    { MP_ROM_QSTR(MP_QSTR_GP15), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_SW_INT), MP_ROM_PTR(&pin_GPIO15) },

    { MP_ROM_QSTR(MP_QSTR_GP16), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_INKY_BUSY), MP_ROM_PTR(&pin_GPIO16) },

    { MP_ROM_QSTR(MP_QSTR_GP17), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_INKY_CS), MP_ROM_PTR(&pin_GPIO17) },

    { MP_ROM_QSTR(MP_QSTR_GP18), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_GPIO18) },

    { MP_ROM_QSTR(MP_QSTR_GP19), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO19) },

    { MP_ROM_QSTR(MP_QSTR_GP20), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_INKY_DC), MP_ROM_PTR(&pin_GPIO20) },

    { MP_ROM_QSTR(MP_QSTR_GP21), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_INKY_RST), MP_ROM_PTR(&pin_GPIO21) },

    { MP_ROM_QSTR(MP_QSTR_GP22), MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_SW_HOME), MP_ROM_PTR(&pin_GPIO22) },

    // GP23, GP24, GP25, and GP29 are reserved for RM2 gSPI

    { MP_ROM_QSTR(MP_QSTR_GP26), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_VBAT_SENSE), MP_ROM_PTR(&pin_GPIO26) },

    // GP27 is the used for I2C power-enable, driven high by board.c
    { MP_ROM_QSTR(MP_QSTR_I2C_POWER_EN), MP_ROM_PTR(&i2c_power_en_pin_obj) },

    { MP_ROM_QSTR(MP_QSTR_GP28), MP_ROM_PTR(&pin_GPIO28) },
    { MP_ROM_QSTR(MP_QSTR_SENSE_1V1), MP_ROM_PTR(&pin_GPIO28) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_STEMMA_I2C), MP_ROM_PTR(&board_i2c_obj) },

    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },

    // Pins accessed though the RM2 module (CYW43439)
    // CYW0, CYW1 is unconnected
    { MP_ROM_QSTR(MP_QSTR_CHARGE_STAT), MP_ROM_PTR(&pin_CYW2) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].epaper_display)},

    // button-state on reset
    // Use `board.RESET_STATE()` to query the state of all buttons at reset.
    // To detect individual key-presses, `board.ON_RESET_PRESSED()`
    // is simpler.
    { MP_ROM_QSTR(MP_QSTR_RESET_STATE), (mp_obj_t)&get_reset_state_obj },

    // Use `board.ON_RESET_PRESSED(board.SW_A)` to check if `SW_A` was pressed
    // during reset.
    { MP_ROM_QSTR(MP_QSTR_ON_RESET_PRESSED), (mp_obj_t)&on_reset_pressed_obj },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
