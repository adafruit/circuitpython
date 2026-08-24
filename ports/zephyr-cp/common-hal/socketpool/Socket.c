// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/socketpool/Socket.h"

#include "shared/runtime/interrupt_char.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "common-hal/socketpool/__init__.h"
#include "common-hal/wifi/__init__.h"
#if CIRCUITPY_SSL
#include "shared-bindings/ssl/SSLSocket.h"
#include "shared-module/ssl/SSLSocket.h"
#endif
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"
#include "supervisor/workflow.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/net/dns_resolve.h>
#include <zephyr/net/socket.h>

#define SOCKETPOOL_IP_STR_LEN 48

static mp_obj_t _format_address(const struct sockaddr *addr, int family) {
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

static mp_obj_t _sockaddr_to_tuple(const struct sockaddr_storage *sockaddr) {
    mp_obj_t args[4] = {
        _format_address((const struct sockaddr *)sockaddr, sockaddr->ss_family),
    };
    int n = 2;
    #if CIRCUITPY_SOCKETPOOL_IPV6
    if (sockaddr->ss_family == AF_INET6) {
        const struct sockaddr_in6 *addr6 = (const void *)sockaddr;
        args[1] = MP_OBJ_NEW_SMALL_INT(ntohs(addr6->sin6_port));
        args[2] = MP_OBJ_NEW_SMALL_INT(addr6->sin6_flowinfo);
        args[3] = MP_OBJ_NEW_SMALL_INT(addr6->sin6_scope_id);
        n = 4;
    } else
    #endif
    {
        const struct sockaddr_in *addr = (const void *)sockaddr;
        args[1] = MP_OBJ_NEW_SMALL_INT(ntohs(addr->sin_port));
    }
    return mp_obj_new_tuple(n, args);
}

static void socketpool_resolve_host_or_throw(int family, int type, const char *hostname, struct sockaddr_storage *addr, int port) {
    const struct zsock_addrinfo hints = {
        .ai_family = family,
        .ai_socktype = type,
    };
    struct zsock_addrinfo *result_i = NULL;
    char service_buf[6];

    snprintf(service_buf, sizeof(service_buf), "%d", port);

    int error = zsock_getaddrinfo(hostname, service_buf, &hints, &result_i);
    if (error != 0 || result_i == NULL) {
        // Report what the resolver said. A null result with no error is the
        // one case that really is "not found".
        if (error == 0) {
            error = DNS_EAI_NONAME;
        }
        common_hal_socketpool_socketpool_raise_gaierror(error);
    }

    memcpy(addr, result_i->ai_addr, sizeof(struct sockaddr_storage));
    zsock_freeaddrinfo(result_i);
}

static void resolve_host_or_throw(socketpool_socket_obj_t *self, const char *hostname, struct sockaddr_storage *addr, int port) {
    socketpool_resolve_host_or_throw(self->family, self->type, hostname, addr, port);
}

// How long to wait between checks for a socket to connect.
#define SOCKET_CONNECT_POLL_INTERVAL_MS 100

void socket_user_reset(void) {
    // User sockets are heap objects with __del__ bound to close().
    // During VM shutdown/reset, gc_sweep_all() runs finalizers, so sockets
    // are closed there rather than being tracked and closed explicitly here.
}

static struct k_work_delayable socketpool_poll_work;
static bool socketpool_poll_work_initialized;

static void socketpool_poll_work_handler(struct k_work *work) {
    (void)work;
    supervisor_workflow_request_background();
}

// Unblock select task (ok if not blocked yet)
void socketpool_socket_poll_resume(void) {
    if (!socketpool_poll_work_initialized) {
        k_work_init_delayable(&socketpool_poll_work, socketpool_poll_work_handler);
        socketpool_poll_work_initialized = true;
    }
    k_work_schedule(&socketpool_poll_work, K_MSEC(10));
}

static bool _socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type,
    int proto,
    socketpool_socket_obj_t *sock) {
    int addr_family;
    int ipproto;

    if (family == SOCKETPOOL_AF_INET) {
        addr_family = AF_INET;
        ipproto = IPPROTO_IP;
    #if CIRCUITPY_SOCKETPOOL_IPV6
    } else { // INET6
        addr_family = AF_INET6;
        ipproto = IPPROTO_IPV6;
    #endif
    }

    int socket_type;
    if (type == SOCKETPOOL_SOCK_STREAM) {
        socket_type = SOCK_STREAM;
    } else if (type == SOCKETPOOL_SOCK_DGRAM) {
        socket_type = SOCK_DGRAM;
    } else { // SOCKETPOOL_SOCK_RAW
        socket_type = SOCK_RAW;
        ipproto = proto;
    }
    sock->type = socket_type;
    sock->family = addr_family;
    sock->ipproto = ipproto;
    sock->pool = self;
    sock->timeout_ms = (uint)-1;

    int socknum = zsock_socket(sock->family, sock->type, sock->ipproto);
    if (socknum < 0) {
        return false;
    }

    sock->num = socknum;

    // Enable address reuse by default to avoid bind failures on recently-used ports.
    int reuseaddr = 1;
    if (zsock_setsockopt(socknum, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) < 0) {
        // Ignore if SO_REUSEADDR is unsupported.
    }

    // Sockets should be nonblocking in most cases.
    if (zsock_fcntl(socknum, F_SETFL, ZVFS_O_NONBLOCK) < 0) {
        // Ignore if non-blocking is unsupported.
    }

    return true;
}

