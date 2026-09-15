// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019-2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>
#include <stdio.h>

#include "py/runtime.h"
#include "py/stream.h"

#include "shared/runtime/interrupt_char.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared-bindings/microcontroller/__init__.h"

#include "supervisor/shared/tick.h"
#include "supervisor/shared/bluetooth/serial.h"

#include "common-hal/_bleio/ble_events.h"

#include "host/ble_att.h"

// The ringbuf and the pending outgoing buffers are shared with the nimble_host
// task, which preempts the CircuitPython task at arbitrary points, and ringbuf
// operations are not atomic, so guard the shared accesses with interrupts
// disabled.

// Runs on the nimble_host task.
void bleio_packet_buffer_extend(bleio_packet_buffer_obj_t *self, uint16_t conn_handle, const uint8_t *data, size_t len) {
    if (self->conn_handle != conn_handle) {
        return;
    }

    if (len + sizeof(uint16_t) > ringbuf_size(&self->ringbuf)) {
        // This shouldn't happen but can if our buffer size was much smaller than
        // the writes the client actually makes.
        return;
    }

    common_hal_mcu_disable_interrupts();

    // Make room for the new value by dropping the oldest packets first.
    while (ringbuf_num_empty(&self->ringbuf) < len + sizeof(uint16_t)) {
        uint16_t packet_length;
        ringbuf_get_n(&self->ringbuf, (uint8_t *)&packet_length, sizeof(uint16_t));
        for (uint16_t i = 0; i < packet_length; i++) {
            ringbuf_get(&self->ringbuf);
        }
        // set an overflow flag?
    }
    ringbuf_put_n(&self->ringbuf, (uint8_t *)&len, sizeof(uint16_t));
    ringbuf_put_n(&self->ringbuf, data, len);

    common_hal_mcu_enable_interrupts();
}

static int packet_buffer_on_ble_client_evt(struct ble_gap_event *event, void *param);
static int queue_next_write(bleio_packet_buffer_obj_t *self);
static void packet_buffer_retry_send(void *data);

// Interval to wait between send attempts when the outgoing mbuf pool is
// exhausted. The pool drains as fast as the radio sends: a handful of packets
// per connection interval, which is typically tens of milliseconds.
#define RETRY_INTERVAL_MS (10)

static int _write_cb(uint16_t conn_handle,
    const struct ble_gatt_error *error,
    struct ble_gatt_attr *attr,
    void *arg) {
    if (error->status != 0) {
        #if CIRCUITPY_VERBOSE_BLE
        // For debugging.
        mp_printf(&mp_plat_print, "write failed %d\n", error->status);
        #endif
    }
    bleio_packet_buffer_obj_t *self = (bleio_packet_buffer_obj_t *)arg;
    // Whether or not the write succeeded, it is no longer awaiting a response. On
    // failure NimBLE already consumed the data, so there is nothing to retry.
    self->packet_queued = false;
    queue_next_write(self);

    return 0;
}

