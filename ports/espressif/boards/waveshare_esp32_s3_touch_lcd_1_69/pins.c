// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Brian Nelson
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

// Boards with 8 MB octal PSRAM (this definition) use these pins. The first
// hardware revision (V1 schematic) put the power key on GPIO36, the power hold
// on GPIO35, the buzzer on GPIO33 and the RTC interrupt on GPIO41; those GPIOs
// belong to the octal PSRAM here, so V1 boards need their own definition.

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // BOOT button
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_BOOT), MP_ROM_PTR(&pin_GPIO0) },

    // PWR key (silkscreen "PWR", net SYS_OUT): high idle, low while pressed.
    // SYS_EN high keeps the board powered from the battery; drive it low to switch off.
    { MP_ROM_QSTR(MP_QSTR_PWR_BUTTON), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_SYS_OUT), MP_ROM_PTR(&pin_GPIO40) },

    { MP_ROM_QSTR(MP_QSTR_SYS_EN), MP_ROM_PTR(&pin_GPIO41) },

    { MP_ROM_QSTR(MP_QSTR_BUZZER), MP_ROM_PTR(&pin_GPIO42) },

    // Battery voltage
    { MP_ROM_QSTR(MP_QSTR_BAT_ADC), MP_ROM_PTR(&pin_GPIO1) },

    // I2C: CST816 touch (0x15), PCF85063 RTC (0x51), QMI8658 IMU (0x6B)
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO11) },

    { MP_ROM_QSTR(MP_QSTR_TP_INT), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_TP_RST), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_RTC_INT), MP_ROM_PTR(&pin_GPIO39) },

    // ST7789V2 display, 4-wire SPI
    { MP_ROM_QSTR(MP_QSTR_LCD_SCK), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_LCD_MOSI), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_LCD_DC), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CS), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_LCD_RST), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_LCD_BACKLIGHT), MP_ROM_PTR(&pin_GPIO15) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
