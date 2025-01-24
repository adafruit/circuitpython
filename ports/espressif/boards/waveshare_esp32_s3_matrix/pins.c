// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 David Sullivan
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Top side of the board - left column
    // (top to bottom, preceded by 5V, GND & 3.3V)
    { MP_ROM_QSTR(MP_QSTR_IO7), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_A7), MP_ROM_PTR(&pin_GPIO7) },
    { MP_ROM_QSTR(MP_QSTR_D7), MP_ROM_PTR(&pin_GPIO7) },

    { MP_ROM_QSTR(MP_QSTR_IO6), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_A6), MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_D6), MP_ROM_PTR(&pin_GPIO6) },

    { MP_ROM_QSTR(MP_QSTR_IO5), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_A5), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_D5), MP_ROM_PTR(&pin_GPIO5) },

    { MP_ROM_QSTR(MP_QSTR_IO4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_A4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_D4), MP_ROM_PTR(&pin_GPIO4) },

    { MP_ROM_QSTR(MP_QSTR_IO3), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_A3), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_D3), MP_ROM_PTR(&pin_GPIO3) },

    { MP_ROM_QSTR(MP_QSTR_IO2), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_A2), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_D2), MP_ROM_PTR(&pin_GPIO2) },

    { MP_ROM_QSTR(MP_QSTR_I01), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_A1), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_D1), MP_ROM_PTR(&pin_GPIO1) },

    // Top side of the board - right column (bottom to top)
    { MP_ROM_QSTR(MP_QSTR_IO33), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_D33), MP_ROM_PTR(&pin_GPIO33) },

    { MP_ROM_QSTR(MP_QSTR_IO34), MP_ROM_PTR(&pin_GPIO34) },
    { MP_ROM_QSTR(MP_QSTR_D34), MP_ROM_PTR(&pin_GPIO34) },

    { MP_ROM_QSTR(MP_QSTR_IO35), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_D35), MP_ROM_PTR(&pin_GPIO35) },

    { MP_ROM_QSTR(MP_QSTR_IO36), MP_ROM_PTR(&pin_GPIO36) },
    { MP_ROM_QSTR(MP_QSTR_D36), MP_ROM_PTR(&pin_GPIO36) },

    { MP_ROM_QSTR(MP_QSTR_IO37), MP_ROM_PTR(&pin_GPIO37) },
    { MP_ROM_QSTR(MP_QSTR_D37), MP_ROM_PTR(&pin_GPIO37) },

    { MP_ROM_QSTR(MP_QSTR_IO38), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_D38), MP_ROM_PTR(&pin_GPIO38) },

    { MP_ROM_QSTR(MP_QSTR_IO39), MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_D39), MP_ROM_PTR(&pin_GPIO39) },

    { MP_ROM_QSTR(MP_QSTR_IO40), MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_D40), MP_ROM_PTR(&pin_GPIO40) },

    // Numbering breaks up for TX/RX but it's still the top side, right column
    { MP_ROM_QSTR(MP_QSTR_IO43), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_D43), MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO43) },

    { MP_ROM_QSTR(MP_QSTR_IO44), MP_ROM_PTR(&pin_GPIO44) },
    { MP_ROM_QSTR(MP_QSTR_D44), MP_ROM_PTR(&pin_GPIO44) },
    { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO44) },

    // Neopixel
    { MP_ROM_QSTR(MP_QSTR_IO14), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_D14), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_NEOPIXEL), MP_ROM_PTR(&pin_GPIO14) },

    // QMI8658C IMU
    { MP_ROM_QSTR(MP_QSTR_IMU_SDA), MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_IMU_SCL), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_IMU_INT1), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_IMU_INT2), MP_ROM_PTR(&pin_GPIO13) },

    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) }
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
