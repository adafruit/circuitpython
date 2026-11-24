// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "lib/tjpgd/src/tjpgd.h"
#include "shared-module/displayio/Bitmap.h"

#define TJPGD_WORKSPACE_SIZE 3500

typedef struct jpegio_jpegdecoder_obj {
    mp_obj_base_t base;
    JDEC decoder;
    byte workspace[TJPGD_WORKSPACE_SIZE];
    mp_obj_t data_obj;
    mp_buffer_info_t bufinfo;
    displayio_bitmap_t *dest;
    uint16_t x, y;
    bitmaptools_rect_t lim;
    uint32_t skip_source_index, skip_dest_index;
    bool skip_source_index_none, skip_dest_index_none;
    uint8_t scale;
    bool software_ok;
    #if CIRCUITPY_JPEGIO_HW
    void *hw_decoder; // port state for the hardware decoder, NULL if unavailable
    #endif
    #if CIRCUITPY_JPEGIO_FAST
    mp_off_t stream_start_position; // where the JPEG starts in a seekable stream, or -1
    #endif
} jpegio_jpegdecoder_obj_t;

// Copies `src`, whose top left pixel is pixel (left, top) of the decoded image,
// into `self->dest`, clipped to the crop area and placed at `self->x`, `self->y`.
// Returns false if `src` starts below the crop area, so no later rows are needed.
// Raises only if `self->dest` is read-only.
bool jpegio_jpegdecoder_blit_rect(jpegio_jpegdecoder_obj_t *self, displayio_bitmap_t *src, int left, int top);

#if CIRCUITPY_JPEGIO_HW
// Ports with JPEG decode hardware provide this. Called when the decoder object is
// constructed. Sets `self->hw_decoder`, or NULL if the hardware is unavailable, in which
// case decode() uses TJpgDec. Must not raise.
void jpegio_jpegdecoder_hw_init(jpegio_jpegdecoder_obj_t *self);

// Ports with JPEG decode hardware provide this. It is called by decode() only when the
// source passed to open() is an in-memory buffer; `jpeg` is that whole buffer. The decode
// arguments (x, y, lim, skip indices) are already stored in `self`, and `self->dest` is
// the target bitmap.
// Returns true if the port decoded the image into `self->dest`. Returns false to decode
// with TJpgDec instead, which rewrites every pixel the image covers. If
// `self->software_ok` is false, raises ValueError instead of returning false.
bool jpegio_jpegdecoder_hw_decode(jpegio_jpegdecoder_obj_t *self, const mp_buffer_info_t *jpeg, int scale);

// Ports with JPEG decode hardware provide this. It releases `self->hw_decoder`, if set,
// and sets it to NULL. Called when the decoder object is collected. Must not raise.
void jpegio_jpegdecoder_hw_deinit(jpegio_jpegdecoder_obj_t *self);
#endif

#if CIRCUITPY_JPEGIO_FAST
// Ports with a faster software decoder provide these two. decode() calls them when
// software decoding is allowed, before TJpgDec. The decode arguments and `self->dest`
// are set as for jpegio_jpegdecoder_hw_decode, and `self->decoder` holds the header
// TJpgDec parsed in open().

// Returns true if the port's decoder may handle this image with these arguments.
// Called before a file or stream is read into memory, so it must be cheap. Must not raise.
bool jpegio_jpegdecoder_fast_supported(jpegio_jpegdecoder_obj_t *self, int scale);

// Called only after jpegio_jpegdecoder_fast_supported returned true, with the whole JPEG
// in `jpeg`: the buffer passed to open(), or a file or stream read into memory.
// Returns true if the port decoded the image into `self->dest`. Returns false to decode
// with TJpgDec instead, which rewrites every pixel the image covers. Must not raise.
bool jpegio_jpegdecoder_fast_decode(jpegio_jpegdecoder_obj_t *self, const mp_buffer_info_t *jpeg, int scale);
#endif
