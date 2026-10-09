// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"
#include "shared-module/usb/core/Device.h"

// The PCM ring holds this many get_buffer() buffers. Playback starts, and
// restarts after an underrun, once USBIN_TARGET_BUFFERS of them are waiting.
#define USBIN_RING_BUFFERS (8)
#define USBIN_TARGET_BUFFERS (2)

typedef struct {
    // so USBIn can be used directly as an audiosample source.
    audiosample_base_t base;
    // Keeps the device object, which owns the open endpoint, alive.
    usb_core_device_obj_t *device;
    uint8_t device_address;
    uint8_t endpoint;
    uint8_t bit_depth; // on the wire; base.bits_per_sample is what we produce
    uint8_t wire_frame_bytes;
    uint8_t frame_bytes;
    // A transfer is queued. False once the stream stopped
    bool streaming;
    // Both buffers below are outside the VM heap; NULL xfer_buffer means deinited.
    uint8_t *xfer_buffer;
    uint16_t xfer_buffer_len;
    // Converted PCM frames. Filled by the transfer's completion callback and
    // drained by get_buffer() and record(). All of them run on core 0 outside
    // interrupts (TinyUSB's task and the audio refill are background callbacks).
    uint8_t *ring;
    uint32_t ring_frames;
    uint32_t ring_head;
    uint32_t ring_count;
    uint32_t buffer_frames;
    bool primed;
    // The ring dropped audio; the next reader skips ahead.
    bool overflowed;
    // Audio was dropped since the user last read the overflow property.
    bool overflow;
    // Average of ring_count as get_buffer() finds it, scaled by 2^USBIN_LEVEL_SHIFT.
    uint32_t level;
    // Sum of how far level was from the target, less what was corrected.
    int32_t drift;
    // Owned double-buffer returned to the output backend by get_buffer(),
    // followed by scratch space for drift correction. Allocated lazily on the
    // first reset_buffer() so record()-only use pays no extra RAM.
    // base.max_buffer_length is the two halves; get_buffer() returns
    // output_half_bytes of it.
    uint8_t *output_buffer;
    uint32_t output_half_bytes;
    uint8_t output_index;
} usb_host_audio_usbin_obj_t;

// Stops a stream left running by the VM that just finished.
void usb_host_audio_reset(void);
// Stops receiving from the device before a control request changes or
// disables its endpoints. The object keeps the audio received so far.
// Never raises or allocates.
void usb_host_audio_stop_device(uint8_t device_address);
