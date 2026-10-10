// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mii/__init__.h"
#include "shared-bindings/mii/Ethernet.h"

#include <string.h>

#include "bindings/zephyr_kernel/__init__.h"

#include "supervisor/port.h"
#include "supervisor/workflow.h"
#include "shared/runtime/interrupt_char.h"

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
#endif

#include "py/runtime.h"
#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-module/ipaddress/__init__.h"

#include <zephyr/kernel.h>
#include <zephyr/net/dhcpv4.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/net/ethernet_mgmt.h>
#include <zephyr/net/hostname.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/phy.h>

#include "common-hal/mii/__init__.h"
#include "common-hal/mii/Ethernet.h"

#define MAC_ADDRESS_LENGTH 6

static struct net_mgmt_event_callback eth_carrier_cb;
static struct net_mgmt_event_callback eth_ipv4_cb;

bool common_hal_mii_ethernet_get_enabled(mii_ethernet_obj_t *self) {
    return self->started;
}

void common_hal_mii_ethernet_set_enabled(mii_ethernet_obj_t *self, bool enabled) {
    if (self->netif == NULL) {
        return;
    }
    if (self->started && !enabled) {
        net_dhcpv4_stop(self->netif);
        CHECK_ZEPHYR_RESULT(net_if_down(self->netif));
        self->started = false;
    } else if (!self->started && enabled) {
        int up_res = net_if_up(self->netif);
        // -EALREADY means the interface is already up. That's not an error.
        if (up_res < 0 && up_res != -EALREADY) {
            raise_zephyr_error(up_res);
        }
        self->started = true;
        net_dhcpv4_start(self->netif);
        // An explicit enable implies imminent use; wait out the lease so the
        // very first getaddrinfo()/connect() does not race it.
        mii_wait_for_lease();
    }
}

bool common_hal_mii_ethernet_get_connected(mii_ethernet_obj_t *self) {
    if (self->netif == NULL) {
        return false;
    }
    return net_if_is_carrier_ok(self->netif) && net_if_is_up(self->netif);
}

mp_obj_t common_hal_mii_ethernet_get_hostname(mii_ethernet_obj_t *self) {
    const char *hostname = net_hostname_get();
    return mp_obj_new_str(hostname, strlen(hostname));
}

void common_hal_mii_ethernet_set_hostname(mii_ethernet_obj_t *self, const char *hostname) {
    CHECK_ZEPHYR_RESULT(net_hostname_set((char *)hostname, strlen(hostname)));
}

mp_obj_t common_hal_mii_ethernet_get_mac_address(mii_ethernet_obj_t *self) {
    if (self->netif == NULL) {
        return mp_obj_new_bytes((const uint8_t *)"\0\0\0\0\0\0", MAC_ADDRESS_LENGTH);
    }
    struct net_linkaddr *mac = net_if_get_link_addr(self->netif);
    return mp_obj_new_bytes(mac->addr, MAC_ADDRESS_LENGTH);
}

mp_int_t common_hal_mii_ethernet_get_link_speed(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !net_if_is_carrier_ok(self->netif)) {
        return 0;
    }
    const struct device *phy = net_eth_get_phy(self->netif);
    if (phy == NULL) {
        return 0;
    }
    struct phy_link_state state;
    if (phy_get_link_state(phy, &state) != 0) {
        return 0;
    }
    switch (state.speed) {
        case LINK_HALF_10BASE:
        case LINK_FULL_10BASE:
            return 10;
        case LINK_HALF_100BASE:
        case LINK_FULL_100BASE:
            return 100;
        case LINK_HALF_1000BASE:
        case LINK_FULL_1000BASE:
            return 1000;
        default:
            return 0;
    }
}

bool common_hal_mii_ethernet_get_full_duplex(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !net_if_is_carrier_ok(self->netif)) {
        return false;
    }
    const struct device *phy = net_eth_get_phy(self->netif);
    if (phy == NULL) {
        return false;
    }
    struct phy_link_state state;
    if (phy_get_link_state(phy, &state) != 0) {
        return false;
    }
    switch (state.speed) {
        case LINK_FULL_10BASE:
        case LINK_FULL_100BASE:
        case LINK_FULL_1000BASE:
            return true;
        default:
            return false;
    }
}

