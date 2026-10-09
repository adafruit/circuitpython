// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/usb_host_audio/USBIn.h"

#include <string.h>

#include "tusb.h"
#include "host/usbh_pvt.h"

#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/usb/core/__init__.h"
#include "shared-bindings/usb/core/Device.h"
#include "supervisor/port.h"
#include "supervisor/port_heap.h"
#include "supervisor/shared/tick.h"

// Room for a good number of records per transfer, for when the ring in the
// host controller has a backlog.
#define USBIN_MIN_XFER_BUFFER (1024)
// The host controller marks lost packets with a record of this length.
#define USBIN_LOST_RECORD (0xffff)
// get_buffer() averages the frames waiting over about 2^this calls.
#define USBIN_LEVEL_SHIFT (6)
// record() gives up when the microphone sends nothing for this long.
#define USBIN_RECORD_TIMEOUT_MS (1000)

// The host controller receives one isochronous IN stream at a time, so there is
// one streaming object at most. TinyUSB copies the transfer and calls back with
// only user_data, which carries _seq so that a completion which was already on
// its way when its stream ended is recognised and dropped.
static usb_host_audio_usbin_obj_t *_active;
static uintptr_t _seq;

static void _ring_append(usb_host_audio_usbin_obj_t *self, const uint8_t *data, uint32_t frames) {
    if (frames > self->ring_frames) {
        data += (frames - self->ring_frames) * self->wire_frame_bytes;
        frames = self->ring_frames;
    }
    if (self->ring_count + frames > self->ring_frames) {
        // Nothing is reading fast enough. Keep the newest audio.
        uint32_t excess = self->ring_count + frames - self->ring_frames;
        self->ring_head = (self->ring_head + excess) % self->ring_frames;
        self->ring_count -= excess;
        self->overflowed = true;
        self->overflow = true;
    }
    uint32_t tail = (self->ring_head + self->ring_count) % self->ring_frames;
    self->ring_count += frames;
    while (frames > 0) {
        uint32_t run = MIN(frames, self->ring_frames - tail);
        uint8_t *dest = self->ring + tail * self->frame_bytes;
        if (self->bit_depth > 16) {
            // Keep the top two bytes of each little endian 3 or 4 byte sample.
            const uint32_t sample_bytes = self->bit_depth / 8;
            for (uint32_t i = 0; i < run * self->base.channel_count; i++) {
                *dest++ = data[sample_bytes - 2];
                *dest++ = data[sample_bytes - 1];
                data += sample_bytes;
            }
        } else {
            memcpy(dest, data, run * self->frame_bytes);
            data += run * self->frame_bytes;
        }
        tail = (tail + run) % self->ring_frames;
        frames -= run;
    }
}

static void _ring_take(usb_host_audio_usbin_obj_t *self, uint8_t *dest, uint32_t frames) {
    uint32_t first = MIN(frames, self->ring_frames - self->ring_head);
    if (dest != NULL) {
        memcpy(dest, self->ring + self->ring_head * self->frame_bytes, first * self->frame_bytes);
        memcpy(dest + first * self->frame_bytes, self->ring, (frames - first) * self->frame_bytes);
    }
    self->ring_head = (self->ring_head + frames) % self->ring_frames;
    self->ring_count -= frames;
}

static void _transfer_done_cb(tuh_xfer_t *xfer);

static bool _queue_transfer(usb_host_audio_usbin_obj_t *self) {
    tuh_xfer_t xfer = {
        .daddr = self->device_address,
        .ep_addr = self->endpoint,
        .buffer = self->xfer_buffer,
        .buflen = self->xfer_buffer_len,
        .complete_cb = _transfer_done_cb,
        .user_data = _seq,
    };
    self->streaming = tuh_edpt_xfer(&xfer);
    return self->streaming;
}

// Runs inside tuh_task(), so as a background callback on core 0.
static void _transfer_done_cb(tuh_xfer_t *xfer) {
    usb_host_audio_usbin_obj_t *self = _active;
    if (self == NULL || xfer->user_data != _seq) {
        // Late completion of a stream that already ended.
        return;
    }
    if (xfer->result != XFER_RESULT_SUCCESS) {
        self->streaming = false;
        return;
    }
    // The transfer holds whole records: a little endian uint16_t length, then
    // one packet's payload.
    const uint8_t *buf = self->xfer_buffer;
    uint32_t len = MIN(xfer->actual_len, self->xfer_buffer_len);
    uint32_t pos = 0;
    while (pos + 2 <= len) {
        uint32_t record_len = buf[pos] | (buf[pos + 1] << 8);
        pos += 2;
        if (record_len == USBIN_LOST_RECORD) {
            self->overflow = true;
            continue;
        }
        record_len = MIN(record_len, len - pos);
        _ring_append(self, buf + pos, record_len / self->wire_frame_bytes);
        pos += record_len;
    }
    _queue_transfer(self);
}

