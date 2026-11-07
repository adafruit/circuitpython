// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/socketpool/SocketPool.h"
#include "common-hal/socketpool/Socket.h"

#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#if CIRCUITPY_HOSTNETWORK
#include "bindings/hostnetwork/__init__.h"
#endif
#if CIRCUITPY_WIFI
#include "shared-bindings/wifi/__init__.h"
#endif
#if CIRCUITPY_MII
#include "shared-bindings/mii/Ethernet.h"
#endif
#include "common-hal/socketpool/__init__.h"

#include <stdio.h>
#include <string.h>

#include <zephyr/net/socket.h>
#include <zephyr/net/dns_resolve.h>

void common_hal_socketpool_socketpool_construct(socketpool_socketpool_obj_t *self, mp_obj_t network_interface) {
    bool is_wifi = false;
    #if CIRCUITPY_WIFI
    is_wifi = network_interface == MP_OBJ_FROM_PTR(&common_hal_wifi_radio_obj);
    #endif
    bool is_ethernet = false;
    #if CIRCUITPY_MII
    is_ethernet = mp_obj_is_type(network_interface, &mii_ethernet_type);
    #endif
    bool is_hostnetwork = false;
    #if CIRCUITPY_HOSTNETWORK
    is_hostnetwork = mp_obj_is_type(network_interface, &hostnetwork_hostnetwork_type);
    #endif
    if (!(is_wifi || is_ethernet || is_hostnetwork)) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("SocketPool can only be used with %q.%q, %q.%q, or %q.%q"),
            MP_QSTR_wifi, MP_QSTR_radio, MP_QSTR_board, MP_QSTR_ETHERNET,
            MP_QSTR_hostnetwork, MP_QSTR_HostNetwork);
    }
}

// common_hal_socketpool_socket is in socketpool/Socket.c to centralize open socket tracking.

static int socketpool_getaddrinfo_common(const char *host, int service, const struct zsock_addrinfo *hints, struct zsock_addrinfo **res) {
    char service_buf[6];
    snprintf(service_buf, sizeof(service_buf), "%d", service);

    return zsock_getaddrinfo(host, service_buf, hints, res);
}

#define SOCKETPOOL_IP_STR_LEN 48

// Zephyr's net namespace renumbers address families (NET_PF_INET = 1,
// NET_PF_INET6 = 2) while CircuitPython exposes the BSD numbers (AF_INET = 2,
// AF_INET6 = 10) through socketpool. Translate in both directions so the
// tuples getaddrinfo() produces and the hints callers pass in are in Python
// terms; AF_UNSPEC (0) is the same in both and passes through.
static int socketpool_family_to_zephyr(int family) {
    switch (family) {
        case SOCKETPOOL_AF_INET:
            return NET_AF_INET;
        #if CIRCUITPY_SOCKETPOOL_IPV6
        case SOCKETPOOL_AF_INET6:
            return NET_AF_INET6;
        #endif
        default:
            return NET_AF_UNSPEC;
    }
}

static int socketpool_family_from_zephyr(int family) {
    switch (family) {
        case NET_AF_INET:
            return SOCKETPOOL_AF_INET;
        case NET_AF_INET6:
            return SOCKETPOOL_AF_INET6;
        default:
            return family;
    }
}

static mp_obj_t format_address(const struct sockaddr *addr, int family) {
    char ip_str[SOCKETPOOL_IP_STR_LEN];
    const struct sockaddr_in *a = (void *)addr;

    switch (family) {
        #if CIRCUITPY_SOCKETPOOL_IPV6
        case AF_INET6:
            zsock_inet_ntop(family, &((const struct sockaddr_in6 *)a)->sin6_addr, ip_str, sizeof(ip_str));
            break;
        #endif
        default:
        case AF_INET:
            zsock_inet_ntop(family, &((const struct sockaddr_in *)a)->sin_addr, ip_str, sizeof(ip_str));
            break;
    }
    return mp_obj_new_str(ip_str, strlen(ip_str));
}

