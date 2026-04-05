// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"
#include "supervisor/background_callback.h"
#include STM32_HAL_H

// Total DMA circular buffer size in 16-bit samples.
// Split into two halves; one half plays while the other is refilled.
// 512 samples at 44100 Hz = ~5.8 ms per half-buffer interrupt.
#define AUDIOOUT_DMA_BUFFER_SAMPLES 512
#define AUDIOOUT_DMA_HALF_SAMPLES   256

typedef struct {
    mp_obj_base_t base;

    // Left channel pin (PA04 = DAC_CH1). NULL when deinited.
    const mcu_pin_obj_t *left_channel;
    // right_channel (PA05 = DAC_CH2) deferred to a future implementation.

    // DMA handle for DMA1 Stream5 Channel7 (DAC CH1).
    // The DAC handle is the shared file-scope handle from AnalogOut.c.
    DMA_HandleTypeDef dma_handle;

    // Circular DMA buffer: AUDIOOUT_DMA_BUFFER_SAMPLES uint16_t elements,
    // allocated on play() and freed on stop().
    uint16_t *dma_buffer;

    // Current audio sample object being played.
    mp_obj_t sample;
    bool loop;

    // Set from ISR context to request a clean stop via background callback.
    volatile bool stopping;
    bool paused;

    // Sample format metadata, populated at play() time.
    uint8_t bytes_per_sample;   // 1 (8-bit) or 2 (16-bit)
    bool samples_signed;
    uint8_t channel_count;      // 1 = mono, 2 = stereo (only L channel output)
    uint16_t quiescent_value;   // 16-bit resting value (default 0x8000)

    // Background callback queued from DMA ISR, processed in main loop.
    background_callback_t callback;

    // Which half of dma_buffer to refill next: 0 = lower, 1 = upper.
    volatile uint8_t buffer_half_to_fill;
} audioio_audioout_obj_t;

// Called from reset_port() to stop any active playback on soft-reset.
void audioout_reset(void);
