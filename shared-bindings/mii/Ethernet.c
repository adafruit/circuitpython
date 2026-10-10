// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mii/Ethernet.h"

#include <string.h>

#include "py/runtime.h"
#include "py/objproperty.h"
#include "shared-bindings/mii/RMII.h"
#include "shared-bindings/microcontroller/Pin.h"

#define MAC_ADDRESS_LENGTH 6

static bool hostname_valid(const char *ptr, size_t len) {
    int partlen = 0;
    while (len) {
        char c = *ptr++;
        len--;
        if (c == '.') {
            if (partlen == 0 || partlen > 63) {
                return false;
            }
            partlen = 0;
            continue;
        }
        partlen++;
        if (c == '-') {
            if (partlen == 1) {
                return false; // part cannot begin with a dash
            }
            continue;
        } else if (
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9')) {
            continue;
        }
        return false;
    }
    // check length of last part
    return !(partlen > 63);
}

//| class Ethernet:
//|     """Wired ethernet interface.
//|
//|     Boards with onboard ethernet provide it as ``board.ETHERNET``. On
//|     Espressif ports, further interfaces can be created by wiring an external
//|     RMII PHY and constructing `mii.RMII` from its wiring. The interface
//|     is enabled and DHCP is started automatically.
//|     """
//|
//|     def __init__(
//|         self,
//|         rmii: mii.RMII,
//|         device: int,
//|     ) -> None:
//|         """Create another ethernet interface driving the PHY wired to
//|         ``rmii``. ``device`` selects the PHY's MDIO address (0-31) so that
//|         multiple interfaces can share one bus.
//|
//|         Only Espressif ports support this; Zephyr ports only serve the
//|         interface from the devicetree, available as ``board.ETHERNET``,
//|         and raise NotImplementedError here."""
//|         ...
//|

