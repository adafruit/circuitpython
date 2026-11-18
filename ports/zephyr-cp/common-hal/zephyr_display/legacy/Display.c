// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/zephyr_display/Display.h"

#include <string.h>

#include "bindings/zephyr_kernel/__init__.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/tick.h"

static const displayio_area_t *zephyr_display_get_refresh_areas(zephyr_display_display_obj_t *self) {
    if (self->core.full_refresh) {
        self->core.area.next = NULL;
        return &self->core.area;
    } else if (self->core.current_group != NULL) {
        return displayio_group_get_refresh_areas(self->core.current_group, NULL);
    }
    return NULL;
}

static enum display_pixel_format zephyr_display_select_pixel_format(const struct display_capabilities *caps) {
    uint32_t formats = caps->supported_pixel_formats;

    if (formats & PIXEL_FORMAT_RGB_565) {
        return PIXEL_FORMAT_RGB_565;
    }
    if (formats & PIXEL_FORMAT_RGB_888) {
        return PIXEL_FORMAT_RGB_888;
    }
    if (formats & PIXEL_FORMAT_ARGB_8888) {
        return PIXEL_FORMAT_ARGB_8888;
    }
    if (formats & PIXEL_FORMAT_RGB_565X) {
        return PIXEL_FORMAT_RGB_565X;
    }
    if (formats & PIXEL_FORMAT_L_8) {
        return PIXEL_FORMAT_L_8;
    }
    if (formats & PIXEL_FORMAT_AL_88) {
        return PIXEL_FORMAT_AL_88;
    }
    if (formats & PIXEL_FORMAT_MONO01) {
        return PIXEL_FORMAT_MONO01;
    }
    if (formats & PIXEL_FORMAT_MONO10) {
        return PIXEL_FORMAT_MONO10;
    }
    return caps->current_pixel_format;
}

static void zephyr_display_select_colorspace(zephyr_display_display_obj_t *self,
    uint16_t *color_depth,
    uint8_t *bytes_per_cell,
    bool *grayscale,
    bool *pixels_in_byte_share_row,
    bool *reverse_pixels_in_byte,
    bool *reverse_bytes_in_word) {
    *color_depth = 16;
    *bytes_per_cell = 2;
    *grayscale = false;
    *pixels_in_byte_share_row = false;
    *reverse_pixels_in_byte = false;
    *reverse_bytes_in_word = false;

    if (self->pixel_format == PIXEL_FORMAT_RGB_565X) {
        // RGB_565X is big-endian RGB_565, so byte-swap from native LE.
        *reverse_bytes_in_word = true;
    } else if (self->pixel_format == PIXEL_FORMAT_RGB_888) {
        *color_depth = 24;
        *bytes_per_cell = 3;
        *reverse_bytes_in_word = false;
    } else if (self->pixel_format == PIXEL_FORMAT_ARGB_8888) {
        *color_depth = 32;
        *bytes_per_cell = 4;
        *reverse_bytes_in_word = false;
    } else if (self->pixel_format == PIXEL_FORMAT_L_8 ||
               self->pixel_format == PIXEL_FORMAT_AL_88) {
        *color_depth = 8;
        *bytes_per_cell = 1;
        *grayscale = true;
        *reverse_bytes_in_word = false;
    } else if (self->pixel_format == PIXEL_FORMAT_MONO01 ||
               self->pixel_format == PIXEL_FORMAT_MONO10) {
        bool vtiled = self->capabilities.screen_info & SCREEN_INFO_MONO_VTILED;
        bool msb_first = self->capabilities.screen_info & SCREEN_INFO_MONO_MSB_FIRST;
        *color_depth = 1;
        *bytes_per_cell = 1;
        *grayscale = true;
        *pixels_in_byte_share_row = !vtiled;
        *reverse_pixels_in_byte = msb_first;
        *reverse_bytes_in_word = false;
    }
}

