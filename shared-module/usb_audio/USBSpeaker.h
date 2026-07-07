// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"
#include "shared-module/usb_audio/usb_audio_descriptors.h"

// One half-buffer of the audiosample double-buffer holds this many frames. The
// output backend pulls a half each get_buffer() call, so this sets the pull
// granularity and the silence-pad chunk on underrun. It is deliberately
// independent of the USB OUT software FIFO (the ring below) so the push and pull
// stages can be tuned separately, and small enough that two halves sit
// comfortably inside the ring.
#define USB_AUDIO_SPEAKER_FRAMES_PER_BUFFER (128)

typedef struct usb_audio_usbspeaker_obj {
    // First member so the object can be used directly as an audiosample source.
    audiosample_base_t base;

    // Host -> board receive ring (the "push" stage). Filled by the USB
    // background task via usb_audio_usbspeaker_background_drain() and drained by
    // get_buffer(). Single producer (task), single consumer (output DMA ISR);
    // see the concurrency notes in USBSpeaker.c.
    uint8_t *ring;
    size_t ring_len;
    size_t ring_head;   // next write offset
    size_t ring_tail;   // next read offset
    size_t ring_count;  // valid bytes currently in the ring

    // Owned double-buffer returned to the output backend by get_buffer().
    uint8_t *output_buffer;
    uint8_t output_index;  // 0 or 1: which half get_buffer() fills next
} usb_audio_usbspeaker_obj_t;

// audiosample protocol implementation. Not exposed to Python because get_buffer()
// runs in the output backend's refill interrupt.
void usb_audio_usbspeaker_reset_buffer(usb_audio_usbspeaker_obj_t *self,
    bool single_channel_output, uint8_t channel);
audioio_get_buffer_result_t usb_audio_usbspeaker_get_buffer(usb_audio_usbspeaker_obj_t *self,
    bool single_channel_output, uint8_t channel, uint8_t **buffer, uint32_t *buffer_length);

// Push n bytes received on the USB OUT endpoint into the active speaker's ring.
// Called from the USB background task (see usb_audio_task()). No-op when no
// speaker is active. Overrun policy: drop the oldest buffered bytes.
void usb_audio_usbspeaker_background_drain(const uint8_t *in, size_t n);

// Drop anything buffered in the active speaker's ring so a (re)opened stream
// starts from live audio. Called when the host opens/closes the OUT streaming
// interface.
void usb_audio_usbspeaker_streaming_reset(void);
