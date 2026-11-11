// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/usb_host_audio/USBIn.h"

#include <string.h>

#include "tusb.h"
#include "host/hcd.h"
#include "host/usbh_pvt.h"

#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/usb/core/__init__.h"
#include "shared-bindings/usb/core/Device.h"
#include "shared-bindings/usb_host/Port.h"
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
// deinit() waits this long for the device to stop streaming. A request without
// a data stage takes at most 50 ms.
#define USBIN_STOP_TIMEOUT_MS (100)
// The stream ends after this many failed transfers in a row. A microphone that
// was unplugged or stopped sending fails every one.
#define USBIN_MAX_FAILURES (32)
// Frame numbers are compared modulo this. Host controllers count to a multiple of it.
#define USBIN_FRAME_MASK (0x7ff)
// An aborted transfer of a periodic endpoint ends at the end of the frame.
#define USBIN_ABORT_SETTLE_US (2000)

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
    #if !CIRCUITPY_USB_HOST_ISO_RECORDS
    // The transfer receives the packet of the endpoint's next service interval.
    // Those that went by since the last transfer was queued are lost.
    uint16_t frame = hcd_frame_number(TUH_OPT_RHPORT) & USBIN_FRAME_MASK;
    if (self->frame_known) {
        uint16_t elapsed = (frame - self->queued_frame) & USBIN_FRAME_MASK;
        if (elapsed > self->frame_interval) {
            self->overflow = true;
        }
    }
    self->queued_frame = frame;
    self->frame_known = true;
    #endif
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
    #if CIRCUITPY_USB_HOST_ISO_RECORDS
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
    #else
    if (xfer->result == XFER_RESULT_SUCCESS) {
        // The transfer holds one packet.
        self->failures = 0;
        uint32_t len = MIN(xfer->actual_len, self->xfer_buffer_len);
        _ring_append(self, self->xfer_buffer, len / self->wire_frame_bytes);
    } else if (xfer->result == XFER_RESULT_FAILED && self->failures < USBIN_MAX_FAILURES) {
        // A packet that was damaged or not received in its frame. Isochronous
        // packets are not retried.
        self->failures++;
        self->overflow = true;
    } else {
        self->streaming = false;
        return;
    }
    #endif
    _queue_transfer(self);
}

// The descriptor of endpoint in the device's configuration, or NULL. Sets
// interface to the number of the interface whose alternate setting has it.
static const tusb_desc_endpoint_t *_find_endpoint(usb_core_device_obj_t *device, uint8_t endpoint,
    uint8_t *interface) {
    *interface = USBIN_NO_INTERFACE;
    const uint8_t *desc = device->configuration_descriptor;
    if (desc == NULL) {
        return NULL;
    }
    const uint8_t *end = desc + tu_le16toh(((const tusb_desc_configuration_t *)desc)->wTotalLength);
    uint8_t current = USBIN_NO_INTERFACE;
    for (const uint8_t *p = tu_desc_next(desc); p < end; p = tu_desc_next(p)) {
        if (tu_desc_type(p) == TUSB_DESC_INTERFACE) {
            current = ((const tusb_desc_interface_t *)p)->bInterfaceNumber;
        } else if (tu_desc_type(p) == TUSB_DESC_ENDPOINT &&
                   ((const tusb_desc_endpoint_t *)p)->bEndpointAddress == endpoint) {
            *interface = current;
            return (const tusb_desc_endpoint_t *)p;
        }
    }
    return NULL;
}

// Select alternate setting 0, which stops the microphone sending. usb.core
// ends the transfer first, as it does for the same request from Python. This
// runs when unwinding from an exception and in the finaliser, so it neither
// raises nor waits long.
static void _stop_device(usb_host_audio_usbin_obj_t *self) {
    if (self->interface == USBIN_NO_INTERFACE || common_hal_usb_core_device_deinited(self->device)) {
        return;
    }
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        common_hal_usb_core_device_ctrl_transfer(self->device,
            TUSB_DIR_OUT | (TUSB_REQ_TYPE_STANDARD << 5) | TUSB_REQ_RCPT_INTERFACE,
            TUSB_REQ_SET_INTERFACE, 0, self->interface, NULL, 0, USBIN_STOP_TIMEOUT_MS);
        nlr_pop();
    }
}

