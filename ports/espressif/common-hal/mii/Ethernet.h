// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/mii/RMII.h"

#include "esp_eth.h"
#include "esp_netif.h"

typedef struct {
    mp_obj_base_t base;
    esp_eth_handle_t eth_handle;
    esp_netif_t *netif;
    esp_eth_netif_glue_handle_t glue;
    esp_netif_ip_info_t ip_info;
    esp_netif_dns_info_t dns_info;
    uint32_t ping_elapsed_time;
    bool started;
    bool connected;
} mii_ethernet_obj_t;

// Brings up the built-in RMII MAC on the given bus and the 802.3 PHY at MDIO
// address device, and starts the interface with DHCP. Raises when bring-up
// fails.
void common_hal_mii_ethernet_construct(mii_ethernet_obj_t *self,
    mii_rmii_obj_t *rmii, uint32_t device);