//|     enabled: bool
//|     """``True`` when the ethernet interface is enabled.
//|     If you set the value to ``False``, any open sockets will be closed.
//|     """
static mp_obj_t mii_ethernet_get_enabled(mp_obj_t self) {
    return mp_obj_new_bool(common_hal_mii_ethernet_get_enabled(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_get_enabled_obj, mii_ethernet_get_enabled);

static mp_obj_t mii_ethernet_set_enabled(mp_obj_t self, mp_obj_t value) {
    const bool enabled = mp_obj_is_true(value);
    common_hal_mii_ethernet_set_enabled(self, enabled);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(mii_ethernet_set_enabled_obj, mii_ethernet_set_enabled);

MP_PROPERTY_GETSET(mii_ethernet_enabled_obj,
    (mp_obj_t)&mii_ethernet_get_enabled_obj,
    (mp_obj_t)&mii_ethernet_set_enabled_obj);

//|     connected: bool
//|     """``True`` when the ethernet cable is plugged in and link is up. (read-only)"""
static mp_obj_t mii_ethernet_get_connected(mp_obj_t self) {
    return mp_obj_new_bool(common_hal_mii_ethernet_get_connected(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_get_connected_obj, mii_ethernet_get_connected);

MP_PROPERTY_GETTER(mii_ethernet_connected_obj,
    (mp_obj_t)&mii_ethernet_get_connected_obj);

//|     hostname: Union[str, ReadableBuffer]
//|     """Hostname for the ethernet interface. When the hostname is altered after the interface
//|        is started the changes would only be reflected once the interface restarts."""
static mp_obj_t mii_ethernet_get_hostname(mp_obj_t self_in) {
    mii_ethernet_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_mii_ethernet_get_hostname(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_get_hostname_obj, mii_ethernet_get_hostname);

static mp_obj_t mii_ethernet_set_hostname(mp_obj_t self_in, mp_obj_t hostname_in) {
    mp_buffer_info_t hostname;
    mp_get_buffer_raise(hostname_in, &hostname, MP_BUFFER_READ);

    mp_arg_validate_length_range(hostname.len, 1, 253, MP_QSTR_hostname);

    if (!hostname_valid(hostname.buf, hostname.len)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid hostname"));
    }

    mii_ethernet_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_mii_ethernet_set_hostname(self, hostname.buf);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mii_ethernet_set_hostname_obj, mii_ethernet_set_hostname);

MP_PROPERTY_GETSET(mii_ethernet_hostname_obj,
    (mp_obj_t)&mii_ethernet_get_hostname_obj,
    (mp_obj_t)&mii_ethernet_set_hostname_obj);

//|     mac_address: bytes
//|     """MAC address of the ethernet interface. (read-only, 6 bytes)"""
static mp_obj_t mii_ethernet_get_mac_address(mp_obj_t self_in) {
    mii_ethernet_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(common_hal_mii_ethernet_get_mac_address(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_get_mac_address_obj, mii_ethernet_get_mac_address);

MP_PROPERTY_GETTER(mii_ethernet_mac_address_obj,
    (mp_obj_t)&mii_ethernet_get_mac_address_obj);

//|     link_speed: int
//|     """Link speed in Mbps (10, 100, 1000, etc.) when connected, 0 otherwise. (read-only)"""
static mp_obj_t mii_ethernet_get_link_speed(mp_obj_t self_in) {
    mii_ethernet_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_mii_ethernet_get_link_speed(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_get_link_speed_obj, mii_ethernet_get_link_speed);

MP_PROPERTY_GETTER(mii_ethernet_link_speed_obj,
    (mp_obj_t)&mii_ethernet_get_link_speed_obj);

//|     full_duplex: bool
//|     """``True`` if the link is full duplex, ``False`` if half duplex. (read-only)"""
static mp_obj_t mii_ethernet_get_full_duplex(mp_obj_t self_in) {
    mii_ethernet_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_mii_ethernet_get_full_duplex(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_get_full_duplex_obj, mii_ethernet_get_full_duplex);

MP_PROPERTY_GETTER(mii_ethernet_full_duplex_obj,
    (mp_obj_t)&mii_ethernet_get_full_duplex_obj);

//|     ipv4_address: Optional[ipaddress.IPv4Address]
//|     """IP v4 address when connected. None otherwise. (read-only)"""
static mp_obj_t mii_mii_get_ipv4_address(mp_obj_t self) {
    return common_hal_mii_mii_get_ipv4_address(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_mii_get_ipv4_address_obj, mii_mii_get_ipv4_address);

MP_PROPERTY_GETTER(mii_ethernet_ipv4_address_obj,
    (mp_obj_t)&mii_mii_get_ipv4_address_obj);

//|     ipv4_gateway: Optional[ipaddress.IPv4Address]
//|     """IP v4 gateway address when connected. None otherwise. (read-only)"""
static mp_obj_t mii_ethernet_get_ipv4_gateway(mp_obj_t self) {
    return common_hal_mii_ethernet_get_ipv4_gateway(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_get_ipv4_gateway_obj, mii_ethernet_get_ipv4_gateway);

MP_PROPERTY_GETTER(mii_ethernet_ipv4_gateway_obj,
    (mp_obj_t)&mii_ethernet_get_ipv4_gateway_obj);

//|     ipv4_subnet: Optional[ipaddress.IPv4Address]
//|     """IP v4 subnet mask when connected. None otherwise. (read-only)"""
static mp_obj_t mii_ethernet_get_ipv4_subnet(mp_obj_t self) {
    return common_hal_mii_ethernet_get_ipv4_subnet(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_get_ipv4_subnet_obj, mii_ethernet_get_ipv4_subnet);

MP_PROPERTY_GETTER(mii_ethernet_ipv4_subnet_obj,
    (mp_obj_t)&mii_ethernet_get_ipv4_subnet_obj);

//|     ipv4_dns: ipaddress.IPv4Address
//|     """IP v4 address of the DNS server to be used."""
static mp_obj_t mii_ethernet_get_ipv4_dns(mp_obj_t self) {
    return common_hal_mii_ethernet_get_ipv4_dns(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_get_ipv4_dns_obj, mii_ethernet_get_ipv4_dns);

static mp_obj_t mii_ethernet_set_ipv4_dns(mp_obj_t self, mp_obj_t ipv4_dns_addr) {
    common_hal_mii_ethernet_set_ipv4_dns(self, ipv4_dns_addr);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mii_ethernet_set_ipv4_dns_obj, mii_ethernet_set_ipv4_dns);

MP_PROPERTY_GETSET(mii_ethernet_ipv4_dns_obj,
    (mp_obj_t)&mii_ethernet_get_ipv4_dns_obj,
    (mp_obj_t)&mii_ethernet_set_ipv4_dns_obj);

//|     addresses: Sequence[str]
//|     """Address(es) of the interface when connected. Empty sequence when not connected. (read-only)"""
static mp_obj_t mii_ethernet_get_addresses(mp_obj_t self) {
    return common_hal_mii_ethernet_get_addresses(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_get_addresses_obj, mii_ethernet_get_addresses);

MP_PROPERTY_GETTER(mii_ethernet_addresses_obj,
    (mp_obj_t)&mii_ethernet_get_addresses_obj);

//|     dns: Sequence[str]
//|     """Address of the DNS server to be used."""
static mp_obj_t mii_ethernet_get_dns(mp_obj_t self) {
    return common_hal_mii_ethernet_get_dns(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_get_dns_obj, mii_ethernet_get_dns);

static mp_obj_t mii_ethernet_set_dns(mp_obj_t self, mp_obj_t dns_addr) {
    common_hal_mii_ethernet_set_dns(self, dns_addr);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mii_ethernet_set_dns_obj, mii_ethernet_set_dns);

MP_PROPERTY_GETSET(mii_ethernet_dns_obj,
    (mp_obj_t)&mii_ethernet_get_dns_obj,
    (mp_obj_t)&mii_ethernet_set_dns_obj);

//|     def set_ipv4_address(
//|         self,
//|         *,
//|         ipv4: ipaddress.IPv4Address,
//|         netmask: ipaddress.IPv4Address,
//|         gateway: ipaddress.IPv4Address,
//|         ipv4_dns: Optional[ipaddress.IPv4Address],
//|     ) -> None:
//|         """Sets the IP v4 address of the interface. Must include the netmask and gateway.
//|         DNS address is optional. Setting the address manually will stop the DHCP client."""
//|         ...
//|
static mp_obj_t mii_ethernet_set_ipv4_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ipv4, ARG_netmask, ARG_gateway, ARG_ipv4_dns };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ipv4, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, },
        { MP_QSTR_netmask, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, },
        { MP_QSTR_gateway, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, },
        { MP_QSTR_ipv4_dns, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
    };

    mii_ethernet_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_mii_ethernet_set_ipv4_address(self, args[ARG_ipv4].u_obj, args[ARG_netmask].u_obj, args[ARG_gateway].u_obj, args[ARG_ipv4_dns].u_obj);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(mii_ethernet_set_ipv4_address_obj, 1, mii_ethernet_set_ipv4_address);

//|     def start_dhcp(self, *, ipv4: bool = True, ipv6: bool = False) -> None:
//|         """Starts the DHCP client.
//|
//|         By default, calling this function starts DHCP for IPv4 networks but not
//|         IPv6 networks. When the ``ipv4`` and ``ipv6`` arguments are `False`
//|         then the corresponding DHCP client is stopped if it was active.
//|         """
//|         ...
//|
static mp_obj_t mii_ethernet_start_dhcp_client(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ipv4, ARG_ipv6 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ipv4, MP_ARG_KW_ONLY | MP_ARG_BOOL, { .u_bool = true } },
        { MP_QSTR_ipv6, MP_ARG_KW_ONLY | MP_ARG_BOOL, { .u_bool = false } },
    };

    mii_ethernet_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_mii_ethernet_start_dhcp_client(self, args[ARG_ipv4].u_bool, args[ARG_ipv6].u_bool);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(mii_ethernet_start_dhcp_client_obj, 1, mii_ethernet_start_dhcp_client);

//|     def stop_dhcp(self) -> None:
//|         """Stops the DHCP client. Needed to assign a static IP address."""
//|         ...
//|
static mp_obj_t mii_ethernet_stop_dhcp_client(mp_obj_t self) {
    common_hal_mii_ethernet_stop_dhcp_client(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mii_ethernet_stop_dhcp_client_obj, mii_ethernet_stop_dhcp_client);

//|     def ping(
//|         self, ip: ipaddress.IPv4Address, *, timeout: Optional[float] = 0.5
//|     ) -> Optional[float]:
//|         """Ping an IP to test connectivity. Returns echo time in seconds.
//|         Returns None when it times out."""
//|         ...
//|
static mp_obj_t mii_ethernet_ping(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ip, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ip, MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    mii_ethernet_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_float_t timeout = 0.5;
    if (args[ARG_timeout].u_obj != mp_const_none) {
        timeout = mp_obj_get_float(args[ARG_timeout].u_obj);
    }

    mp_int_t time_ms = common_hal_mii_ethernet_ping(self, args[ARG_ip].u_obj, timeout);
    if (time_ms == -1) {
        return mp_const_none;
    }

    return mp_obj_new_float(time_ms / 1000.0);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(mii_ethernet_ping_obj, 1, mii_ethernet_ping);

static mp_obj_t mii_ethernet_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_rmii, ARG_device };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_rmii, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_device, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_arg_validate_type(args[ARG_rmii].u_obj, &mii_rmii_type, MP_QSTR_rmii);
    mii_rmii_obj_t *rmii = MP_OBJ_TO_PTR(args[ARG_rmii].u_obj);
    (void)mp_arg_validate_int_range(args[ARG_device].u_int, 0, 31, MP_QSTR_device);

    mii_ethernet_obj_t *self = mp_obj_malloc(mii_ethernet_obj_t, &mii_ethernet_type);
    common_hal_mii_ethernet_construct(self, rmii, (uint32_t)args[ARG_device].u_int);
    return MP_OBJ_FROM_PTR(self);
}

static const mp_rom_map_elem_t mii_ethernet_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_enabled),       MP_ROM_PTR(&mii_ethernet_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_connected),     MP_ROM_PTR(&mii_ethernet_connected_obj) },

    { MP_ROM_QSTR(MP_QSTR_hostname),      MP_ROM_PTR(&mii_ethernet_hostname_obj) },
    { MP_ROM_QSTR(MP_QSTR_mac_address),   MP_ROM_PTR(&mii_ethernet_mac_address_obj) },

    { MP_ROM_QSTR(MP_QSTR_link_speed),    MP_ROM_PTR(&mii_ethernet_link_speed_obj) },
    { MP_ROM_QSTR(MP_QSTR_full_duplex),   MP_ROM_PTR(&mii_ethernet_full_duplex_obj) },

    { MP_ROM_QSTR(MP_QSTR_start_dhcp),    MP_ROM_PTR(&mii_ethernet_start_dhcp_client_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_dhcp),     MP_ROM_PTR(&mii_ethernet_stop_dhcp_client_obj) },

    { MP_ROM_QSTR(MP_QSTR_ipv4_dns),      MP_ROM_PTR(&mii_ethernet_ipv4_dns_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipv4_gateway),  MP_ROM_PTR(&mii_ethernet_ipv4_gateway_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipv4_subnet),   MP_ROM_PTR(&mii_ethernet_ipv4_subnet_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipv4_address),  MP_ROM_PTR(&mii_ethernet_ipv4_address_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_ipv4_address), MP_ROM_PTR(&mii_ethernet_set_ipv4_address_obj) },

    { MP_ROM_QSTR(MP_QSTR_addresses),     MP_ROM_PTR(&mii_ethernet_addresses_obj) },
    { MP_ROM_QSTR(MP_QSTR_dns),           MP_ROM_PTR(&mii_ethernet_dns_obj) },

    { MP_ROM_QSTR(MP_QSTR_ping),          MP_ROM_PTR(&mii_ethernet_ping_obj) },
};

static MP_DEFINE_CONST_DICT(mii_ethernet_locals_dict, mii_ethernet_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mii_ethernet_type,
    MP_QSTR_Ethernet,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, &mii_ethernet_make_new,
    locals_dict, &mii_ethernet_locals_dict
    );
