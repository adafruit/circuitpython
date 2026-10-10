// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/socketpool/__init__.h"
#include "common-hal/socketpool/__init__.h"

#include "common-hal/socketpool/Socket.h"

#include "py/runtime.h"

#include "lwip/inet.h"
#include "lwip/sockets.h"

#include <string.h>

void socketpool_user_reset(void) {
    socket_user_reset();
}

mp_obj_t sockaddr_to_str(const struct sockaddr_storage *sockaddr) {
    char buf[IPADDR_STRLEN_MAX];
    #if CIRCUITPY_SOCKETPOOL_IPV6
    if (sockaddr->ss_family == AF_INET6) {
        const struct sockaddr_in6 *addr6 = (const void *)sockaddr;
        inet_ntop(AF_INET6, &addr6->sin6_addr, buf, sizeof(buf));
    } else
    #endif
    {
        const struct sockaddr_in *addr = (const void *)sockaddr;
        inet_ntop(AF_INET, &addr->sin_addr, buf, sizeof(buf));
    }
    return mp_obj_new_str(buf, strlen(buf));
}

mp_obj_t sockaddr_to_tuple(const struct sockaddr_storage *sockaddr) {
    mp_obj_t args[4] = {
        sockaddr_to_str(sockaddr),
    };
    int n = 2;
    #if CIRCUITPY_SOCKETPOOL_IPV6
    if (sockaddr->ss_family == AF_INET6) {
        const struct sockaddr_in6 *addr6 = (const void *)sockaddr;
        args[1] = MP_OBJ_NEW_SMALL_INT(htons(addr6->sin6_port));
        args[2] = MP_OBJ_NEW_SMALL_INT(addr6->sin6_flowinfo);
        args[3] = MP_OBJ_NEW_SMALL_INT(addr6->sin6_scope_id);
        n = 4;
    } else
    #endif
    {
        const struct sockaddr_in *addr = (const void *)sockaddr;
        args[1] = MP_OBJ_NEW_SMALL_INT(htons(addr->sin_port));
    }
    return mp_obj_new_tuple(n, args);
}
