// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/socketpool/SocketPool.h"
#include "shared-bindings/wifi/Radio.h"

typedef enum {
    SOCKET_CLOSED = 0,
    SOCKET_LISTEN = 1,
    SOCKET_SYN_SENT = 2,
    SOCKET_SYN_RCVD = 3,
    SOCKET_ESTABLISHED = 4,
    SOCKET_FIN_WAIT_1 = 5,
    SOCKET_FIN_WAIT_2 = 6,
    SOCKET_CLOSE_WAIT = 7,
    SOCKET_CLOSING = 8,
    SOCKET_LAST_ACK = 9,
    SOCKET_TIME_WAIT = 10,
} airlift_socket_status_t;

typedef struct ssl_sslsocket_obj ssl_sslsocket_obj_t;

typedef struct {
    mp_obj_base_t base;
    socketpool_socketpool_obj_t *socketpool;
    ssl_sslsocket_obj_t *ssl_socket;
    mp_uint_t timeout_ms;   // SOCKET_BLOCK_FOREVER is (mp_uint_t)-1.
    size_t hostname_len;
    uint8_t hostname[MAX_HOSTNAME_LENGTH + 1];
    mp_uint_t port;
    uint8_t num;
    uint8_t type;
    uint8_t family;
    uint8_t proto;
    bool connected;
    bool bound;
    bool client_started;
    bool server_started;
} socketpool_socket_obj_t;

#define AIRLIFT_SOCKET_DEFAULT_TIMEOUT (3000)

#define IPPROTO_IP      0
#define IPPROTO_ICMP    1
#define IPPROTO_TCP     6
#define IPPROTO_UDP     17

#define AF_UNSPEC       0
#define AF_INET         2

#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3

// Used both internally and by AirLift.
#define NO_SOCKET       255

void socket_user_reset(void);
// Unblock workflow socket select thread (platform specific)
void socketpool_socket_poll_resume(void);

void socketpool_socket_start_client_mode(socketpool_socket_obj_t *self, const char *host, size_t hostlen, uint32_t port, airlift_conn_mode_t mode);
void socketpool_socket_stop_client(socketpool_socket_obj_t *self);

void socketpool_socket_start_server_mode(socketpool_socket_obj_t *self, airlift_conn_mode_t mode);
