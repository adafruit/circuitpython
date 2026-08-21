// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Pin map from Tim Knapen's reverse-engineering of the Teenage Engineering SP-1
// https://github.com/timknapen/SP-1-dev/wiki

#include "shared-bindings/board/__init__.h"

static const mp_rom_map_elem_t board_module_globals_table[] = {
    CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

    // Four track faders, in physical left-to-right order.
    { MP_ROM_QSTR(MP_QSTR_FADER1), MP_ROM_PTR(&pin_P0_05) },
    { MP_ROM_QSTR(MP_QSTR_FADER2), MP_ROM_PTR(&pin_P0_30) },
    { MP_ROM_QSTR(MP_QSTR_FADER3), MP_ROM_PTR(&pin_P0_04) },
    { MP_ROM_QSTR(MP_QSTR_FADER4), MP_ROM_PTR(&pin_P0_31) },

    // Two resistor-ladder button rows, read as analog voltages. Both need
    // LADDER_POWER driven high to read anything; the faders need it too.
    //
    // Rungs in ascending voltage:
    //   LADDER1: TRACK1, TRACK2, TRACK3, TRACK4, PLAY
    //   LADDER2: ROCKER-, VOL-, ROCKER+, VOL+
    // Both rows are the same resistor network; LADDER2 is LADDER1 with the
    // lowest rung unpopulated, so one threshold table serves both.
    //
    // "ROCKER" is the left-side rocker switch
    { MP_ROM_QSTR(MP_QSTR_LADDER1), MP_ROM_PTR(&pin_P0_02) },
    { MP_ROM_QSTR(MP_QSTR_LADDER2), MP_ROM_PTR(&pin_P0_03) },
    { MP_ROM_QSTR(MP_QSTR_LADDER_POWER), MP_ROM_PTR(&pin_P1_10) },

    // The Function button ("••") is the only GPIO button, active low with a
    // pull-up, and the only wake source out of SYSTEM_OFF.
    { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_P0_27) },

    // Playback LED row on the side of the device, active high.
    { MP_ROM_QSTR(MP_QSTR_LED_PLAY1), MP_ROM_PTR(&pin_P1_13) },
    { MP_ROM_QSTR(MP_QSTR_LED_PLAY2), MP_ROM_PTR(&pin_P0_00) },
    { MP_ROM_QSTR(MP_QSTR_LED_PLAY3), MP_ROM_PTR(&pin_P1_12) },
    { MP_ROM_QSTR(MP_QSTR_LED_PLAY4), MP_ROM_PTR(&pin_P0_01) },

    // Track LED row above the track buttons, active high.
    { MP_ROM_QSTR(MP_QSTR_LED_TRACK1), MP_ROM_PTR(&pin_P0_29) },
    { MP_ROM_QSTR(MP_QSTR_LED_TRACK2), MP_ROM_PTR(&pin_P0_26) },
    { MP_ROM_QSTR(MP_QSTR_LED_TRACK3), MP_ROM_PTR(&pin_P1_15) },
    { MP_ROM_QSTR(MP_QSTR_LED_TRACK4), MP_ROM_PTR(&pin_P1_14) },

    // I2C to both codecs: CS42L42 headphone amp at 0x48, TAS2505 speaker amp
    // at 0x18.
    { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_P1_11) },
    { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_P1_07) },
    { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },

    // Codec resets, both active low.
    { MP_ROM_QSTR(MP_QSTR_TAS_RESET), MP_ROM_PTR(&pin_P0_09) },
    { MP_ROM_QSTR(MP_QSTR_CS42_RESET), MP_ROM_PTR(&pin_P0_15) },

    // I2S
    { MP_ROM_QSTR(MP_QSTR_I2S_DOUT), MP_ROM_PTR(&pin_P1_09) },
    { MP_ROM_QSTR(MP_QSTR_I2S_LRCLK), MP_ROM_PTR(&pin_P0_11) },
    { MP_ROM_QSTR(MP_QSTR_I2S_WORD_SELECT), MP_OBJ_FROM_PTR(&pin_P0_11) },
    { MP_ROM_QSTR(MP_QSTR_I2S_BCLK), MP_ROM_PTR(&pin_P0_12) },
    { MP_ROM_QSTR(MP_QSTR_I2S_BIT_CLOCK), MP_OBJ_FROM_PTR(&pin_P0_12) },
    { MP_ROM_QSTR(MP_QSTR_OSC_EN), MP_ROM_PTR(&pin_P0_13) },

    // 4 GB eMMC
    { MP_ROM_QSTR(MP_QSTR_EMMC_CLK), MP_ROM_PTR(&pin_P0_06) },
    { MP_ROM_QSTR(MP_QSTR_EMMC_DAT0), MP_ROM_PTR(&pin_P0_07) },
    { MP_ROM_QSTR(MP_QSTR_EMMC_CMD), MP_ROM_PTR(&pin_P0_08) },
    { MP_ROM_QSTR(MP_QSTR_EMMC_RESET), MP_ROM_PTR(&pin_P1_08) },
    { MP_ROM_QSTR(MP_QSTR_EMMC_VCCQ), MP_ROM_PTR(&pin_P0_14) },

    // BQ24232 charger. CHARGE_ENABLE and the two status lines are active low.
    { MP_ROM_QSTR(MP_QSTR_CHARGE_ISET), MP_ROM_PTR(&pin_P1_00) },
    { MP_ROM_QSTR(MP_QSTR_CHARGE_ENABLE), MP_ROM_PTR(&pin_P0_21) },
    { MP_ROM_QSTR(MP_QSTR_CHARGE_STATUS), MP_ROM_PTR(&pin_P0_22) },
    { MP_ROM_QSTR(MP_QSTR_POWER_GOOD), MP_ROM_PTR(&pin_P0_24) },

    // Battery sense, AIN4, through a divider.
    { MP_ROM_QSTR(MP_QSTR_VBATT), MP_ROM_PTR(&pin_P0_28) },
    { MP_ROM_QSTR(MP_QSTR_BATTERY), MP_ROM_PTR(&pin_P0_28) },
    { MP_ROM_QSTR(MP_QSTR_VOLTAGE_MONITOR), MP_ROM_PTR(&pin_P0_28) },

    // CYBT-353027-02 Bluetooth module reset, active low
    { MP_ROM_QSTR(MP_QSTR_BT_RESET), MP_ROM_PTR(&pin_P0_10) },
};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
