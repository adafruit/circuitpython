// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/misc.h"

#include "shared-bindings/usb_audio/USBSpeaker.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-module/usb_audio/__init__.h"

#include "tusb.h"

// Only one speaker can be fed by the single USB OUT endpoint at a time. This
// points at the most recently constructed USBSpeaker, or NULL when none exists,
// mirroring active_microphone in USBMicrophone.c. The USB background task pushes
// received bytes into it via usb_audio_usbspeaker_background_drain().
static usb_audio_usbspeaker_obj_t *active_speaker = NULL;

void common_hal_usb_audio_usbspeaker_construct(usb_audio_usbspeaker_obj_t *self) {
    // The pipeline treats the speaker as an ordinary audiosample source, so
    // populate base from the format negotiated by usb_audio.enable(). The UAC2
    // format we present is 16-bit signed LE PCM, which is exactly what the
    // CircuitPython audio pipeline carries, so no conversion is needed.
    self->base.sample_rate = usb_audio_sample_rate;
    self->base.bits_per_sample = usb_audio_bits_per_sample;
    self->base.channel_count = usb_audio_channel_count;
    self->base.samples_signed = true;
    self->base.single_buffer = false;
    
    // Full owned double-buffer: two halves of FRAMES_PER_BUFFER frames each. Matches
    // audiocore.RawSample's convention where base.max_buffer_length is the whole
    // buffer and get_buffer() returns half of it.
    self->base.max_buffer_length = (2 * USB_AUDIO_SPEAKER_FRAMES_PER_BUFFER * USB_AUDIO_N_BYTES_PER_SAMPLE * usb_audio_channel_count);
    self->output_buffer = m_malloc_without_collect(self->base.max_buffer_length);
    if (self->output_buffer == NULL) {
        common_hal_usb_audio_usbspeaker_deinit(self);
        m_malloc_fail(self->base.max_buffer_length);
    }
    memset(self->output_buffer, 0, self->base.max_buffer_length);

    // The ring is sized independently of the TinyUSB headers; check it still
    // matches the OUT endpoint's software FIFO so the push side can be reasoned
    // about against the USB plumbing.
    self->ring_len = 16 * ((usb_audio_sample_rate / 1000 + 1) * (USB_AUDIO_N_BYTES_PER_SAMPLE * usb_audio_channel_count));
    assert(self->ring_len == CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ);

    self->ring = m_malloc_without_collect(self->ring_len);
    if (self->ring == NULL) {
        common_hal_usb_audio_usbspeaker_deinit(self);
        m_malloc_fail(self->ring_len);
    }
    memset(self->ring, 0, self->ring_len);

    self->ring_head = 0;
    self->ring_tail = 0;
    self->ring_count = 0;
    self->output_index = 0;

    // The most recently created speaker receives the host's OUT stream.
    active_speaker = self;
}

void common_hal_usb_audio_usbspeaker_deinit(usb_audio_usbspeaker_obj_t *self) {
    // Stop directing USB OUT data at this object. The producer (drain) and this
    // deinit both run in non-interrupt context, so the pointer swap needs no
    // interrupt guard.
    if (active_speaker == self) {
        active_speaker = NULL;
    }
    audiosample_mark_deinit(&self->base);
}

bool common_hal_usb_audio_usbspeaker_deinited(usb_audio_usbspeaker_obj_t *self) {
    return audiosample_deinited(&self->base);
}

bool common_hal_usb_audio_usbspeaker_get_connected(usb_audio_usbspeaker_obj_t *self) {
    (void)self;
    // True while the host has opened the OUT streaming interface, i.e. it is
    // actively sending audio. Speaker-specific so it stays correct when a mic
    // shares the same headset function.
    return usb_audio_speaker_streaming();
}

// --------------------------------------------------------------------+
// Receive ring (push side, producer = USB background task)
// --------------------------------------------------------------------+
//
// The ring decouples the two independently clocked stages from
// usb_audio_output_plan.md: USB push (paced by the host's SOF clock, in the
// background task) and the audiosample pull (paced by the output backend's
// sample clock, in its refill interrupt). It is a single-producer/
// single-consumer ring across an interrupt boundary:
//
//   * Producer: usb_audio_usbspeaker_background_drain(), task context.
//   * Consumer: usb_audio_usbspeaker_get_buffer(), output DMA/refill ISR.
//
// The consumer is an interrupt, so it can never be preempted by the producer and
// needs no guard of its own. The producer can be preempted by the consumer, and
// its drop-oldest overrun handling touches both cursors, so it does its whole
// read-modify-write with interrupts disabled.

void usb_audio_usbspeaker_streaming_reset(void) {
    usb_audio_usbspeaker_obj_t *self = active_speaker;
    if (self == NULL) {
        return;
    }
    common_hal_mcu_disable_interrupts();
    self->ring_head = 0;
    self->ring_tail = 0;
    self->ring_count = 0;
    common_hal_mcu_enable_interrupts();
}