// special entry for workflow listener (register system socket)
bool socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type,
    int proto, socketpool_socket_obj_t *sock) {

    if (!_socketpool_socket(self, family, type, proto, sock)) {
        return false;
    }

    return true;
}

socketpool_socket_obj_t *common_hal_socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type, int proto) {
    switch (family) {
        #if CIRCUITPY_SOCKETPOOL_IPV6
        case SOCKETPOOL_AF_INET6:
        #endif
        case SOCKETPOOL_AF_INET:
            break;
        default:
            mp_raise_NotImplementedError(MP_ERROR_TEXT("Unsupported socket type"));
    }

    socketpool_socket_obj_t *sock = mp_obj_malloc_with_finaliser(socketpool_socket_obj_t, &socketpool_socket_type);

    if (!_socketpool_socket(self, family, type, proto, sock)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Out of sockets"));
    }
    return sock;
}

int socketpool_socket_accept(socketpool_socket_obj_t *self, mp_obj_t *peer_out, socketpool_socket_obj_t *accepted) {
    struct sockaddr_storage peer_addr;
    socklen_t socklen = sizeof(peer_addr);
    int newsoc = -1;
    bool timed_out = false;
    uint64_t start_ticks = supervisor_ticks_ms64();

    // Allow timeouts and interrupts
    while (newsoc == -1 && !timed_out) {
        if (self->timeout_ms != (uint)-1 && self->timeout_ms != 0) {
            timed_out = supervisor_ticks_ms64() - start_ticks >= self->timeout_ms;
        }
        RUN_BACKGROUND_TASKS;
        newsoc = zsock_accept(self->num, (struct sockaddr *)&peer_addr, &socklen);
        // In non-blocking mode, fail instead of timing out
        if (newsoc == -1 && (self->timeout_ms == 0 || mp_hal_is_interrupted())) {
            return -MP_EAGAIN;
        }
        if (newsoc == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
            return -errno;
        }
    }

    if (timed_out) {
        return -ETIMEDOUT;
    }

    if (newsoc < 0) {
        return -MP_EBADF;
    }

    // We got a socket. New client socket will not be non-blocking by default, so make it non-blocking.
    if (zsock_fcntl(newsoc, F_SETFL, ZVFS_O_NONBLOCK) < 0) {
        // Ignore if non-blocking is unsupported.
    }

    if (accepted != NULL) {
        // Error if called with open socket object.
        assert(common_hal_socketpool_socket_get_closed(accepted));

        // Replace the old accepted socket with the new one.
        accepted->num = newsoc;
        accepted->pool = self->pool;
        accepted->connected = true;
        accepted->type = self->type;
    }

    if (peer_out) {
        *peer_out = _sockaddr_to_tuple(&peer_addr);
    }

    return newsoc;
}