static mp_obj_t convert_sockaddr(const struct zsock_addrinfo *ai, int port) {
    #if CIRCUITPY_SOCKETPOOL_IPV6
    mp_int_t n_tuple = ai->ai_family == AF_INET6 ? 4 : 2;
    #else
    mp_int_t n_tuple = 2;
    #endif
    mp_obj_tuple_t *result = MP_OBJ_TO_PTR(mp_obj_new_tuple(n_tuple, NULL));
    result->items[0] = format_address(ai->ai_addr, ai->ai_family);
    result->items[1] = MP_OBJ_NEW_SMALL_INT(port);
    #if CIRCUITPY_SOCKETPOOL_IPV6
    if (ai->ai_family == AF_INET6) {
        const struct sockaddr_in6 *ai6 = (void *)ai->ai_addr;
        result->items[2] = MP_OBJ_NEW_SMALL_INT(ai6->sin6_flowinfo);
        result->items[3] = MP_OBJ_NEW_SMALL_INT(ai6->sin6_scope_id);
    }
    #endif
    return result;
}

static mp_obj_t convert_addrinfo(const struct zsock_addrinfo *ai, int port) {
    mp_obj_tuple_t *result = MP_OBJ_TO_PTR(mp_obj_new_tuple(5, NULL));
    result->items[0] = MP_OBJ_NEW_SMALL_INT(socketpool_family_from_zephyr(ai->ai_family));
    result->items[1] = MP_OBJ_NEW_SMALL_INT(ai->ai_socktype);
    result->items[2] = MP_OBJ_NEW_SMALL_INT(ai->ai_protocol);
    result->items[3] = ai->ai_canonname ? mp_obj_new_str(ai->ai_canonname, strlen(ai->ai_canonname)) : MP_OBJ_NEW_QSTR(MP_QSTR_);
    result->items[4] = convert_sockaddr(ai, port);
    return result;
}

mp_obj_t common_hal_socketpool_getaddrinfo_raise(socketpool_socketpool_obj_t *self, const char *host, int port, int family, int type, int proto, int flags) {
    // A fetch can run before DHCP has fed the resolver its server (scripts
    // commonly connect() and then immediately fetch). Until the resolver has
    // at least one server, a hostname lookup fails outright, reported as
    // DNS_EAI_SYSTEM ("getaddrinfo failure: -11"). Rather than racing the
    // lease at every caller, only wait when the resolver is genuinely empty;
    // a ready resolver (the common case) passes straight through.
    #if defined(CONFIG_DNS_RESOLVER) && defined(CONFIG_NET_DHCPV4)
    {
        struct dns_resolve_context *ctx = dns_resolve_get_default();
        bool have_server = false;
        int64_t deadline = k_uptime_get() + 10000;
        while (k_uptime_get() < deadline) {
            for (int i = 0; i < CONFIG_DNS_RESOLVER_MAX_SERVERS; i++) {
                if (ctx->servers[i].dns_server.sa_family != NET_AF_UNSPEC) {
                    have_server = true;
                    break;
                }
            }
            if (have_server || mp_hal_is_interrupted()) {
                break;
            }
            RUN_BACKGROUND_TASKS;
            k_msleep(50);
        }
    }
    #endif

    const struct zsock_addrinfo hints = {
        .ai_flags = flags,
        .ai_family = socketpool_family_to_zephyr(family),
        .ai_protocol = proto,
        .ai_socktype = type,
    };

    struct zsock_addrinfo *res = NULL;
    int err = socketpool_getaddrinfo_common(host, port, &hints, &res);
    if (err != 0 || res == NULL) {
        if (err == 0) {
            err = DNS_EAI_NONAME;
        }
        common_hal_socketpool_socketpool_raise_gaierror(err);
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t result = mp_obj_new_list(0, NULL);
        for (struct zsock_addrinfo *ai = res; ai; ai = ai->ai_next) {
            mp_obj_list_append(result, convert_addrinfo(ai, port));
        }
        nlr_pop();
        zsock_freeaddrinfo(res);
        return result;
    } else {
        zsock_freeaddrinfo(res);
        nlr_raise(MP_OBJ_FROM_PTR(nlr.ret_val));
    }
}
