// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/mii/RMII.h"

#include <zephyr/net/net_if.h>

typedef struct {
    mp_obj_base_t base;
    struct net_if *netif;
    bool started;
} mii_ethernet_obj_t;

// Brings up the devicetree ethernet interface, enables DHCP and starts it.
// Constructing with an RMII bus is not supported (the wiring comes from the
// devicetree) and raises NotImplementedError; the board's generated code
// passes NULL/0.
void common_hal_mii_ethernet_construct(mii_ethernet_obj_t *self,
    mii_rmii_obj_t *rmii, uint32_t device);