// Try to send the data waiting in the pending buffer, if any. May be called
// from the CircuitPython task or from the nimble_host task,
// which preempts the CircuitPython task.
// BLE_HS_ENOMEM means we are out of mbufs because the radio hasn't drained
// the outgoing queue yet. That is expected, not an error: keep
// the data: the background callback scheduled below and the wait loops in
// write() and flush() will retry sending.
// On other errors drop the data.
static int queue_next_write(bleio_packet_buffer_obj_t *self) {
    // Remember the current time if a previous send failed.
    uint64_t now_ms = 0;
    if (self->last_failed_ms != 0) {
        now_ms = supervisor_ticks_ms64();
    }
    common_hal_mcu_disable_interrupts();
    if (self->send_in_progress || self->packet_queued ||
        self->pending_size == 0 ||
        self->conn_handle == BLEIO_HANDLE_INVALID) {
        // Succeed if:
        // - there is nothing to send
        // - a packet is already awaiting its completion event
        // - another send call is already in progress (including the
        //   synchronous BLE_GAP_EVENT_NOTIFY_TX that our own notify and
        //   indicate calls below raise on this very stack).
        common_hal_mcu_enable_interrupts();
        return NIMBLE_OK;
    }
    if (self->last_failed_ms != 0 &&
        (now_ms == 0 || now_ms - self->last_failed_ms < RETRY_INTERVAL_MS)) {
        // Don't try before retry interval expires: the radio can only go so fast.
        // Schedule a retry for later.
        common_hal_mcu_enable_interrupts();
        background_callback_add(&self->retry_send_callback, packet_buffer_retry_send, self);
        return BLE_HS_ENOMEM;
    }
    // Claim the pending buffer. While send_in_progress is set, writers wait
    // instead of appending to the buffer. Other callers of this function return above,
    // so the buffer contents stay stable during the send call even though
    // interrupts are enabled again while it runs.
    self->send_in_progress = true;
    // A client Write with Response or an Indicate gets a completion event (a
    // write response or an indicate acknowledgment) that calls back here to
    // send whatever accumulates in the meantime. Mark such a packet as in
    // flight *before* submitting it: if this task is preempted for long enough,
    // the completion can arrive before the code after the send call runs,
    // and it must find the flag already set so it can clear it.
    bool is_completion_type = self->write_type == CHAR_PROP_WRITE ||
        self->write_type == CHAR_PROP_INDICATE;
    self->packet_queued = is_completion_type;
    uint16_t conn_handle = self->conn_handle;
    uint16_t attr_handle = self->characteristic->handle;
    const uint8_t *data = (const uint8_t *)self->outgoing[0];
    uint16_t data_len = self->pending_size;
    common_hal_mcu_enable_interrupts();

    int err_code;
    if (self->client) {
        if (self->write_type == CHAR_PROP_WRITE_NO_RESPONSE) {
            // Copies the data into an mbuf before returning. No completion
            // callback follows; NimBLE buffers the outgoing packet itself.
            err_code = ble_gattc_write_no_rsp_flat(conn_handle, attr_handle,
                data, data_len);
        } else {
            // _write_cb will be called only if the write was actually queued.
            err_code = ble_gattc_write_flat(conn_handle, attr_handle,
                data, data_len, _write_cb, self);
        }
    } else {
        // Allocate an mbuf because the functions below consume it, on
        // success and on failure.
        struct os_mbuf *om = ble_hs_mbuf_from_flat(data, data_len);
        if (om == NULL) {
            err_code = BLE_HS_ENOMEM;
        } else if (self->write_type == CHAR_PROP_NOTIFY) {
            // Raises BLE_GAP_EVENT_NOTIFY_TX on this stack before returning,
            // on success and on failure. There is no later completion event
            // for a notification.
            err_code = ble_gatts_notify_custom(conn_handle, attr_handle, om);
        } else if (self->write_type == CHAR_PROP_INDICATE) {
            // The acknowledgment (or a timeout) will raise
            // BLE_GAP_EVENT_NOTIFY_TX later, from the nimble_host task.
            err_code = ble_gatts_indicate_custom(conn_handle, attr_handle, om);
        } else {
            // Placeholder error.
            err_code = BLE_HS_EUNKNOWN;
        }
    }

    uint64_t fail_ms = 0;
    if (err_code == BLE_HS_ENOMEM) {
        fail_ms = supervisor_ticks_ms64();
    }
    common_hal_mcu_disable_interrupts();
    self->send_in_progress = false;
    if (self->conn_handle != conn_handle) {
        // Disconnected or unsubscribed during the send. The event handler
        // already discarded the pending data; make sure nothing looks like a
        // send in progress to the new connection, if any.
        self->packet_queued = false;
        common_hal_mcu_enable_interrupts();
        return err_code;
    }
    if (err_code == NIMBLE_OK) {
        // NimBLE copied the data, so the buffer is immediately reusable,
        // even while a write with response or an indicate is still in
        // flight: new writes coalesce in it until the completion event.
        // Don't touch packet_queued: it was set before the send, and the
        // completion event may already have cleared it.
        self->pending_size = 0;
        self->last_failed_ms = 0;
    } else {
        // Nothing was queued, so no completion event will come.
        self->packet_queued = false;
        if (err_code == BLE_HS_ENOMEM) {
            // Out of mbufs: normal while the radio works through the outgoing
            // queue. Keep the data and retry from the background,
            // because a failed send gets no completion event to prompt
            // another attempt. Re-adding a queued callback does nothing.
            self->last_failed_ms = fail_ms;
            background_callback_add(&self->retry_send_callback, packet_buffer_retry_send, self);
        } else {
            // A real error, such as a dropped connection or an ATT failure.
            // Drop the data, as before.
            self->pending_size = 0;
            self->last_failed_ms = 0;
        }
    }
    common_hal_mcu_enable_interrupts();
    return err_code;
}

