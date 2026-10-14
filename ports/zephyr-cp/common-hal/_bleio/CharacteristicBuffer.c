// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>
#include <stdio.h>

#include <zephyr/sys/ring_buffer.h>

#include "py/runtime.h"
#include "py/stream.h"

#include "shared/runtime/interrupt_char.h"

#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/CharacteristicBuffer.h"

#include "supervisor/shared/tick.h"

#include "common-hal/_bleio/Adapter.h"

#include "common-hal/_bleio/CharacteristicBuffer.h"
#include "common-hal/_bleio/Characteristic.h"

// Zephyr's ring_buf is safe for single-producer/single-consumer without
// locks. The GATT callbacks (system workqueue) are the sole producer;
// the CircuitPython VM (main thread) is the sole consumer.

// Called from Zephyr GATT callbacks (system workqueue context).
void bleio_characteristic_buffer_extend(bleio_characteristic_buffer_obj_t *self,
    const uint8_t *data, size_t len) {
    if (self->watch_for_interrupt_char) {
        for (size_t i = 0; i < len; i++) {
            if (data[i] == mp_interrupt_char) {
                mp_sched_keyboard_interrupt();
                ring_buf_reset(&self->ringbuf);
            } else {
                ring_buf_put(&self->ringbuf, &data[i], 1);
            }
        }
    } else {
        ring_buf_put(&self->ringbuf, data, len);
    }
    // Wake the main task so it can service the new RX data instead of
    // sleeping out its full timeout. This is what makes the "Press any key"
    // wait end early when REPL input arrives over the BLE serial service.
    bleio_request_bluetooth_background();
}

void _common_hal_bleio_characteristic_buffer_construct(bleio_characteristic_buffer_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_float_t timeout,
    uint8_t *buffer, size_t buffer_size,
    void *static_handler_entry,
    bool watch_for_interrupt_char) {

    self->characteristic = characteristic;
    self->timeout_ms = timeout * 1000;
    self->watch_for_interrupt_char = watch_for_interrupt_char;
    self->ringbuf_data = buffer;
    ring_buf_init(&self->ringbuf, buffer_size, buffer);

    // Set ourselves as the characteristic's observer so we receive
    // incoming writes (local) and notifications (remote).
    bleio_characteristic_set_observer(characteristic, MP_OBJ_FROM_PTR(self));
}

// Assumes that timeout and buffer_size have been validated before call.
void common_hal_bleio_characteristic_buffer_construct(bleio_characteristic_buffer_obj_t *self,
    bleio_characteristic_obj_t *characteristic,
    mp_float_t timeout,
    size_t buffer_size) {
    uint8_t *buffer = m_malloc_without_collect(buffer_size);
    _common_hal_bleio_characteristic_buffer_construct(self, characteristic, timeout,
        buffer, buffer_size, NULL, false);
}

uint32_t common_hal_bleio_characteristic_buffer_read(bleio_characteristic_buffer_obj_t *self,
    uint8_t *data, size_t len, int *errcode) {
    uint64_t start_ticks = supervisor_ticks_ms64();

    // Wait for all bytes received or timeout
    while ((ring_buf_size_get(&self->ringbuf) < len) &&
           (supervisor_ticks_ms64() - start_ticks < self->timeout_ms)) {
        RUN_BACKGROUND_TASKS;
        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            return 0;
        }
    }

    return ring_buf_get(&self->ringbuf, data, len);
}

uint32_t common_hal_bleio_characteristic_buffer_rx_characters_available(
    bleio_characteristic_buffer_obj_t *self) {
    return ring_buf_size_get(&self->ringbuf);
}

void common_hal_bleio_characteristic_buffer_clear_rx_buffer(
    bleio_characteristic_buffer_obj_t *self) {
    ring_buf_reset(&self->ringbuf);
}

bool common_hal_bleio_characteristic_buffer_deinited(bleio_characteristic_buffer_obj_t *self) {
    return self->characteristic == NULL;
}

void common_hal_bleio_characteristic_buffer_deinit(bleio_characteristic_buffer_obj_t *self) {
    if (common_hal_bleio_characteristic_buffer_deinited(self)) {
        return;
    }
    bleio_characteristic_clear_observer(self->characteristic);
    self->characteristic = NULL;
    // ringbuf_data was allocated with m_malloc_without_collect; free it.
    m_free(self->ringbuf_data);
    self->ringbuf_data = NULL;
}

bool common_hal_bleio_characteristic_buffer_connected(bleio_characteristic_buffer_obj_t *self) {
    bleio_characteristic_obj_t *characteristic = self->characteristic;
    if (characteristic == NULL || characteristic->service == NULL) {
        return false;
    }

    if (!characteristic->service->is_remote) {
        // Local service: we're always "connected" as long as the service exists.
        return true;
    }

    // Remote service: check if the connection is still active.
    if (characteristic->service->connection == mp_const_none) {
        return false;
    }
    bleio_connection_obj_t *connection =
        MP_OBJ_TO_PTR(characteristic->service->connection);
    return common_hal_bleio_connection_get_connected(connection);
}
