// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Buttons
    { MP_ROM_QSTR(MP_QSTR_BUTTON_UP), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_LEFT), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_DOWN), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_RIGHT), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_A), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_B), MP_ROM_PTR(&pin_GPIO25) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_BUMPER_LEFT), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_BUMPER_RIGHT), MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON_MENU), MP_ROM_PTR(&pin_GPIO26) },

    // LED
    { MP_ROM_QSTR(MP_QSTR_LED_R), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_LED_G), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_LED_B), MP_ROM_PTR(&pin_GPIO12) },

    // Rumble
    { MP_ROM_QSTR(MP_QSTR_RUMBLE), MP_ROM_PTR(&pin_GPIO5) },

    // Mono PWM Speaker
    { MP_ROM_QSTR(MP_QSTR_SPEAKER), MP_ROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_SPEAKER_ENABLE), MP_ROM_PTR(&pin_GPIO20) },

    // 0.85 inch TFT GC9107
    { MP_ROM_QSTR(MP_QSTR_LCD_CS), MP_ROM_PTR(CIRCUITPY_BOARD_LCD_CS) },
    { MP_ROM_QSTR(MP_QSTR_LCD_DC), MP_ROM_PTR(CIRCUITPY_BOARD_LCD_DC) },
    { MP_ROM_QSTR(MP_QSTR_LCD_RESET), MP_ROM_PTR(CIRCUITPY_BOARD_LCD_RESET) },
    { MP_ROM_QSTR(MP_QSTR_LCD_BACKLIGHT), MP_ROM_PTR(CIRCUITPY_BOARD_LCD_BACKLIGHT) },
    { MP_ROM_QSTR(MP_QSTR_LCD_SCK), MP_ROM_PTR(DEFAULT_SPI_BUS_SCK) },
    { MP_ROM_QSTR(MP_QSTR_LCD_MOSI), MP_ROM_PTR(DEFAULT_SPI_BUS_MOSI) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display) },
    { MP_ROM_QSTR(MP_QSTR_LCD_SPI), MP_ROM_PTR(&board_spi_obj) },

    // RTC I2C
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(DEFAULT_I2C_BUS_SDA) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(DEFAULT_I2C_BUS_SCL) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

    // Power pins
    { MP_ROM_QSTR(MP_QSTR_CHARGE_STAT), MP_ROM_PTR(&pin_GPIO24) },
    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_MONITOR), MP_ROM_PTR(&pin_GPIO29) },

};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
