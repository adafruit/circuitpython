// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/mii/MDIO.h"
#include "common-hal/microcontroller/Pin.h"

#include "esp_eth.h"


typedef struct {
    mp_obj_base_t base;
    // Shared management bus, owned by the mii_mdio_obj_t.
    mii_mdio_obj_t *mdio;
    // MDIO address (0-31) of the PHY hanging off the shared bus.
    uint32_t device;
    uint8_t rxd0;
    uint8_t rxd1;
    uint8_t rx_dv;
    uint8_t txd0;
    uint8_t txd1;
    uint8_t tx_en;
    uint8_t refclk;
    // -1 when the PHY has no reset wiring.
    int8_t reset;
    // SMI master of the interface built on this bus, set by the ethernet
    // construct. One EMAC instance per bus on these chips, so a non-NULL
    // master means the bus can't serve another interface.
    esp_eth_mac_t *mac;
} mii_rmii_obj_t;

// Claims the signals and marks them never-reset so VM teardowns leave the
// RMII wiring alone. Raises NotImplementedError on chips whose EMAC cannot
// route RMII signals via the GPIO matrix.
void common_hal_mii_rmii_construct(mii_rmii_obj_t *self,
    mii_mdio_obj_t *mdio, uint32_t device,
    const mcu_pin_obj_t *rxd0, const mcu_pin_obj_t *rxd1, const mcu_pin_obj_t *rx_dv,
    const mcu_pin_obj_t *txd0, const mcu_pin_obj_t *txd1, const mcu_pin_obj_t *tx_en,
    const mcu_pin_obj_t *refclk, const mcu_pin_obj_t *reset);
