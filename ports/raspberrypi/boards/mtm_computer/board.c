// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "shared-bindings/mcp4822/MCP4822.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "peripherals/pins.h"
#include "py/runtime.h"

// board.DAC() — factory function that constructs an mcp4822.MCP4822 with
// the MTM Workshop Computer's DAC pins (GP18=SCK, GP19=SDI, GP21=CS).

static mp_obj_t board_dac_singleton = MP_OBJ_NULL;

static mp_obj_t board_dac_factory(void) {
    if (board_dac_singleton == MP_OBJ_NULL) {
        mcp4822_mcp4822_obj_t *dac = mp_obj_malloc_with_finaliser(
            mcp4822_mcp4822_obj_t, &mcp4822_mcp4822_type);
        common_hal_mcp4822_mcp4822_construct(
            dac,
            &pin_GPIO18,   // clock (SCK)
            &pin_GPIO19,   // mosi (SDI)
            &pin_GPIO21,   // cs
            1);            // gain 1x
        board_dac_singleton = MP_OBJ_FROM_PTR(dac);
    }
    return board_dac_singleton;
}
MP_DEFINE_CONST_FUN_OBJ_0(board_dac_obj, board_dac_factory);

void board_deinit(void) {
    if (board_dac_singleton != MP_OBJ_NULL) {
        common_hal_mcp4822_mcp4822_deinit(MP_OBJ_TO_PTR(board_dac_singleton));
        board_dac_singleton = MP_OBJ_NULL;
    }
}
// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
