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
// 4096-sample halves at 22050 Hz = ~186 ms per half-buffer interrupt,
// giving the background callback enough headroom to absorb SDIO cluster
// reads, NeoPixel updates, and other main-loop stalls without underrun.
#define AUDIOOUT_DMA_BUFFER_SAMPLES 8192
#define AUDIOOUT_DMA_HALF_SAMPLES   4096

typedef struct {
    mp_obj_base_t base;

    // Left channel pin (PA04 = DAC_CH1). NULL when deinited.
    const mcu_pin_obj_t *left_channel;
    // Right channel pin (PA05 = DAC_CH2). NULL when mono.
    const mcu_pin_obj_t *right_channel;

    // DMA handle for DMA1 Stream5 Channel7 (DAC CH1, left).
    // DMA handle for DMA1 Stream6 Channel7 (DAC CH2, right).
    // The DAC handle is the shared file-scope handle from AnalogOut.c.
    DMA_HandleTypeDef dma_handle;
    DMA_HandleTypeDef dma_handle_r;

    // Circular DMA buffers: AUDIOOUT_DMA_BUFFER_SAMPLES uint16_t elements each,
    // allocated on play() and freed on stop().
    uint16_t *dma_buffer;    // left (CH1)
    uint16_t *dma_buffer_r;  // right (CH2), NULL when mono

    // Current audio sample object being played.
    mp_obj_t sample;
    bool loop;

    // Set from ISR context to request a clean stop via background callback.
    volatile bool stopping;
    bool paused;

    // Sample format metadata, populated at play() time.
    uint8_t bytes_per_sample;   // 1 (8-bit) or 2 (16-bit)
    bool samples_signed;
    uint8_t channel_count;      // 1 = mono, 2 = stereo
    uint16_t quiescent_value;   // 16-bit resting value (default 0x8000)

    // Background callback queued from DMA ISR, processed in main loop.
    background_callback_t callback;

    // Bitmask of DMA halves pending refill: bit0 = lower, bit1 = upper.
    // Set from half/full IRQ, drained by the background callback. A bitmask
    // (not a scalar) so a back-to-back half+full pair queues both fills even
    // if the callback hasn't run yet.
    volatile uint8_t halves_to_fill;

    // Source buffer position tracking. Allows consuming large source buffers
    // (e.g. RawSample > 256 samples) across multiple DMA half-fills.
    const uint8_t *src_ptr;         // current read position in source buffer
    uint32_t src_remaining_len;     // bytes remaining in current source buffer
    bool src_done;                  // GET_BUFFER_DONE received for current buffer
} audioio_audioout_obj_t;

// Called from reset_port() to stop any active playback on soft-reset.
void audioout_reset(void);
