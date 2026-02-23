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
    // Encoder 1: V3-E uses GPIO26/13, V3 uses GPIO35/13
    { MP_ROM_QSTR(MP_QSTR_ENC1A), MP_OBJ_FROM_PTR(&pin_GPIO26) },  // V3-E: GPIO26, V3: GPIO35
    { MP_ROM_QSTR(MP_QSTR_ENC1A_V3), MP_OBJ_FROM_PTR(&pin_GPIO35) },  // V3-E: GPIO26, V3: GPIO35
    { MP_ROM_QSTR(MP_QSTR_ENC1B), MP_OBJ_FROM_PTR(&pin_GPIO13) },

    // Encoder 2: Both V3 and V3-E use GPIO27/4
    { MP_ROM_QSTR(MP_QSTR_ENC2A), MP_OBJ_FROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_ENC2B), MP_OBJ_FROM_PTR(&pin_GPIO4) },

    // Light sensor (V3/V3-E)
    { MP_ROM_QSTR(MP_QSTR_LIGHT_SENSOR), MP_OBJ_FROM_PTR(&pin_GPIO36) },

    // Temperature sensors (V3/V3-E)
    { MP_ROM_QSTR(MP_QSTR_TEMP_SENSOR), MP_OBJ_FROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_QI_RX_TEMP_SENSOR), MP_OBJ_FROM_PTR(&pin_GPIO35) },  // V3-E only

    { MP_ROM_QSTR(MP_QSTR_SCL), MP_OBJ_FROM_PTR(&pin_GPIO25) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_OBJ_FROM_PTR(&pin_GPIO21) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY_SCK), MP_OBJ_FROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_OBJ_FROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_MOSI), MP_OBJ_FROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_OBJ_FROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_MISO), MP_OBJ_FROM_PTR(&pin_GPIO26) },  // V3/V3-E have MISO
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_OBJ_FROM_PTR(&pin_GPIO26) },

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

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_OBJ_FROM_PTR(&board_i2c_obj) },

    // Filled in by board_init()
    { MP_ROM_QSTR(MP_QSTR_IOEXPANDER0), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_IOEXPANDER1), mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_BATTERY_ALERT), mp_const_none },  // IOX.0.6
    { MP_ROM_QSTR(MP_QSTR_QI_STATUS), mp_const_none },      // IOX.0.7
    { MP_ROM_QSTR(MP_QSTR_USB_STATUS), mp_const_none },     // IOX.1.0
    { MP_ROM_QSTR(MP_QSTR_CHARGE_STATUS), mp_const_none },  // IOX.1.4
    { MP_ROM_QSTR(MP_QSTR_POWER_BUTTON), mp_const_none },   // IOX.1.3
    { MP_ROM_QSTR(MP_QSTR_ENC1_BUTTON), mp_const_none },    // IOX.0.5
    { MP_ROM_QSTR(MP_QSTR_ENC2_BUTTON), mp_const_none },    // IOX.0.4
    { MP_ROM_QSTR(MP_QSTR_HEADPHONE_DETECT), mp_const_none }, // IOX.1.1
    { MP_ROM_QSTR(MP_QSTR_PACTRL), mp_const_none },         // IOX.2.4
    // V3/V3-E use ht16d35x display with 4 CS lines
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_CS0), mp_const_none },    // IOX.2.0
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_CS1), mp_const_none },    // IOX.2.1
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_CS2), mp_const_none },    // IOX.2.2
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_CS3), mp_const_none },    // IOX.2.3

    { MP_ROM_QSTR(MP_QSTR_LEVEL_CONVERTER), mp_const_none }, // IOX.3.0
    { MP_ROM_QSTR(MP_QSTR_LEVEL_POWER_ENABLE), mp_const_none }, // IOX.2.5
    { MP_ROM_QSTR(MP_QSTR_LEVEL_VINHOLD), mp_const_none },  // IOX.3.1
    { MP_ROM_QSTR(MP_QSTR_LEVEL_VOUTEN), mp_const_none },   // IOX.3.3

    { MP_ROM_QSTR(MP_QSTR_TILT), mp_const_none },           // IOX.1.2
    { MP_ROM_QSTR(MP_QSTR_RTC_INT), mp_const_none },        // IOX.0.1

    // Qi charging pins (V3-E)
    { MP_ROM_QSTR(MP_QSTR_QI_CHARGE_ENABLE), mp_const_none }, // IOX.2.6
    { MP_ROM_QSTR(MP_QSTR_QI_ENABLE_5W), mp_const_none },   // IOX.3.5
    { MP_ROM_QSTR(MP_QSTR_QI_I2C_INT), mp_const_none },     // IOX.0.0

    // USB-C charging pins (V3/V3-E)
    { MP_ROM_QSTR(MP_QSTR_USB_CHARGE_ENABLE), mp_const_none }, // IOX.2.7

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
