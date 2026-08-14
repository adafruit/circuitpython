// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/ring_buffer.h>

#include "py/runtime.h"
#include "py/stream.h"

#include "shared/runtime/interrupt_char.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/PacketBuffer.h"

#include "supervisor/shared/tick.h"
#include "supervisor/port_heap.h"

#include "common-hal/_bleio/Characteristic.h"
#include "common-hal/_bleio/PacketBuffer.h"
#include "common-hal/_bleio/Adapter.h"
#include "common-hal/_bleio/Connection.h"
#include "common-hal/_bleio/__init__.h"
#include "bindings/zephyr_kernel/__init__.h"

// Zephyr's ring_buf is safe for single-producer/single-consumer without
// locks. The GATT callbacks (system workqueue) are the sole producer of
// incoming data; the CircuitPython VM (main thread) is the sole consumer.

// Forward declarations.
static bool conn_is_valid(bleio_packet_buffer_obj_t *self);
static void packet_buffer_send_work_handler(struct k_work *work);
static void notify_complete_cb(struct bt_conn *conn, void *user_data);

// Called from Zephyr GATT callbacks (system workqueue context).
// Wraps incoming data with a uint16_t length prefix and pushes into ringbuf.
// Tracks the first connection so notifications target the right peer.
void bleio_packet_buffer_extend(bleio_packet_buffer_obj_t *self,
    struct bt_conn *conn, const uint8_t *data, size_t len) {
    // Track the first/current connection for notifications.
    if (conn != NULL) {
        conn_is_valid(self);  // clear stale conn if needed
        if (self->conn == NULL) {
            self->conn = conn;
        }
    }
    if (len > UINT16_MAX) {
        return;
    }

    uint16_t packet_len = (uint16_t)len;
    size_t total = sizeof(uint16_t) + len;

    // If the packet doesn't fit, drop oldest packets to make room.
    while (ring_buf_space_get(&self->ringbuf) < total) {
        uint16_t old_len;
        if (ring_buf_size_get(&self->ringbuf) < sizeof(uint16_t)) {
            // Not enough data for a length prefix, just reset.
            ring_buf_reset(&self->ringbuf);
            break;
        }
        uint32_t peeked = ring_buf_peek(&self->ringbuf, (uint8_t *)&old_len, sizeof(uint16_t));
        if (peeked < sizeof(uint16_t)) {
            ring_buf_reset(&self->ringbuf);
            break;
        }
        // Discard the length prefix
        ring_buf_get(&self->ringbuf, (uint8_t *)&old_len, sizeof(uint16_t));
        // Discard the packet data
        size_t to_discard = old_len;
        if (to_discard > ring_buf_size_get(&self->ringbuf)) {
            to_discard = ring_buf_size_get(&self->ringbuf);
        }
        uint8_t discard_buf[32];
        while (to_discard > 0) {
            size_t chunk = to_discard > sizeof(discard_buf) ? sizeof(discard_buf) : to_discard;
            ring_buf_get(&self->ringbuf, discard_buf, chunk);
            to_discard -= chunk;
        }
    }

    ring_buf_put(&self->ringbuf, (uint8_t *)&packet_len, sizeof(uint16_t));
    ring_buf_put(&self->ringbuf, data, len);

    // Wake the VM background task so it drains the ring buffer promptly.
    bleio_request_bluetooth_background();
}

void bleio_packet_buffer_set_conn(bleio_packet_buffer_obj_t *self,
    struct bt_conn *conn) {
    self->conn = conn;
}

// Completion callback for bt_gatt_notify_cb. Runs on the system workqueue (per
// the Zephyr bt_gatt_notify_cb contract) — the same context send_work runs in
// — so notify_in_flight is single-threaded and needs no lock.
static void notify_complete_cb(struct bt_conn *conn, void *user_data) {
    (void)conn;
    bleio_packet_buffer_obj_t *self = (bleio_packet_buffer_obj_t *)user_data;
    self->notify_in_flight = false;
    // Drain any data that accumulated in the pipe while this notify was in
    // flight. k_work_submit is a no-op if the work is already queued.
    k_work_submit(&self->send_work);
}

