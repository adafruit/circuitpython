// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/picodvi/Framebuffer.h"

// More than one video frame of audio (800 frames at 48 kHz, 60 Hz), so a
// bank can always be filled completely.
#define PICODVI_AUDIOOUT_STAGE_FRAMES 1024

typedef struct {
    mp_obj_base_t base;
    picodvi_framebuffer_obj_t *framebuffer;
    mp_obj_t sample;
    // The sample buffer being converted, and how many frames remain in it.
    const uint8_t *source;
    size_t source_frames;
    bool source_last;
    bool source_done;
    // True until the current pass through the sample gives a frame.
    bool pass_empty;
    uint8_t bytes_per_frame;
    bool loop;
    bool paused;
    // Set while a refill runs. A refill started inside it, such as from
    // background tasks during an SD card read, returns at once.
    bool refilling;
    // Converted 16-bit stereo frames waiting to be sent.
    size_t stage_frames;
    int16_t stage[PICODVI_AUDIOOUT_STAGE_FRAMES * 2];
} picodvi_audioout_obj_t;

// Called by the frame interrupt at a frame boundary. Returns the command
// list for the next frame.
uint32_t *picodvi_audioout_next_frame(void);
// Called by the frame interrupt once the next frame has started.
void picodvi_audioout_frame_done(void);
// Called by framebuffer deinit, after DMA stops, to free the audio state
// and deinit its AudioOut.
void picodvi_audioout_framebuffer_deinit(picodvi_framebuffer_obj_t *framebuffer);

// Deinit any AudioOut before the VM heap goes away.
void picodvi_audioout_reset(void);
