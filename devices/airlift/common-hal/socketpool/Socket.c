// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/socketpool/Socket.h"

#include "shared/runtime/interrupt_char.h"
#include "py/mperrno.h"
///TEMP
#include "py/mphal.h"
#include "py/runtime.h"
#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "shared-bindings/wifi/Radio.h"
#include "common-hal/socketpool/__init__.h"
#include "common-hal/wifi/__init__.h"
// #if CIRCUITPY_SSL
// #include "shared-bindings/ssl/SSLSocket.h"
// #include "shared-module/ssl/SSLSocket.h"
// #endif
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"
#include "supervisor/workflow.h"

void socket_user_reset(void) {
}

NORETURN static void raise_failed(void) {
    mp_raise_RuntimeError(MP_ERROR_TEXT("Failed"));
}

// Unblock select task (ok if not blocked yet)
void socketpool_socket_poll_resume(void) {
}

// The SOCKETPOOL_ constants are actually the same as the LWIP constants, but it's
// possible they might not be, so map them, just in case.
static int socketpool_type_to_airlift_type(socketpool_socketpool_sock_t type) {
    switch (type) {
        case SOCKETPOOL_SOCK_STREAM:
            return SOCK_STREAM;
        case SOCKETPOOL_SOCK_DGRAM:
            return SOCK_DGRAM;
        case SOCKETPOOL_SOCK_RAW:
            return SOCK_RAW;
        default:
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_type);
    }
}

static bool _socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type,
    int proto,
    socketpool_socket_obj_t *sock) {

    sock->proto = IPPROTO_IP;

    // The SOCKETPOOL_ constants are actually the same as the LWIP constants, but it's
    // possible they might not be, so map them, just in case.
    switch (family) {
        case SOCKETPOOL_AF_INET:
            sock->family = AF_INET;
            break;
        default:
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_family);
    }

    sock->type = socketpool_type_to_airlift_type(type);

    sock->socketpool = self;
    sock->timeout_ms = SOCKET_BLOCK_FOREVER;

    uint8_t socket_num;
    uint8_t *responses[1] = { &socket_num };
    size_t response_lengths[1] = { 1 };

    size_t num_responses = wifi_radio_send_command_get_response(self->radio, GET_SOCKET_CMD,
        NULL, NULL, LENGTHS_8, 0,
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 0) {
        raise_failed();
    }

    sock->num = socket_num;
    sock->bound = false;
    sock->connected = false;
    sock->client_started = false;
    sock->server_started = false;

    return sock->num != NO_SOCKET;

}

// special entry for workflow listener (register system socket)
bool socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type,
    int proto, socketpool_socket_obj_t *sock) {

    return _socketpool_socket(self, family, type, proto, sock);
}

socketpool_socket_obj_t *common_hal_socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type, int proto) {
    socketpool_socket_obj_t *sock = mp_obj_malloc_with_finaliser(socketpool_socket_obj_t, &socketpool_socket_type);

    if (!_socketpool_socket(self, family, type, proto, sock)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Out of sockets"));
    }
    return sock;
}

// Polling values, specific to NINA-FW.
#define SOCKET_POLL_RD       (0x01)
#define SOCKET_POLL_WR       (0x02)
#define SOCKET_POLL_ERR      (0x04)
#define SOCKET_POLL_FAIL     (0x80)

static uint8_t socketpool_socket_poll(socketpool_socket_obj_t *self) {
    // TODO: not gonna work on the mapped socket number.
    mp_raise_NotImplementedError(NULL);
}

// Return, via parameters, the IP address and port of the connection.
// Return false if not connected.
static bool socketpool_socket_get_connection_info(socketpool_socket_obj_t *self, uint8_t ipv4[IPV4_LENGTH], uint16_t *port) {
    const uint8_t *params[1] = { &self->num };
    size_t param_lengths[1] = { 1 };

    *port = 0;
    memset(ipv4, 0, IPV4_LENGTH);
    uint8_t *responses[3] = { ipv4, (uint8_t *)&port };
    size_t response_lengths[3] = { IPV4_LENGTH, sizeof(port) };

    size_t num_responses = wifi_radio_send_command_get_response(self->socketpool->radio, GET_REMOTE_DATA_CMD,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 0) {
        raise_failed();
    }

    return port != 0;
}