void common_hal_zephyr_display_display_construct_from_device(zephyr_display_display_obj_t *self,
    const struct device *device,
    uint16_t rotation,
    bool auto_refresh) {
    self->auto_refresh = false;

    if (device == NULL || !device_is_ready(device)) {
        raise_zephyr_error(-ENODEV);
    }

    self->device = device;
    display_get_capabilities(self->device, &self->capabilities);

    self->pixel_format = zephyr_display_select_pixel_format(&self->capabilities);
    if (self->pixel_format != self->capabilities.current_pixel_format) {
        (void)display_set_pixel_format(self->device, self->pixel_format);
        display_get_capabilities(self->device, &self->capabilities);
        self->pixel_format = self->capabilities.current_pixel_format;
    }

    uint16_t color_depth;
    uint8_t bytes_per_cell;
    bool grayscale;
    bool pixels_in_byte_share_row;
    bool reverse_pixels_in_byte;
    bool reverse_bytes_in_word;
    zephyr_display_select_colorspace(self, &color_depth, &bytes_per_cell,
        &grayscale, &pixels_in_byte_share_row, &reverse_pixels_in_byte,
        &reverse_bytes_in_word);

    displayio_display_core_construct(
        &self->core,
        self->capabilities.x_resolution,
        self->capabilities.y_resolution,
        0,
        color_depth,
        grayscale,
        pixels_in_byte_share_row,
        bytes_per_cell,
        reverse_pixels_in_byte,
        reverse_bytes_in_word);

    self->native_frames_per_second = 60;
    self->native_ms_per_frame = 1000 / self->native_frames_per_second;
    self->first_manual_refresh = !auto_refresh;

    if (rotation != 0) {
        common_hal_zephyr_display_display_set_rotation(self, rotation);
    }

    (void)display_blanking_off(self->device);

    displayio_display_core_set_root_group(&self->core, &circuitpython_splash);
    common_hal_zephyr_display_display_set_auto_refresh(self, auto_refresh);
}

uint16_t common_hal_zephyr_display_display_get_width(zephyr_display_display_obj_t *self) {
    return displayio_display_core_get_width(&self->core);
}

uint16_t common_hal_zephyr_display_display_get_height(zephyr_display_display_obj_t *self) {
    return displayio_display_core_get_height(&self->core);
}

mp_float_t common_hal_zephyr_display_display_get_brightness(zephyr_display_display_obj_t *self) {
    (void)self;
    return -1;
}

bool common_hal_zephyr_display_display_set_brightness(zephyr_display_display_obj_t *self, mp_float_t brightness) {
    (void)self;
    (void)brightness;
    return false;
}

