// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

CIRCUITPY_BOARD_BUS_SINGLETON(epd_spi, spi, 0)
CIRCUITPY_BOARD_BUS_SINGLETON(lora_spi, spi, 1)

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // User buttons
    { MP_ROM_QSTR(MP_QSTR_BUTTON0), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_BUTTON1), MP_ROM_PTR(&pin_GPIO21) },

    // LED
    { MP_ROM_QSTR(MP_QSTR_LED0), MP_ROM_PTR(&pin_GPIO45) },

    // ADC (When ADC_CTRL is high, VBAT is connected to ADC_IN with 390K/100K voltage divider)
    { MP_ROM_QSTR(MP_QSTR_ADC_CTRL), MP_ROM_PTR(&pin_GPIO46) },
    { MP_ROM_QSTR(MP_QSTR_ADC_IN), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_BATTERY), MP_ROM_PTR(&pin_GPIO7) },

    // VEXT powers epd and lora antenna boost when high
    { MP_ROM_QSTR(MP_QSTR_VEXT_CTRL), MP_ROM_PTR(&pin_GPIO18) },

    // LORA SPI, Reset, and More
    { MP_ROM_QSTR(MP_QSTR_LORA_CS), MP_ROM_PTR(&pin_GPIO8) },
    { MP_ROM_QSTR(MP_QSTR_LORA_SCK), MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_LORA_MOSI), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_LORA_MISO), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_LORA_RESET), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_LORA_BUSY), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_LORA_DIO1), MP_ROM_PTR(&pin_GPIO14) },

    // eInk Display
    { MP_ROM_QSTR(MP_QSTR_EPD_MOSI), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_EPD_SCK), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_EPD_CS), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_EPD_DC), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_EPD_RESET), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_EPD_BUSY), MP_ROM_PTR(&pin_GPIO6) },

    // I2C on external connector P2
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO39) },

    // UART
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO44) },

    // User available pins on GPIO header
    { MP_ROM_QSTR(MP_QSTR_IO0), MP_ROM_PTR(&pin_GPIO0) },       // BOOT pin (IO0) is on header
    { MP_ROM_QSTR(MP_QSTR_IO17), MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_IO38), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_IO39), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_IO40), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_IO41), MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_IO42), MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_IO45), MP_ROM_PTR(&pin_GPIO45) },
    { MP_ROM_QSTR(MP_QSTR_IO47), MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_IO48), MP_ROM_PTR(&pin_GPIO48) },

    // objects
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_EPD_SPI), MP_ROM_PTR(&board_epd_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_LORA_SPI), MP_ROM_PTR(&board_lora_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].epaper_display)},

};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
