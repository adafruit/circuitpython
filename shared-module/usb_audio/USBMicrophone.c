// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/misc.h"
#include "py/runtime.h"

#include "shared-bindings/usb_audio/USBMicrophone.h"
#include "shared-module/usb_audio/__init__.h"
#include "shared-module/audiocore/__init__.h"

// Only one microphone may feed the single USB IN endpoint at a time. This points
// at the USBMicrophone whose play() was called most recently, or NULL when none
// is streaming. The USB background task pulls from it via
// usb_audio_usbmicrophone_background_fill().
static usb_audio_usbmicrophone_obj_t *active_microphone = NULL;

void common_hal_usb_audio_usbmicrophone_construct(usb_audio_usbmicrophone_obj_t *self) {
    self->sample = MP_OBJ_NULL;
    self->buffer = NULL;
    self->buffer_length = 0;
    self->loop = false;
    self->playing = false;
    self->paused = false;
    self->deinited = false;
    self->more_data = false;
}

void common_hal_usb_audio_usbmicrophone_deinit(usb_audio_usbmicrophone_obj_t *self) {
    common_hal_usb_audio_usbmicrophone_stop(self);
    self->deinited = true;
}

bool common_hal_usb_audio_usbmicrophone_deinited(usb_audio_usbmicrophone_obj_t *self) {
    return self->deinited;
}

void common_hal_usb_audio_usbmicrophone_play(usb_audio_usbmicrophone_obj_t *self, mp_obj_t sample, bool loop) {
    // The negotiated USB format is fixed 16-bit signed LE PCM at the rate/channel
    // count chosen by usb_audio.enable() in boot.py. Resampling and format
    // conversion are out of scope, so the source must already match exactly. This
    // mirrors how audiocore's audiosample_must_match() validates an output's input
    // (and reuses its error messages), but checks against the USB format rather
    // than another audiosample.
    audiosample_base_t *sample_base = audiosample_check(sample);
    if (audiosample_get_sample_rate(sample_base) != usb_audio_sample_rate) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_sample_rate);
    }
    if (audiosample_get_channel_count(sample_base) != usb_audio_channel_count) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_channel_count);
    }
    if (audiosample_get_bits_per_sample(sample_base) != usb_audio_bits_per_sample) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_bits_per_sample);
    }
    if (!sample_base->samples_signed) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_signedness);
    }

    // Start at the beginning and arm the pull loop to request the first chunk.
    audiosample_reset_buffer(sample, false, 0);
    self->sample = sample;
    self->buffer = NULL;
    self->buffer_length = 0;
    self->loop = loop;
    self->playing = true;
    self->paused = false;
    self->more_data = true;
    // Take over the single USB IN endpoint from any other microphone.
    active_microphone = self;
}

void common_hal_usb_audio_usbmicrophone_stop(usb_audio_usbmicrophone_obj_t *self) {
    self->sample = MP_OBJ_NULL;
    self->buffer = NULL;
    self->buffer_length = 0;
    self->playing = false;
    self->paused = false;
    self->more_data = false;
    if (active_microphone == self) {
        active_microphone = NULL;
    }
}

bool common_hal_usb_audio_usbmicrophone_get_playing(usb_audio_usbmicrophone_obj_t *self) {
    return self->playing && !self->paused;
}

void common_hal_usb_audio_usbmicrophone_pause(usb_audio_usbmicrophone_obj_t *self) {
    self->paused = true;
}

void common_hal_usb_audio_usbmicrophone_resume(usb_audio_usbmicrophone_obj_t *self) {
    self->paused = false;
}

bool common_hal_usb_audio_usbmicrophone_get_paused(usb_audio_usbmicrophone_obj_t *self) {
    return self->playing && self->paused;
}

size_t usb_audio_usbmicrophone_background_fill(uint8_t *out, size_t max_bytes) {
    usb_audio_usbmicrophone_obj_t *self = active_microphone;
    if (self == NULL || !self->playing || self->paused || self->sample == MP_OBJ_NULL) {
        return 0;
    }

    // The negotiated USB format is 16-bit signed PCM. For this step the
    // bound sample is assumed to already be in that format (e.g. a 16-bit signed
    // audiocore.RawSample), so its bytes are copied straight through.
    size_t filled = 0;
    while (filled < max_bytes) {
        if (self->buffer_length == 0) {
            if (!self->more_data) {
                // The previous chunk was the sample's last and we've played it
                // out. Loop back to the start or stop.
                if (self->loop) {
                    audiosample_reset_buffer(self->sample, false, 0);
                    self->more_data = true;
                } else {
                    common_hal_usb_audio_usbmicrophone_stop(self);
                    break;
                }
            }
            audioio_get_buffer_result_t result =
                audiosample_get_buffer(self->sample, false, 0, &self->buffer, &self->buffer_length);
            if (result == GET_BUFFER_ERROR) {
                common_hal_usb_audio_usbmicrophone_stop(self);
                break;
            }
            self->more_data = (result == GET_BUFFER_MORE_DATA);
            if (self->buffer_length == 0) {
                // No samples available right now (underrun); the caller fills the
                // rest of the frame with silence and we retry next tick.
                break;
            }
        }

        size_t n = MIN(self->buffer_length, max_bytes - filled);
        memcpy(out + filled, self->buffer, n);
        self->buffer += n;
        self->buffer_length -= n;
        filled += n;
    }

    return filled;
}