static airlift_client_socket_status_t client_socket_status(socketpool_socket_obj_t *self) {
    const uint8_t *params[1] = { &self->num };
    size_t param_lengths[1] = { 1 };

    uint8_t result;
    uint8_t *responses[1] = { &result };
    size_t response_lengths[1] = { 1 };

    size_t num_responses = wifi_radio_send_command_get_response(self->socketpool->radio, GET_CLIENT_STATE_TCP_CMD,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 0) {
        raise_failed();
    }

    return result;
}

static bool server_socket_status(socketpool_socket_obj_t *self) {
    const uint8_t *params[1] = { &self->num };
    size_t param_lengths[1] = { 1 };

    uint8_t result;
    uint8_t *responses[1] = { &result };
    size_t response_lengths[1] = { 1 };

    // GET_STATE_TCP_CMD is a misnomer. It only checks whether there's a tcpserver for the socket.
    size_t num_responses = wifi_radio_send_command_get_response(self->socketpool->radio, GET_STATE_TCP_CMD,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    if (num_responses == 0) {
        raise_failed();
    }

    return result == 1;
}

int socketpool_socket_accept(socketpool_socket_obj_t *self, mp_obj_t *peer_out, socketpool_socket_obj_t *accepted) {
    if (self->type != SOCK_STREAM) {
        return -MP_EOPNOTSUPP;
    }

    if (!server_socket_status(self)) {
        return -MP_ENOTCONN;
    }

    const uint8_t *params[1] = { &self->num };
    size_t param_lengths[1] = { 1 };

    uint16_t accept_socket_num;
    uint8_t *responses[1] = { (uint8_t *)&accept_socket_num };
    size_t response_lengths[1] = { sizeof(accept_socket_num) };

    const uint64_t start_time = supervisor_ticks_ms64();
    while (true) {
        // When passed a server socket, AVAIL_DATA_TCP_CMD returns the socket number on which to read data.
        // For client and UDP sockets, it returns the number of bytes available. (Quite a difference!)
        size_t num_responses = wifi_radio_send_command_get_response(self->socketpool->radio, AVAIL_DATA_TCP_CMD,
            params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
            responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
            AIRLIFT_DEFAULT_TIMEOUT_MS);

        if (num_responses == 0) {
            raise_failed();
        }

        if (accept_socket_num != NO_SOCKET) {
            return accept_socket_num;
        }

        if (self->timeout_ms == 0) {
            // Non-blocking raises a different exception than a timeout.
            return -MP_EAGAIN;
        }
        if ((self->timeout_ms != SOCKET_BLOCK_FOREVER &&
             supervisor_ticks_ms64() - start_time >= self->timeout_ms) ||
            mp_hal_is_interrupted()) {
            return -MP_ETIMEDOUT;
        }

        RUN_BACKGROUND_TASKS;
        // Give the AirLift some time to do work instead of asking again immediately.
        mp_hal_delay_ms(50);
    }
}


socketpool_socket_obj_t *common_hal_socketpool_socket_accept(socketpool_socket_obj_t *self, mp_obj_t *peer_out) {
    socketpool_socket_obj_t *accepted = mp_obj_malloc_with_finaliser(socketpool_socket_obj_t, NULL);
    socketpool_socket_reset(accepted);

    int ret = socketpool_socket_accept(self, peer_out, accepted);
    if (ret < 0) {
        if (ret == -MP_ETIMEDOUT) {
            // There is a specific subclass for timeouts.
            mp_raise_msg(&mp_type_TimeoutError, NULL);
        }
        // Otherwise, raise a general OSError. Includes EAGAIN.
        mp_raise_OSError(-ret);
    }

    return accepted;
}

int common_hal_socketpool_socket_bind(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port) {
    if (self->server_started) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_socket);
    }
    if (self->bound) {
        return MP_EINVAL;
    }

    // Validate the host name (which might be a numeric IP string) to an IPv4 address first.
    uint8_t ipv4[IPV4_LENGTH];
    if (!socketpool_gethostbyname_ipv4(self->socketpool, host, ipv4)) {
        // Could not resolve hostname.
        common_hal_socketpool_socketpool_raise_gaierror_noname();
    }

    const uint8_t zero_ipv4[IPV4_LENGTH] = { 0 };
    uint8_t self_ipv4[IPV4_LENGTH];
    ipv4_uint32_to_bytes(wifi_radio_get_ipv4_address(self->socketpool->radio), self_ipv4);

    // The bound host's IP must be this host: 0.0.0.0 or wifi.radio.ipv4_address.
    if (memcmp(ipv4, zero_ipv4, IPV4_LENGTH) != 0 &&
        memcmp(ipv4, self_ipv4, IPV4_LENGTH) != 0) {
        // Same as CPython.
        return 99;  // EADDRNOTAVAIL (sometimes 125!)
    }
    self->bound = true;
    memcpy(self->hostname, host, hostlen);
    self->hostname_len = hostlen;
    self->port = port;
    return 0;
}

