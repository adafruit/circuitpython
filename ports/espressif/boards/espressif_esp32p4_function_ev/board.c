// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#if CIRCUITPY_MII
#include "common-hal/mii/__init__.h"
#include "shared-bindings/mii/Ethernet.h"
#include "shared-bindings/mii/RMII.h"
#include "shared-bindings/microcontroller/Pin.h"

// The board owns its ethernet instance and the RMII bus wiring.
mii_rmii_obj_t board_rmii_obj;
mii_ethernet_obj_t board_ethernet_obj;

void board_init(void) {
    common_hal_mii_rmii_construct(&board_rmii_obj,
        &pin_GPIO31, &pin_GPIO52,              // MDC, MDIO
        &pin_GPIO29, &pin_GPIO30, &pin_GPIO28, // RXD0, RXD1, RX_DV
        &pin_GPIO34, &pin_GPIO35, &pin_GPIO49, // TXD0, TXD1, TX_EN
        &pin_GPIO50,                           // REFCLK
        &pin_GPIO51);                          // PHY reset (active low)
    common_hal_mii_ethernet_construct(&board_ethernet_obj, &board_rmii_obj, 1);
    mii_set_default_instance(&board_ethernet_obj);
}
#endif

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
