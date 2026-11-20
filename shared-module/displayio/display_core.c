// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/displayio/display_core.h"

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/tick.h"

#include <stdint.h>
#include <string.h>

#define DISPLAYIO_CORE_DEBUG(...) (void)0
// #define DISPLAYIO_CORE_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)

void displayio_display_core_construct(displayio_display_core_t *self,
    uint16_t width, uint16_t height, uint16_t rotation,
    uint16_t color_depth, bool grayscale, bool pixels_in_byte_share_row, uint8_t bytes_per_cell, bool reverse_pixels_in_byte, bool reverse_bytes_in_word) {
    self->colorspace.depth = color_depth;
    self->colorspace.grayscale = grayscale;
    self->colorspace.grayscale_bit = 8 - color_depth;
    self->colorspace.pixels_in_byte_share_row = pixels_in_byte_share_row;
    self->colorspace.bytes_per_cell = bytes_per_cell;
    self->colorspace.reverse_pixels_in_byte = reverse_pixels_in_byte;
    self->colorspace.reverse_bytes_in_word = reverse_bytes_in_word;
    self->colorspace.dither = false;
    self->current_group = NULL;
    self->last_refresh = 0;

    supervisor_start_terminal(width, height);

    self->width = width;
    self->height = height;

    displayio_display_core_set_rotation(self, rotation);

    // If nothing has claimed the primary display yet, this display becomes it. A display created
    // by user code (for example a USB video framebuffer) must be kept across the end of the VM so
    // that the supervisor terminal and REPL have somewhere to draw.
    common_hal_displayio_auto_primary_display();
}

void displayio_display_core_set_rotation(displayio_display_core_t *self,
    int rotation) {
    rotation = rotation % 360;
    self->rotation = rotation;
    #if !CIRCUITPY_DISPLAYIO_LVGL
    int height = self->height;
    int width = self->width;
    self->transform.x = 0;
    self->transform.y = 0;
    self->transform.scale = 1;
    self->transform.mirror_x = false;
    self->transform.mirror_y = false;
    self->transform.transpose_xy = false;
    if (rotation == 0 || rotation == 180) {
        if (rotation == 180) {
            self->transform.mirror_x = true;
            self->transform.mirror_y = true;
        }
    } else {
        self->transform.transpose_xy = true;
        if (rotation == 270) {
            self->transform.mirror_y = true;
        } else {
            self->transform.mirror_x = true;
        }
    }

    self->area.x1 = 0;
    self->area.y1 = 0;
    self->area.next = NULL;

    self->transform.dx = 1;
    self->transform.dy = 1;
    if (self->transform.transpose_xy) {
        self->area.x2 = height;
        self->area.y2 = width;
        if (self->transform.mirror_x) {
            self->transform.x = height;
            self->transform.dx = -1;
        }
        if (self->transform.mirror_y) {
            self->transform.y = width;
            self->transform.dy = -1;
        }
    } else {
        self->area.x2 = width;
        self->area.y2 = height;
        if (self->transform.mirror_x) {
            self->transform.x = width;
            self->transform.dx = -1;
        }
        if (self->transform.mirror_y) {
            self->transform.y = height;
            self->transform.dy = -1;
        }
    }
    #endif
}

bool displayio_display_core_set_root_group(displayio_display_core_t *self, displayio_group_t *root_group) {

    if (root_group == NULL) {
        // Show nothing on the display
    }
    if (root_group == self->current_group) {
        return true;
    }
    if (root_group != NULL && root_group->in_group) {
        return false;
    }
    if (self->current_group != NULL) {
        // Deactivate the outgoing tree recursively. This clears its transforms
        // and tells layers (such as a VideoBuffer) that they are no longer
        // shown.
        #if !CIRCUITPY_DISPLAYIO_LVGL
        displayio_group_update_transform(self->current_group, NULL);
        #endif
        self->current_group->in_group = false;
    }

    if (root_group != NULL) {
        // The LVGL compositor updates its own state when the group is attached; the
        // legacy compositor re-applies transforms at the start of every display refresh.
        #if !CIRCUITPY_DISPLAYIO_LVGL
        displayio_group_update_transform(root_group, &self->transform);
        #endif
        root_group->in_group = true;
    }
    self->current_group = root_group;
    #if !CIRCUITPY_DISPLAYIO_LVGL
    self->full_refresh = true;
    #endif
    return true;
}

uint16_t displayio_display_core_get_width(displayio_display_core_t *self) {
    return self->width;
}

uint16_t displayio_display_core_get_height(displayio_display_core_t *self) {
    return self->height;
}

void displayio_display_core_set_dither(displayio_display_core_t *self, bool dither) {
    self->colorspace.dither = dither;
}

bool displayio_display_core_get_dither(displayio_display_core_t *self) {
    return self->colorspace.dither;
}

#if !CIRCUITPY_DISPLAYIO_LVGL
bool displayio_display_core_start_refresh(displayio_display_core_t *self) {
    if (self->refresh_in_progress) {
        return false;
    }
    self->refresh_in_progress = true;
    self->last_refresh = supervisor_ticks_ms64();
    return true;
}

void displayio_display_core_finish_refresh(displayio_display_core_t *self) {
    self->full_refresh = false;
    self->refresh_in_progress = false;
    self->last_refresh = supervisor_ticks_ms64();
    if (self->current_group != NULL) {
        displayio_group_finish_refresh(self->current_group);
    }
}

#endif

void release_display_core(displayio_display_core_t *self) {
    if (self->current_group != NULL) {
        // Deactivate the tree so layers stop acting on it (e.g. a VideoBuffer
        // pauses its source).
        #if !CIRCUITPY_DISPLAYIO_LVGL
        displayio_group_update_transform(self->current_group, NULL);
        #endif
        self->current_group->in_group = false;
    }
}

void displayio_display_core_collect_ptrs(displayio_display_core_t *self) {
    gc_collect_ptr(self->current_group);
}
