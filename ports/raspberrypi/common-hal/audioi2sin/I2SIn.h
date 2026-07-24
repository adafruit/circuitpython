// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "bindings/rp2pio/StateMachine.h"

#include "py/obj.h"

#include "shared-module/audiocore/__init__.h"

// Number of audio frames a single get_buffer() call hands to the output backend
// (one frame = one mono sample, or an L+R pair in stereo). This sets the pull
// granularity and the silence-pad chunk on underrun. Kept independent of the DMA
// half-buffer so the two can be tuned separately.
#define AUDIOI2SIN_STREAM_FRAMES (256)

typedef struct {
    // so I2SIn can be used directly as an audiosample source.
    audiosample_base_t base;
    uint32_t sample_rate;
    uint8_t bit_depth;
    bool mono;
    bool samples_signed;
    bool left_justified;
    bool clock_follower;
    bool settled;
    rp2pio_statemachine_obj_t state_machine;
    // Background DMA ring buffer. The state machine alternates DMA writes
    // between the two halves so BCLK never stalls between record() calls.
    uint8_t *ring;
    size_t ring_size;
    size_t half_size;
    size_t read_pos;
    int dma_channel;
    bool overflow;
    // Owned double-buffer returned to the output backend by get_buffer(), holding
    // converted (output-depth, interleaved) samples. Allocated lazily on the first
    // reset_buffer() so record()-only use pays no extra RAM. base.max_buffer_length
    // is the whole buffer; get_buffer() returns output_half_bytes of it.
    uint8_t *output_buffer;
    size_t output_half_bytes;
    uint8_t output_index;
} audioi2sin_i2sin_obj_t;