void socketpool_socket_close(socketpool_socket_obj_t *self) {
    if (self->client_started) {
        socketpool_socket_stop_client(self);
    }

    // Re server_started: there is no way to shut down a server.
}

void common_hal_socketpool_socket_close(socketpool_socket_obj_t *self) {
    socketpool_socket_close(self);
}

void socketpool_socket_start_client_mode(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port, airlift_conn_mode_t mode) {

    mp_arg_validate_length_max(hostlen, MAX_HOSTNAME_LENGTH, MP_QSTR_hostname);

    // if (self->client_started) {
    //     // Is this client already started on the given host and port? If so, nothing need be done.
    //     if (self->hostname_len == hostlen &&
    //         strncasecmp((char *)self->hostname, host, hostlen) == 0 &&
    //         self->port == port) {
    //         return;
    //     } else {
    //         // Otherwise, stop the current client, so we can restart it with a different address.
    //         socketpool_socket_stop_client(self);
    //     }
    // }

    uint8_t unused_ipv4[IPV4_LENGTH] = { 0 };
    uint8_t port_bytes[2];
    be_uint16_to_uint8_bytes((uint16_t)port, port_bytes);
    uint8_t conn_mode = mode;

    const uint8_t *params[5] = { (uint8_t *)host, unused_ipv4, port_bytes, &self->num, &conn_mode };
    size_t param_lengths[5] = { hostlen, 4, 2, 1, 1 };

    uint8_t result;
    uint8_t *responses[1] = { &result };
    size_t response_lengths[1] = { 1 };

    size_t num_responses = wifi_radio_send_command_get_response(self->socketpool->radio, START_CLIENT_TCP_CMD,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses), self->timeout_ms);

    if (num_responses == 0 || result != 1) {
        mp_raise_ConnectionError(MP_ERROR_TEXT("Failed"));
    }

    // Update any previous IP and port with the requested ones.
    // The memcpy won't overflow because hostlen has been validated above.
    memcpy(self->hostname, host, hostlen);
    self->hostname_len = hostlen;
    self->port = port;
    self->client_started = true;
}

void socketpool_socket_stop_client(socketpool_socket_obj_t *self) {
    const uint8_t *params[1] = { &self->num };

    if (!self->client_started) {
        return;
    }

    size_t param_lengths[1] = { 1 };

    uint8_t result;
    uint8_t *responses[1] = { &result };
    size_t response_lengths[1] = { 1 };

    size_t num_responses = wifi_radio_send_command_get_response(self->socketpool->radio, STOP_CLIENT_TCP_CMD,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses),
        AIRLIFT_DEFAULT_TIMEOUT_MS);

    // Don't raise an error if VM isn't running, because socket close is the socket finaliser.
    if ((num_responses == 0 || result != 1) && vm_is_running()) {
        raise_failed();
    }

    self->client_started = false;
}

void socketpool_socket_start_server_mode(socketpool_socket_obj_t *self, airlift_conn_mode_t mode) {
    // Once bind() is called, the ipv4 and port won't change, so a restart will not change anything.
    if (self->server_started) {
        return;
    }

    uint8_t zero_ipv4[IPV4_LENGTH] = { 0 };
    uint8_t port_bytes[2];
    be_uint16_to_uint8_bytes((uint16_t)self->port, port_bytes);
    uint8_t conn_mode = mode;

    const uint8_t *params[4] = { zero_ipv4, port_bytes, &self->num, &conn_mode };
    size_t param_lengths[4] = { 4, 2, 1, 1 };

    uint8_t result;
    uint8_t *responses[1] = { &result };
    size_t response_lengths[1] = { 1 };

    size_t num_responses = wifi_radio_send_command_get_response(self->socketpool->radio, START_SERVER_TCP_CMD,
        params, param_lengths, LENGTHS_8, MP_ARRAY_SIZE(params),
        responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses), self->timeout_ms);

    if (num_responses == 0 || result != 1) {
        mp_raise_ConnectionError(MP_ERROR_TEXT("Failed"));
    }

    self->server_started = true;
}