// Returns true if the tracked connection is still connected.
// Clears self->conn if the connection is stale.
static bool conn_is_valid(bleio_packet_buffer_obj_t *self) {
    if (self->conn == NULL) {
        return false;
    }
    struct bt_conn_info info;
    if (bt_conn_get_info(self->conn, &info) != 0 ||
        info.state == BT_CONN_STATE_DISCONNECTED) {
        self->conn = NULL;
        return false;
    }
    return true;
}

// The deferred sender. Runs only on the system workqueue (submitted by the VM
// on write/flush and by notify_complete_cb on completion). It is the sole
// consumer of outgoing_pipe, and the only toucher of send_size /
// notify_in_flight / outgoing_buffer (server-side), so those need no lock.
static void packet_buffer_send_work_handler(struct k_work *work) {
    bleio_packet_buffer_obj_t *self =
        CONTAINER_OF(work, bleio_packet_buffer_obj_t, send_work);

    // A notification is awaiting its completion callback; it will resubmit us.
    if (self->notify_in_flight) {
        return;
    }

    // If nothing is staged from a previous attempt, pull from the pipe.
    if (self->send_size == 0) {
        bleio_characteristic_obj_t *c = self->characteristic;
        if (c == NULL || c->service == NULL || c->service->is_remote) {
            return;  // server-side notify path only; clients write directly
        }
        if (!(c->props & CHAR_PROP_NOTIFY) || !c->service->registered) {
            return;
        }

        mp_int_t opl = common_hal_bleio_packet_buffer_get_outgoing_packet_length(self);
        if (opl <= 0) {
            return;  // no connection / MTU yet
        }
        size_t to_read = MIN((size_t)opl, self->max_packet_size);
        int n = k_pipe_read(&self->outgoing_pipe, self->outgoing_buffer,
            to_read, K_NO_WAIT);
        if (n <= 0) {
            return;  // pipe empty (-EAGAIN or 0)
        }
        self->send_size = (size_t)n;
    }

    // We have send_size bytes staged in outgoing_buffer. Try to notify.
    bleio_characteristic_obj_t *c = self->characteristic;

    // If the tracked connection is stale, clear it.
    conn_is_valid(self);

    struct bt_gatt_notify_params params = {
        .attr = &c->service->attrs[c->value_attr_index],
        .data = self->outgoing_buffer,
        .len = self->send_size,
        .func = notify_complete_cb,
        .user_data = self,
    };

    int err = bt_gatt_notify_cb(self->conn, &params);
    if (err == 0) {
        // Accepted by the controller; the buffer was copied and will be sent
        // on-air at the next connection event. Count it as handed off.
        atomic_sub(&self->outgoing_pending, (atomic_val_t)self->send_size);
        self->send_size = 0;
        self->notify_in_flight = true;
        // notify_complete_cb clears notify_in_flight and resubmits to drain
        // any further accumulated data.
        return;
    }
    if (err == -ENOTCONN) {
        // Peer disconnected — discard everything pending. (We're here only
        // when notify_in_flight is clear, so no in-flight completion is owed.)
        self->conn = NULL;
        self->send_size = 0;
        k_pipe_reset(&self->outgoing_pipe);   // drop unread data
        atomic_set(&self->outgoing_pending, 0);
        return;
    }
    // -ENOMEM (no TX buffer available right now) — leave send_size staged and
    // rely on the next completion (a TX buffer will free up, since -ENOMEM
    // means the pool is full i.e. notifications are in flight), the next
    // write(), or flush() to resubmit. We must NOT resubmit here: that would
    // busy-loop the workqueue against a full pool.
}

