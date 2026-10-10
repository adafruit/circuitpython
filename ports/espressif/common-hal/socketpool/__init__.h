// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

struct addrinfo;
struct sockaddr_storage;

int socketpool_getaddrinfo_common(const char *host, int service, const struct addrinfo *hints, struct addrinfo **res);
void socketpool_resolve_host_or_throw(int family, int type, const char *hostname, struct sockaddr_storage *addr, int port);

mp_obj_t sockaddr_to_str(const struct sockaddr_storage *addr);
mp_obj_t sockaddr_to_tuple(const struct sockaddr_storage *addr);