void common_hal_usb_host_audio_usbin_construct(usb_host_audio_usbin_obj_t *self,
    usb_core_device_obj_t *device, uint8_t endpoint, uint32_t sample_rate,
    uint8_t channel_count, uint8_t bit_depth, bool samples_signed,
    uint16_t max_packet_size, uint32_t buffer_frames) {
    // The finaliser may run on an object that a raise below left half made.
    self->xfer_buffer = NULL;
    if (_active != NULL) {
        if (common_hal_usb_host_audio_usbin_get_streaming(_active)) {
            // The host controller would hand the one stream it receives to the
            // newer endpoint and fail the older one.
            mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_USBIn);
        }
        // Its stream ended, so it is of no more use. Don't make the caller
        // find and deinit an object that it may not even have any more.
        common_hal_usb_host_audio_usbin_deinit(_active);
    }

    self->device = device;
    self->device_address = device->device_address;
    self->endpoint = endpoint;
    self->bit_depth = bit_depth;
    self->wire_frame_bytes = (bit_depth / 8) * channel_count;
    // The audio pipeline carries 8 and 16 bit samples, so 24 and 32 bit ones are narrowed.
    self->base.bits_per_sample = bit_depth == 8 ? 8 : 16;
    self->base.channel_count = channel_count;
    self->base.sample_rate = sample_rate;
    self->base.samples_signed = samples_signed;
    self->base.single_buffer = false;
    self->frame_bytes = (self->base.bits_per_sample / 8) * channel_count;
    self->buffer_frames = buffer_frames;
    self->output_half_bytes = buffer_frames * self->frame_bytes;
    self->base.max_buffer_length = self->output_half_bytes * 2;
    self->output_buffer = NULL;
    self->output_index = 0;
    self->ring_frames = buffer_frames * USBIN_RING_BUFFERS;
    self->ring_head = 0;
    self->ring_count = 0;
    self->primed = false;
    self->overflowed = false;
    self->overflow = false;
    self->streaming = false;

    if (!common_hal_usb_core_device_open_endpoint(device, endpoint)) {
        mp_raise_usb_core_USBError(NULL);
    }

    // A record that doesn't fit in the transfer buffer is dropped by the host
    // controller, so leave room for the largest packet and its length.
    self->xfer_buffer_len = MAX(USBIN_MIN_XFER_BUFFER, max_packet_size + 2);
    size_t ring_bytes = self->ring_frames * self->frame_bytes;
    self->ring = port_malloc(ring_bytes, false);
    // The host controller writes the transfer buffer.
    self->xfer_buffer = port_malloc(self->xfer_buffer_len, true);
    if (self->ring == NULL || self->xfer_buffer == NULL) {
        port_free(self->ring);
        port_free(self->xfer_buffer);
        self->ring = NULL;
        self->xfer_buffer = NULL;
        m_malloc_fail(ring_bytes + self->xfer_buffer_len);
    }

    _seq++;
    _active = self;
    if (!_queue_transfer(self)) {
        common_hal_usb_host_audio_usbin_deinit(self);
        mp_raise_usb_core_USBError(NULL);
    }
}

bool common_hal_usb_host_audio_usbin_deinited(usb_host_audio_usbin_obj_t *self) {
    return self->xfer_buffer == NULL;
}

void common_hal_usb_host_audio_usbin_deinit(usb_host_audio_usbin_obj_t *self) {
    if (common_hal_usb_host_audio_usbin_deinited(self)) {
        return;
    }
    if (_active == self) {
        if (self->streaming) {
            // Once this returns the host controller no longer writes the
            // transfer buffer. It fails when the device is gone, and then its
            // endpoints are closed already.
            tuh_edpt_abort_xfer(self->device_address, self->endpoint);
        }
        // Drop a completion that is queued already.
        _seq++;
        _active = NULL;
    }
    self->streaming = false;
    port_free(self->xfer_buffer);
    port_free(self->ring);
    port_free(self->output_buffer);
    self->xfer_buffer = NULL;
    self->ring = NULL;
    self->output_buffer = NULL;
    self->device = NULL;
    self->ring_count = 0;
}