// Shared core for both the Python-facing (allocating) and workflow
// (caller-supplied static buffer) constructors. Wires the ring buffer,
// outgoing buffers, characteristic observer, and (for client-side) CCCD
// subscription. No GC heap allocation happens here.
static void packet_buffer_init_common(bleio_packet_buffer_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    uint8_t *ringbuf_data, size_t ringbuf_size, bool owns_ringbuf_data,
    uint8_t *outgoing_buffer, bool owns_outgoing_buffer,
    uint8_t *pipe_buffer, size_t pipe_size, bool owns_pipe_buffer,
    size_t max_packet_size) {

    self->characteristic = characteristic;
    self->timeout_ms = 0;
    self->max_packet_size = max_packet_size;
    self->conn = NULL;
    self->client = (characteristic->service != NULL && characteristic->service->is_remote);
    self->outgoing_buffer = outgoing_buffer;
    self->owns_outgoing_buffer = owns_outgoing_buffer;
    self->pipe_buffer = pipe_buffer;
    self->pipe_size = pipe_size;
    self->owns_pipe_buffer = owns_pipe_buffer;
    self->send_size = 0;
    self->notify_in_flight = false;
    atomic_set(&self->outgoing_pending, 0);

    self->ringbuf_data = ringbuf_data;
    self->ringbuf_size = ringbuf_size;
    self->owns_ringbuf_data = owns_ringbuf_data;
    if (ringbuf_data != NULL && ringbuf_size > 0) {
        ring_buf_init(&self->ringbuf, ringbuf_size, ringbuf_data);
    } else {
        // No incoming buffer (e.g. a server-side NOTIFY-only characteristic).
        ring_buf_init(&self->ringbuf, 0, NULL);
    }

    // The pipe feeds the deferred notifier. Only server-side notify
    // characteristics need it; for client-side it's left empty (the client
    // writes directly and synchronously).
    if (pipe_buffer != NULL && pipe_size > 0) {
        k_pipe_init(&self->outgoing_pipe, pipe_buffer, pipe_size);
        k_work_init(&self->send_work, packet_buffer_send_work_handler);
    }

    // Set ourselves as the characteristic's observer so GATT write/notify
    // callbacks push incoming data into our ring buffer.
    bleio_characteristic_set_observer(characteristic, MP_OBJ_FROM_PTR(self));

    bleio_characteristic_properties_t props =
        common_hal_bleio_characteristic_get_properties(characteristic);

    // For client-side characteristics with NOTIFY/INDICATE, subscribe to
    // notifications from the remote peer.
    if (self->client && (props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE))) {
        bool do_notify = (props & CHAR_PROP_NOTIFY) != 0;
        bool do_indicate = (props & CHAR_PROP_INDICATE) != 0;
        common_hal_bleio_characteristic_set_cccd(characteristic, do_notify, do_indicate);
    }
}

void common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    size_t buffer_size, size_t max_packet_size) {

    bleio_characteristic_properties_t props =
        common_hal_bleio_characteristic_get_properties(characteristic);
    bool client = (characteristic->service != NULL && characteristic->service->is_remote);

    // Allocate ring buffer: buffer_size packets, each with 2-byte length prefix
    size_t ringbuf_size = buffer_size * (sizeof(uint16_t) + max_packet_size);
    uint8_t *ringbuf_data = port_malloc(ringbuf_size, false);

    // Allocate outgoing buffers. Client-side writes go out directly and only
    // need one scratch buffer. Server-side notifications use a k_pipe (backed
    // by pipe_buffer) plus a notify buffer (outgoing_buffer) the workqueue
    // drains into.
    uint8_t *outgoing_buffer = NULL;
    uint8_t *pipe_buffer = NULL;
    bool owns_outgoing = false;
    bool owns_pipe = false;
    if (client) {
        outgoing_buffer = port_malloc(max_packet_size, false);
        owns_outgoing = true;
    } else if (props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE)) {
        outgoing_buffer = port_malloc(max_packet_size, false);
        pipe_buffer = port_malloc(max_packet_size, false);
        owns_outgoing = true;
        owns_pipe = true;
    }

    packet_buffer_init_common(self, characteristic,
        ringbuf_data, ringbuf_size, true,
        outgoing_buffer, owns_outgoing,
        pipe_buffer, max_packet_size, owns_pipe,
        max_packet_size);
}