void common_hal_mii_ethernet_start_dhcp_client(mii_ethernet_obj_t *self, bool ipv4, bool ipv6) {
    if (self->netif == NULL) {
        return;
    }
    if (ipv4) {
        net_dhcpv4_start(self->netif);
    } else {
        net_dhcpv4_stop(self->netif);
    }
    if (ipv6) {
        // TODO: DHCPv6 support
    }
}

void common_hal_mii_ethernet_stop_dhcp_client(mii_ethernet_obj_t *self) {
    if (self->netif == NULL) {
        return;
    }
    net_dhcpv4_stop(self->netif);
}

mp_obj_t common_hal_mii_ethernet_get_ipv4_gateway(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !net_if_is_up(self->netif)) {
        return mp_const_none;
    }
    struct net_if_config *config = net_if_get_config(self->netif);
    if (config == NULL || config->ip.ipv4 == NULL) {
        return mp_const_none;
    }
    return common_hal_ipaddress_new_ipv4address(config->ip.ipv4->gw.s_addr);
}

mp_obj_t common_hal_mii_ethernet_get_ipv4_subnet(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !net_if_is_up(self->netif)) {
        return mp_const_none;
    }
    struct net_if_config *config = net_if_get_config(self->netif);
    if (config == NULL || config->ip.ipv4 == NULL) {
        return mp_const_none;
    }
    // Return netmask of first active address
    for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
        if (config->ip.ipv4->unicast[i].ipv4.addr_state == NET_ADDR_PREFERRED) {
            return common_hal_ipaddress_new_ipv4address(config->ip.ipv4->unicast[i].netmask.s_addr);
        }
    }
    return mp_const_none;
}

mp_obj_t common_hal_mii_mii_get_ipv4_address(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !net_if_is_up(self->netif)) {
        return mp_const_none;
    }
    struct net_if_config *config = net_if_get_config(self->netif);
    if (config == NULL || config->ip.ipv4 == NULL) {
        return mp_const_none;
    }
    // Return the first preferred unicast address
    for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
        struct net_if_addr *addr = &config->ip.ipv4->unicast[i].ipv4;
        if (addr->addr_state == NET_ADDR_PREFERRED) {
            return common_hal_ipaddress_new_ipv4address(addr->address.in_addr.s_addr);
        }
    }
    return mp_const_none;
}

mp_obj_t common_hal_mii_ethernet_get_ipv4_dns(mii_ethernet_obj_t *self) {
    (void)self;
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_ipv4_dns);
}

void common_hal_mii_ethernet_set_ipv4_dns(mii_ethernet_obj_t *self, mp_obj_t ipv4_dns_addr) {
    (void)self;
    (void)ipv4_dns_addr;
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_ipv4_dns);
}

mp_obj_t common_hal_mii_ethernet_get_addresses(mii_ethernet_obj_t *self) {
    if (self->netif == NULL || !net_if_is_up(self->netif)) {
        return mp_const_empty_tuple;
    }
    struct net_if_config *config = net_if_get_config(self->netif);
    if (config == NULL || config->ip.ipv4 == NULL) {
        return mp_const_empty_tuple;
    }

    // Count valid addresses
    int count = 0;
    for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
        if (config->ip.ipv4->unicast[i].ipv4.addr_state == NET_ADDR_PREFERRED) {
            count++;
        }
    }
    if (count == 0) {
        return mp_const_empty_tuple;
    }

    mp_obj_tuple_t *result = MP_OBJ_TO_PTR(mp_obj_new_tuple(count, NULL));
    int idx = 0;
    for (int i = 0; i < NET_IF_MAX_IPV4_ADDR && idx < count; i++) {
        struct net_if_addr *addr = &config->ip.ipv4->unicast[i].ipv4;
        if (addr->addr_state == NET_ADDR_PREFERRED) {
            char buf[NET_IPV4_ADDR_LEN];
            net_addr_ntop(AF_INET, &addr->address.in_addr, buf, sizeof(buf));
            result->items[idx++] = mp_obj_new_str(buf, strlen(buf));
        }
    }
    return MP_OBJ_FROM_PTR(result);
}