socketpool_socket_obj_t *common_hal_socketpool_socket_accept(socketpool_socket_obj_t *self, mp_obj_t *peer_out) {
    // Set the socket type only after the socketpool_socket_accept succeeds, so that the
    // finaliser is not called on a bad socket.
    socketpool_socket_obj_t *sock = mp_obj_malloc_with_finaliser(socketpool_socket_obj_t, NULL);
    int newsoc = socketpool_socket_accept(self, peer_out, NULL);

    if (newsoc > 0) {
        // Create the socket
        sock->base.type = &socketpool_socket_type;
        sock->num = newsoc;
        sock->pool = self->pool;
        sock->connected = true;
        sock->type = self->type;

        return sock;
    } else {
        mp_raise_OSError(-newsoc);
        return NULL;
    }
}

size_t common_hal_socketpool_socket_bind(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port) {
    struct sockaddr_storage bind_addr;
    const char *broadcast = "<broadcast>";
    uint32_t local_port = port;

    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.ss_family = self->family;

    #if CIRCUITPY_SOCKETPOOL_IPV6
    if (self->family == AF_INET6) {
        struct sockaddr_in6 *addr6 = (void *)&bind_addr;
        addr6->sin6_port = htons(local_port);
        // no ipv6 broadcast
        if (hostlen == 0) {
            memset(&addr6->sin6_addr, 0, sizeof(addr6->sin6_addr));
        } else {
            socketpool_resolve_host_or_throw(self->family, self->type, host, &bind_addr, local_port);
        }
    } else
    #endif
    {
        struct sockaddr_in *addr4 = (void *)&bind_addr;
        addr4->sin_port = htons(local_port);
        if (hostlen == 0) {
            addr4->sin_addr.s_addr = htonl(INADDR_ANY);
        } else if (hostlen == strlen(broadcast) &&
                   memcmp(host, broadcast, strlen(broadcast)) == 0) {
            addr4->sin_addr.s_addr = htonl(INADDR_BROADCAST);
        } else {
            socketpool_resolve_host_or_throw(self->family, self->type, host, &bind_addr, local_port);
        }
    }

    int result = zsock_bind(self->num, (struct sockaddr *)&bind_addr, sizeof(bind_addr));
    if (result == 0) {
        return 0;
    }
    return errno;
}

void socketpool_socket_close(socketpool_socket_obj_t *self) {
    #if CIRCUITPY_SSL
    if (self->ssl_socket) {
        ssl_sslsocket_obj_t *ssl_socket = self->ssl_socket;
        self->ssl_socket = NULL;
        common_hal_ssl_sslsocket_close(ssl_socket);
        return;
    }
    #endif
    self->connected = false;
    int fd = self->num;
    if (fd >= 0) {
        zsock_shutdown(fd, ZSOCK_SHUT_RDWR);
        zsock_close(fd);
    }
    self->num = -1;
}

void common_hal_socketpool_socket_close(socketpool_socket_obj_t *self) {
    socketpool_socket_close(self);
}

void common_hal_socketpool_socket_connect(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port) {
    (void)hostlen;
    struct sockaddr_storage addr;
    resolve_host_or_throw(self, host, &addr, port);

    int result = zsock_connect(self->num, (struct sockaddr *)&addr, sizeof(addr));

    if (result == 0) {
        // Connected immediately.
        self->connected = true;
        return;
    }

    if (result < 0 && errno != EINPROGRESS) {
        // Some error happened; error is in errno.
        mp_raise_OSError(errno);
        return;
    }

    // Keep checking, using poll(), until timeout expires, at short intervals.
    // This allows ctrl-C interrupts to be detected and background tasks to run.
    mp_uint_t timeout_left = self->timeout_ms;

    while (timeout_left > 0) {
        RUN_BACKGROUND_TASKS;
        // Allow ctrl-C interrupt
        if (mp_hal_is_interrupted()) {
            return;
        }

        struct zsock_pollfd fd = {
            .fd = self->num,
            .events = ZSOCK_POLLOUT,
        };
        int poll_timeout = SOCKET_CONNECT_POLL_INTERVAL_MS;
        if (self->timeout_ms == (uint)-1) {
            poll_timeout = -1;
        } else if (timeout_left < SOCKET_CONNECT_POLL_INTERVAL_MS) {
            poll_timeout = timeout_left;
        }

        result = zsock_poll(&fd, 1, poll_timeout);
        if (result == 0) {
            if (self->timeout_ms == (uint)-1) {
                continue;
            }
            if (timeout_left < SOCKET_CONNECT_POLL_INTERVAL_MS) {
                timeout_left = 0;
            } else {
                timeout_left -= SOCKET_CONNECT_POLL_INTERVAL_MS;
            }
            continue;
        }

        if (result < 0) {
            mp_raise_OSError(errno);
        }

        int error_code = 0;
        socklen_t socklen = sizeof(error_code);
        result = zsock_getsockopt(self->num, SOL_SOCKET, SO_ERROR, &error_code, &socklen);
        if (result < 0 || error_code != 0) {
            mp_raise_OSError(error_code != 0 ? error_code : errno);
        }
        self->connected = true;
        return;
    }

    // No connection after timeout. The connection attempt is not stopped.
    // This imitates what happens in Python.
    mp_raise_OSError(ETIMEDOUT);
}

