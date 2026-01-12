// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // External pins are in silkscreen order, from top to bottom, left side, then right side
    { MP_ROM_QSTR(MP_QSTR_ENC1A), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_D39), MP_ROM_PTR(&pin_GPIO39) },

    { MP_ROM_QSTR(MP_QSTR_ENC1B), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_D35), MP_ROM_PTR(&pin_GPIO35) },
	
	{ MP_ROM_QSTR(MP_QSTR_ENC2A), MP_ROM_PTR(&pin_GPIO36) },
    { MP_ROM_QSTR(MP_QSTR_D36), MP_ROM_PTR(&pin_GPIO36) },
	
	{ MP_ROM_QSTR(MP_QSTR_ENC2B), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_D27), MP_ROM_PTR(&pin_GPIO27) },

    { MP_ROM_QSTR(MP_QSTR_NFC_IN), MP_ROM_PTR(&pin_GPIO32) },
    { MP_ROM_QSTR(MP_QSTR_D32), MP_ROM_PTR(&pin_GPIO32) },

    { MP_ROM_QSTR(MP_QSTR_NFC_OUT), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_D33), MP_ROM_PTR(&pin_GPIO33) },
	
	{ MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO25) },
    { MP_ROM_QSTR(MP_QSTR_D25), MP_ROM_PTR(&pin_GPIO25) },

    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_D21), MP_ROM_PTR(&pin_GPIO21) },

    { MP_ROM_QSTR(MP_QSTR_DIS_SCK), MP_ROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_D23), MP_ROM_PTR(&pin_GPIO23) },

    { MP_ROM_QSTR(MP_QSTR_DIS_MOSI), MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_D22), MP_ROM_PTR(&pin_GPIO22) },

    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO32) },
    { MP_ROM_QSTR(MP_QSTR_D32), MP_ROM_PTR(&pin_GPIO32) },

    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_D33), MP_ROM_PTR(&pin_GPIO33) },

    { MP_ROM_QSTR(MP_QSTR_D34), MP_ROM_PTR(&pin_GPIO34) },
	
	{ MP_ROM_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_GPIO14) },
	
	{ MP_ROM_QSTR(MP_QSTR_D26), MP_ROM_PTR(&pin_GPIO26) },
	
	{ MP_ROM_QSTR(MP_QSTR_D12), MP_ROM_PTR(&pin_GPIO12) },
	
	{ MP_ROM_QSTR(MP_QSTR_D13), MP_ROM_PTR(&pin_GPIO13) },
	
	{ MP_ROM_QSTR(MP_QSTR_D19), MP_ROM_PTR(&pin_GPIO19) },
	
	{ MP_ROM_QSTR(MP_QSTR_D18), MP_ROM_PTR(&pin_GPIO18) },
	
	{ MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_GPIO5) },
	
	{ MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_GPIO4) },
	
	{ MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_GPIO2) },
	
	{ MP_ROM_QSTR(MP_QSTR_D15), MP_ROM_PTR(&pin_GPIO15) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) }
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
