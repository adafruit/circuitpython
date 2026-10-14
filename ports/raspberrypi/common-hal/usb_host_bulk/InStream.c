// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-bindings/usb_host_bulk/InStream.h"

#include "hardware/sync.h"
#include "py/runtime.h"
#include "shared-bindings/usb/core/__init__.h"
#include "shared-bindings/usb/core/Device.h"
#include "supervisor/port_heap.h"
#include "tusb.h"

#include "pio_usb_bulk_stream.h"
#include "pio_usb_configuration.h"

// Store the streams the host is polling, at most one per endpoint, so that if
// a device disappears we can deinit all streams polling it.
static usb_host_bulk_instream_obj_t *_active[PIO_USB_EP_POOL_CNT];

static void _forget(usb_host_bulk_instream_obj_t *self) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(_active); i++) {
        if (_active[i] == self) {
            _active[i] = NULL;
        }
    }
}

// Detach the ring from the host but keep it, so that bytes captured before
// the stop can still be read. No Python allocation: also used on unplug.
// This waits about a millisecond at most.
static void _stop(usb_host_bulk_instream_obj_t *self) {
    while (!pio_usb_host_bulk_stream_stop(self->ring, 1000)) {
    }
    _forget(self);
}

static uint32_t _ring_in_waiting(pio_usb_bulk_ring_t *ring) {
    uint32_t const read = ring->read_pos;
    uint32_t const written = ring->write_pos;
    __dmb();
    uint32_t const available = written - read;
    return available > ring->capacity ? 0 : available;
}

static void _free_ring(usb_host_bulk_instream_obj_t *self) {
    self->lost_packets = self->ring->stats.overrun_packets;
    port_free(self->storage);
    port_free(self->ring);
    self->ring = NULL;
    self->storage = NULL;
    self->device = MP_OBJ_NULL;
}

// Finish deiniting a stream that ended without deinit(), because the
// device stalled the endpoint, was unplugged or was reconfigured:
// move the unread bytes to the VM heap, which the GC cleans up, and
// free the ring. Returns false, leaving the stream as it was, if
// there is no room for them.
static bool _retire(usb_host_bulk_instream_obj_t *self) {
    uint32_t remaining = _ring_in_waiting(self->ring);
    if (remaining > 0) {
        uint8_t *leftover = m_malloc_maybe_without_collect(remaining);
        if (leftover == NULL) {
            return false;
        }
        pio_usb_host_bulk_stream_read(self->ring, leftover, remaining);
        self->leftover = leftover;
        self->leftover_len = remaining;
        self->leftover_pos = 0;
    }
    _free_ring(self);
    return true;
}

void common_hal_usb_host_bulk_instream_construct(usb_host_bulk_instream_obj_t *self,
    usb_core_device_obj_t *device, uint8_t endpoint, uint32_t buffer_size) {
    size_t slot = MP_ARRAY_SIZE(_active);
    for (size_t i = 0; i < MP_ARRAY_SIZE(_active); i++) {
        usb_host_bulk_instream_obj_t *other = _active[i];
        if (other != NULL && !other->ring->active) {
            // It ended on its own, and finishes deiniting when next used.
            _active[i] = other = NULL;
        }
        if (other == NULL) {
            slot = i;
        } else if (other->device_address == device->device_address &&
                   other->endpoint == endpoint) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("Already running"));
        }
    }
    if (slot == MP_ARRAY_SIZE(_active)) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("Too many %q"), MP_QSTR_InStream);
    }
    tuh_bus_info_t bus_info;
    if (!tuh_bus_info_get(device->device_address, &bus_info) || bus_info.rhport < 1 ||
        bus_info.speed != TUSB_SPEED_FULL) {
        mp_raise_usb_core_USBError(MP_ERROR_TEXT("Operation or feature not supported"));
    }
    if (!common_hal_usb_core_device_open_endpoint(device, endpoint)) {
        mp_raise_usb_core_USBError(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_endpoint);
    }
    // The host core writes both from the frame handler, so they must be in SRAM.
    pio_usb_bulk_ring_t *ring = port_malloc(sizeof(*ring), true);
    if (ring == NULL) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Could not allocate DMA capable buffer"));
    }
    memset(ring, 0, sizeof(*ring));
    uint8_t *storage = port_malloc(buffer_size, true);
    if (storage == NULL) {
        port_free(ring);
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Could not allocate DMA capable buffer"));
    }
    // TinyUSB numbers the PIO root ports from 1.
    if (!pio_usb_host_bulk_stream_start(bus_info.rhport - 1, device->device_address, endpoint,
        ring, storage, buffer_size)) {
        port_free(storage);
        port_free(ring);
        mp_raise_usb_core_USBError(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_endpoint);
    }
    self->device = MP_OBJ_FROM_PTR(device);
    self->ring = ring;
    self->storage = storage;
    self->leftover = NULL;
    self->leftover_len = 0;
    self->leftover_pos = 0;
    self->lost_packets = 0;
    self->device_address = device->device_address;
    self->endpoint = endpoint;
    _active[slot] = self;
}

