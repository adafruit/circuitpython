// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/ringbuf.h"

#include "shared-module/audiocore/__init__.h"
#include "shared-module/usb/core/Device.h"

// The PCM ring holds this many get_buffer() buffers. Playback starts, and
// restarts after an underrun, once USBIN_TARGET_BUFFERS of them are waiting.
// The ring is what holds the stream while its consumer is held up. Where
// TinyUSB runs in a task of its own, packets keep arriving while the VM is
// held up by other work. A consumer that doesn't catch up afterwards, such as
// an audio output, is left with up to that much more latency, which drift
// correction then takes out slowly.
#ifndef USBIN_RING_BUFFERS
#if CIRCUITPY_USB_HOST_TASK
#define USBIN_RING_BUFFERS (32)
#else
#define USBIN_RING_BUFFERS (8)
#endif
#endif
#define USBIN_TARGET_BUFFERS (2)
#define USBIN_NO_INTERFACE (0xff)

typedef struct {
    // so USBIn can be used directly as an audiosample source.
    audiosample_base_t base;
    // Keeps the device object, which owns the open endpoint, alive.
    usb_core_device_obj_t *device;
    uint8_t device_address;
    uint8_t endpoint;
    // The streaming interface that has endpoint, or USBIN_NO_INTERFACE.
    uint8_t interface;
    uint8_t bit_depth; // on the wire; base.bits_per_sample is what we produce
    uint8_t wire_frame_bytes;
    uint8_t frame_bytes;
    // A transfer is queued. False once the stream stopped
    volatile bool streaming;
    // Both buffers below are outside the VM heap; NULL xfer_buffer means deinited.
    uint8_t *xfer_buffer;
    uint16_t xfer_buffer_len;
    // Converted PCM frames, whole ones only. Filled by the transfer's
    // completion callback, which runs in tuh_task(), and drained by
    // get_buffer(), record() and reset_buffer() on the VM. tuh_task() may be a
    // task of its own (CIRCUITPY_USB_HOST_TASK), so the ring, overflowed and
    // overflow are only used with the lock in USBIn.c held. The buffer is
    // outside the VM heap.
    ringbuf_t ring;
    uint32_t buffer_frames;
    bool primed;
    // The ring was full and audio was dropped; get_buffer() skips ahead.
    bool overflowed;
    // Audio was dropped since the user last read the overflow property.
    bool overflow;
    // Average of the frames waiting as get_buffer() finds them, scaled by 2^USBIN_LEVEL_SHIFT.
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
    #if !CIRCUITPY_USB_HOST_ISO_RECORDS
    // The host controller frame number when the last transfer was queued.
    uint16_t queued_frame;
    // Frames (microframes at high speed) between the endpoint's packets.
    uint16_t frame_interval;
    // queued_frame is set.
    bool frame_known;
    // A transfer was aborted. The host controller may write the transfer
    // buffer until the end of the frame.
    bool aborted;
    // Transfers in a row that failed.
    uint8_t failures;
    #endif
} usb_host_audio_usbin_obj_t;

// Stops a stream left running by the VM that just finished.
void usb_host_audio_reset(void);
// Stops receiving from the device before a control request changes or
// disables its endpoints. The object keeps the audio received so far.
// Never raises or allocates.
void usb_host_audio_stop_device(uint8_t device_address);