// Allocation-free version for BLE workflow use. The caller supplies static
// buffers so this can run before gc_init() without touching the GC heap.
// outgoing_buffer1 backs the pipe; outgoing_buffer2 is the notify buffer.
void _common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    uint32_t *incoming_buffer, size_t incoming_buffer_size,
    uint32_t *outgoing_buffer1, uint32_t *outgoing_buffer2, size_t max_packet_size,
    ble_event_handler_t *static_handler_entry) {
    (void)static_handler_entry;

    uint8_t *ringbuf_data = (uint8_t *)incoming_buffer;
    size_t ringbuf_size = incoming_buffer_size;

    bleio_characteristic_properties_t props =
        common_hal_bleio_characteristic_get_properties(characteristic);
    bool client = (characteristic->service != NULL && characteristic->service->is_remote);

    uint8_t *outgoing_buffer = NULL;  // client: write scratch; server: notify buf
    uint8_t *pipe_buffer = NULL;     // server: k_pipe backing store
    bool owns_outgoing = false;
    bool owns_pipe = false;

    if (client) {
        // Client-side: outgoing_buffer1 is the write scratch; no pipe needed.
        if (outgoing_buffer1 != NULL) {
            outgoing_buffer = (uint8_t *)outgoing_buffer1;
        } else {
            outgoing_buffer = port_malloc(max_packet_size, false);
            owns_outgoing = true;
        }
    } else if (props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE)) {
        // Server-side: outgoing_buffer1 backs the pipe, outgoing_buffer2 is the
        // notify buffer the workqueue drains into.
        if (outgoing_buffer1 != NULL) {
            pipe_buffer = (uint8_t *)outgoing_buffer1;
        } else {
            pipe_buffer = port_malloc(max_packet_size, false);
            owns_pipe = true;
        }
        if (outgoing_buffer2 != NULL) {
            outgoing_buffer = (uint8_t *)outgoing_buffer2;
        } else {
            outgoing_buffer = port_malloc(max_packet_size, false);
            owns_outgoing = true;
        }
    }

    packet_buffer_init_common(self, characteristic,
        ringbuf_data, ringbuf_size, false,
        outgoing_buffer, owns_outgoing,
        pipe_buffer, max_packet_size, owns_pipe,
        max_packet_size);
}

mp_int_t common_hal_bleio_packet_buffer_readinto(bleio_packet_buffer_obj_t *self,
    uint8_t *data, size_t len) {
    // Need at least 2 bytes for the length prefix
    if (ring_buf_size_get(&self->ringbuf) < sizeof(uint16_t)) {
        return 0;
    }

    // Peek at the packet length (don't consume yet)
    uint16_t packet_length;
    uint32_t peeked = ring_buf_peek(&self->ringbuf, (uint8_t *)&packet_length, sizeof(uint16_t));
    if (peeked < sizeof(uint16_t)) {
        return 0;
    }

    mp_int_t ret;
    if (packet_length > len) {
        // Packet is longer than requested. Return negative of overrun value.
        ret = len - packet_length;
        // Discard the packet
        ring_buf_get(&self->ringbuf, (uint8_t *)&packet_length, sizeof(uint16_t));
        if (packet_length <= ring_buf_size_get(&self->ringbuf)) {
            // Discard data in chunks
            uint8_t discard[32];
            size_t remaining = packet_length;
            while (remaining > 0) {
                size_t chunk = remaining > sizeof(discard) ? sizeof(discard) : remaining;
                ring_buf_get(&self->ringbuf, discard, chunk);
                remaining -= chunk;
            }
        }
    } else {
        // Consume the length prefix
        ring_buf_get(&self->ringbuf, (uint8_t *)&packet_length, sizeof(uint16_t));
        // Read packet data
        ring_buf_get(&self->ringbuf, data, packet_length);
        ret = packet_length;
    }

    return ret;
}