void common_hal_usb_host_audio_usbin_construct(usb_host_audio_usbin_obj_t *self,
    usb_core_device_obj_t *device, uint8_t endpoint, uint32_t sample_rate,
    uint8_t channel_count, uint8_t bit_depth, bool samples_signed,
    uint16_t max_packet_size, uint32_t buffer_frames, uint32_t iso_buffer_size) {
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
    const tusb_desc_endpoint_t *endpoint_desc = _find_endpoint(device, endpoint, &self->interface);

    #if CIRCUITPY_USB_HOST_ISO_RECORDS
    (void)endpoint_desc;
    // The host controller keeps the packets that arrive between transfers in a
    // ring of its own. Only one isochronous stream at a time can use it.
    if (!common_hal_usb_host_port_claim_iso_buffer(iso_buffer_size)) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_usb_host);
    }

    // A record that doesn't fit in the transfer buffer is dropped by the host
    // controller, so leave room for the largest packet and its length.
    self->xfer_buffer_len = MAX(USBIN_MIN_XFER_BUFFER, max_packet_size + 2);
    #else
    (void)iso_buffer_size;
    // Each transfer receives one packet. A longer one would ask for more
    // packets in the same frame.
    uint8_t interval = 1;
    if (endpoint_desc != NULL) {
        max_packet_size = tu_edpt_packet_size(endpoint_desc);
        interval = endpoint_desc->bInterval;
    }
    mp_arg_validate_int_min(max_packet_size, 1, MP_QSTR_max_packet_size);
    self->xfer_buffer_len = max_packet_size;
    self->frame_interval = 1 << (MIN(MAX(interval, 1), 16) - 1);
    self->frame_known = false;
    self->aborted = false;
    self->failures = 0;
    #endif
    size_t ring_bytes = self->ring_frames * self->frame_bytes;
    self->ring = port_malloc(ring_bytes, false);
    // The host controller writes the transfer buffer.
    self->xfer_buffer = port_malloc(self->xfer_buffer_len, true);
    if (self->ring == NULL || self->xfer_buffer == NULL) {
        #if CIRCUITPY_USB_HOST_ISO_RECORDS
        common_hal_usb_host_port_release_iso_buffer();
        #endif
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
    if (common_hal_usb_host_audio_usbin_get_streaming(self)) {
        // Don't leave the microphone sending to an endpoint that is no longer
        // polled. A stream that already ended was stopped by whoever ended it,
        // and its interface may be streaming for someone else by now.
        _stop_device(self);
    }
    if (_active == self) {
        if (self->streaming) {
            // It fails when the device is gone, and then its endpoints are
            // closed already.
            tuh_edpt_abort_xfer(self->device_address, self->endpoint);
            #if !CIRCUITPY_USB_HOST_ISO_RECORDS
            self->aborted = true;
            #endif
        }
        // Drop a completion that is queued already.
        _seq++;
        _active = NULL;
        #if CIRCUITPY_USB_HOST_ISO_RECORDS
        // Whatever ended the stream also stopped the host controller writing
        // to its ring, and once the abort returned it no longer writes the
        // transfer buffer.
        common_hal_usb_host_port_release_iso_buffer();
        #endif
    }
    #if !CIRCUITPY_USB_HOST_ISO_RECORDS
    if (self->aborted) {
        // The aborted transfer may still receive its packet.
        common_hal_mcu_delay_us(USBIN_ABORT_SETTLE_US);
    }
    #endif
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
        #if !CIRCUITPY_USB_HOST_ISO_RECORDS
        self->aborted = true;
        #endif
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
                a = is_signed ? ((const int16_t *)(const void *)in)[a_at] : ((const uint16_t *)(const void *)in)[a_at];
                b = is_signed ? ((const int16_t *)(const void *)in)[b_at] : ((const uint16_t *)(const void *)in)[b_at];
                ((uint16_t *)(void *)out)[out_at] = a + (((b - a) * frac) >> 15);
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
