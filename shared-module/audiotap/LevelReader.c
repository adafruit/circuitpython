// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiotap/LevelReader.h"
#include "shared-bindings/audiocore/__init__.h"

#include <math.h>
#include <stdint.h>

#include "py/runtime.h"

void common_hal_audiotap_levelreader_construct(audiotap_levelreader_obj_t *self, audiotap_tap_obj_t *tap) {
    audiosample_check_for_deinit(&tap->base);
    if (tap->levels != NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_tap);
    }
    self->tap = tap;
    self->peak = 0;
    self->peak_reset = false;
    self->sum_squares = 0;
    self->level_count = 0;
    self->rms_reset = false;
    // The levels must start from 0 before the tap's writer can see them.
    __sync_synchronize();
    tap->levels = self;
}

bool common_hal_audiotap_levelreader_deinited(audiotap_levelreader_obj_t *self) {
    return self->tap == NULL;
}

void common_hal_audiotap_levelreader_deinit(audiotap_levelreader_obj_t *self) {
    audiotap_tap_obj_t *tap = self->tap;
    if (tap == NULL) {
        return;
    }
    // The tap stops measuring. If its writer is measuring right now, it
    // finishes with this object, which is still allocated, and no harm done.
    if (tap->levels == self) {
        tap->levels = NULL;
    }
    self->tap = NULL;
}

// The sample at i, as a signed 16 bit value.
static int32_t sample_at(const audiosample_base_t *format, const uint8_t *data, uint32_t i) {
    if (MP_LIKELY(format->bits_per_sample == 16)) {
        // data is the source's 16 bit buffer, advanced only in whole samples,
        // so it is aligned.
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wcast-align"
        uint16_t raw = ((const uint16_t *)data)[i];
        #pragma GCC diagnostic pop
        return format->samples_signed ? (int16_t)raw : (int16_t)(raw ^ 0x8000);
    }
    uint8_t raw = data[i];
    return (format->samples_signed ? (int8_t)raw : (int8_t)(raw ^ 0x80)) * 256;
}

void audiotap_levelreader_measure(audiotap_levelreader_obj_t *self,
    const audiosample_base_t *format, const uint8_t *data, uint32_t count) {
    if (self->peak_reset) {
        self->peak = 0;
        self->peak_reset = false;
    }
    if (self->rms_reset) {
        self->sum_squares = 0;
        self->level_count = 0;
        self->rms_reset = false;
    }
    uint32_t peak = self->peak;
    uint64_t sum = 0;
    for (uint32_t i = 0; i < count; i++) {
        int32_t value = sample_at(format, data, i);
        uint32_t magnitude = value < 0 ? -value : value;
        if (magnitude > peak) {
            peak = magnitude;
        }
        sum += (uint64_t)(value * value);
    }
    self->peak = peak;
    self->sum_squares += (float)sum / (32768.0f * 32768.0f);
    self->level_count += count;
}

static void check_tap(audiotap_levelreader_obj_t *self) {
    audiosample_check_for_deinit(&self->tap->base);
}

mp_float_t common_hal_audiotap_levelreader_get_peak(audiotap_levelreader_obj_t *self) {
    check_tap(self);
    if (self->peak_reset) {
        // No audio passed since the last read.
        return 0;
    }
    mp_float_t peak = (mp_float_t)self->peak / MICROPY_FLOAT_CONST(32768.0);
    self->peak_reset = true;
    return peak;
}

mp_float_t common_hal_audiotap_levelreader_get_rms(audiotap_levelreader_obj_t *self) {
    check_tap(self);
    if (self->rms_reset) {
        // No audio passed since the last read.
        return 0;
    }
    uint32_t count = self->level_count;
    float sum_squares = self->sum_squares;
    self->rms_reset = true;
    return count > 0 ? MICROPY_FLOAT_C_FUN(sqrt)((mp_float_t)sum_squares / count) : 0;
}
