// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/ring_buffer.h>

#include "py/runtime.h"
#include "py/stream.h"

#include "shared/runtime/interrupt_char.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared-bindings/microcontroller/__init__.h"

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

// Completion callback for bt_gatt_notify_cb. Runs on the system workqueue (per
// the Zephyr bt_gatt_notify_cb contract) — the same context send_work runs in
// — so packet_queued is single-threaded here and needs no lock.
static void notify_complete_cb(struct bt_conn *conn, void *user_data) {
    (void)conn;
    bleio_packet_buffer_obj_t *self = (bleio_packet_buffer_obj_t *)user_data;
    self->packet_queued = false;
    // Drain the other buffer now that this notify is done. K_NO_WAIT runs
    // send_work immediately, cancelling any pending delayed retry.
    k_work_reschedule(&self->send_work, K_NO_WAIT);
}

// The deferred sender. Runs only on the system workqueue (submitted by the VM
// on write/flush and by notify_complete_cb on completion). It is the sole
// caller of bt_gatt_notify_cb, so packet_queued / pending_index / pending_size
// are touched here and in the VM's brief critical section — nowhere else.
static void packet_buffer_send_work_handler(struct k_work *work) {
    bleio_packet_buffer_obj_t *self = CONTAINER_OF(
        k_work_delayable_from_work(work), bleio_packet_buffer_obj_t, send_work);

    // Server-side notify path only; clients write directly. characteristic is
    // NULL after deinit, so bail before touching it (notify_complete_cb() can
    // reschedule send_work after teardown).
    bleio_characteristic_obj_t *c = self->characteristic;
    if (c == NULL || self->client) {
        return;
    }
    if (!conn_is_valid(self)) {
        // The peer is gone: drop everything staged, and clear packet_queued even
        // if a notification was handed to bt_gatt_notify_cb() already and its
        // notify_complete_cb() has not run. It never will: Zephyr does not call
        // the ATT callback for a PDU destroyed by a disconnect (att.c
        // att_on_sent_cb(): "Bearer not connected, dropping ATT cb"). If
        // packet_queued stayed set, the early return below would skip every
        // later send on this buffer, even after the peer reconnects.
        self->pending_size = 0;
        self->packet_queued = false;
        return;
    }

    // A notification has been handed to bt_gatt_notify_cb() and its
    // notify_complete_cb() has not run yet. That callback reschedules send_work,
    // so this handler will run again once the controller has sent the PDU.
    if (self->packet_queued) {
        return;
    }
    if (self->pending_size == 0) {
        return;  // nothing staged
    }

    // Staging keeps pending_size <= the negotiated ATT MTU payload, so the
    // whole staged packet fits in one notification.
    struct bt_gatt_notify_params params = {
        .attr = &c->service->attrs[c->value_attr_index],
        .data = self->outgoing[self->pending_index],
        .len = self->pending_size,
        .func = notify_complete_cb,
        .user_data = self,
    };

    int err = bt_gatt_notify_cb(self->conn, &params);
    if (err == 0) {
        // Accepted by the controller; the payload was copied into a PDU and
        // will go out at the next connection event. Hand this buffer off and
        // let the VM fill the other one while it's in flight.
        self->packet_queued = true;
        self->pending_size = 0;
        self->pending_index ^= 1;  // VM fills the other buffer next
        return;
    }
    if (err == -ENOMEM || err == -EAGAIN) {
        // No ATT TX buffer right now. The ATT TX pool is shared across all ATT
        // traffic on all connections, so it can be full from other
        // notifies/indications/responses even when we have nothing in flight.
        // Running on the workqueue makes the allocator use K_NO_WAIT: it returns
        // NULL and the stack returns -ENOMEM *before* copying or queueing a PDU —
        // nothing sent, nothing dropped; the bytes are still in
        // outgoing[pending_index]. Leave the data staged and reschedule ourselves
        // after a short delay so we retry even when the VM is idle (no write/flush
        // to drive us). The delay — not an immediate resubmit — keeps the workqueue
        // from busy-looping against a full pool.
        k_work_reschedule(&self->send_work, K_MSEC(2));
        return;
    }
    if (err == -ENOTCONN) {
        // Peer disconnected — forget the connection too. (We're here only when
        // packet_queued is clear, so no in-flight completion is owed.)
        self->conn = NULL;
    }
    // Any other error is not transient: -ENOTCONN (peer gone), -EPERM (link not
    // encrypted and the characteristic requires it), -EINVAL (peer not
    // subscribed; CONFIG_BT_GATT_ENFORCE_SUBSCRIPTION), -ENOENT (attribute not
    // registered). Retrying would spin forever and wedge this buffer. Drop the
    // staged packet, as nordic and espressif do on non-resource errors, and
    // cancel any pending delayed retry.
    self->pending_size = 0;
    self->packet_queued = false;
    k_work_cancel_delayable(&self->send_work);
}

