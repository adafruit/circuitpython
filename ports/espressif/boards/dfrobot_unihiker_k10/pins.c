// This file is part of the CircuitPython project: https://circuitpython.org
// SPDX-FileCopyrightText: Copyright (c) 2026 Djair Guilherme
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // =================================================================
    // ONBOARD PERIPHERALS - Functional Names
    // =================================================================

    // Buttons are PMIC-controlled (TCA9555), not direct GPIOs
    { MP_ROM_QSTR(MP_QSTR_BOOT),        MP_ROM_PTR(&pin_GPIO0) },

    // NeoPixel (3x WS2812)
    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL),    MP_ROM_PTR(&pin_GPIO46) },
    { MP_ROM_QSTR(MP_QSTR_LED),         MP_ROM_PTR(&pin_GPIO46) },

    // I2C (sensors: AHT20, LTR303ALS, SC7A20H)
    { MP_ROM_QSTR(MP_QSTR_SDA),         MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_SCL),         MP_ROM_PTR(&pin_GPIO48) },
    { MP_ROM_QSTR(MP_QSTR_I2C),         MP_ROM_PTR(&board_i2c_obj) },

    // SPI (Display ILI9341)
    { MP_ROM_QSTR(MP_QSTR_MOSI),        MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_SCK),         MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_SPI),         MP_ROM_PTR(&board_spi_obj) },

    // Display control
    { MP_ROM_QSTR(MP_QSTR_TFT_CS),      MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_TFT_DC),      MP_ROM_PTR(&pin_GPIO13) },
    // TFT_RESET is tied to 3.3V (no GPIO)

    // Camera (GC2145)
    { MP_ROM_QSTR(MP_QSTR_CAM_XCLK),    MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_CAM_SIOD),    MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_CAM_SIOC),    MP_ROM_PTR(&pin_GPIO48) },
    { MP_ROM_QSTR(MP_QSTR_CAM_D7),      MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_CAM_D6),      MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_CAM_D5),      MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_CAM_D4),      MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_CAM_D3),      MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_CAM_D2),      MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_CAM_D1),      MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_CAM_D0),      MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_CAM_VSYNC),   MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_CAM_HREF),    MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_CAM_PCLK),    MP_ROM_PTR(&pin_GPIO17) },

    // I2S Audio
    { MP_ROM_QSTR(MP_QSTR_I2S_BCLK),    MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_I2S_LRCLK),   MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_I2S_DIN),     MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_I2S_DOUT),    MP_ROM_PTR(&pin_GPIO45) },
    { MP_ROM_QSTR(MP_QSTR_I2S_MCLK),    MP_ROM_PTR(&pin_GPIO3) },

    // SD Card (SDMMC) - internal to SDMMC peripheral, no discrete GPIOs

    // =================================================================
    // GENERAL PURPOSE I/O
    // =================================================================
    { MP_ROM_QSTR(MP_QSTR_IO1),         MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_IO3),         MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_IO14),        MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_IO21),        MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_IO40),        MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_IO41),        MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_IO42),        MP_ROM_PTR(&pin_GPIO42) },

    // Display
    { MP_ROM_QSTR(MP_QSTR_DISPLAY),     MP_ROM_PTR(&displays[0].display) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