// Runs as a background callback after a send failed for lack of mbufs.
static void packet_buffer_retry_send(void *data) {
    bleio_packet_buffer_obj_t *self = (bleio_packet_buffer_obj_t *)data;
    if (common_hal_bleio_packet_buffer_deinited(self)) {
        return;
    }
    // Reschedules this callback (in queue_next_write()) if still out of mbufs.
    queue_next_write(self);
}

// This is usually called from the nimble task, *not* CircuitPython's.
// BLE_GAP_EVENT_NOTIFY_TX is also raised synchronously, on the task that
// called into NimBLE, by our own sends in queue_next_write().
static int packet_buffer_on_ble_client_evt(struct ble_gap_event *event, void *param) {
    bleio_packet_buffer_obj_t *self = (bleio_packet_buffer_obj_t *)param;
    if (event->type == BLE_GAP_EVENT_DISCONNECT && self->conn_handle == event->disconnect.conn.conn_handle) {
        self->conn_handle = BLEIO_HANDLE_INVALID;
        // Discard the pending state so stale data can't be sent into a
        // later connection or subscription.
        self->pending_size = 0;
        self->packet_queued = false;
        return false;
    }
    if (event->type == BLE_GAP_EVENT_SUBSCRIBE) {
        if (self->conn_handle == BLEIO_HANDLE_INVALID && (event->subscribe.cur_notify == 1 || event->subscribe.cur_indicate == 1)) {
            self->conn_handle = event->subscribe.conn_handle;
        } else if (self->conn_handle == event->subscribe.conn_handle && event->subscribe.cur_notify == 0 && event->subscribe.cur_indicate == 0) {
            self->conn_handle = BLEIO_HANDLE_INVALID;
            // Discard the pending state so stale data can't be sent into a
            // later subscription.
            self->pending_size = 0;
            self->packet_queued = false;
        }
        return false;
    }
    if (event->type == BLE_GAP_EVENT_NOTIFY_TX) {
        if (self->conn_handle == event->notify_tx.conn_handle && self->characteristic->handle == event->notify_tx.attr_handle) {
            if (event->notify_tx.indication == 1) {
                if (event->notify_tx.status == 0) {
                    // The indicate has been queued. This event is raised
                    // synchronously inside our own indicate call.
                    return false;
                }
                if (event->notify_tx.status == BLE_HS_EDONE ||
                    event->notify_tx.status == BLE_HS_ETIMEOUT) {
                    // Acknowledged or timed out: the indicate is no longer
                    // awaiting a response. These statuses come only from the nimble_host
                    // task later on, never from inside our own send call, so
                    // clear packet_queued even if a new claim is in progress.
                    self->packet_queued = false;
                    if (!self->send_in_progress) {
                        // Send any data that accumulated in the meantime.
                        queue_next_write(self);
                    }
                    return false;
                }
                // Any other status is either our own synchronous submission
                // failure, handled by the return code in queue_next_write(),
                // or a failure that comes with a disconnect, which discards
                // the pending state.
                return false;
            }
            // A notification transmission was attempted (status 0 on
            // success). It was raised synchronously inside a notify call:
            // ours, in queue_next_write(), which acts on the result code
            // itself (send_in_progress is set), or someone else's on this
            // characteristic, which we can use as a prompt to send anything
            // that is waiting.
            if (!self->send_in_progress) {
                queue_next_write(self);
            }
            return false;
        }
    }
    // Notify and indicate events are managed by the characteristic.
    return false;
}

