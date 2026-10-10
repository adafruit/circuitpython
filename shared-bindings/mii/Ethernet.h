// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "common-hal/mii/Ethernet.h"

#include "py/objstr.h"

extern const mp_obj_type_t mii_ethernet_type;

extern bool common_hal_mii_ethernet_get_enabled(mii_ethernet_obj_t *self);
extern void common_hal_mii_ethernet_set_enabled(mii_ethernet_obj_t *self, bool enabled);

extern bool common_hal_mii_ethernet_get_connected(mii_ethernet_obj_t *self);

extern mp_obj_t common_hal_mii_ethernet_get_hostname(mii_ethernet_obj_t *self);
extern void common_hal_mii_ethernet_set_hostname(mii_ethernet_obj_t *self, const char *hostname);

extern mp_obj_t common_hal_mii_ethernet_get_mac_address(mii_ethernet_obj_t *self);

extern mp_int_t common_hal_mii_ethernet_get_link_speed(mii_ethernet_obj_t *self);
extern bool common_hal_mii_ethernet_get_full_duplex(mii_ethernet_obj_t *self);

extern void common_hal_mii_ethernet_start_dhcp_client(mii_ethernet_obj_t *self, bool ipv4, bool ipv6);
extern void common_hal_mii_ethernet_stop_dhcp_client(mii_ethernet_obj_t *self);

extern mp_obj_t common_hal_mii_ethernet_get_ipv4_dns(mii_ethernet_obj_t *self);
extern void common_hal_mii_ethernet_set_ipv4_dns(mii_ethernet_obj_t *self, mp_obj_t ipv4_dns_addr);
extern mp_obj_t common_hal_mii_ethernet_get_ipv4_gateway(mii_ethernet_obj_t *self);
extern mp_obj_t common_hal_mii_ethernet_get_ipv4_subnet(mii_ethernet_obj_t *self);
extern mp_obj_t common_hal_mii_mii_get_ipv4_address(mii_ethernet_obj_t *self);

extern mp_obj_t common_hal_mii_ethernet_get_addresses(mii_ethernet_obj_t *self);
extern mp_obj_t common_hal_mii_ethernet_get_dns(mii_ethernet_obj_t *self);
extern void common_hal_mii_ethernet_set_dns(mii_ethernet_obj_t *self, mp_obj_t dns_addr);

extern void common_hal_mii_ethernet_set_ipv4_address(mii_ethernet_obj_t *self, mp_obj_t ipv4, mp_obj_t netmask, mp_obj_t gateway, mp_obj_t ipv4_dns_addr);

extern mp_int_t common_hal_mii_ethernet_ping(mii_ethernet_obj_t *self, mp_obj_t ip_address, mp_float_t timeout);
