// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/socketpool/SocketPool.h"
#include "common-hal/socketpool/Socket.h"

#include "py/runtime.h"
#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-bindings/wifi/__init__.h"
#include "common-hal/socketpool/__init__.h"

void common_hal_socketpool_socketpool_construct(socketpool_socketpool_obj_t *self, mp_obj_t radio) {
    if (radio != MP_OBJ_FROM_PTR(&common_hal_wifi_radio_obj)) {
        mp_raise_ValueError(MP_ERROR_TEXT("SocketPool can only be used with wifi.radio"));
    }
    // Not really needed, but more convenient.
    self->radio = radio;
}

// common_hal_socketpool_socket is in socketpool/Socket.c to centralize open socket tracking.

bool socketpool_gethostbyname_ipv4(socketpool_socketpool_obj_t *self, const char *host, uint8_t ipv4[4]) {
    const uint8_t *req_host_params[1] = { (uint8_t *)host };
    size_t req_host_param_lengths[1] = { strlen(host) };

    uint8_t result = 0;
    uint8_t *req_host_responses[1] = { &result };
    size_t req_host_response_lengths[1] = { 1 };

    // If host is a numeric IP address, AirLift will just parse and return the address.

    size_t num_responses = wifi_radio_send_command_get_response(self->radio, REQ_HOST_BY_NAME_CMD,
        req_host_params, req_host_param_lengths, LENGTHS_8, MP_ARRAY_SIZE(req_host_params),
        req_host_responses, req_host_response_lengths, LENGTHS_8, MP_ARRAY_SIZE(req_host_responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses >= 1) {
        if (result == 0) {
            return false;
        }

        uint8_t *get_host_responses[1] = { ipv4 };
        size_t get_host_response_lengths[1] = { IPV4_LENGTH };

        // Now actually get the name.
        num_responses = wifi_radio_send_command_get_response(self->radio, GET_HOST_BY_NAME_CMD,
            NULL, NULL, LENGTHS_8, 0,
            get_host_responses, get_host_response_lengths, LENGTHS_8, MP_ARRAY_SIZE(get_host_responses),
            AIRLIFT_DEFAULT_TIMEOUT_MS);
        if (num_responses == 1) {
            return true;
        }
    }

    return false;
}

static mp_obj_t socketpool_socketpool_gethostbyname_str(socketpool_socketpool_obj_t *self, const char *host) {
    uint8_t ipv4[4] = { 0 };

    if (!socketpool_gethostbyname_ipv4(self, host, ipv4)) {
        // Could not resolve or parse hostname.
        return mp_const_none;
    }

    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 16, &print);
    mp_printf(&print, "%d.%d.%d.%d", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
    return mp_obj_new_str_from_vstr(&vstr);
}

mp_obj_t common_hal_socketpool_getaddrinfo_raise(socketpool_socketpool_obj_t *self, const char *host, int port, int family, int type, int proto, int flags) {
    mp_obj_t ip_str = socketpool_socketpool_gethostbyname_str(self, host);
    if (ip_str == mp_const_none) {
        // Could not resolve hostname.
        common_hal_socketpool_socketpool_raise_gaierror_noname();
    }

    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(5, NULL));
    tuple->items[0] = MP_OBJ_NEW_SMALL_INT(SOCKETPOOL_AF_INET);
    tuple->items[1] = MP_OBJ_NEW_SMALL_INT(SOCKETPOOL_SOCK_STREAM);
    tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
    tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    mp_obj_tuple_t *sockaddr = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    sockaddr->items[0] = ip_str;
    sockaddr->items[1] = MP_OBJ_NEW_SMALL_INT(port);
    tuple->items[4] = MP_OBJ_FROM_PTR(sockaddr);
    return mp_obj_new_list(1, (mp_obj_t *)&tuple);
}
