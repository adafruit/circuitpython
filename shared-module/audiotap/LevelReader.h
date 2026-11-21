// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#pragma once

#include "py/obj.h"

#include "shared-module/audiotap/Tap.h"

extern const mp_obj_type_t audiotap_levelreader_type;

typedef struct audiotap_levelreader_obj {
    mp_obj_base_t base;

    // The tap whose audio is measured. NULL once deinitialized.
    audiotap_tap_obj_t *tap;

    // Levels of the audio since each was last read. Reading one asks for its
    // reset, and the tap's writer does it, so that only the writer changes them.
    volatile uint32_t peak; // largest magnitude, 0 to 32768
    volatile bool peak_reset;
    volatile float sum_squares; // of samples scaled to -1.0 to 1.0
    volatile uint32_t level_count; // samples summed
    volatile bool rms_reset;
} audiotap_levelreader_obj_t;

// Called by the tap for each run of samples that passes through it, by
// whatever plays the tap. It may be an interrupt, so this never allocates.
void audiotap_levelreader_measure(audiotap_levelreader_obj_t *self,
    const audiosample_base_t *format, const uint8_t *data, uint32_t count);
