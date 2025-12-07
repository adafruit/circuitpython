// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/objexcept.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/ipaddress/__init__.h"
#include "shared-bindings/ipaddress/IPv4Address.h"

//| """
//| The `ipaddress` module provides types for IP addresses. It is a subset of CPython's ipaddress
//| module.
//| """
//|
//|
//| def ip_address(obj: Union[int, str]) -> IPv4Address:
//|     """Return a corresponding IP address object or raise ValueError if not possible."""
//|     ...
//|
//|

static mp_obj_t ipaddress_ip_address(mp_obj_t ip_in) {
    uint32_t value;
    if (mp_obj_get_int_maybe(ip_in, (mp_int_t *)&value)) {
        // We're done.
    } else if (mp_obj_is_str(ip_in)) {
        GET_STR_DATA_LEN(ip_in, str_data, str_len);
        if (!ipaddress_parse_ipv4address((const char *)str_data, str_len, &value)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Not a valid IP string"));
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Only int or string supported for ip"));
    }

    return common_hal_ipaddress_new_ipv4address(value);
}
MP_DEFINE_CONST_FUN_OBJ_1(ipaddress_ip_address_obj, ipaddress_ip_address);

static const mp_rom_map_elem_t ipaddress_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),      MP_ROM_QSTR(MP_QSTR_ipaddress) },
    { MP_ROM_QSTR(MP_QSTR_ip_address),    MP_ROM_PTR(&ipaddress_ip_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_IPv4Address),   MP_ROM_PTR(&ipaddress_ipv4address_type) },
};

static MP_DEFINE_CONST_DICT(ipaddress_module_globals, ipaddress_module_globals_table);


const mp_obj_module_t ipaddress_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ipaddress_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ipaddress, ipaddress_module);