mp_obj_t common_hal_mii_ethernet_get_dns(mii_ethernet_obj_t *self) {
    (void)self;
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_dns);
}

void common_hal_mii_ethernet_set_dns(mii_ethernet_obj_t *self, mp_obj_t dns_addr) {
    (void)self;
    (void)dns_addr;
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_dns);
}

void common_hal_mii_ethernet_set_ipv4_address(mii_ethernet_obj_t *self, mp_obj_t ipv4, mp_obj_t netmask, mp_obj_t gateway, mp_obj_t ipv4_dns) {
    if (self->netif == NULL) {
        return;
    }

    // Stop DHCP first
    common_hal_mii_ethernet_stop_dhcp_client(self);

    // Get packed IPv4 bytes from ipaddress objects
    mp_obj_t packed;
    size_t len;
    const char *bytes;

    // Set the address
    struct in_addr addr;
    packed = common_hal_ipaddress_ipv4address_get_packed(ipv4);
    bytes = mp_obj_str_get_data(packed, &len);
    memcpy(&addr, bytes, sizeof(addr));
    struct net_if_addr *if_addr = net_if_ipv4_addr_add(self->netif, &addr, NET_ADDR_MANUAL, 0);
    if (if_addr == NULL) {
        // No free slots; go through the port's standard errno path rather
        // than adding a new translatable string.
        raise_zephyr_error(-ENOMEM);
    }

    // Set the netmask
    struct in_addr mask;
    packed = common_hal_ipaddress_ipv4address_get_packed(netmask);
    bytes = mp_obj_str_get_data(packed, &len);
    memcpy(&mask, bytes, sizeof(mask));
    net_if_ipv4_set_netmask_by_addr(self->netif, &addr, &mask);

    // Set the gateway
    struct in_addr gw;
    packed = common_hal_ipaddress_ipv4address_get_packed(gateway);
    bytes = mp_obj_str_get_data(packed, &len);
    memcpy(&gw, bytes, sizeof(gw));
    net_if_ipv4_set_gw(self->netif, &gw);

    if (ipv4_dns != MP_OBJ_NULL) {
        common_hal_mii_ethernet_set_ipv4_dns(self, ipv4_dns);
    }
}

mp_int_t common_hal_mii_ethernet_ping(mii_ethernet_obj_t *self, mp_obj_t ip_address, mp_float_t timeout) {
    (void)self;
    (void)ip_address;
    (void)timeout;
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_ping);
}

static void _eth_event_handler(struct net_mgmt_event_callback *cb, uint64_t mgmt_event, struct net_if *iface) {
    (void)cb;
    (void)iface;

    switch (mgmt_event) {
        case NET_EVENT_ETHERNET_CARRIER_ON:
        case NET_EVENT_ETHERNET_CARRIER_OFF:
        case NET_EVENT_IPV4_ADDR_ADD:
            #if CIRCUITPY_STATUS_BAR
            supervisor_status_bar_request_update(false);
            #endif
            port_wake_main_task();
            break;
    }
}