bool common_hal_socketpool_socket_get_closed(socketpool_socket_obj_t *self) {
    return self->num < 0;
}

bool common_hal_socketpool_socket_get_connected(socketpool_socket_obj_t *self) {
    return self->connected;
}

bool common_hal_socketpool_socket_listen(socketpool_socket_obj_t *self, int backlog) {
    return zsock_listen(self->num, backlog) == 0;
}

mp_uint_t common_hal_socketpool_socket_recvfrom_into(socketpool_socket_obj_t *self,
    uint8_t *buf, uint32_t len, mp_obj_t *source_out) {

    struct sockaddr_storage source_addr;
    socklen_t socklen = sizeof(source_addr);

    uint64_t start_ticks = supervisor_ticks_ms64();
    int received = -1;
    bool timed_out = false;
    while (received == -1 &&
           !timed_out &&
           !mp_hal_is_interrupted()) {
        if (self->timeout_ms != (uint)-1 && self->timeout_ms != 0) {
            timed_out = supervisor_ticks_ms64() - start_ticks >= self->timeout_ms;
        }
        RUN_BACKGROUND_TASKS;
        received = zsock_recvfrom(self->num, buf, len, ZSOCK_MSG_DONTWAIT, (struct sockaddr *)&source_addr, &socklen);

        if (received < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            mp_raise_OSError(errno);
        }

        if (received == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            // In non-blocking mode, fail instead of looping
            if (self->timeout_ms == 0) {
                mp_raise_OSError(MP_EAGAIN);
            }
            continue;
        }
    }

    if (timed_out) {
        mp_raise_OSError(ETIMEDOUT);
    }

    if (received < 0) {
        mp_raise_BrokenPipeError();
        return 0;
    }

    if (source_out) {
        *source_out = _sockaddr_to_tuple(&source_addr);
    }

    return received;
}

int socketpool_socket_recv_into(socketpool_socket_obj_t *self,
    const uint8_t *buf, uint32_t len) {
    int received = 0;
    bool timed_out = false;

    if (self->num != -1) {
        uint64_t start_ticks = supervisor_ticks_ms64();
        received = -1;
        while (received == -1 &&
               !timed_out) {
            if (self->timeout_ms != (uint)-1 && self->timeout_ms != 0) {
                timed_out = supervisor_ticks_ms64() - start_ticks >= self->timeout_ms;
            }
            RUN_BACKGROUND_TASKS;
            received = zsock_recv(self->num, (void *)buf, len, ZSOCK_MSG_DONTWAIT);
            if (received < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                return -errno;
            }
            if (received < 1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                if (self->timeout_ms == 0) {
                    return -MP_EAGAIN;
                }
                continue;
            }
            // In non-blocking mode, fail instead of looping
            if (received < 1 && self->timeout_ms == 0) {
                if ((received == 0) || (errno == ENOTCONN)) {
                    self->connected = false;
                    return -MP_ENOTCONN;
                }
                return -MP_EAGAIN;
            }
            // Check this after going through the loop once so it can make
            // progress while interrupted.
            if (mp_hal_is_interrupted()) {
                if (received == -1) {
                    return -MP_EAGAIN;
                }
                break;
            }
        }
    } else {
        return -MP_EBADF;
    }

    if (timed_out) {
        return -ETIMEDOUT;
    }
    return received;
}