static bool zephyr_display_refresh_area(zephyr_display_display_obj_t *self, const displayio_area_t *area) {
    uint16_t buffer_size = CIRCUITPY_DISPLAY_AREA_BUFFER_SIZE / sizeof(uint32_t);

    displayio_area_t clipped;
    if (!displayio_display_core_clip_area(&self->core, area, &clipped)) {
        return true;
    }

    uint16_t rows_per_buffer = displayio_area_height(&clipped);
    uint8_t pixels_per_word = (sizeof(uint32_t) * 8) / self->core.colorspace.depth;
    // For AL_88, displayio fills at 1 byte/pixel (L_8) but output needs 2 bytes/pixel,
    // so halve the effective pixels_per_word for buffer sizing.
    uint8_t effective_pixels_per_word = pixels_per_word;
    if (self->pixel_format == PIXEL_FORMAT_AL_88) {
        effective_pixels_per_word = sizeof(uint32_t) / 2;
    }
    uint16_t pixels_per_buffer = displayio_area_size(&clipped);
    uint16_t subrectangles = 1;

    // When pixels_in_byte_share_row is false (mono vtiled), 8 vertical pixels
    // pack into one column byte. The byte layout needs width * ceil(height/8)
    // bytes, which can exceed the pixel-count-based buffer size.
    bool vtiled = self->core.colorspace.depth < 8 &&
        !self->core.colorspace.pixels_in_byte_share_row;

    bool needs_subdivision = displayio_area_size(&clipped) > buffer_size * effective_pixels_per_word;
    if (vtiled && !needs_subdivision) {
        uint16_t width = displayio_area_width(&clipped);
        uint16_t height = displayio_area_height(&clipped);
        uint32_t vtiled_bytes = (uint32_t)width * ((height + 7) / 8);
        needs_subdivision = vtiled_bytes > buffer_size * sizeof(uint32_t);
    }

    if (needs_subdivision) {
        rows_per_buffer = buffer_size * effective_pixels_per_word / displayio_area_width(&clipped);
        if (vtiled) {
            rows_per_buffer = (rows_per_buffer / 8) * 8;
            if (rows_per_buffer == 0) {
                rows_per_buffer = 8;
            }
        }
        if (rows_per_buffer == 0) {
            rows_per_buffer = 1;
        }
        subrectangles = displayio_area_height(&clipped) / rows_per_buffer;
        if (displayio_area_height(&clipped) % rows_per_buffer != 0) {
            subrectangles++;
        }
        pixels_per_buffer = rows_per_buffer * displayio_area_width(&clipped);
        buffer_size = pixels_per_buffer / pixels_per_word;
        if (pixels_per_buffer % pixels_per_word) {
            buffer_size += 1;
        }
        // Ensure buffer is large enough for vtiled packing.
        if (vtiled) {
            uint16_t width = displayio_area_width(&clipped);
            uint16_t vtiled_words = (width * ((rows_per_buffer + 7) / 8) + sizeof(uint32_t) - 1) / sizeof(uint32_t);
            if (vtiled_words > buffer_size) {
                buffer_size = vtiled_words;
            }
        }
        // Ensure buffer is large enough for AL_88 expansion.
        if (self->pixel_format == PIXEL_FORMAT_AL_88) {
            uint16_t al88_words = (pixels_per_buffer * 2 + sizeof(uint32_t) - 1) / sizeof(uint32_t);
            if (al88_words > buffer_size) {
                buffer_size = al88_words;
            }
        }
    }

    uint32_t buffer[buffer_size];
    uint32_t mask_length = (pixels_per_buffer / 32) + 1;
    uint32_t mask[mask_length];

    uint16_t remaining_rows = displayio_area_height(&clipped);

    for (uint16_t j = 0; j < subrectangles; j++) {
        displayio_area_t subrectangle = {
            .x1 = clipped.x1,
            .y1 = clipped.y1 + rows_per_buffer * j,
            .x2 = clipped.x2,
            .y2 = clipped.y1 + rows_per_buffer * (j + 1),
        };

        if (remaining_rows < rows_per_buffer) {
            subrectangle.y2 = subrectangle.y1 + remaining_rows;
        }
        remaining_rows -= rows_per_buffer;

        memset(mask, 0, mask_length * sizeof(mask[0]));
        memset(buffer, 0, buffer_size * sizeof(buffer[0]));

        displayio_display_core_fill_area(&self->core, &subrectangle, mask, buffer);

        uint16_t width = displayio_area_width(&subrectangle);
        uint16_t height = displayio_area_height(&subrectangle);
        size_t pixel_count = (size_t)width * (size_t)height;

        if (self->pixel_format == PIXEL_FORMAT_MONO10) {
            uint8_t *bytes = (uint8_t *)buffer;
            size_t byte_count = (pixel_count + 7) / 8;
            for (size_t i = 0; i < byte_count; i++) {
                bytes[i] = ~bytes[i];
            }
        }

        if (self->pixel_format == PIXEL_FORMAT_AL_88) {
            uint8_t *bytes = (uint8_t *)buffer;
            for (size_t i = pixel_count; i > 0; i--) {
                bytes[(i - 1) * 2 + 1] = 0xFF;
                bytes[(i - 1) * 2] = bytes[i - 1];
            }
        }

        // Compute buf_size based on the Zephyr pixel format.
        uint32_t buf_size_bytes;
        if (self->pixel_format == PIXEL_FORMAT_MONO01 ||
            self->pixel_format == PIXEL_FORMAT_MONO10) {
            buf_size_bytes = (pixel_count + 7) / 8;
        } else if (self->pixel_format == PIXEL_FORMAT_AL_88) {
            buf_size_bytes = pixel_count * 2;
        } else {
            buf_size_bytes = pixel_count * (self->core.colorspace.depth / 8);
        }

        struct display_buffer_descriptor desc = {
            .buf_size = buf_size_bytes,
            .width = width,
            .height = height,
            .pitch = width,
            .frame_incomplete = false,
        };

        int err = display_write(self->device, subrectangle.x1, subrectangle.y1, &desc, buffer);
        if (err < 0) {
            return false;
        }

        RUN_BACKGROUND_TASKS;
    }

    return true;
}