void common_hal_mii_ethernet_construct(mii_ethernet_obj_t *self,
    mii_rmii_obj_t *rmii, uint32_t device) {
    self->base.type = &mii_ethernet_type;
    self->started = false;
    self->netif = NULL;
    (void)device;
    if (rmii != NULL) {
        mp_raise_NotImplementedError(
            MP_ERROR_TEXT("cannot construct ethernet interfaces; use board.ETHERNET"));
    }

    // Find the first ethernet interface
    self->netif = net_if_get_first_by_type(&NET_L2_GET_NAME(ETHERNET));
    if (self->netif == NULL) {
        return;
    }

    net_mgmt_init_event_callback(&eth_carrier_cb, _eth_event_handler,
        NET_EVENT_ETHERNET_CARRIER_ON |
        NET_EVENT_ETHERNET_CARRIER_OFF);
    net_mgmt_init_event_callback(&eth_ipv4_cb, _eth_event_handler,
        NET_EVENT_IPV4_ADDR_ADD);

    net_mgmt_add_event_callback(&eth_carrier_cb);
    net_mgmt_add_event_callback(&eth_ipv4_cb);

    #if defined(CONFIG_NET_HOSTNAME)
    // Set hostname based on board name and MAC address.
    size_t board_len = MIN(NET_HOSTNAME_MAX_LEN - ((MAC_ADDRESS_LENGTH * 2) + 6), strlen(CIRCUITPY_BOARD_ID));
    size_t board_trim = strlen(CIRCUITPY_BOARD_ID) - board_len;
    if (CIRCUITPY_BOARD_ID[board_trim] == '_') {
        board_trim++;
    }

    char cpy_default_hostname[board_len + (MAC_ADDRESS_LENGTH * 2) + 6];
    struct net_linkaddr *mac = net_if_get_link_addr(self->netif);
    if (mac->len >= MAC_ADDRESS_LENGTH) {
        snprintf(cpy_default_hostname, sizeof(cpy_default_hostname), "cpy-%s-%02x%02x%02x%02x%02x%02x",
            CIRCUITPY_BOARD_ID + board_trim,
            mac->addr[0], mac->addr[1], mac->addr[2],
            mac->addr[3], mac->addr[4], mac->addr[5]);
        net_hostname_set(cpy_default_hostname, strlen(cpy_default_hostname));
    }
    #endif

    // Enabled by default, start DHCP
    int up_res = net_if_up(self->netif);
    // -EALREADY means the interface is already up. That's not an error.
    if (up_res < 0 && up_res != -EALREADY) {
        raise_zephyr_error(up_res);
    }
    self->started = true;
    net_dhcpv4_start(self->netif);
}

// Cross-translation-unit helpers for supervisor/shared/web_workflow.c; they
// all operate on the first registered instance (the devicetree one).

// Raw uint32_t IPv4 address of the interface, in network byte order, or 0
// when the interface has none.
uint32_t mii_get_ipv4_address(void) {
    mii_ethernet_obj_t *self = (mii_ethernet_obj_t *)mii_get_default_instance();
    if (self == NULL || self->netif == NULL || !net_if_is_up(self->netif)) {
        return 0;
    }
    struct in_addr *addr = net_if_ipv4_get_global_addr(self->netif, NET_ADDR_PREFERRED);
    if (addr == NULL) {
        return 0;
    }
    return addr->s_addr;
}

// Raw hostname of the network interface as set via net_hostname_set (and sent
// as DHCP option 12). Never allocates. The returned pointer points into
// storage owned by Zephyr and stays valid until set via
// common_hal_mii_ethernet_set_hostname, so use it promptly rather than
// holding onto it.
const char *mii_get_hostname_raw(void) {
    mii_ethernet_obj_t *self = (mii_ethernet_obj_t *)mii_get_default_instance();
    if (self == NULL || self->netif == NULL) {
        return "";
    }
    return net_hostname_get();
}

// Blocks until the DHCP lease is bound. DHCPv4 parses its options (which adds
// the DNS server to the resolver) and only then binds the address, so a
// preferred global address implies getaddrinfo() has everything it needs.
// Starting a query before that fails with DNS_EAI_SYSTEM ("getaddrinfo
// failure: -11") because the resolver still has no server to send to.
//
// Returns immediately when the interface cannot ever get a lease: it is down,
// disabled or has no carrier. Used by the supervisor so the web workflow's
// listening socket can be created during this boot instead of being deferred
// until the next VM start.
void mii_wait_for_lease(void) {
    mii_ethernet_obj_t *self = (mii_ethernet_obj_t *)mii_get_default_instance();
    if (self == NULL || self->netif == NULL || !self->started || !net_if_is_carrier_ok(self->netif)) {
        return;
    }
    int64_t deadline = k_uptime_get() + 20000;
    while (k_uptime_get() < deadline && mii_get_ipv4_address() == 0) {
        if (mp_hal_is_interrupted()) {
            break;
        }
        RUN_BACKGROUND_TASKS;
        k_msleep(50);
    }
}
