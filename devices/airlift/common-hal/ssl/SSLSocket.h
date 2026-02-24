// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-bindings/socketpool/Socket.h"

typedef struct ssl_sslsocket_obj {
    mp_obj_base_t base;
    socketpool_socket_obj_t *socket;
    char hostname[MAX_HOSTNAME_LENGTH + 1];
    bool closed;
    bool server_side;
} ssl_sslsocket_obj_t;