mp_uint_t common_hal_socketpool_socket_recv_into(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int received = socketpool_socket_recv_into(self, buf, len);
    if (received < 0) {
        mp_raise_OSError(-received);
    }
    return received;
}

int socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int sent = -1;
    if (self->num != -1) {
        sent = zsock_send(self->num, buf, len, 0);
    } else {
        sent = -MP_EBADF;
    }

    if (sent < 0) {
        if (errno == ECONNRESET || errno == ENOTCONN) {
            self->connected = false;
        }
        return -errno;
    }

    return sent;
}

mp_uint_t common_hal_socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int sent = socketpool_socket_send(self, buf, len);

    if (sent < 0) {
        mp_raise_OSError(-sent);
    }
    return sent;
}

mp_uint_t common_hal_socketpool_socket_sendto(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port, const uint8_t *buf, uint32_t len) {

    (void)hostlen;
    struct sockaddr_storage addr;
    resolve_host_or_throw(self, host, &addr, port);

    int bytes_sent = zsock_sendto(self->num, buf, len, 0, (struct sockaddr *)&addr, sizeof(addr));
    if (bytes_sent < 0) {
        mp_raise_BrokenPipeError();
        return 0;
    }
    return bytes_sent;
}

void common_hal_socketpool_socket_settimeout(socketpool_socket_obj_t *self, uint32_t timeout_ms) {
    self->timeout_ms = timeout_ms;
}

mp_int_t common_hal_socketpool_socket_get_type(socketpool_socket_obj_t *self) {
    return self->type;
}


int common_hal_socketpool_socket_setsockopt(socketpool_socket_obj_t *self, int level, int optname, const void *value, size_t optlen) {
    int zephyr_level = level;
    int zephyr_optname = optname;

    switch (level) {
        case SOCKETPOOL_SOL_SOCKET:
            zephyr_level = SOL_SOCKET;
            break;
        case SOCKETPOOL_IPPROTO_IP:
            zephyr_level = IPPROTO_IP;
            break;
        case SOCKETPOOL_IPPROTO_TCP:
            zephyr_level = IPPROTO_TCP;
            break;
        case SOCKETPOOL_IPPROTO_UDP:
            zephyr_level = IPPROTO_UDP;
            break;
        #if CIRCUITPY_SOCKETPOOL_IPV6
        case SOCKETPOOL_IPPROTO_IPV6:
            zephyr_level = IPPROTO_IPV6;
            break;
        #endif
    }

    if (zephyr_level == SOL_SOCKET) {
        switch (optname) {
            case SOCKETPOOL_SO_REUSEADDR:
                zephyr_optname = SO_REUSEADDR;
                break;
        }
    } else if (zephyr_level == IPPROTO_TCP) {
        switch (optname) {
            case SOCKETPOOL_TCP_NODELAY:
                zephyr_optname = TCP_NODELAY;
                break;
        }
    }

    int err = zsock_setsockopt(self->num, zephyr_level, zephyr_optname, value, optlen);
    if (err != 0) {
        return -errno;
    }
    return 0;
}

bool common_hal_socketpool_readable(socketpool_socket_obj_t *self) {
    struct zsock_pollfd fd = {
        .fd = self->num,
        .events = ZSOCK_POLLIN,
    };

    int num_triggered = zsock_poll(&fd, 1, 0);
    return num_triggered > 0;
}

bool common_hal_socketpool_writable(socketpool_socket_obj_t *self) {
    struct zsock_pollfd fd = {
        .fd = self->num,
        .events = ZSOCK_POLLOUT,
    };

    int num_triggered = zsock_poll(&fd, 1, 0);
    return num_triggered > 0;
}

void socketpool_socket_move(socketpool_socket_obj_t *self, socketpool_socket_obj_t *sock) {
    *sock = *self;
    self->connected = false;
    self->num = -1;
}

void socketpool_socket_reset(socketpool_socket_obj_t *self) {
    if (self->base.type == &socketpool_socket_type) {
        return;
    }
    self->base.type = &socketpool_socket_type;
    self->connected = false;
    self->num = -1;
}
