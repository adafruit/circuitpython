// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-module/displayio/Bitmap.h"

#include "supervisor/filesystem.h"

#define LVFONTIO_INVALID_CODEPOINT 0xFFFFFFFF

// LV Font header information
typedef struct {
    // Font size and metrics
    uint16_t font_size;
    uint16_t ascent;
    uint16_t default_advance_width;

    // Encoding formats
    uint8_t index_to_loc_format;
    uint8_t bits_per_pixel;
    uint8_t glyph_bbox_xy_bits;
    uint8_t glyph_bbox_wh_bits;
    uint8_t glyph_advance_bits;

    // Calculated values
    uint8_t glyph_header_bits;
    uint8_t glyph_header_bytes;
} lvfontio_header_t;

// Mapping of codepoint ranges to glyph IDs
typedef struct {
    uint32_t range_start;   // Start of codepoint range
    uint32_t range_end;     // End of codepoint range (exclusive)
    uint16_t glyph_offset;  // Offset to apply to codepoint
    uint8_t format_type;    // Format type: 0=sparse mapping, 2=range to range, 3=direct mapping
    uint16_t entries_count; // Number of entries in sparse data
    uint32_t data_offset;   // File offset to the cmap data
} lvfontio_cmap_range_t;

typedef struct {
    mp_obj_base_t base;
    // Bitmap containing cached glyphs
    displayio_bitmap_t *bitmap;
    // Source of font file path (either a const char* or a copied string)
    const char *file_path;
    // Array mapping glyph indices to codepoints
    uint32_t *codepoints;
    // Array of reference counts for each glyph slot
    uint16_t *reference_counts; // Use uint16_t to handle higher reference counts
    // Maximum number of glyphs to cache at once
    uint16_t max_glyphs;
    // Flag indicating whether to use m_malloc (true) or port_malloc (false)
    bool use_gc_allocator;
    uint8_t half_width_px;

    supervisor_vfs_file_t file;
    // Which mount the file is open on, for locking around file reads.
    supervisor_vfs_t *vfs;
    bool file_is_open;

    // Font metrics information loaded from file
    lvfontio_header_t header;

    // CMAP information
    lvfontio_cmap_range_t *cmap_ranges;
    uint16_t cmap_range_count;

    // Offsets for tables in the file
    uint32_t loca_table_offset;
    uint32_t glyf_table_offset;
    uint32_t max_cid;
} lvfontio_ondiskfont_t;