void usb_host_audio_reset(void) {
    if (_active != NULL) {
        common_hal_usb_host_audio_usbin_deinit(_active);
    }
}

void usb_host_audio_stop_device(uint8_t device_address) {
    usb_host_audio_usbin_obj_t *self = _active;
    if (self == NULL || self->device_address != device_address) {
        return;
    }
    if (self->streaming) {
        // The host controller stops polling the endpoint when it sends the
        // request. A transfer queued after that would start polling it again,
        // which some devices answer by no longer answering control requests.
        tuh_edpt_abort_xfer(self->device_address, self->endpoint);
        self->streaming = false;
    }
    // Drop a completion that is queued already, so it queues nothing.
    _seq++;
}

uint32_t common_hal_usb_host_audio_usbin_record_to_buffer(usb_host_audio_usbin_obj_t *self,
    uint8_t *buffer, uint32_t length) {
    uint32_t frames = length / self->base.channel_count;
    uint32_t done = 0;
    if (self->overflowed) {
        // There is a gap behind the buffered audio. Record what comes after it.
        _ring_take(self, NULL, self->ring_count);
        self->overflowed = false;
    }
    uint32_t last_audio = supervisor_ticks_ms32();
    while (done < frames) {
        uint32_t take = MIN(frames - done, self->ring_count);
        if (take > 0) {
            _ring_take(self, buffer + done * self->frame_bytes, take);
            done += take;
            last_audio = supervisor_ticks_ms32();
            continue;
        }
        if (!common_hal_usb_host_audio_usbin_get_streaming(self) || mp_hal_is_interrupted() ||
            supervisor_ticks_ms32() - last_audio > USBIN_RECORD_TIMEOUT_MS) {
            break;
        }
        // The background tasks include TinyUSB, which fills the ring.
        RUN_BACKGROUND_TASKS;
    }
    return done * self->base.channel_count;
}

uint8_t common_hal_usb_host_audio_usbin_get_bit_depth(usb_host_audio_usbin_obj_t *self) {
    return self->bit_depth;
}

bool common_hal_usb_host_audio_usbin_get_samples_signed(usb_host_audio_usbin_obj_t *self) {
    return self->base.samples_signed;
}

bool common_hal_usb_host_audio_usbin_get_streaming(usb_host_audio_usbin_obj_t *self) {
    // Closing the endpoint, as usb.core.Device.set_configuration() does, ends
    // the transfer without a completion.
    if (self->streaming && !usbh_edpt_busy(self->device_address, self->endpoint)) {
        self->streaming = false;
    }
    return self->streaming;
}

bool common_hal_usb_host_audio_usbin_get_overflow(usb_host_audio_usbin_obj_t *self) {
    bool overflow = self->overflow;
    self->overflow = false;
    return overflow;
}

void common_hal_usb_host_audio_usbin_reset_buffer(usb_host_audio_usbin_obj_t *self,
    bool single_channel_output, uint8_t channel) {
    (void)single_channel_output;
    (void)channel;
    if (common_hal_usb_host_audio_usbin_deinited(self)) {
        return;
    }
    if (self->output_buffer == NULL) {
        // The output backend's DMA may read this buffer directly. After the
        // two halves there is room for the frames that _stretch() works from.
        size_t size = self->base.max_buffer_length + (self->buffer_frames + 1) * self->frame_bytes;
        self->output_buffer = port_malloc(size, true);
        if (self->output_buffer == NULL) {
            m_malloc_fail(size);
        }
    }
    self->output_index = 0;
    // Start on live audio rather than on what piled up while nothing played.
    // Dropping that is not an overflow.
    _ring_take(self, NULL, self->ring_count);
    self->primed = false;
    self->overflowed = false;
    self->overflow = false;
}

static void _fill_silence(usb_host_audio_usbin_obj_t *self, uint8_t *buffer, uint32_t bytes) {
    if (self->base.samples_signed) {
        memset(buffer, 0, bytes);
    } else if (self->base.bits_per_sample == 8) {
        memset(buffer, 0x80, bytes);
    } else {
        for (uint32_t i = 0; i < bytes; i += 2) {
            buffer[i] = 0;
            buffer[i + 1] = 0x80;
        }
    }
}

