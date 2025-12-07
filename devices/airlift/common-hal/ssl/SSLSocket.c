// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/ssl/SSLSocket.h"
#include "shared-bindings/ssl/SSLContext.h"

#include "shared/runtime/interrupt_char.h"
#include "shared/netutils/netutils.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/shared/tick.h"

#include "shared-bindings/socketpool/enum.h"

ssl_sslsocket_obj_t *common_hal_ssl_sslcontext_wrap_socket(ssl_sslcontext_obj_t *self,
    mp_obj_t socket, bool server_side, const char *server_hostname) {

    if (server_side) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q not implemented"), MP_QSTR_server_side);
    }

    mp_arg_validate_length_max(strlen(server_hostname), MAX_HOSTNAME_LENGTH, MP_QSTR_hostname);

    ssl_sslsocket_obj_t *ssl_socket = mp_obj_malloc(ssl_sslsocket_obj_t, &ssl_sslsocket_type);
    ssl_socket->socket = MP_OBJ_TO_PTR(socket);
    // Max length already validated above, so this is safe.
    strcpy(ssl_socket->hostname, server_hostname);
    ssl_socket->server_side = server_side;
    return ssl_socket;
}

mp_uint_t common_hal_ssl_sslsocket_recv_into(ssl_sslsocket_obj_t *self, uint8_t *buf, mp_uint_t len) {
    return common_hal_socketpool_socket_recv_into(self->socket, buf, len);
}

mp_uint_t common_hal_ssl_sslsocket_send(ssl_sslsocket_obj_t *self, const uint8_t *buf, mp_uint_t len) {
    return common_hal_socketpool_socket_send(self->socket, buf, len);
}

void common_hal_ssl_sslsocket_bind(ssl_sslsocket_obj_t *self, mp_obj_t addr_in) {
    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);

    size_t hostlen;
    const char *host = mp_obj_str_get_data(addr_items[0], &hostlen);
    mp_int_t port = mp_arg_validate_int_min(mp_obj_get_int(addr_items[1]), 0, MP_QSTR_port);

    common_hal_socketpool_socket_bind(self->socket, host, hostlen, port);
}

void common_hal_ssl_sslsocket_close(ssl_sslsocket_obj_t *self) {
    // TODO: MORE??
    common_hal_socketpool_socket_close(self->socket);
}

void common_hal_ssl_sslsocket_connect(ssl_sslsocket_obj_t *self, mp_obj_t addr_in) {
    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);

    size_t hostlen;
    const char *host = mp_obj_str_get_data(addr_items[0], &hostlen);
    mp_int_t port = mp_arg_validate_int_min(mp_obj_get_int(addr_items[1]), 0, MP_QSTR_port);

    socketpool_socket_start_client_mode(self->socket, host, hostlen, (uint32_t)port, AIRLIFT_TLS_MODE);
}

bool common_hal_ssl_sslsocket_get_closed(ssl_sslsocket_obj_t *self) {
    return common_hal_socketpool_socket_get_closed(self->socket);
}

bool common_hal_ssl_sslsocket_get_connected(ssl_sslsocket_obj_t *self) {
    return common_hal_socketpool_socket_get_connected(self->socket);
}

void common_hal_ssl_sslsocket_listen(ssl_sslsocket_obj_t *self, int backlog) {
    mp_raise_NotImplementedError(NULL);
}

mp_obj_t common_hal_ssl_sslsocket_accept(ssl_sslsocket_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_ssl_sslsocket_setsockopt(ssl_sslsocket_obj_t *self, mp_obj_t level_obj, mp_obj_t optname_obj, mp_obj_t optval_obj) {
    mp_int_t level = mp_obj_get_int(level_obj);
    mp_int_t optname = mp_obj_get_int(optname_obj);

    const void *optval;
    mp_uint_t optlen;
    mp_int_t val;
    if (mp_obj_is_integer(optval_obj)) {
        val = mp_obj_get_int_truncated(optval_obj);
        optval = &val;
        optlen = sizeof(val);
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(optval_obj, &bufinfo, MP_BUFFER_READ);
        optval = bufinfo.buf;
        optlen = bufinfo.len;
    }

    int _errno = common_hal_socketpool_socket_setsockopt(self->socket, level, optname, optval, optlen);
    if (_errno < 0) {
        mp_raise_OSError(-_errno);
    }
}

void common_hal_ssl_sslsocket_settimeout(ssl_sslsocket_obj_t *self, mp_obj_t timeout_obj) {
    mp_uint_t timeout_ms;
    if (timeout_obj == mp_const_none) {
        timeout_ms = SOCKET_BLOCK_FOREVER;
    } else {
        #if MICROPY_PY_BUILTINS_FLOAT
        timeout_ms = 1000 * mp_obj_get_float(timeout_obj);
        #else
        timeout_ms = 1000 * mp_obj_get_int(timeout_obj);
        #endif
    }
    common_hal_socketpool_socket_settimeout(self->socket, timeout_ms);
}

bool common_hal_ssl_sslsocket_readable(ssl_sslsocket_obj_t *self) {
    return common_hal_socketpool_socket_readable(self->socket);
}

bool common_hal_ssl_sslsocket_writable(ssl_sslsocket_obj_t *self) {
    return common_hal_socketpool_socket_writable(self->socket);
}
