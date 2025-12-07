// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/ipaddress/IPv4Address.h"

extern const mp_obj_type_t ipaddress_ipv4address_type;

uint32_t ipv4_bytes_to_uint32(const uint8_t bytes[4]);
void ipv4_uint32_to_bytes(uint32_t ipv4, uint8_t bytes[4]);
mp_obj_t ipv4_bytes_to_str(const uint8_t ipv4[4]);
void ipaddress_ipv4address_to_bytes(ipaddress_ipv4address_obj_t *ipv4_address, uint8_t ipv4_bytes[4]);
bool ipaddress_parse_ipv4address(const char *ip_str, size_t len, uint32_t *ip_out);

void common_hal_ipaddress_ipv4address_construct(ipaddress_ipv4address_obj_t *self, uint8_t *buf, size_t len);
mp_obj_t common_hal_ipaddress_ipv4address_get_packed(ipaddress_ipv4address_obj_t *self);
mp_obj_t common_hal_ipaddress_new_ipv4address(uint32_t value);
mp_obj_t common_hal_ipaddress_new_ipv4address_from_bytes(uint8_t bytes[4]);