// Make buffer_frames frames in out from the in_frames frames in in, by linear
// interpolation. The first and last frames are kept as they are, so that
// consecutive buffers still join up.
static void _stretch(usb_host_audio_usbin_obj_t *self, uint8_t *out, const uint8_t *in, uint32_t in_frames) {
    const uint32_t channels = self->base.channel_count;
    const uint32_t out_frames = self->buffer_frames;
    // 16.16 fixed point position in the input
    const uint32_t step = ((in_frames - 1) << 16) / (out_frames - 1);
    const bool is_16 = self->base.bits_per_sample == 16;
    const bool is_signed = self->base.samples_signed;
    uint32_t pos = 0;
    for (uint32_t i = 0; i < out_frames; i++, pos += step) {
        uint32_t index = pos >> 16;
        // 15 bits, so that its product with a difference of samples fits.
        int32_t frac = (pos & 0xffff) >> 1;
        if (i == out_frames - 1 || index >= in_frames - 1) {
            index = in_frames - 1;
            frac = 0;
        }
        for (uint32_t c = 0; c < channels; c++) {
            uint32_t a_at = index * channels + c;
            uint32_t b_at = frac ? a_at + channels : a_at;
            uint32_t out_at = i * channels + c;
            int32_t a, b;
            if (is_16) {
                a = is_signed ? ((const int16_t *)in)[a_at] : ((const uint16_t *)in)[a_at];
                b = is_signed ? ((const int16_t *)in)[b_at] : ((const uint16_t *)in)[b_at];
                ((uint16_t *)out)[out_at] = a + (((b - a) * frac) >> 15);
            } else {
                a = is_signed ? ((const int8_t *)in)[a_at] : in[a_at];
                b = is_signed ? ((const int8_t *)in)[b_at] : in[b_at];
                out[out_at] = a + (((b - a) * frac) >> 15);
            }
        }
    }
}

// Called by the output backend's refill, so it must never block.
audioio_get_buffer_result_t common_hal_usb_host_audio_usbin_get_buffer(
    usb_host_audio_usbin_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {
    if (common_hal_usb_host_audio_usbin_deinited(self) || self->output_buffer == NULL) {
        *buffer = NULL;
        *buffer_length = 0;
        return GET_BUFFER_DONE;
    }
    uint8_t *out = self->output_buffer + self->output_half_bytes * self->output_index;
    self->output_index = 1 - self->output_index;

    uint32_t target = self->buffer_frames * USBIN_TARGET_BUFFERS;
    if (self->overflowed) {
        // The consumer stalled or runs slower than the microphone. Skip ahead
        // so that the latency stays bounded.
        self->overflowed = false;
        if (self->ring_count > target) {
            _ring_take(self, NULL, self->ring_count - target);
        }
    }
    if (!self->primed && self->ring_count >= target) {
        self->primed = true;
        self->level = self->ring_count << USBIN_LEVEL_SHIFT;
        self->drift = 0;
    }
    if (self->primed && self->ring_count >= self->buffer_frames) {
        // The microphone's clock and the consumer's drift apart, which shows
        // as a slow change of how much audio is waiting. The count at any one
        // call depends on when both sides were last serviced, so follow its
        // average. Make a buffer from one frame more or less than it holds, the
        // more often the further that average is from the target: the sum of
        // the differences says when. That spreads the corrections evenly,
        // where correcting only beyond a threshold would bunch them up into
        // an audible wobble of the pitch.
        self->level += (int32_t)self->ring_count - (int32_t)(self->level >> USBIN_LEVEL_SHIFT);
        const int32_t every = self->buffer_frames;
        self->drift += (int32_t)(self->level >> USBIN_LEVEL_SHIFT) - (int32_t)target;
        self->drift = MAX(-2 * every, MIN(2 * every, self->drift));
        uint32_t take = self->buffer_frames;
        if (self->drift >= every && self->ring_count > take) {
            take++;
            self->drift -= every;
        } else if (self->drift <= -every) {
            take--;
            self->drift += every;
        }
        if (take == self->buffer_frames) {
            _ring_take(self, out, take);
        } else {
            uint8_t *stretch = self->output_buffer + self->output_half_bytes * 2;
            _ring_take(self, stretch, take);
            _stretch(self, out, stretch, take);
        }
    } else {
        // Play silence until enough audio is waiting again to ride out the
        // jitter in how both sides are serviced.
        self->primed = false;
        _fill_silence(self, out, self->output_half_bytes);
    }

    if (single_channel_output) {
        out += (channel % self->base.channel_count) * (self->base.bits_per_sample / 8);
    }
    *buffer = out;
    *buffer_length = self->output_half_bytes;
    // A live microphone is an infinite stream; never report DONE or the backend stops.
    return GET_BUFFER_MORE_DATA;
}
