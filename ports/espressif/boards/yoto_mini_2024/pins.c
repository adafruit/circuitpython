// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT
//

#include "board.h"

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

static mp_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_MUTABLE_BOARD_DICT_STANDARD_ITEMS

    // External pins are in silkscreen order, from top to bottom, left side, then right side
    { MP_ROM_QSTR(MP_QSTR_ENC1A), MP_OBJ_FROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_ENC1B), MP_OBJ_FROM_PTR(&pin_GPIO35) },

    { MP_ROM_QSTR(MP_QSTR_ENC2A), MP_OBJ_FROM_PTR(&pin_GPIO36) },
    { MP_ROM_QSTR(MP_QSTR_ENC2B), MP_OBJ_FROM_PTR(&pin_GPIO27) },

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_OBJ_FROM_PTR(&pin_GPIO25) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_OBJ_FROM_PTR(&pin_GPIO21) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY_SCK), MP_OBJ_FROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_MOSI), MP_OBJ_FROM_PTR(&pin_GPIO22) },

    { MP_ROM_QSTR(MP_QSTR_NFC_IN), MP_OBJ_FROM_PTR(&pin_GPIO32) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_OBJ_FROM_PTR(&pin_GPIO32) },
    { MP_ROM_QSTR(MP_QSTR_NFC_OUT), MP_OBJ_FROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_OBJ_FROM_PTR(&pin_GPIO33) },

    { MP_ROM_QSTR(MP_QSTR_I2S_MCLK), MP_OBJ_FROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_I2S_BIT_CLOCK), MP_OBJ_FROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_I2S_BCLK), MP_OBJ_FROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_I2S_WORD_SELECT), MP_OBJ_FROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_I2S_LRCLK), MP_OBJ_FROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_I2S_DOUT), MP_OBJ_FROM_PTR(&pin_GPIO19) },

    { MP_ROM_QSTR(MP_QSTR_IOEXPANDER_INT), MP_OBJ_FROM_PTR(&pin_GPIO34) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_OBJ_FROM_PTR(&displays[0].display)},
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_OBJ_FROM_PTR(&board_i2c_obj) },

    // Filled in by board_init()
    { MP_ROM_QSTR(MP_QSTR_IOEXPANDER), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_PLUG_STATUS), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_CHARGE_STATUS), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_POWER_BUTTON), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_ENC1_BUTTON), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_ENC2_BUTTON), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_HEADPHONE_DETECT), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_PACTRL), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_CS), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_DC), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_RESET), mp_const_none },

    { MP_ROM_QSTR(MP_QSTR_LEVEL_CONVERTER), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_LEVEL_POWER_ENABLE), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_LEVEL_VINHOLD), mp_const_none },

    { MP_ROM_QSTR(MP_QSTR_TILT), mp_const_none },

    // Only on some variants
    { MP_ROM_QSTR(MP_QSTR_RTC_INT), mp_const_none },

};
MP_DEFINE_MUTABLE_DICT(board_module_globals, board_module_globals_table);

void board_set(qstr q, mp_obj_t value) {
    mp_obj_t key = MP_OBJ_NEW_QSTR(q);
    for (size_t i = 0; i < MP_ARRAY_SIZE(board_module_globals_table); i++) {
        if (board_module_globals_table[i].key == key) {
            board_module_globals_table[i].value = value;
            return;
        }
    }
}
