// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Hassan Naeem
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "shared-bindings/board/__init__.h"

#include "board.h"
#include "shared-module/displayio/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // BOOT button. PWR is read through the AXP2101 PMU.
    { MP_ROM_QSTR(MP_QSTR_BOOT),   MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_GPIO0) },

    // I2C: CST9217 touch (0x5A), AXP2101 PMU (0x34), ES8311 (0x18),
    // ES7210 (0x40), QMI8658 IMU (0x6B), PCF85063 RTC (0x51)
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

    // Touch
    { MP_ROM_QSTR(MP_QSTR_TP_INT),   MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_TP_RESET), MP_ROM_PTR(&pin_GPIO40) },

    // IMU
    { MP_ROM_QSTR(MP_QSTR_IMU_INT2), MP_ROM_PTR(&pin_GPIO21) },

    // Audio: I2S to the ES8311 (speaker) and from the ES7210 (microphones)
    { MP_ROM_QSTR(MP_QSTR_I2S_MCLK), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_I2S_BCLK), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_I2S_WS),   MP_ROM_PTR(&pin_GPIO45) },
    { MP_ROM_QSTR(MP_QSTR_I2S_DOUT), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_I2S_DIN),  MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_PA_CTRL),  MP_ROM_PTR(&pin_GPIO46) },

    // QSPI display (CO5300)
    { MP_ROM_QSTR(MP_QSTR_LCD_CS),    MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CLK),   MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_LCD_D0),    MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_LCD_D1),    MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_LCD_D2),    MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_LCD_D3),    MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_LCD_RESET), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_LCD_TE),    MP_ROM_PTR(&pin_GPIO13) },

    // UART
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO44) },

    // microSD: SDMMC (CLK, CMD, D0), or SPI with SD_CS
    { MP_ROM_QSTR(MP_QSTR_SDIO_CLK), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_SDIO_CMD), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_SDIO_D0),  MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_SD_SCK),   MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_SD_MOSI),  MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_SD_MISO),  MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_SD_CS),    MP_ROM_PTR(&pin_GPIO41) },

    // Header pins, marked for an optional GPS module
    { MP_ROM_QSTR(MP_QSTR_IO17), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_IO18), MP_ROM_PTR(&pin_GPIO18) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&board_display) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