bool common_hal_usb_host_bulk_instream_deinited(usb_host_bulk_instream_obj_t *self) {
    if (self->device != MP_OBJ_NULL && !self->ring->active) {
        // Clearing active is the host's last access, so the ring now holds
        // every byte it wrote.
        __dmb();
        _retire(self);
    }
    return self->device == MP_OBJ_NULL;
}

// Also the finaliser
void common_hal_usb_host_bulk_instream_deinit(usb_host_bulk_instream_obj_t *self) {
    if (self->device != MP_OBJ_NULL) {
        _stop(self);
        _free_ring(self);
    }
    // Drop the bytes left by an ended stream. The GC frees their buffer.
    self->leftover = NULL;
    self->leftover_len = 0;
    self->leftover_pos = 0;
}

uint32_t common_hal_usb_host_bulk_instream_read(usb_host_bulk_instream_obj_t *self, uint8_t *data, uint32_t len) {
    if (self->ring != NULL) {
        return pio_usb_host_bulk_stream_read(self->ring, data, len);
    }
    uint32_t count = MIN(len, self->leftover_len - self->leftover_pos);
    if (count > 0) {
        memcpy(data, self->leftover + self->leftover_pos, count);
        self->leftover_pos += count;
    }
    if (self->leftover != NULL && self->leftover_pos == self->leftover_len) {
        m_del(uint8_t, self->leftover, self->leftover_len);
        self->leftover = NULL;
        self->leftover_len = 0;
        self->leftover_pos = 0;
    }
    return count;
}

uint32_t common_hal_usb_host_bulk_instream_get_in_waiting(usb_host_bulk_instream_obj_t *self) {
    if (self->ring != NULL) {
        return _ring_in_waiting(self->ring);
    }
    return self->leftover_len - self->leftover_pos;
}

uint32_t common_hal_usb_host_bulk_instream_get_lost_packets(usb_host_bulk_instream_obj_t *self) {
    if (self->ring != NULL) {
        return self->ring->stats.overrun_packets;
    }
    return self->lost_packets;
}

void common_hal_usb_host_bulk_instream_reset_input_buffer(usb_host_bulk_instream_obj_t *self) {
    // read_pos belongs to the consumer, so moving it is safe while capturing.
    self->ring->read_pos = self->ring->write_pos;
}

// Stops every stream from device_address, or every stream opened through
// device if it is not NULL.
static void _stop_matching(uint8_t device_address, usb_core_device_obj_t *device) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(_active); i++) {
        usb_host_bulk_instream_obj_t *stream = _active[i];
        if (stream == NULL) {
            continue;
        }
        if (device != NULL ? stream->device == MP_OBJ_FROM_PTR(device) :
            stream->device_address == device_address) {
            _stop(stream);
        }
    }
}

void usb_host_bulk_stop_device(uint8_t device_address) {
    _stop_matching(device_address, NULL);
}

bool usb_host_bulk_endpoint_busy(uint8_t device_address, uint8_t endpoint) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(_active); i++) {
        usb_host_bulk_instream_obj_t *stream = _active[i];
        if (stream != NULL && stream->device_address == device_address &&
            stream->endpoint == endpoint && stream->ring->active) {
            return true;
        }
    }
    return false;
}

void usb_host_bulk_device_deinit(usb_core_device_obj_t *device) {
    _stop_matching(0, device);
}