// Shared core for both the Python-facing (allocating) and workflow
// (caller-supplied static buffer) constructors. Wires the ring buffer, the two
// outgoing buffers, the characteristic observer, and (for client-side) CCCD
// subscription. No GC heap allocation happens here.
static void packet_buffer_init_common(bleio_packet_buffer_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    uint8_t *ringbuf_data, size_t ringbuf_size, bool owns_ringbuf_data,
    uint8_t *outgoing0, uint8_t *outgoing1,
    bool owns_outgoing0, bool owns_outgoing1,
    size_t max_packet_size) {

    self->characteristic = characteristic;
    self->timeout_ms = 0;
    self->max_packet_size = max_packet_size;
    self->conn = NULL;
    self->client = (characteristic->service != NULL && characteristic->service->is_remote);
    self->outgoing[0] = outgoing0;
    self->outgoing[1] = outgoing1;
    self->owns_outgoing[0] = owns_outgoing0;
    self->owns_outgoing[1] = owns_outgoing1;
    self->pending_index = 0;
    self->pending_size = 0;
    self->packet_queued = false;

    self->ringbuf_data = ringbuf_data;
    self->ringbuf_size = ringbuf_size;
    self->owns_ringbuf_data = owns_ringbuf_data;
    if (ringbuf_data != NULL && ringbuf_size > 0) {
        ring_buf_init(&self->ringbuf, ringbuf_size, ringbuf_data);
    } else {
        // No incoming buffer (e.g. a server-side NOTIFY-only characteristic).
        ring_buf_init(&self->ringbuf, 0, NULL);
    }

    // The deferred sender drives server-side notifications. Client-side writes
    // go out synchronously and never schedule it, but initializing it always is
    // harmless and keeps flush()/deinit() simple.
    k_work_init_delayable(&self->send_work, packet_buffer_send_work_handler);

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
    // need one scratch buffer. Server-side notifications ping-pong between two
    // buffers so the VM can fill one while the other is in flight.
    uint8_t *outgoing0 = NULL;
    uint8_t *outgoing1 = NULL;
    bool owns0 = false;
    bool owns1 = false;
    if (client) {
        outgoing0 = port_malloc(max_packet_size, false);
        owns0 = true;
    } else if (props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE)) {
        outgoing0 = port_malloc(max_packet_size, false);
        outgoing1 = port_malloc(max_packet_size, false);
        owns0 = true;
        owns1 = true;
    }

    packet_buffer_init_common(self, characteristic,
        ringbuf_data, ringbuf_size, true,
        outgoing0, outgoing1, owns0, owns1,
        max_packet_size);
}