void _common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    uint32_t *incoming_buffer, size_t incoming_buffer_size,
    uint32_t *outgoing_buffer1, uint32_t *outgoing_buffer2, size_t max_packet_size,
    ble_event_handler_t *static_handler_entry) {
    self->characteristic = characteristic;
    self->client = self->characteristic->service->is_remote;
    self->max_packet_size = max_packet_size;
    bleio_characteristic_properties_t incoming = self->characteristic->props & (CHAR_PROP_WRITE_NO_RESPONSE | CHAR_PROP_WRITE);
    bleio_characteristic_properties_t outgoing = self->characteristic->props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE);

    if (self->client) {
        // Swap if we're the client.
        bleio_characteristic_properties_t temp = incoming;
        incoming = outgoing;
        outgoing = temp;
        self->conn_handle = bleio_connection_get_conn_handle(MP_OBJ_TO_PTR(self->characteristic->service->connection));
    } else {
        self->conn_handle = BLEIO_HANDLE_INVALID;
    }

    if (incoming) {
        ringbuf_init(&self->ringbuf, (uint8_t *)incoming_buffer, incoming_buffer_size);
    }

    self->packet_queued = false;
    self->send_in_progress = false;
    self->pending_size = 0;
    self->last_failed_ms = 0;
    self->outgoing[0] = outgoing_buffer1;
    self->outgoing[1] = outgoing_buffer2;
    // Don't touch retry_send_callback: for a statically allocated buffer that
    // is being reconstructed, it may still be on the background callback list.

    if (static_handler_entry != NULL) {
        ble_event_add_handler_entry((ble_event_handler_entry_t *)static_handler_entry, packet_buffer_on_ble_client_evt, self);
    } else {
        ble_event_add_handler(packet_buffer_on_ble_client_evt, self);
    }
    bleio_characteristic_set_observer(self->characteristic, self);
    if (self->client) {
        if (incoming) {
            // Prefer notify if both are available.
            if (incoming & CHAR_PROP_NOTIFY) {
                common_hal_bleio_characteristic_set_cccd(self->characteristic, true, false);
            } else {
                common_hal_bleio_characteristic_set_cccd(self->characteristic, false, true);
            }
        }
        if (outgoing) {
            self->write_type = CHAR_PROP_WRITE;
            if (outgoing & CHAR_PROP_WRITE_NO_RESPONSE) {
                self->write_type = CHAR_PROP_WRITE_NO_RESPONSE;
            }
        }
    } else {
        if (outgoing) {
            self->write_type = CHAR_PROP_NOTIFY;
            if (outgoing & CHAR_PROP_INDICATE) {
                self->write_type = CHAR_PROP_INDICATE;
            }
        }
    }
}

void common_hal_bleio_packet_buffer_construct(
    bleio_packet_buffer_obj_t *self, bleio_characteristic_obj_t *characteristic,
    size_t buffer_size, size_t max_packet_size) {

    // Cap the packet size to our implementation limits.
    max_packet_size = MIN(max_packet_size, BLE_ATT_ATTR_MAX_LEN - 3);

    bleio_characteristic_properties_t incoming = characteristic->props & (CHAR_PROP_WRITE_NO_RESPONSE | CHAR_PROP_WRITE);
    bleio_characteristic_properties_t outgoing = characteristic->props & (CHAR_PROP_NOTIFY | CHAR_PROP_INDICATE);
    if (characteristic->service->is_remote) {
        // Swap if we're the client.
        bleio_characteristic_properties_t temp = incoming;
        incoming = outgoing;
        outgoing = temp;
    }
    size_t incoming_buffer_size = 0;
    uint32_t *incoming_buffer = NULL;
    if (incoming) {
        incoming_buffer_size = buffer_size * (sizeof(uint16_t) + max_packet_size);
        incoming_buffer = m_malloc_without_collect(incoming_buffer_size);
    }

    uint32_t *outgoing1 = NULL;
    uint32_t *outgoing2 = NULL;
    if (outgoing) {
        outgoing1 = m_malloc_without_collect(max_packet_size);
        // Only allocate the second buffer if we are doing writes with responses.
        // Without responses, we just write as quickly as we can.
        //
        // TODO: this port never reads outgoing[1]. NimBLE copies the data at send
        // time, so one buffer can be refilled immediately, unlike the nordic
        // SoftDevice which keeps the caller's buffer until TX completes. Kept
        // allocated so that reintroducing two-buffer use here is not a bug.
        if (outgoing == CHAR_PROP_WRITE || outgoing == CHAR_PROP_INDICATE) {
            outgoing2 = m_malloc_without_collect(max_packet_size);
        }
    }
    _common_hal_bleio_packet_buffer_construct(self, characteristic,
        incoming_buffer, incoming_buffer_size,
        outgoing1, outgoing2, max_packet_size,
        NULL);
}

