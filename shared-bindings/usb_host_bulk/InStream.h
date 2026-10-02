// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "common-hal/usb_host_bulk/InStream.h"
#include "shared-module/usb/core/Device.h"

extern const mp_obj_type_t usb_host_bulk_instream_type;

void common_hal_usb_host_bulk_instream_construct(usb_host_bulk_instream_obj_t *self,
    usb_core_device_obj_t *device, uint8_t endpoint, uint32_t buffer_size);
// Stops capture and moves the unread bytes to the VM heap, where they stay
// readable. Raises USBTimeoutError, and stays usable, if the host keeps the
// ring. From a finaliser, pass abandon=true instead: nothing is allocated, the
// unread bytes are dropped, and the ring is leaked rather than freed while the
// host may still write to it.
void common_hal_usb_host_bulk_instream_deinit(usb_host_bulk_instream_obj_t *self, bool abandon);
// A stream the host ended by itself (stall, unplug, reconfiguration) is deinited
// here, so this may allocate: call it only from the VM.
bool common_hal_usb_host_bulk_instream_deinited(usb_host_bulk_instream_obj_t *self);
uint32_t common_hal_usb_host_bulk_instream_read(usb_host_bulk_instream_obj_t *self, uint8_t *data, uint32_t len);
uint32_t common_hal_usb_host_bulk_instream_get_in_waiting(usb_host_bulk_instream_obj_t *self);
uint32_t common_hal_usb_host_bulk_instream_get_lost_packets(usb_host_bulk_instream_obj_t *self);
void common_hal_usb_host_bulk_instream_reset_input_buffer(usb_host_bulk_instream_obj_t *self);

// Hooks for usb.core.Device, which must not leave a stream polling an endpoint
// it reconfigures, reads, writes or closes.

// Stops every stream from device_address. Raises USBTimeoutError if the host
// does not let go of a ring in time.
void usb_host_bulk_stop_device(uint8_t device_address);
// True while a stream is polling this endpoint.
bool usb_host_bulk_endpoint_busy(uint8_t device_address, uint8_t endpoint);
// Stops every stream from device_address that it can. Never raises or
// allocates, so it is safe from finalisers and TinyUSB callbacks.
void usb_host_bulk_device_gone(uint8_t device_address);
// Stops the streams opened through this Device object, but not ones opened
// through another Device for the same address. Never raises or allocates.
void usb_host_bulk_device_deinit(usb_core_device_obj_t *device);