// Allocation-free version for BLE workflow use. The caller supplies static
// buffers so this can run before gc_init() without touching the GC heap.
// outgoing_buffer1 -> outgoing[0], outgoing_buffer2 -> outgoing[1].
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

    uint8_t *outgoing0 = NULL;
    uint8_t *outgoing1 = NULL;
    bool owns0 = false;
    bool owns1 = false;

    if (client) {
        // Client-side: one scratch buffer for synchronous writes.
        if (outgoing_buffer1 != NULL) {
            outgoing0 = (uint8_t *)outgoing_buffer1;
        } else {
            outgoing0 = port_malloc(max_packet_size, false);
            owns0 = true;
        }
    } else if (props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE)) {
        // Server-side: two buffers to ping-pong between.
        if (outgoing_buffer1 != NULL) {
            outgoing0 = (uint8_t *)outgoing_buffer1;
        } else {
            outgoing0 = port_malloc(max_packet_size, false);
            owns0 = true;
        }
        if (outgoing_buffer2 != NULL) {
            outgoing1 = (uint8_t *)outgoing_buffer2;
        } else {
            outgoing1 = port_malloc(max_packet_size, false);
            owns1 = true;
        }
    }

    packet_buffer_init_common(self, characteristic,
        ringbuf_data, ringbuf_size, false,
        outgoing0, outgoing1, owns0, owns1,
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
    if (self->outgoing[0] == NULL) {
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
    // remote GATT server. The server-side path below stages into a double
    // buffer and drains it from the system workqueue via send_work, so handle
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
        memcpy(self->outgoing[0], header, header_len);
        memcpy(self->outgoing[0] + header_len, data, len);
        size_t total = header_len + len;

        if (c->props & CHAR_PROP_WRITE_NO_RESPONSE) {
            // Fire-and-forget write. Retry on transient "no TX buffer"
            // (-EAGAIN) so paced protocols (e.g. BLE file transfer) don't
            // silently drop data.
            int err;
            while ((err = bt_gatt_write_without_response(conn, c->handle,
                self->outgoing[0], total, false)) == -EAGAIN) {
                RUN_BACKGROUND_TASKS;
            }
            if (err != 0) {
                raise_zephyr_error(err);
            }
        } else if (c->props & CHAR_PROP_WRITE) {
            bleio_gattc_write_sync(conn, c->handle, self->outgoing[0], total);
        } else {
            // No write property; nothing to send.
            return -1;
        }
        return (mp_int_t)total;
    }

    // Server-side notify path: stage header + data into the pending buffer,
    // then hand it to send_work on the system workqueue. bt_gatt_notify_cb
    // blocks (K_FOREVER) off the workqueue, so we never call it here; send_work
    // does, and the completion callback paces us to one notification in flight.
    // The header prefixes each packet, so it's only written when the packet is
    // empty (pending_size == 0). pending_size counts bytes staged but not yet
    // accepted by the controller.

    if (mp_hal_is_interrupted()) {
        return -1;
    }

    // If this write would overflow the current packet, wait for send_work to
    // drain it (which requires the in-flight notification to complete first).
    if ((size_t)len + self->pending_size > (size_t)outgoing_packet_length) {
        while (self->pending_size != 0 &&
               conn_is_valid(self) &&
               !mp_hal_is_interrupted()) {
            k_work_reschedule(&self->send_work, K_NO_WAIT);
            RUN_BACKGROUND_TASKS;
        }
    }
    if (!conn_is_valid(self) || mp_hal_is_interrupted()) {
        return -1;
    }

    size_t num_bytes_written = 0;

    // send_work (system workqueue) may preempt us and modify pending_index /
    // pending_size / packet_queued, so guard the append.
    common_hal_mcu_disable_interrupts();
    {
        uint8_t *pending = self->outgoing[self->pending_index];
        if (self->pending_size == 0 && header_len > 0) {
            memcpy(pending, header, header_len);
            self->pending_size += header_len;
            num_bytes_written += header_len;
        }
        memcpy(pending + self->pending_size, data, len);
        self->pending_size += len;
        num_bytes_written += len;
    }
    common_hal_mcu_enable_interrupts();

    // If no notification is in flight, kick the deferred sender to drain what
    // we just staged (K_NO_WAIT runs it immediately, cancelling any pending
    // delayed retry). If one is in flight, its completion will resubmit.
    if (!self->packet_queued) {
        k_work_reschedule(&self->send_work, K_NO_WAIT);
    }
    return (mp_int_t)num_bytes_written;
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
    // Wait until everything written has been handed to the controller AND its
    // completion has fired (we pace to one notification in flight at a time).
    // send_work runs on the system workqueue, which is higher priority than
    // this thread, so k_work_reschedule(K_NO_WAIT) lets it preempt us;
    // RUN_BACKGROUND_TASKS advances simulated time / lets completion callbacks
    // fire so packet_queued clears. For client-side writes (synchronous) there
    // is nothing pending, so this returns immediately.
    while ((self->pending_size != 0 || self->packet_queued) &&
           conn_is_valid(self) &&
           !mp_hal_is_interrupted()) {
        k_work_reschedule(&self->send_work, K_NO_WAIT);
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
    // Cancel any pending delayed retry so it can't fire after we free the
    // buffers (or after the object is gone).
    k_work_cancel_delayable(&self->send_work);
    // Free buffers if we own them (port_malloc'd). The BLE workflow path
    // supplies static buffers that must not be freed.
    if (self->owns_ringbuf_data && self->ringbuf_data != NULL) {
        port_free(self->ringbuf_data);
    }
    self->ringbuf_data = NULL;
    for (int i = 0; i < 2; i++) {
        if (self->owns_outgoing[i] && self->outgoing[i] != NULL) {
            port_free(self->outgoing[i]);
        }
        self->outgoing[i] = NULL;
    }
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
