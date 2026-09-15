// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>

#include "py/obj.h"
#include "shared-bindings/_bleio/Characteristic.h"

struct bt_conn;

typedef void *ble_event_handler_t;

typedef struct {
    mp_obj_base_t base;
    bleio_characteristic_obj_t *characteristic;
    uint32_t timeout_ms;
    // Store the connection we are talking to. We expect to talk over a single
    // connection at a time.
    struct bt_conn *conn;
    bool client;
    // The current MTU negotiated with the active connection.
    size_t max_packet_size;

    // ringbuf to store incoming packets.
    struct ring_buf ringbuf;
    uint8_t *ringbuf_data;
    size_t ringbuf_size;

    // Outgoing path. Two fixed buffers alternate so the VM can keep filling one
    // while the other is in flight with the controller.
    //
    // outgoing[pending_index] is the buffer the VM is filling (pending_size
    // bytes staged, not yet sent). packet_queued means a notification has been
    // accepted by the controller and its completion callback hasn't fired yet
    // — at most one is in flight at a time.
    //
    uint8_t *outgoing[2];
    uint8_t pending_index;            // which buffer the VM is filling
    volatile uint16_t pending_size;   // bytes staged in outgoing[pending_index]
    volatile bool packet_queued;      // a notification is in flight

    // send_work is a delayable work item, the only caller of bt_gatt_notify_cb
    // (that API blocks (K_FOREVER) off the workqueue but returns -ENOMEM from
    // it). It's rescheduled with K_NO_WAIT to drain after a write/completion,
    // and with a short delay on -ENOMEM so it self-retries even when the VM is
    // idle and the shared ATT TX pool is full from other traffic. It and the
    // completion callback both run on the workqueue (serialized with each
    // other); the VM is the only other toucher of these fields and guards its
    // read-modify-write with common_hal_mcu_disable_interrupts() (the workqueue
    // preempts the VM, so the critical section keeps it out while we append).
    // bt_gatt_notify_cb copies the payload into a PDU on success, so the source
    // buffer may be reused once the call returns 0; the completion callback
    // paces us to one in flight and makes flush() accurate.
    struct k_work_delayable send_work;

    // Ownership: true if the buffer was port_malloc'd and should be freed in
    // deinit; false if it is a caller-supplied static buffer (the BLE workflow
    // path, which runs before gc_init()).
    bool owns_ringbuf_data;
    bool owns_outgoing[2];
} bleio_packet_buffer_obj_t;

// Called from GATT callbacks (system workqueue context) to push
// data into the PacketBuffer ring buffer with length-prefix framing.
void bleio_packet_buffer_extend(bleio_packet_buffer_obj_t *self,
    struct bt_conn *conn, const uint8_t *data, size_t len);

// Called from CCCD write callback to record the subscribing connection.
void bleio_packet_buffer_set_conn(bleio_packet_buffer_obj_t *self,
    struct bt_conn *conn);
