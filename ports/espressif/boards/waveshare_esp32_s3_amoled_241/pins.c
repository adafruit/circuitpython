// SPDX-FileCopyrightText: Copyright (c) 2026 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // =================================================================
    // ONBOARD PERIPHERALS - Functional Names
    // =================================================================

    // Boot/Control/Battery/Display Power
    // NOTE: GPIO16 is shared between battery control circuitry and LCD power
    // (see CIRCUITPY_QSPIBUS_PANEL_POWER_PIN in mpconfigboard.h).
    { MP_ROM_QSTR(MP_QSTR_BOOT),        MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_KEY_BAT),     MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_BAT_CONTROL), MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_LCD_POWER),   MP_ROM_PTR(&pin_GPIO16) },
    { MP_ROM_QSTR(MP_QSTR_BAT_ADC),     MP_ROM_PTR(&pin_GPIO17) },

    // I2C Bus (shared by Touch, RTC, IMU, IO Expander)
    // NOTE: board.I2C auto-initialization is disabled (CIRCUITPY_BOARD_I2C=0)
    // to avoid boot conflicts. Users must manually create I2C bus:
    //   i2c = busio.I2C(board.SCL, board.SDA)
    { MP_ROM_QSTR(MP_QSTR_SDA),     MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_SCL),     MP_ROM_PTR(&pin_GPIO48) },

    // Touch Panel (FT6336U on I2C)
    { MP_ROM_QSTR(MP_QSTR_TP_SDA),    MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_TP_SCL),    MP_ROM_PTR(&pin_GPIO48) },
    { MP_ROM_QSTR(MP_QSTR_TP_RESET),  MP_ROM_PTR(&pin_GPIO3) },

    // RTC (PCF85063 on I2C)
    { MP_ROM_QSTR(MP_QSTR_RTC_SDA), MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_RTC_SCL), MP_ROM_PTR(&pin_GPIO48) },

    // IMU (QMI8658 on I2C)
    { MP_ROM_QSTR(MP_QSTR_IMU_SDA), MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_IMU_SCL), MP_ROM_PTR(&pin_GPIO48) },

    // I/O Expander (TCA9554 on I2C)
    { MP_ROM_QSTR(MP_QSTR_EXIO_SDA), MP_ROM_PTR(&pin_GPIO47) },
    { MP_ROM_QSTR(MP_QSTR_EXIO_SCL), MP_ROM_PTR(&pin_GPIO48) },

    // USB
    { MP_ROM_QSTR(MP_QSTR_USB_D_N), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_USB_D_P), MP_ROM_PTR(&pin_GPIO20) },

    // UART
    { MP_ROM_QSTR(MP_QSTR_TX),  MP_ROM_PTR(&pin_GPIO43) },
    { MP_ROM_QSTR(MP_QSTR_RX),  MP_ROM_PTR(&pin_GPIO44) },

    // QSPI Display (RM690B0) - canonical generic LCD aliases.
    { MP_ROM_QSTR(MP_QSTR_LCD_CS),    MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_LCD_CLK),   MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_LCD_D0),    MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_LCD_D1),    MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_LCD_D2),    MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_LCD_D3),    MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_LCD_RESET), MP_ROM_PTR(&pin_GPIO21) },

    // Display Aliases
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_CS),  MP_ROM_PTR(&pin_GPIO9) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_SCK), MP_ROM_PTR(&pin_GPIO10) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_D0),  MP_ROM_PTR(&pin_GPIO11) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_D1),  MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_D2),  MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_D3),  MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_DISPLAY_RST), MP_ROM_PTR(&pin_GPIO21) },

    // SD Card (SDIO / SDMMC)
    { MP_ROM_QSTR(MP_QSTR_SDIO_CLK), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_SDIO_CMD), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_SDIO_D0),  MP_ROM_PTR(&pin_GPIO6) },
    { MP_ROM_QSTR(MP_QSTR_SDIO_D3),  MP_ROM_PTR(&pin_GPIO2) },

    // =================================================================
    // GENERAL PURPOSE I/O (IOxx - Espressif Convention)
    // =================================================================
    // Only pins NOT dedicated to onboard peripherals are exposed here.
    // Use functional names above for dedicated pins (e.g., SDA, SD_CS).

    { MP_ROM_QSTR(MP_QSTR_IO0),   MP_ROM_PTR(&pin_GPIO0) },   // BOOT button (available when not holding BOOT)
    { MP_ROM_QSTR(MP_QSTR_IO1),   MP_ROM_PTR(&pin_GPIO1) },   // Available
    { MP_ROM_QSTR(MP_QSTR_IO3),   MP_ROM_PTR(&pin_GPIO3) },   // TP_RESET (available if touch not used)
    { MP_ROM_QSTR(MP_QSTR_IO7),   MP_ROM_PTR(&pin_GPIO7) },   // Available
    { MP_ROM_QSTR(MP_QSTR_IO8),   MP_ROM_PTR(&pin_GPIO8) },   // Available
    { MP_ROM_QSTR(MP_QSTR_IO18),  MP_ROM_PTR(&pin_GPIO18) },  // Available
    { MP_ROM_QSTR(MP_QSTR_IO40),  MP_ROM_PTR(&pin_GPIO40) },  // Available
    { MP_ROM_QSTR(MP_QSTR_IO41),  MP_ROM_PTR(&pin_GPIO41) },  // Available
    { MP_ROM_QSTR(MP_QSTR_IO42),  MP_ROM_PTR(&pin_GPIO42) },  // Available
    { MP_ROM_QSTR(MP_QSTR_IO45),  MP_ROM_PTR(&pin_GPIO45) },  // Available
    { MP_ROM_QSTR(MP_QSTR_IO46),  MP_ROM_PTR(&pin_GPIO46) },  // Available
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