mp_int_t common_hal_bleio_packet_buffer_readinto(bleio_packet_buffer_obj_t *self, uint8_t *data, size_t len) {
    if (ringbuf_num_filled(&self->ringbuf) < 2) {
        return 0;
    }

    common_hal_mcu_disable_interrupts();

    // Get packet length, which is in first two bytes of packet.
    uint16_t packet_length;
    ringbuf_get_n(&self->ringbuf, (uint8_t *)&packet_length, sizeof(uint16_t));

    mp_int_t ret;
    if (packet_length > len) {
        // Packet is longer than requested. Return negative of overrun value.
        ret = len - packet_length;
        // Discard the packet if it's too large. Don't fill data.
        while (packet_length--) {
            (void)ringbuf_get(&self->ringbuf);
        }
    } else {
        // Read as much as possible, but might be shorter than len.
        ringbuf_get_n(&self->ringbuf, data, packet_length);
        ret = packet_length;
    }

    common_hal_mcu_enable_interrupts();

    return ret;
}

mp_int_t common_hal_bleio_packet_buffer_write(bleio_packet_buffer_obj_t *self, const uint8_t *data, size_t len, uint8_t *header, size_t header_len) {
    if (self->outgoing[0] == NULL) {
        mp_raise_bleio_BluetoothError(MP_ERROR_TEXT("Writes not supported on Characteristic"));
    }
    if (self->conn_handle == BLEIO_HANDLE_INVALID) {
        return -1;
    }
    mp_int_t outgoing_packet_length = common_hal_bleio_packet_buffer_get_outgoing_packet_length(self);
    if (outgoing_packet_length < 0) {
        return -1;
    }

    mp_int_t total_len = len + header_len;
    if (total_len > outgoing_packet_length) {
        // Supplied data will not fit in a single BLE packet.
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Total data to write is larger than %q"), MP_QSTR_outgoing_packet_length);
    }
    if (total_len > self->max_packet_size) {
        // Supplied data will not fit in a single BLE packet.
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Total data to write is larger than %q"), MP_QSTR_max_packet_size);
    }
    outgoing_packet_length = MIN(outgoing_packet_length, self->max_packet_size);

    size_t num_bytes_written = 0;

    // Append the data to the pending packet, waiting for room if it is full.
    // The append must happen with interrupts disabled, with the connection
    // still up and with no send call in progress (the nimble_host task can
    // start one at any point), so check those conditions inside the critical
    // section and retry until they all hold at once.
    while (true) {
        if (mp_hal_is_interrupted()) {
            return -1;
        }
        common_hal_mcu_disable_interrupts();
        if (self->conn_handle == BLEIO_HANDLE_INVALID) {
            common_hal_mcu_enable_interrupts();
            return -1;
        }
        if (!self->send_in_progress &&
            len + self->pending_size <= (size_t)outgoing_packet_length) {
            // Append below, still inside the critical section.
            break;
        }
        common_hal_mcu_enable_interrupts();
        // No room for data yet. Retry the pending packet ourselves: a send that
        // failed for lack of mbufs gets no completion event to prompt
        // another attempt, and if we were called from a background callback
        // (as the BLE workflow does), RUN_BACKGROUND_TASKS below cannot
        // re-enter background callbacks to do it for us.
        queue_next_write(self);
        RUN_BACKGROUND_TASKS;
    }

    uint8_t *pending = (uint8_t *)self->outgoing[0];

    if (self->pending_size == 0) {
        memcpy(pending, header, header_len);
        self->pending_size += header_len;
        num_bytes_written += header_len;
    }
    memcpy(pending + self->pending_size, data, len);
    self->pending_size += len;
    num_bytes_written += len;

    common_hal_mcu_enable_interrupts();

    // Send now. This is a no-op if a packet is already awaiting its completion
    // event; the completion will send the data instead.
    queue_next_write(self);

    return num_bytes_written;
}