void common_hal_socketpool_socket_connect(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port) {
    socketpool_socket_start_client_mode(self, host, hostlen, port,
        self->type == SOCK_STREAM ? AIRLIFT_TCP_MODE : AIRLIFT_UDP_MODE);
}

bool common_hal_socketpool_socket_get_closed(socketpool_socket_obj_t *self) {
    if (self->client_started) {
        return client_socket_status(self) == SOCKET_CLOSED;
    } else if (self->server_started) {
        return !server_socket_status(self);
    } else {
        return false;
    }
}

bool common_hal_socketpool_socket_get_connected(socketpool_socket_obj_t *self) {
    if (self->client_started) {
        return client_socket_status(self) == SOCKET_ESTABLISHED;
    } else if (self->server_started) {
        return server_socket_status(self);
    } else {
        return false;
    }
}

bool common_hal_socketpool_socket_listen(socketpool_socket_obj_t *self, int backlog) {
    if (self->type != SOCK_STREAM) {
        // Same as CPython.
        mp_raise_OSError(MP_EOPNOTSUPP);
    }
    if (!self->bound) {
        // Other impls will assign an address and port, but AirLift can't do that.
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q() without %q()"), MP_QSTR_listen, MP_QSTR_bind);
    }
    socketpool_socket_start_server_mode(self, AIRLIFT_TCP_MODE);
    return true;
}

mp_uint_t common_hal_socketpool_socket_recvfrom_into(socketpool_socket_obj_t *self,
    uint8_t *buf, uint32_t len, mp_obj_t *source_out) {
    uint8_t ipv4[IPV4_LENGTH];
    uint16_t port;

    if (self->type != SOCK_DGRAM) {
        mp_raise_ValueError(MP_ERROR_TEXT("Unsupported socket type"));
    }

    const mp_uint_t bytes_received = common_hal_socketpool_socket_recv_into(self, buf, len);

    if (!socketpool_socket_get_connection_info(self, ipv4, &port)) {
        raise_failed();
    }

    // Pass back a tuple of stringified IP address and port.
    mp_obj_t items[2] = {
        ipv4_bytes_to_str(ipv4),
        MP_OBJ_NEW_SMALL_INT(port),
    };
    *source_out = mp_obj_new_tuple(MP_ARRAY_SIZE(items), items);

    return bytes_received;
}

int socketpool_socket_recv_into(socketpool_socket_obj_t *self, uint8_t *buf, uint32_t len) {
    const uint8_t *params[2] = { &self->num, (uint8_t *)&len };
    size_t param_lengths[2] = { 1, 2 };

    const uint64_t start_time = supervisor_ticks_ms64();

    uint8_t *responses[1] = { buf };
    while (true) {
        // This will be the max length read. We need to re-initialize each time we read, because its
        // value is changed by wifi_radio_send_command_get_response().
        size_t response_lengths[1] = { len };

        size_t num_responses = wifi_radio_send_command_get_response(self->socketpool->radio, GET_DATABUF_TCP_CMD,
            params, param_lengths, LENGTHS_16, MP_ARRAY_SIZE(params),
            responses, response_lengths, LENGTHS_16, MP_ARRAY_SIZE(responses), self->timeout_ms);

        if (num_responses == 0) {
            return -1;
        }

        const size_t num_read = response_lengths[0];
        if (num_read > 0) {
            return num_read;
        }

        // Give up if timeout was exceeded, or ctrl-C was typed.
        if ((self->timeout_ms != SOCKET_BLOCK_FOREVER &&
             supervisor_ticks_ms64() - start_time >= self->timeout_ms) ||
            mp_hal_is_interrupted()) {
            return 0;
        }

        RUN_BACKGROUND_TASKS;
        // Give the AirLift some time to do work instead of asking again immediately.
        mp_hal_delay_ms(50);
    }
}

