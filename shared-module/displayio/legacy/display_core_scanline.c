// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Scanline (legacy) compositor helpers that display the group tree. Defined here
// instead of in ../display_core.c so that display-side modules such as busdisplay,
// epaperdisplay and framebufferio implement them on top of the legacy compositor
// while LVGL-backed builds do not carry the scanline rendering machinery at all.

#include "shared-module/displayio/display_core.h"

bool displayio_display_core_fill_area(displayio_display_core_t *self, displayio_area_t *area, uint32_t *mask, uint32_t *buffer) {
    if (self->current_group != NULL) {
        return displayio_group_fill_area(self->current_group, &self->colorspace, area, mask, buffer);
    }
    return false;
}

bool displayio_display_core_clip_area(displayio_display_core_t *self, const displayio_area_t *area, displayio_area_t *clipped) {
    bool overlaps = displayio_area_compute_overlap(&self->area, area, clipped);
    if (!overlaps) {
        return false;
    }
    // Expand the area if we have multiple pixels per byte and we need to byte
    // align the bounds.
    if (self->colorspace.depth < 8) {
        uint8_t pixels_per_byte = 8 / self->colorspace.depth * self->colorspace.bytes_per_cell;
        if (self->colorspace.pixels_in_byte_share_row) {
            if (clipped->x1 % pixels_per_byte != 0) {
                clipped->x1 -= clipped->x1 % pixels_per_byte;
            }
            if (clipped->x2 % pixels_per_byte != 0) {
                clipped->x2 += pixels_per_byte - clipped->x2 % pixels_per_byte;
            }
        } else {
            if (clipped->y1 % pixels_per_byte != 0) {
                clipped->y1 -= clipped->y1 % pixels_per_byte;
            }
            if (clipped->y2 % pixels_per_byte != 0) {
                clipped->y2 += pixels_per_byte - clipped->y2 % pixels_per_byte;
            }
        }
    }
    return true;
}
