// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019-2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/ringbuf.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "common-hal/_bleio/ble_events.h"
#include "supervisor/background_callback.h"

typedef struct {
    mp_obj_base_t base;
    bleio_characteristic_obj_t *characteristic;
    // Ring buffer storing consecutive incoming values.
    ringbuf_t ringbuf;
    // This is the outgoing packet being assembled. NimBLE copies the data at send
    // time, so we only need a single buffer. The second entry exists here but is
    // unused: the shared constructors expect two buffers.
    uint32_t *outgoing[2];
    // Number of bytes waiting in outgoing[0] to be sent.
    volatile uint16_t pending_size;
    // We remember the conn_handle so we can do a NOTIFY/INDICATE to a client.
    // We can find out the conn_handle on a Characteristic write or a CCCD write (but not a read).
    volatile uint16_t conn_handle;
    uint16_t max_packet_size;
    uint8_t write_type;
    bool client;
    // True while a client Write with Response or an Indicate is awaiting its
    // completion event: a response or acknowledgment from the peer. Not used for
    // notifications, whose NOTIFY_TX arrives inside the send call itself.
    // Its completion event sends any data that accumulated in the meantime.
    volatile bool packet_queued;
    // True while a task is inside a send call in queue_next_write(). Writers
    // wait instead of appending, and other send attempts (including the
    // synchronous BLE_GAP_EVENT_NOTIFY_TX raised inside our own notify and
    // indicate calls) return without doing anything.
    volatile bool send_in_progress;
    // Retries sends that failed for lack of mbufs; they get no completion
    // event, so nothing else would try again.
    background_callback_t retry_send_callback;
    // Time of the last send that failed for lack of mbufs, to pace retries
    // while the radio drains the pool. Zero when the last send succeeded.
    uint64_t last_failed_ms;
} bleio_packet_buffer_obj_t;

typedef ble_event_handler_entry_t ble_event_handler_t;

void bleio_packet_buffer_extend(bleio_packet_buffer_obj_t *self, uint16_t conn_handle, const uint8_t *buffer, size_t len);
