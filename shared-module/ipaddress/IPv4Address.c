// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/nlr.h"
#include "py/obj.h"

#include "py/parsenum.h"
#include "shared-bindings/ipaddress/__init__.h"
#include "shared-bindings/ipaddress/IPv4Address.h"


uint32_t ipv4_bytes_to_uint32(const uint8_t bytes[4]) {
    return bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;
}

void ipv4_uint32_to_bytes(uint32_t ipv4, uint8_t bytes[4]) {
    bytes[0] = ipv4;
    bytes[1] = ipv4 >> 8;
    bytes[2] = ipv4 >> 16;
    bytes[3] = ipv4 >> 24;
}

mp_obj_t ipv4_bytes_to_str(const uint8_t ipv4[4]) {
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 16, &print);
    mp_printf(&print, "%d.%d.%d.%d", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
    return mp_obj_new_str_from_vstr(&vstr);
}

void common_hal_ipaddress_ipv4address_construct(ipaddress_ipv4address_obj_t *self, uint8_t *buf, size_t len) {
    self->ip_bytes = mp_obj_new_bytes(buf, len);
}

mp_obj_t common_hal_ipaddress_ipv4address_get_packed(ipaddress_ipv4address_obj_t *self) {
    return self->ip_bytes;
}

mp_obj_t common_hal_ipaddress_new_ipv4address(uint32_t value) {
    ipaddress_ipv4address_obj_t *self = mp_obj_malloc(ipaddress_ipv4address_obj_t, &ipaddress_ipv4address_type);
    common_hal_ipaddress_ipv4address_construct(self, (uint8_t *)&value, 4);
    return MP_OBJ_FROM_PTR(self);
}

mp_obj_t common_hal_ipaddress_new_ipv4address_from_bytes(uint8_t bytes[4]) {
    return common_hal_ipaddress_new_ipv4address(ipv4_bytes_to_uint32(bytes));
}

void ipaddress_ipv4address_to_bytes(ipaddress_ipv4address_obj_t *ipv4_address, uint8_t ipv4_bytes[4]) {
    mp_obj_t packed = common_hal_ipaddress_ipv4address_get_packed(ipv4_address);
    size_t len;
    const char *bytes = mp_obj_str_get_data(packed, &len);
    memcpy(ipv4_bytes, bytes, 4);
}

bool ipaddress_parse_ipv4address(const char *str_data, size_t str_len, uint32_t *ip_out) {
    size_t period_count = 0;
    size_t period_index[4] = {0, 0, 0, str_len};
    for (size_t i = 0; i < str_len; i++) {
        if (str_data[i] == '.') {
            if (period_count < 3) {
                period_index[period_count] = i;
            }
            period_count++;
        }
    }
    if (period_count > 3) {
        return false;
    }

    size_t last_period = 0;
    if (ip_out != NULL) {
        *ip_out = 0;
    }
    for (size_t i = 0; i < 4; i++) {
        // Catch exceptions thrown by mp_parse_num_integer
        nlr_buf_t nlr;
        mp_obj_t octet;
        if (nlr_push(&nlr) == 0) {
            octet = mp_parse_num_integer((const char *)str_data + last_period, period_index[i] - last_period, 10, NULL);
            nlr_pop();
        } else {
            return false;
        }
        last_period = period_index[i] + 1;
        if (ip_out != NULL) {
            mp_int_t int_octet = MP_OBJ_SMALL_INT_VALUE(octet);
            *ip_out |= int_octet << (i * 8);
        }
    }
    return true;
}
