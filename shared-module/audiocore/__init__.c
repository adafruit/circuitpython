// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/audioio/__init__.h"

#include "py/obj.h"
#include "py/runtime.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/audiocore/WaveFile.h"
#include "shared-module/audiocore/RawSample.h"
#include "shared-module/audiocore/WaveFile.h"

#if CIRCUITPY_AUDIOSPEED
#include "shared-bindings/audiospeed/Resampler.h"
#endif

#include "shared-bindings/audiomixer/Mixer.h"
#include "shared-module/audiomixer/Mixer.h"

// Need to confirm that a sample is not deinited before using it, otherwise we
// we read and write through NULL points and fault
static bool audiosample_dispatch_ok(mp_obj_t sample_obj) {
    return !audiosample_deinited((const audiosample_base_t *)MP_OBJ_TO_PTR(sample_obj));
}

void audiosample_reset_buffer(mp_obj_t sample_obj, bool single_channel_output, uint8_t audio_channel) {
    const audiosample_p_t *proto = mp_proto_get_or_throw(MP_QSTR_protocol_audiosample, sample_obj);
    if (!audiosample_dispatch_ok(sample_obj)) {
        return;
    }
    proto->reset_buffer(MP_OBJ_TO_PTR(sample_obj), single_channel_output, audio_channel);
}

audioio_get_buffer_result_t audiosample_get_buffer(mp_obj_t sample_obj,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {
    const audiosample_p_t *proto = mp_proto_get_or_throw(MP_QSTR_protocol_audiosample, sample_obj);
    if (!audiosample_dispatch_ok(sample_obj)) {
        *buffer = NULL;
        *buffer_length = 0;
        return GET_BUFFER_ERROR;
    }
    return proto->get_buffer(MP_OBJ_TO_PTR(sample_obj), single_channel_output, channel, buffer, buffer_length);
}

void audiosample_convert_u8m_s16s(int16_t *buffer_out, const uint8_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        int16_t sample = (*buffer_in++ - 0x80) << 8;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_u8s_s16s(int16_t *buffer_out, const uint8_t *buffer_in, size_t nframes) {
    size_t nsamples = 2 * nframes;
    for (; nsamples--;) {
        int16_t sample = (*buffer_in++ - 0x80) << 8;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_s8m_s16s(int16_t *buffer_out, const int8_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        int16_t sample = (*buffer_in++) << 8;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_s8s_s16s(int16_t *buffer_out, const int8_t *buffer_in, size_t nframes) {
    size_t nsamples = 2 * nframes;
    for (; nsamples--;) {
        int16_t sample = (*buffer_in++) << 8;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_u16m_s16s(int16_t *buffer_out, const uint16_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        int16_t sample = *buffer_in++ - 0x8000;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_u16s_s16s(int16_t *buffer_out, const uint16_t *buffer_in, size_t nframes) {
    size_t nsamples = 2 * nframes;
    for (; nsamples--;) {
        int16_t sample = *buffer_in++ - 0x8000;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_s16m_s16s(int16_t *buffer_out, const int16_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        int16_t sample = *buffer_in++;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}


void audiosample_convert_u8s_u8m(uint8_t *buffer_out, const uint8_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        uint8_t sample = *buffer_in++ + 0x80;
        *buffer_out++ = sample;
        buffer_in++;
    }
}

void audiosample_convert_s8m_u8m(uint8_t *buffer_out, const int8_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        uint8_t sample = *buffer_in++ + 0x80;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_s8s_u8m(uint8_t *buffer_out, const int8_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        uint8_t sample = *buffer_in++ + 0x80;
        *buffer_out++ = sample;
        buffer_in++;
    }
}

void audiosample_convert_u16m_u8m(uint8_t *buffer_out, const uint16_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        uint8_t sample = (*buffer_in++) >> 8;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_u16s_u8m(uint8_t *buffer_out, const uint16_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        uint8_t sample = (*buffer_in++) >> 8;
        *buffer_out++ = sample;
        buffer_in++;
    }
}

void audiosample_convert_s16m_u8m(uint8_t *buffer_out, const int16_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        uint8_t sample = (*buffer_in++ + 0x8000) >> 8;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_s16s_u8m(uint8_t *buffer_out, const int16_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        uint8_t sample = (*buffer_in++ + 0x8000) >> 8;
        *buffer_out++ = sample;
        buffer_in++;
    }
}


void audiosample_convert_u8m_u8s(uint8_t *buffer_out, const uint8_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        uint8_t sample = *buffer_in++;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_s8m_u8s(uint8_t *buffer_out, const int8_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        uint8_t sample = *buffer_in++ + 0x80;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_s8s_u8s(uint8_t *buffer_out, const int8_t *buffer_in, size_t nframes) {
    size_t nsamples = 2 * nframes;
    for (; nsamples--;) {
        uint8_t sample = *buffer_in++ + 0x80;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_u16m_u8s(uint8_t *buffer_out, const uint16_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        uint8_t sample = (*buffer_in++) >> 8;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_u16s_u8s(uint8_t *buffer_out, const uint16_t *buffer_in, size_t nframes) {
    size_t nsamples = 2 * nframes;
    for (; nsamples--;) {
        uint8_t sample = (*buffer_in++) >> 8;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_s16m_u8s(uint8_t *buffer_out, const int16_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        uint8_t sample = (*buffer_in++ + 0x8000) >> 8;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_s16s_u8s(uint8_t *buffer_out, const int16_t *buffer_in, size_t nframes) {
    size_t nsamples = 2 * nframes;
    for (; nsamples--;) {
        uint8_t sample = (*buffer_in++ + 0x8000) >> 8;
        *buffer_out++ = sample;
    }
}

void audiosample_must_match(audiosample_base_t *self, mp_obj_t other_in, bool allow_mono_to_stereo) {
    const audiosample_base_t *other = audiosample_check(other_in);
    // Attaching a sample that has already been deinited would leave the caller
    // playing something that can never produce audio.
    audiosample_check_for_deinit(other);

    #if !CIRCUITPY_AUDIOSPEED
    if (other->sample_rate != self->sample_rate) {
    #else
    if (other->sample_rate != self->sample_rate && !mp_obj_is_type(other_in, &audiospeed_resampler_type)) {
        #endif
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_sample_rate);
    }
    if ((!allow_mono_to_stereo || (allow_mono_to_stereo && self->channel_count != 2)) && other->channel_count != self->channel_count) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_channel_count);
    }
    if (other->bits_per_sample != self->bits_per_sample) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_bits_per_sample);
    }
    if (other->samples_signed != self->samples_signed) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The sample's %q does not match"), MP_QSTR_signedness);
    }

    #if CIRCUITPY_AUDIOSPEED
    if (mp_obj_is_type(other_in, &audiospeed_resampler_type)) {
        audiospeed_resampler_obj_t *other_resampler = MP_OBJ_TO_PTR(other_in);
        audiospeed_resampler_set_sample_rate(other_resampler, self->sample_rate);
    }
    #endif
}