mp_uint_t common_hal_socketpool_socket_recv_into(socketpool_socket_obj_t *self, uint8_t *buf, uint32_t len) {
    int received = socketpool_socket_recv_into(self, buf, len);
    if (received < 0) {
        raise_failed();
    }
    return received;
}

int socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    const uint8_t *params[2] = { &self->num, buf };
    size_t param_lengths[2] = { 1, len };

    size_t num_responses;

    // Will only be SOCK_STREAM or SOCK_DGRAM.
    switch (self->type) {
        case SOCK_STREAM: {
            uint8_t bytes_sent[2];
            uint8_t *responses[1] = { bytes_sent };
            size_t response_lengths[1] = { sizeof(bytes_sent) };

            num_responses = wifi_radio_send_command_get_response(self->socketpool->radio, SEND_DATA_TCP_CMD,
                params, param_lengths, LENGTHS_16, MP_ARRAY_SIZE(params),
                responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses), self->timeout_ms);
            if (num_responses == 0) {
                return -1;
            }
            return le_uint8_bytes_to_uint16(bytes_sent);
        }

        case SOCK_DGRAM: {
            uint8_t result;
            uint8_t *responses[1] = { &result };
            size_t response_lengths[1] = { 1 };

            num_responses = wifi_radio_send_command_get_response(self->socketpool->radio, INSERT_DATABUF_TCP_CMD,
                params, param_lengths, LENGTHS_16, MP_ARRAY_SIZE(params),
                responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses), self->timeout_ms);
            if (num_responses == 0 || result != 1) {
                return -1;
            }

            const uint8_t *send_udp_data_params[1] = { &self->num };
            size_t send_udp_data_param_lengths[1] = { 1 };
            num_responses = wifi_radio_send_command_get_response(self->socketpool->radio, SEND_UDP_DATA_CMD,
                send_udp_data_params, send_udp_data_param_lengths, LENGTHS_8, MP_ARRAY_SIZE(send_udp_data_params),
                responses, response_lengths, LENGTHS_8, MP_ARRAY_SIZE(responses), self->timeout_ms);
            if (num_responses == 0 || result != 1) {
                return -1;
            }

            // Using DATA_SENT_TCP_CMD is not necessary. It's a no-op on the AirLift side.
            return len;
        }

        default:
            // This shouldn't happen.
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), self->type);
    }
}

mp_uint_t common_hal_socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int sent = socketpool_socket_send(self, buf, len);

    if (sent < 0) {
        raise_failed();
    }
    return sent;
}

mp_uint_t common_hal_socketpool_socket_sendto(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port, const uint8_t *buf, uint32_t len) {
    if (self->type != SOCK_DGRAM) {
        mp_raise_ValueError(MP_ERROR_TEXT("Unsupported socket type"));
    }
    socketpool_socket_start_client_mode(self, host, hostlen, port, AIRLIFT_UDP_MODE);
    return common_hal_socketpool_socket_send(self, buf, len);
}

void common_hal_socketpool_socket_settimeout(socketpool_socket_obj_t *self, uint32_t timeout_ms) {
    // 0 means non-blocking. (uint32_t) -1 means block forever.
    self->timeout_ms = timeout_ms;
}

mp_int_t common_hal_socketpool_socket_get_type(socketpool_socket_obj_t *self) {
    return self->type;
}


int common_hal_socketpool_socket_setsockopt(socketpool_socket_obj_t *self, int level, int optname, const void *value, size_t optlen) {
    // TODO: not sure any of this can be implemented.
    return 0;
}

bool common_hal_socketpool_socket_readable(socketpool_socket_obj_t *self) {
    return socketpool_socket_poll(self) & SOCKET_POLL_RD;
}

bool common_hal_socketpool_socket_writable(socketpool_socket_obj_t *self) {
    return socketpool_socket_poll(self) & SOCKET_POLL_WR;
}

void socketpool_socket_move(socketpool_socket_obj_t *self, socketpool_socket_obj_t *sock) {
    *sock = *self;
    self->connected = false;
    self->num = NO_SOCKET;
}

void socketpool_socket_reset(socketpool_socket_obj_t *self) {
    self->num = NO_SOCKET;
    self->bound = false;
    self->connected = false;
    self->client_started = false;
    self->server_started = false;
    self->connected = false;
}
