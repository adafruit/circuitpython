// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

#include "shared-module/displayio/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // M5-Bus and general GPIO
    { MP_ROM_QSTR(MP_QSTR_G2), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_G3), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_G4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_G6), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_G7), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_G16), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_G17), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_G35), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_G45), MP_ROM_PTR(&pin_GPIO45) },
    { MP_ROM_QSTR(MP_QSTR_G47), MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_G48), MP_ROM_PTR(&pin_GPIO48) },
    { MP_ROM_QSTR(MP_QSTR_G51), MP_ROM_PTR(&pin_GPIO51) },
    { MP_ROM_QSTR(MP_QSTR_G52), MP_ROM_PTR(&pin_GPIO52) },

    // SPI (on M5-Bus)
    { MP_ROM_QSTR(MP_QSTR_G5), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_SCK), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_G18), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_MOSI), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_G19), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_MISO), MP_ROM_PTR(&pin_GPIO19) },

    // UART (on M5-Bus)
    { MP_ROM_QSTR(MP_QSTR_G37), MP_ROM_PTR(&pin_GPIO37) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO37) },
    { MP_ROM_QSTR(MP_QSTR_TXD0), MP_ROM_PTR(&pin_GPIO37) },
    { MP_ROM_QSTR(MP_QSTR_G38), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_RXD0), MP_ROM_PTR(&pin_GPIO38) },

    // I2C (Touch, IMU, RTC, Power Monitor, GPIO Expanders)
    { MP_ROM_QSTR(MP_QSTR_G31), MP_ROM_PTR(&pin_GPIO31) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO31) },
    { MP_ROM_QSTR(MP_QSTR_G32), MP_ROM_PTR(&pin_GPIO32) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO32) },

    // Touch Panel
    { MP_ROM_QSTR(MP_QSTR_G23), MP_ROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_TP_INT), MP_ROM_PTR(&pin_GPIO23) },

    // LCD
    { MP_ROM_QSTR(MP_QSTR_G22), MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_LCD_BL), MP_ROM_PTR(&pin_GPIO22) },

    // Audio ES8388/ES7210
    { MP_ROM_QSTR(MP_QSTR_G26), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_I2S_DSDIN), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_G27), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_I2S_SCLK), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_G28), MP_ROM_PTR(&pin_GPIO28) },
    { MP_ROM_QSTR(MP_QSTR_I2S_ASDOUT), MP_ROM_PTR(&pin_GPIO28) },
    { MP_ROM_QSTR(MP_QSTR_G29), MP_ROM_PTR(&pin_GPIO29) },
    { MP_ROM_QSTR(MP_QSTR_I2S_LRCK), MP_ROM_PTR(&pin_GPIO29) },
    { MP_ROM_QSTR(MP_QSTR_G30), MP_ROM_PTR(&pin_GPIO30) },
    { MP_ROM_QSTR(MP_QSTR_I2S_MCLK), MP_ROM_PTR(&pin_GPIO30) },

    // Camera
    { MP_ROM_QSTR(MP_QSTR_G36), MP_ROM_PTR(&pin_GPIO36) },
    { MP_ROM_QSTR(MP_QSTR_CAM_MCLK), MP_ROM_PTR(&pin_GPIO36) },

    // microSD Card (SDIO mode)
    { MP_ROM_QSTR(MP_QSTR_G39), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_SD_DAT0), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_SD_MISO), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_G40), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_SD_DAT1), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_G41), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_SD_DAT2), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_G42), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_SD_DAT3), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_SD_CS), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_G43), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_SD_CLK), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_SD_SCK), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_G44), MP_ROM_PTR(&pin_GPIO44) },
    { MP_ROM_QSTR(MP_QSTR_SD_CMD), MP_ROM_PTR(&pin_GPIO44) },
    { MP_ROM_QSTR(MP_QSTR_SD_MOSI), MP_ROM_PTR(&pin_GPIO44) },

    // RS485
    { MP_ROM_QSTR(MP_QSTR_G20), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_RS485_TX), MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_G21), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_RS485_RX), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_G34), MP_ROM_PTR(&pin_GPIO34) },
    { MP_ROM_QSTR(MP_QSTR_RS485_DIR), MP_ROM_PTR(&pin_GPIO34) },

    // HY2.0-4P PORT.A
    { MP_ROM_QSTR(MP_QSTR_G53), MP_ROM_PTR(&pin_GPIO53) },
    { MP_ROM_QSTR(MP_QSTR_PORTA_Y), MP_ROM_PTR(&pin_GPIO53) },
    { MP_ROM_QSTR(MP_QSTR_G54), MP_ROM_PTR(&pin_GPIO54) },
    { MP_ROM_QSTR(MP_QSTR_PORTA_W), MP_ROM_PTR(&pin_GPIO54) },

    // ESP32-C6 SDIO interface
    { MP_ROM_QSTR(MP_QSTR_G8), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_C6_SDIO2_D3), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_G9), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_C6_SDIO2_D2), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_G10), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_C6_SDIO2_D1), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_G11), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_C6_SDIO2_D0), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_G12), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_C6_SDIO2_CK), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_G13), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_C6_SDIO2_CMD), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_G14), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_C6_IO2), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_G15), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_C6_RESET), MP_ROM_PTR(&pin_GPIO15) },

    // I2C and SPI objects
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },

    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display)}
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