void usb_audio_usbspeaker_background_drain(const uint8_t *in, size_t n) {
    usb_audio_usbspeaker_obj_t *self = active_speaker;
    if (self == NULL || n == 0) {
        return;
    }
    if (n >= self->ring_len) {
        // A single chunk larger than the whole ring can only contribute its
        // newest tail. (Cannot happen with USB packets << ring size, but keep
        // the copies provably in-bounds.)
        in += n - self->ring_len;
        n = self->ring_len;
    }

    common_hal_mcu_disable_interrupts();

    size_t free_space = self->ring_len - self->ring_count;
    if (n > free_space) {
        // Overrun: advance the read cursor past the oldest bytes we're about to
        // overwrite, keeping latency bounded and following the newest host audio.
        size_t drop = n - free_space;
        self->ring_tail = (self->ring_tail + drop) % self->ring_len;
        self->ring_count -= drop;
    }

    // Copy in one or two segments, wrapping at the end of the ring.
    size_t first = MIN(n, self->ring_len - self->ring_head);
    memcpy(&self->ring[self->ring_head], in, first);
    if (n > first) {
        memcpy(&self->ring[0], in + first, n - first);
    }
    self->ring_head = (self->ring_head + n) % self->ring_len;
    self->ring_count += n;

    common_hal_mcu_enable_interrupts();
}

uint32_t common_hal_usb_audio_usbspeaker_read(usb_audio_usbspeaker_obj_t *self,
    void *buffer, uint32_t length) {
    // Hand the most recent host audio to Python so it can be analysed (e.g. an
    // audio-reactive effect or VU meter). This drains the ring just like the
    // output backend's get_buffer() does, so it is an alternative *consumer*:
    // a USBSpeaker being read this way must not also be play()ed to an output
    // backend at the same time, or the two consumers would race on the single
    // SPSC ring.
    //
    // Unlike get_buffer() (which runs in the output refill ISR and so needs no
    // guard) this runs in VM/task context and can be preempted by the USB
    // producer, so it brackets its read-modify-write with interrupts disabled,
    // mirroring usb_audio_usbspeaker_background_drain().
    size_t bytes_per_frame = (self->base.bits_per_sample / 8) * self->base.channel_count;
    size_t want = (size_t)length * bytes_per_frame;

    common_hal_mcu_disable_interrupts();
    size_t to_copy = MIN(self->ring_count, want);
    // Never split a frame across calls (the ring always holds whole UAC2 frames,
    // but stay defensive so the returned count is always a whole number).
    to_copy -= to_copy % bytes_per_frame;

    size_t first = MIN(to_copy, self->ring_len - self->ring_tail);
    memcpy(buffer, &self->ring[self->ring_tail], first);
    if (to_copy > first) {
        memcpy((uint8_t *)buffer + first, &self->ring[0], to_copy - first);
    }
    self->ring_tail = (self->ring_tail + to_copy) % self->ring_len;
    self->ring_count -= to_copy;
    common_hal_mcu_enable_interrupts();

    return to_copy / bytes_per_frame;
}

// --------------------------------------------------------------------+
// audiosample protocol (pull side, consumer = output backend)
// --------------------------------------------------------------------+

void usb_audio_usbspeaker_reset_buffer(usb_audio_usbspeaker_obj_t *self,
    bool single_channel_output, uint8_t channel) {
    (void)single_channel_output;
    (void)channel;
    // Begin playback from live audio rather than whatever was buffered before:
    // drop the ring and restart the double-buffer. Guarded because some ports may
    // call reset_buffer outside the initial setup.
    common_hal_mcu_disable_interrupts();
    self->ring_head = 0;
    self->ring_tail = 0;
    self->ring_count = 0;
    common_hal_mcu_enable_interrupts();
    self->output_index = 0;
}

audioio_get_buffer_result_t usb_audio_usbspeaker_get_buffer(usb_audio_usbspeaker_obj_t *self,
    bool single_channel_output, uint8_t channel, uint8_t **buffer, uint32_t *buffer_length) {

    uint32_t half = self->base.max_buffer_length / 2;
    uint8_t *out = self->output_buffer + half * self->output_index;
    self->output_index = 1 - self->output_index;

    // Consumer side of the SPSC ring (runs in the output backend's refill ISR).
    // It is never preempted by the producer, so no interrupt guard is required.
    size_t to_copy = MIN(self->ring_count, (size_t)half);

    size_t first = MIN(to_copy, self->ring_len - self->ring_tail);
    memcpy(out, &self->ring[self->ring_tail], first);
    if (to_copy > first) {
        memcpy(out + first, &self->ring[0], to_copy - first);
    }
    self->ring_tail = (self->ring_tail + to_copy) % self->ring_len;
    self->ring_count -= to_copy;

    if (to_copy < half) {
        // Underrun: pad the remainder with silence. Samples are signed, so
        // silence is 0. This is the consume-side of the pacing failure mode
        // tracked in the usb-audio-artifact-pacing memory: we never spin.
        memset(out + to_copy, 0, half - to_copy);
    }

    // Mono only for v1, so the single-channel offset is always 0; computed the
    // same way as audiocore.RawSample so stereo (interleaved ring) can extend it.
    if (single_channel_output) {
        out += (channel % self->base.channel_count) * (self->base.bits_per_sample / 8);
    }

    *buffer = out;
    *buffer_length = half;
    // A live USB stream is infinite; never report DONE or the backend would stop.
    return GET_BUFFER_MORE_DATA;
}
