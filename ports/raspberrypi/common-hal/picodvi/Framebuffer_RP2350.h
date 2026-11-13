#pragma once

/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/obj.h"

// ----------------------------------------------------------------------------
// DVI constants

#define TMDS_CTRL_00 0x354u
#define TMDS_CTRL_01 0x0abu
#define TMDS_CTRL_10 0x154u
#define TMDS_CTRL_11 0x2abu

#define SYNC_V0_H0 (TMDS_CTRL_00 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))
#define SYNC_V0_H1 (TMDS_CTRL_01 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))
#define SYNC_V1_H0 (TMDS_CTRL_10 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))
#define SYNC_V1_H1 (TMDS_CTRL_11 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))

#define MODE_720_H_SYNC_POLARITY 0
#define MODE_720_H_FRONT_PORCH   8
#define MODE_720_H_SYNC_WIDTH    32
#define MODE_720_H_BACK_PORCH    40
#define MODE_720_H_ACTIVE_PIXELS 720

#define MODE_720_V_SYNC_POLARITY 0
#define MODE_720_V_FRONT_PORCH   3
#define MODE_720_V_SYNC_WIDTH    4
#define MODE_720_V_BACK_PORCH    218
#define MODE_720_V_ACTIVE_LINES  400

#define MODE_640_H_SYNC_POLARITY 0
#define MODE_640_H_FRONT_PORCH   16
#define MODE_640_H_SYNC_WIDTH    96
#define MODE_640_H_BACK_PORCH    48
#define MODE_640_H_ACTIVE_PIXELS 640

#define MODE_640_V_SYNC_POLARITY 0
#define MODE_640_V_FRONT_PORCH   10
#define MODE_640_V_SYNC_WIDTH    2
#define MODE_640_V_BACK_PORCH    133
#define MODE_640_V_ACTIVE_LINES  480

#define MODE_720_V_TOTAL_LINES  ( \
    MODE_720_V_FRONT_PORCH + MODE_720_V_SYNC_WIDTH + \
    MODE_720_V_BACK_PORCH + MODE_720_V_ACTIVE_LINES \
    )
#define MODE_640_V_TOTAL_LINES  ( \
    MODE_640_V_FRONT_PORCH + MODE_640_V_SYNC_WIDTH + \
    MODE_640_V_BACK_PORCH + MODE_640_V_ACTIVE_LINES \
    )

#define HSTX_CMD_RAW         (0x0u << 12)
#define HSTX_CMD_RAW_REPEAT  (0x1u << 12)
#define HSTX_CMD_TMDS        (0x2u << 12)
#define HSTX_CMD_TMDS_REPEAT (0x3u << 12)
#define HSTX_CMD_NOP         (0xfu << 12)

struct dvi_audio_state;

typedef struct {
    mp_obj_base_t base;
    struct dvi_audio_state *dvi_audio;
    // The picodvi.AudioOut using this framebuffer, or MP_OBJ_NULL. It lives
    // as long as the framebuffer.
    mp_obj_t audioout;
    uint32_t *framebuffer;
    size_t framebuffer_len; // in words
    uint32_t *dma_commands;
    size_t dma_commands_len; // in words
    mp_uint_t width;
    mp_uint_t height;
    mp_uint_t output_width;
    uint16_t pitch; // Number of words between rows. (May be more than a width's worth.)
    uint8_t color_depth;
    int dma_pixel_channel;
    int dma_command_channel;
} picodvi_framebuffer_obj_t;

// The framebuffer the frame interrupt is driving, or NULL.
extern picodvi_framebuffer_obj_t *active_picodvi;
