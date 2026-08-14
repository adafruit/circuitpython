// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/ring_buffer.h>

#include "py/obj.h"
#include "shared-bindings/_bleio/Characteristic.h"

struct bt_conn;

typedef void *ble_event_handler_t;

typedef struct {
    mp_obj_base_t base;
    bleio_characteristic_obj_t *characteristic;
    uint32_t timeout_ms;
    struct ring_buf ringbuf;
    uint8_t *ringbuf_data;
    size_t ringbuf_size;
    size_t max_packet_size;

    // Outgoing path. No manual mutex: the structures below all carry their own
    // locking (k_pipe) or synchronization (k_work), and the remaining state is
    // confined to a single execution context.
    //
    // Client-side (remote characteristic): outgoing_buffer is a VM-local
    // scratch used to assemble header+data before a direct, synchronous GATT
    // write. It is only ever touched by the VM thread.
    //
    // Server-side (local NOTIFY/INDICATE characteristic): the VM is the sole
    // producer writing bytes into outgoing_pipe (a k_pipe, whose built-in
    // spinlock serializes producer vs. consumer). send_work drains that pipe on
    // the system workqueue — the same context bt_gatt_notify_cb runs its
    // completion callback on — so send_size/notify_in_flight are touched only
    // there. k_work's built-in submit deduplication replaces the old
    // packet_queued flag. outgoing_pending counts bytes not yet accepted by the
    // controller so flush() can poll for "all handed off".
    uint8_t *outgoing_buffer;
    struct k_pipe outgoing_pipe;
    uint8_t *pipe_buffer;
    size_t pipe_size;
    struct k_work send_work;
    size_t send_size;
    bool notify_in_flight;
    atomic_t outgoing_pending;
    struct bt_conn *conn;
    bool client;

    // Ownership: true if the buffer was port_malloc'd and should be freed in
    // deinit; false if it is a caller-supplied static buffer (the BLE workflow
    // path, which runs before gc_init()).
    bool owns_ringbuf_data;
    bool owns_outgoing_buffer;
    bool owns_pipe_buffer;
} bleio_packet_buffer_obj_t;

// Called from GATT callbacks (system workqueue context) to push
// data into the PacketBuffer ring buffer with length-prefix framing.
void bleio_packet_buffer_extend(bleio_packet_buffer_obj_t *self,
    struct bt_conn *conn, const uint8_t *data, size_t len);

// Called from CCCD write callback to record the subscribing connection.
void bleio_packet_buffer_set_conn(bleio_packet_buffer_obj_t *self,
    struct bt_conn *conn);