mp_int_t common_hal_bleio_packet_buffer_write(bleio_packet_buffer_obj_t *self,
    const uint8_t *data, size_t len, uint8_t *header, size_t header_len) {
    if (self->outgoing_buffer == NULL) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Writes not supported on Characteristic"));
    }

    mp_int_t outgoing_packet_length =
        common_hal_bleio_packet_buffer_get_outgoing_packet_length(self);
    if (outgoing_packet_length < 0) {
        return -1;
    }

    mp_int_t total_len = len + header_len;
    if (total_len > outgoing_packet_length) {
        mp_raise_ValueError_varg(
            MP_ERROR_TEXT("Total data to write is larger than %q"),
            MP_QSTR_outgoing_packet_length);
    }
    if (total_len > (mp_int_t)self->max_packet_size) {
        mp_raise_ValueError_varg(
            MP_ERROR_TEXT("Total data to write is larger than %q"),
            MP_QSTR_max_packet_size);
    }

    // Client-side (remote characteristic): write the request directly to the
    // remote GATT server. The server-side path below buffers outgoing data in a
    // k_pipe and drains it from the system workqueue via send_work, so handle
    // client writes separately (and synchronously).
    if (self->characteristic != NULL &&
        self->characteristic->service != NULL &&
        self->characteristic->service->is_remote) {
        bleio_characteristic_obj_t *c = self->characteristic;
        bleio_connection_obj_t *connection = MP_OBJ_TO_PTR(c->service->connection);
        if (connection == NULL || connection->connection == NULL ||
            connection->connection->conn == NULL) {
            return -1;
        }
        struct bt_conn *conn = connection->connection->conn;

        // Combine header + data into the outgoing buffer (max_packet_size).
        memcpy(self->outgoing_buffer, header, header_len);
        memcpy(self->outgoing_buffer + header_len, data, len);
        size_t total = header_len + len;

        if (c->props & CHAR_PROP_WRITE_NO_RESPONSE) {
            // Fire-and-forget write. Retry on transient "no TX buffer"
            // (-EAGAIN) so paced protocols (e.g. BLE file transfer) don't
            // silently drop data.
            int err;
            while ((err = bt_gatt_write_without_response(conn, c->handle,
                self->outgoing_buffer, total, false)) == -EAGAIN) {
                RUN_BACKGROUND_TASKS;
            }
            if (err != 0) {
                raise_zephyr_error(err);
            }
        } else if (c->props & CHAR_PROP_WRITE) {
            bleio_gattc_write_sync(conn, c->handle, self->outgoing_buffer, total);
        } else {
            // No write property; nothing to send.
            return -1;
        }
        return (mp_int_t)total;
    }

    // Server-side notify path: push bytes into the k_pipe. The pipe's built-in
    // spinlock serializes us (sole producer) against send_work (sole consumer),
    // so no manual lock is needed. If the pipe is full (send_work hasn't
    // drained the previous packet yet), spin cooperatively until it has room.
    //
    // The header is a per-packet prefix: it's only written when the outgoing
    // packet is empty (nothing pending), matching the documented write()
    // semantics. outgoing_pending is the same notion as the old pending_size:
    // bytes the controller hasn't accepted yet.
    bool include_header = (atomic_get(&self->outgoing_pending) == 0);

    if (mp_hal_is_interrupted()) {
        return -1;
    }

    // Push header (if included) then data into the pipe, accounting each chunk
    // in outgoing_pending as it lands. send_work may preempt us during
    // RUN_BACKGROUND_TASKS (the system workqueue runs at higher priority) and
    // drain+decrement those same bytes, so the increment must precede the
    // yield to keep the counter from going negative.
    mp_int_t num_bytes_written = 0;
    const uint8_t *segs[2];
    size_t seglens[2];
    int nsegs = 0;
    if (include_header && header_len > 0) {
        segs[nsegs] = header;
        seglens[nsegs] = header_len;
        nsegs++;
    }
    segs[nsegs] = data;
    seglens[nsegs] = len;
    nsegs++;

    for (int s = 0; s < nsegs && !mp_hal_is_interrupted(); s++) {
        const uint8_t *p = segs[s];
        size_t left = seglens[s];
        while (left > 0 && !mp_hal_is_interrupted()) {
            int wrote = k_pipe_write(&self->outgoing_pipe, p, left, K_NO_WAIT);
            if (wrote < 0) {
                wrote = 0;  // -EAGAIN: try again after yielding
            }
            if (wrote > 0) {
                atomic_add(&self->outgoing_pending, (atomic_val_t)wrote);
                num_bytes_written += wrote;
                p += wrote;
                left -= wrote;
            }
            if (left > 0) {
                RUN_BACKGROUND_TASKS;  // let send_work drain the pipe
            }
        }
    }
    if (mp_hal_is_interrupted()) {
        return -1;
    }

    // Kick the deferred sender. k_work_submit is a no-op if already queued, so
    // a burst of writes only schedules one drain.
    k_work_submit(&self->send_work);
    return num_bytes_written;
}