static void zephyr_display_refresh(zephyr_display_display_obj_t *self) {
    if (!displayio_display_core_start_refresh(&self->core)) {
        return;
    }

    const displayio_area_t *current_area = zephyr_display_get_refresh_areas(self);
    while (current_area != NULL) {
        if (!zephyr_display_refresh_area(self, current_area)) {
            break;
        }
        current_area = current_area->next;
    }

    displayio_display_core_finish_refresh(&self->core);
}

void common_hal_zephyr_display_display_set_rotation(zephyr_display_display_obj_t *self, int rotation) {
    bool transposed = (self->core.rotation == 90 || self->core.rotation == 270);
    bool will_transposed = (rotation == 90 || rotation == 270);
    if (transposed != will_transposed) {
        int tmp = self->core.width;
        self->core.width = self->core.height;
        self->core.height = tmp;
    }

    displayio_display_core_set_rotation(&self->core, rotation);

    if (self == &displays[0].zephyr_display) {
        supervisor_stop_terminal();
        supervisor_start_terminal(self->core.width, self->core.height);
    }

    if (self->core.current_group != NULL) {
        displayio_group_update_transform(self->core.current_group, &self->core.transform);
    }
}

uint16_t common_hal_zephyr_display_display_get_rotation(zephyr_display_display_obj_t *self) {
    return self->core.rotation;
}

bool common_hal_zephyr_display_display_refresh(zephyr_display_display_obj_t *self,
    uint32_t target_ms_per_frame,
    uint32_t maximum_ms_per_real_frame) {
    if (!self->auto_refresh && !self->first_manual_refresh && (target_ms_per_frame != NO_FPS_LIMIT)) {
        uint64_t current_time = supervisor_ticks_ms64();
        uint32_t current_ms_since_real_refresh = current_time - self->core.last_refresh;
        if (current_ms_since_real_refresh > maximum_ms_per_real_frame) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("Below minimum frame rate"));
        }
        uint32_t current_ms_since_last_call = current_time - self->last_refresh_call;
        self->last_refresh_call = current_time;
        if (current_ms_since_last_call > target_ms_per_frame) {
            return false;
        }
        uint32_t remaining_time = target_ms_per_frame - (current_ms_since_real_refresh % target_ms_per_frame);
        while (supervisor_ticks_ms64() - self->last_refresh_call < remaining_time) {
            RUN_BACKGROUND_TASKS;
        }
    }
    self->first_manual_refresh = false;
    zephyr_display_refresh(self);
    return true;
}

bool common_hal_zephyr_display_display_get_auto_refresh(zephyr_display_display_obj_t *self) {
    return self->auto_refresh;
}

void common_hal_zephyr_display_display_set_auto_refresh(zephyr_display_display_obj_t *self, bool auto_refresh) {
    self->first_manual_refresh = !auto_refresh;
    if (auto_refresh != self->auto_refresh) {
        if (auto_refresh) {
            supervisor_enable_tick();
        } else {
            supervisor_disable_tick();
        }
    }
    self->auto_refresh = auto_refresh;
}

void zephyr_display_display_background(zephyr_display_display_obj_t *self) {
    if (self->auto_refresh && (supervisor_ticks_ms64() - self->core.last_refresh) > self->native_ms_per_frame) {
        zephyr_display_refresh(self);
    }
}

void release_zephyr_display(zephyr_display_display_obj_t *self) {
    common_hal_zephyr_display_display_set_auto_refresh(self, false);
    release_display_core(&self->core);
    self->device = NULL;
    self->base.type = &mp_type_NoneType;
}

void zephyr_display_display_collect_ptrs(zephyr_display_display_obj_t *self) {
    (void)self;
    displayio_display_core_collect_ptrs(&self->core);
}

void zephyr_display_display_reset(zephyr_display_display_obj_t *self) {
    if (self->device != NULL && device_is_ready(self->device)) {
        common_hal_zephyr_display_display_set_auto_refresh(self, true);
        displayio_display_core_set_root_group(&self->core, &circuitpython_splash);
        self->core.full_refresh = true;
    } else {
        release_zephyr_display(self);
    }
}

mp_obj_t common_hal_zephyr_display_display_get_root_group(zephyr_display_display_obj_t *self) {
    if (self->core.current_group == NULL) {
        return mp_const_none;
    }
    return self->core.current_group;
}

bool common_hal_zephyr_display_display_set_root_group(zephyr_display_display_obj_t *self, displayio_group_t *root_group) {
    return displayio_display_core_set_root_group(&self->core, root_group);
}