mp_int_t common_hal_bleio_packet_buffer_get_incoming_packet_length(bleio_packet_buffer_obj_t *self) {
    // If this PacketBuffer is coming from a remote service via NOTIFY or INDICATE
    // the maximum size is what can be sent in one
    // BLE packet. But we must be connected to know that value.
    //
    // Otherwise it can be as long as the characteristic
    // will permit, whether or not we're connected.

    if (self->characteristic == NULL) {
        return -1;
    }

    if (self->characteristic->service != NULL &&
        self->characteristic->service->is_remote &&
        (common_hal_bleio_characteristic_get_properties(self->characteristic) &
         (CHAR_PROP_INDICATE | CHAR_PROP_NOTIFY))) {
        // We are talking to a remote service, and data is arriving via NOTIFY or INDICATE.
        if (self->conn_handle != BLEIO_HANDLE_INVALID) {
            bleio_connection_internal_t *connection = bleio_conn_handle_to_connection(self->conn_handle);
            if (connection) {
                return common_hal_bleio_connection_get_max_packet_length(connection);
            }
        }
        // There's no current connection, so we don't know the MTU, and
        // we can't tell what the largest incoming packet length would be.
        return -1;
    }
    return self->characteristic->max_length;
}

mp_int_t common_hal_bleio_packet_buffer_get_outgoing_packet_length(bleio_packet_buffer_obj_t *self) {
    // If we are sending data via NOTIFY or INDICATE, the maximum size
    // is what can be sent in one BLE packet. But we must be connected
    // to know that value.
    //
    // Otherwise it can be as long as the characteristic
    // will permit, whether or not we're connected.

    if (self->characteristic == NULL) {
        return -1;
    }

    if (self->characteristic->service != NULL &&
        !self->characteristic->service->is_remote &&
        (common_hal_bleio_characteristic_get_properties(self->characteristic) &
         (CHAR_PROP_INDICATE | CHAR_PROP_NOTIFY))) {
        // We are sending to a client, via NOTIFY or INDICATE.
        if (self->conn_handle != BLEIO_HANDLE_INVALID) {
            bleio_connection_internal_t *connection = bleio_conn_handle_to_connection(self->conn_handle);
            if (connection) {
                return MIN(MIN(common_hal_bleio_connection_get_max_packet_length(connection),
                    self->max_packet_size),
                    self->characteristic->max_length);
            }
        }
        // There's no current connection, so we don't know the MTU, and
        // we can't tell what the largest outgoing packet length would be.
        return -1;
    }
    // If we are talking to a remote service, we'll be bound by the MTU. (We don't actually
    // know the max size of the remote characteristic.)
    if (self->characteristic->service != NULL &&
        self->characteristic->service->is_remote) {
        // We are talking to a remote service so we're writing.
        if (self->conn_handle != BLEIO_HANDLE_INVALID) {
            bleio_connection_internal_t *connection = bleio_conn_handle_to_connection(self->conn_handle);
            if (connection) {
                return MIN(common_hal_bleio_connection_get_max_packet_length(connection),
                    self->max_packet_size);
            }
        }
    }
    return MIN(self->characteristic->max_length, self->max_packet_size);
}

void common_hal_bleio_packet_buffer_flush(bleio_packet_buffer_obj_t *self) {
    while ((self->pending_size != 0 ||
            self->packet_queued) &&
           self->conn_handle != BLEIO_HANDLE_INVALID &&
           !mp_hal_is_interrupted()) {
        // Retry sends that failed for lack of mbufs ourselves: they get no
        // completion event, and background callbacks can't run if we were
        // called from one (as the BLE workflow does).
        queue_next_write(self);
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
    // Take the packet buffer off the event list before changing anything
    // inside the object.
    ble_event_remove_handler(packet_buffer_on_ble_client_evt, self);
    bleio_characteristic_clear_observer(self->characteristic);
    self->characteristic = NULL;
    // A still-queued retry_send_callback checks deinited(), so it sees the
    // NULL characteristic and does nothing.
    self->pending_size = 0;
    self->packet_queued = false;
    ringbuf_deinit(&self->ringbuf);
}

bool common_hal_bleio_packet_buffer_connected(bleio_packet_buffer_obj_t *self) {
    return !common_hal_bleio_packet_buffer_deinited(self) && self->conn_handle != BLEIO_HANDLE_INVALID;
}