mp_int_t common_hal_bleio_packet_buffer_get_incoming_packet_length(
    bleio_packet_buffer_obj_t *self) {
    if (self->characteristic == NULL) {
        return -1;
    }

    if (self->characteristic->service != NULL &&
        self->characteristic->service->is_remote &&
        self->characteristic->service->connection != mp_const_none &&
        (common_hal_bleio_characteristic_get_properties(self->characteristic) &
         (CHAR_PROP_INDICATE | CHAR_PROP_NOTIFY))) {
        // We are receiving from a remote service via NOTIFY/INDICATE.
        bleio_connection_obj_t *connection =
            MP_OBJ_TO_PTR(self->characteristic->service->connection);
        if (connection != NULL && connection->connection != NULL &&
            common_hal_bleio_connection_get_connected(connection)) {
            return common_hal_bleio_connection_get_max_packet_length(connection->connection);
        }
        return -1;
    }
    return self->characteristic->max_length;
}

mp_int_t common_hal_bleio_packet_buffer_get_outgoing_packet_length(
    bleio_packet_buffer_obj_t *self) {
    if (self->characteristic == NULL) {
        return -1;
    }

    if (self->characteristic->service != NULL &&
        !self->characteristic->service->is_remote &&
        (common_hal_bleio_characteristic_get_properties(self->characteristic) &
         (CHAR_PROP_INDICATE | CHAR_PROP_NOTIFY))) {
        // We are sending to a client via NOTIFY/INDICATE. The maximum payload
        // per packet is bounded by the negotiated ATT MTU (ATT_MTU - 3 for the
        // opcode and handle in a Handle Value Notification PDU). Without a
        // current connection we can't know the MTU, so return -1 to signal
        // that writes aren't possible yet.
        if (!conn_is_valid(self)) {
            return -1;
        }
        uint16_t mtu = bt_gatt_get_mtu(self->conn);
        if (mtu < 3) {
            return -1;
        }
        mp_int_t mtu_payload = (mp_int_t)mtu - 3;
        return MIN(MIN(mtu_payload, (mp_int_t)self->max_packet_size),
            (mp_int_t)self->characteristic->max_length);
    }
    // Writing to remote characteristic or local without NOTIFY
    return MIN(self->characteristic->max_length, self->max_packet_size);
}

void common_hal_bleio_packet_buffer_flush(bleio_packet_buffer_obj_t *self) {
    // Wait until everything written has been handed to the controller (accepted
    // by bt_gatt_notify_cb). send_work runs on the system workqueue, which is
    // higher priority than this thread, so k_work_submit lets it preempt us;
    // RUN_BACKGROUND_TASKS advances simulated time / lets completion callbacks
    // fire so notify_in_flight clears.
    while (atomic_get(&self->outgoing_pending) > 0 &&
           !mp_hal_is_interrupted()) {
        k_work_submit(&self->send_work);
        RUN_BACKGROUND_TASKS;
    }
}

bool common_hal_bleio_packet_buffer_deinited(bleio_packet_buffer_obj_t *self) {
    return self->characteristic == NULL;
}

void common_hal_bleio_packet_buffer_deinit(bleio_packet_buffer_obj_t *self) {
    if (common_hal_bleio_packet_buffer_deinited(self)) {
        return;
    }
    bleio_characteristic_clear_observer(self->characteristic);
    self->characteristic = NULL;
    // Free buffers if we own them (port_malloc'd). The BLE workflow path
    // supplies static buffers that must not be freed.
    if (self->owns_ringbuf_data && self->ringbuf_data != NULL) {
        port_free(self->ringbuf_data);
    }
    self->ringbuf_data = NULL;
    if (self->owns_outgoing_buffer && self->outgoing_buffer != NULL) {
        port_free(self->outgoing_buffer);
    }
    self->outgoing_buffer = NULL;
    if (self->owns_pipe_buffer && self->pipe_buffer != NULL) {
        port_free(self->pipe_buffer);
    }
    self->pipe_buffer = NULL;
}

bool common_hal_bleio_packet_buffer_connected(bleio_packet_buffer_obj_t *self) {
    if (common_hal_bleio_packet_buffer_deinited(self)) {
        return false;
    }
    // Check if the characteristic's connection is still active.
    if (self->characteristic->service != NULL &&
        self->characteristic->service->is_remote) {
        if (self->characteristic->service->connection == mp_const_none) {
            return false;
        }
        bleio_connection_obj_t *connection =
            MP_OBJ_TO_PTR(self->characteristic->service->connection);
        return common_hal_bleio_connection_get_connected(connection);
    }
    return true;
}
