// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // LCD (SPI0)
    { MP_ROM_QSTR(MP_QSTR_LCD_SCK),   MP_ROM_PTR(&pin_GPIO40) },
    { MP_ROM_QSTR(MP_QSTR_LCD_MOSI),  MP_ROM_PTR(&pin_GPIO45) },
    { MP_ROM_QSTR(MP_QSTR_LCD_MISO),  MP_ROM_PTR(&pin_GPIO46) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CS),    MP_ROM_PTR(&pin_GPIO42) },
    { MP_ROM_QSTR(MP_QSTR_LCD_DC),    MP_ROM_PTR(&pin_GPIO41) },
    { MP_ROM_QSTR(MP_QSTR_LCD_RST),   MP_ROM_PTR(&pin_GPIO39) },
    { MP_ROM_QSTR(MP_QSTR_LCD_BL),    MP_ROM_PTR(&pin_GPIO5)  },   // PWM-capable

    // microSD (SPI1)
    { MP_ROM_QSTR(MP_QSTR_SD_SCK),    MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_SD_MOSI),   MP_ROM_PTR(&pin_GPIO17) },
    { MP_ROM_QSTR(MP_QSTR_SD_MISO),   MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_SD_CS),     MP_ROM_PTR(&pin_GPIO21) },

    // Touch panel (I2C0)
    { MP_ROM_QSTR(MP_QSTR_TP_SCL),    MP_ROM_PTR(&pin_GPIO3)  },
    { MP_ROM_QSTR(MP_QSTR_TP_SDA),    MP_ROM_PTR(&pin_GPIO1)  },
    { MP_ROM_QSTR(MP_QSTR_TP_RST),    MP_ROM_PTR(&pin_GPIO2)  },
    { MP_ROM_QSTR(MP_QSTR_TP_INT),    MP_ROM_PTR(&pin_GPIO4)  },

    // IMU (I2C1)
    { MP_ROM_QSTR(MP_QSTR_IMU_SCL),  MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_IMU_SDA),  MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_IMU_INT2), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_IMU_INT1), MP_ROM_PTR(&pin_GPIO13) },

    // I2S Audio
    { MP_ROM_QSTR(MP_QSTR_I2S_BCK),   MP_ROM_PTR(&pin_GPIO48) },
    { MP_ROM_QSTR(MP_QSTR_I2S_DIN),   MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_I2S_LRCK),  MP_ROM_PTR(&pin_GPIO38) },

    // Battery management
    { MP_ROM_QSTR(MP_QSTR_BAT_CONTROL), MP_ROM_PTR(&pin_GPIO7)  }, // control pin
    { MP_ROM_QSTR(MP_QSTR_BAT_PWR),     MP_ROM_PTR(&pin_GPIO6)  }, // Board name
    { MP_ROM_QSTR(MP_QSTR_KEY_BAT),     MP_ROM_PTR(&pin_GPIO6)  }, // Schematics name
    { MP_ROM_QSTR(MP_QSTR_BAT_ADC),     MP_ROM_PTR(&pin_GPIO8)  }, // VBAT sense (ADC)

    // UART header
    { MP_ROM_QSTR(MP_QSTR_TX),        MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_RX),        MP_ROM_PTR(&pin_GPIO44) },

    // I2C header
    { MP_ROM_QSTR(MP_QSTR_I2C_SCL), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_I2C_SDA), MP_ROM_PTR(&pin_GPIO11) },

    // Boot/User button
    { MP_ROM_QSTR(MP_QSTR_BOOT),     MP_ROM_PTR(&pin_GPIO0)  },
    { MP_ROM_QSTR(MP_QSTR_BUTTON0),  MP_ROM_PTR(&pin_GPIO0)  },

    // Primary bus pins
    { MP_ROM_QSTR(MP_QSTR_SCK),       MP_ROM_PTR(&pin_GPIO40) }, // Primary SPI (LCD)
    { MP_ROM_QSTR(MP_QSTR_MOSI),      MP_ROM_PTR(&pin_GPIO45) },
    { MP_ROM_QSTR(MP_QSTR_MISO),      MP_ROM_PTR(&pin_GPIO46) },
    { MP_ROM_QSTR(MP_QSTR_SCL),       MP_ROM_PTR(&pin_GPIO3)  }, // Primary I2C (TP)
    { MP_ROM_QSTR(MP_QSTR_SDA),       MP_ROM_PTR(&pin_GPIO1)  },

    // Objects
    { MP_ROM_QSTR(MP_QSTR_I2C),     MP_ROM_PTR(&board_i2c_obj) },
    { MP_ROM_QSTR(MP_QSTR_SPI),     MP_ROM_PTR(&board_spi_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART),    MP_ROM_PTR(&board_uart_obj) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display) },

    // User accessible
    { MP_ROM_QSTR(MP_QSTR_IO10),  MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_IO11),  MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_IO15),  MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_IO18),  MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_IO19),  MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_IO20),  MP_ROM_PTR(&pin_GPIO20) },
    { MP_ROM_QSTR(MP_QSTR_IO43),  MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_IO44),  MP_ROM_PTR(&pin_GPIO44) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
