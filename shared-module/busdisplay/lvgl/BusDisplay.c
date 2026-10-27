// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// LVGL BusDisplay implementation for fourwire/SPI displays
// Direct render via LVGL partial buffers, SPI flush
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/lvgl/lvgl_internal.h"
#include "supervisor/port.h"
#include "supervisor/port_heap.h"
#include "supervisor/shared/display.h"
#include "py/runtime.h"
#include "py/gc.h"
#include <string.h>
#include "lvgl.h"
#include "lvgl/draw/lv_draw_utils.h"

// Port allocations must be aligned enough for LVGL draw buffers and the PPA.
MP_STATIC_ASSERT(MICROPY_BYTES_PER_GC_BLOCK % LV_DRAW_BUF_ALIGN == 0);

#define LVGL_DRAW_BUF_LINES 10

static void busdisplay_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    busdisplay_busdisplay_obj_t *self = lv_display_get_user_data(disp);
    lv_display_rotation_t rotation = lv_display_get_rotation(disp);
    lv_area_t rotated_area;
    if (rotation != LV_DISPLAY_ROTATION_0) {
        int32_t width = lv_area_get_width(area);
        int32_t height = lv_area_get_height(area);
        rotated_area = *area;
        lv_display_rotate_area(disp, &rotated_area);
        uint32_t src_stride = lv_draw_buf_width_to_stride(width, self->lv_color_format);
        uint32_t dest_stride = lv_draw_buf_width_to_stride(
            lv_area_get_width(&rotated_area), self->lv_color_format);
        if ((size_t)dest_stride * lv_area_get_height(&rotated_area) >
            self->lv_rotation_buf_size) {
            lv_display_flush_ready(disp);
            return;
        }
        lv_color_format_t rotate_format = self->lv_color_format;
        if (rotate_format == LV_COLOR_FORMAT_RGB565_SWAPPED) {
            rotate_format = LV_COLOR_FORMAT_RGB565;
        }
        lv_draw_rotate(px_map, self->lv_rotation_buf, width, height,
            src_stride, dest_stride, rotation, rotate_format);
        area = &rotated_area;
        px_map = self->lv_rotation_buf;
    }
    uint16_t w = area->x2 - area->x1 + 1;
    uint16_t h = area->y2 - area->y1 + 1;
    size_t len = (size_t)w * h * ((self->color_depth + 7) / 8);
    if (self->color_depth == 1) {
        len = (w * h + 7) / 8;
        if (self->bytes_per_cell > 1) {
            len *= self->bytes_per_cell;
        }
    }

    if (!displayio_display_bus_is_free(&self->bus)) {
        // Don't run background tasks from here: on ports that render LVGL on a task, this flush
        // would otherwise execute background callbacks (including the BusDisplay proxy) from
        // inside LVGL's render. Block instead and let the bus owner run.
        while (!displayio_display_bus_begin_transaction(&self->bus)) {
            port_task_sleep_ms(1);
        }
        displayio_display_bus_end_transaction(&self->bus);
    }
    if (self->bus.bus == mp_const_none) {
        lv_display_flush_ready(disp);
        return;
    }

    displayio_area_t displayio_area = {
        .x1 = area->x1,
        .y1 = area->y1,
        .x2 = area->x2 + 1,
        .y2 = area->y2 + 1,
    };
    displayio_display_bus_set_region_to_update(&self->bus, &self->colorspace, &displayio_area);

    while (!displayio_display_bus_begin_transaction(&self->bus)) {
        port_task_sleep_ms(1);
    }
    if (!self->bus.data_as_commands) {
        uint8_t command = self->write_ram_command;
        self->bus.send(self->bus.bus, DISPLAY_COMMAND, CHIP_SELECT_TOGGLE_EVERY_BYTE, &command, 1);
    }
    self->bus.send(self->bus.bus, DISPLAY_DATA, CHIP_SELECT_UNTOUCHED, px_map, len);
    displayio_display_bus_end_transaction(&self->bus);

    lv_display_flush_ready(disp);
}

static lv_color_format_t busdisplay_lvgl_color_format(busdisplay_busdisplay_obj_t *self) {
    switch (self->color_depth) {
        case 16:
            return LV_COLOR_FORMAT_RGB565;
        case 24:
            return LV_COLOR_FORMAT_RGB888;
        case 32:
            return LV_COLOR_FORMAT_ARGB8888;
        case 8:
            return LV_COLOR_FORMAT_L8;
        case 1:
            return LV_COLOR_FORMAT_I1;
        default:
            return LV_COLOR_FORMAT_RGB565;
    }
}

typedef struct {
    nlr_jump_callback_node_t node;
    busdisplay_busdisplay_obj_t *self;
    bool locked;
} busdisplay_construct_cleanup_t;

static void busdisplay_construct_abort(void *ctx) {
    busdisplay_construct_cleanup_t *cleanup = ctx;
    busdisplay_busdisplay_obj_t *self = cleanup->self;
    if (!cleanup->locked) {
        displayio_lvgl_lock();
    }
    if (self->root_group != NULL) {
        displayio_lvgl_set_root_group(NULL);
        self->root_group = NULL;
    }
    if (self->lv_display != NULL) {
        lv_display_delete(self->lv_display);
        self->lv_display = NULL;
    }
    port_free(self->lv_buf1);
    port_free(self->lv_buf2);
    port_free(self->lv_rotation_buf);
    self->lv_buf1 = NULL;
    self->lv_buf2 = NULL;
    self->lv_rotation_buf = NULL;
    self->lv_buf_size = 0;
    self->lv_rotation_buf_size = 0;
    busdisplay_backlight_deinit(&self->backlight);
    self->deinited = true;
    self->base.type = &mp_type_NoneType;
    displayio_lvgl_unlock();
}

void common_hal_busdisplay_busdisplay_construct(busdisplay_busdisplay_obj_t *self,
    mp_obj_t bus, uint16_t width, uint16_t height, int16_t colstart, int16_t rowstart,
    uint16_t rotation, uint16_t color_depth, bool grayscale, bool pixels_in_byte_share_row,
    uint8_t bytes_per_cell, bool reverse_pixels_in_byte, bool reverse_bytes_in_word, uint8_t set_column_command,
    uint8_t set_row_command, uint8_t write_ram_command,
    uint8_t *init_sequence, uint16_t init_sequence_len, const mcu_pin_obj_t *backlight_pin,
    uint16_t brightness_command, mp_float_t brightness,
    bool single_byte_bounds, bool data_as_commands, bool auto_refresh, uint16_t native_frames_per_second,
    bool backlight_on_high, bool SH1107_addressing, uint16_t backlight_pwm_frequency) {

    self->lv_display = NULL;
    self->lv_buf1 = NULL;
    self->lv_buf2 = NULL;
    self->lv_rotation_buf = NULL;
    self->lv_buf_size = 0;
    self->lv_rotation_buf_size = 0;
    self->root_group = NULL;
    self->backlight.inout.base.type = &mp_type_NoneType;
    self->auto_refresh = false;
    busdisplay_construct_cleanup_t cleanup = { .self = self, .locked = false };
    nlr_push_jump_callback(&cleanup.node, busdisplay_construct_abort);
    // Packed pixels require panel-specific packing and LVGL's indexed palette
    // header must not be sent to the panel as pixels.
    if (color_depth < 8) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Format not supported"));
    }

    uint16_t ram_width = 0x100;
    uint16_t ram_height = 0x100;
    if (single_byte_bounds) {
        ram_width = 0xff;
        ram_height = 0xff;
    }
    displayio_display_bus_construct(&self->bus, bus, ram_width, ram_height, colstart, rowstart,
        set_column_command, set_row_command, NO_COMMAND, NO_COMMAND, data_as_commands,
        false /* always_toggle_chip_select */, SH1107_addressing && color_depth == 1,
        false /* address_little_endian */);
    self->width = width;
    self->height = height;
    self->rotation = 0;
    self->color_depth = color_depth;
    self->grayscale = grayscale;
    self->pixels_in_byte_share_row = pixels_in_byte_share_row;
    self->bytes_per_cell = bytes_per_cell;
    self->reverse_pixels_in_byte = reverse_pixels_in_byte;
    self->reverse_bytes_in_word = reverse_bytes_in_word;
    self->colorspace.depth = color_depth;
    self->colorspace.grayscale = grayscale;
    self->colorspace.grayscale_bit = 8 - color_depth;
    self->colorspace.pixels_in_byte_share_row = pixels_in_byte_share_row;
    self->colorspace.bytes_per_cell = bytes_per_cell;
    self->colorspace.reverse_pixels_in_byte = reverse_pixels_in_byte;
    self->colorspace.reverse_bytes_in_word = reverse_bytes_in_word;
    self->write_ram_command = write_ram_command;
    self->brightness_command = brightness_command;
    self->backlight_on_high = backlight_on_high;
    self->auto_refresh = false;
    self->first_manual_refresh = !auto_refresh;
    self->native_frames_per_second = native_frames_per_second;
    self->native_ms_per_frame = 1000 / (native_frames_per_second ? native_frames_per_second : 60);
    self->current_brightness = brightness;
    self->deinited = false;
    self->root_group = NULL;
    busdisplay_backlight_construct(&self->backlight, backlight_pin, backlight_pwm_frequency);
    common_hal_busdisplay_busdisplay_set_brightness(self, brightness);

    displayio_display_bus_send_init_sequence(&self->bus, init_sequence, init_sequence_len);

    displayio_lvgl_port_init();
    // Hold the render lock across the whole setup so the render task can't see a half-configured
    // display. The port lock is recursive.
    displayio_lvgl_lock();
    cleanup.locked = true;

    // Create LVGL display
    self->lv_display = lv_display_create(width, height);
    if (self->lv_display == NULL) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("LVGL display create failed"));
    }
    lv_display_set_user_data(self->lv_display, self);
    self->lv_color_format = busdisplay_lvgl_color_format(self);
    lv_display_set_color_format(self->lv_display, self->lv_color_format);

    // Allocate draw buffers via port_malloc. Displays outlive the VM, so these
    // can't come from the garbage collected heap.
    size_t bpp = (color_depth + 7) / 8;
    if (bpp == 0) {
        bpp = 1;
    }
    size_t buf_lines = LVGL_DRAW_BUF_LINES;
    if (buf_lines > height) {
        buf_lines = height;
    }
    size_t buf_size = width * buf_lines * bpp;
    // Align to 4 bytes
    buf_size = (buf_size + 3) & ~3;
    // Ports with LVGL keep port_malloc() allocations aligned for the PPA and LVGL draw
    // buffers.
    self->lv_buf1 = port_malloc(buf_size, false);
    self->lv_buf2 = port_malloc(buf_size, false);
    self->lv_rotation_buf = port_malloc(buf_size, false);
    if (self->lv_buf1 == NULL || self->lv_buf2 == NULL || self->lv_rotation_buf == NULL) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("LVGL buffer alloc failed"));
    }
    self->lv_buf_size = buf_size;
    self->lv_rotation_buf_size = buf_size;
    lv_display_set_buffers(self->lv_display, self->lv_buf1, self->lv_buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(self->lv_display, busdisplay_lvgl_flush_cb);

    // BusDisplay's flush drives the display bus and may run background tasks, so ports that
    // render LVGL on a separate task route this display's refresh through a proxy that runs
    // lv_refr_now() on the CircuitPython thread. Ports that render BusDisplay on their own LVGL
    // thread (Zephyr) leave the refresh timer alone.
    displayio_lvgl_port_install_busdisplay_refresh(self->lv_display);

    displayio_lvgl_init();

    // Set initial root group to splash - make this display default first
    self->root_group = &circuitpython_splash;
    lv_display_set_default(self->lv_display);
    displayio_lvgl_register_display(self->lv_display);
    displayio_lvgl_set_root_group(&circuitpython_splash);
    nlr_pop_jump_callback(false);
    displayio_lvgl_unlock();

    if (rotation != 0) {
        common_hal_busdisplay_busdisplay_set_rotation(self, rotation);
    }

    common_hal_busdisplay_busdisplay_set_auto_refresh(self, auto_refresh);
}

// Stubs for remaining API - to be implemented
uint16_t common_hal_busdisplay_busdisplay_get_width(busdisplay_busdisplay_obj_t *self) {
    return self->width;
}
uint16_t common_hal_busdisplay_busdisplay_get_height(busdisplay_busdisplay_obj_t *self) {
    return self->height;
}
uint16_t common_hal_busdisplay_busdisplay_get_rotation(busdisplay_busdisplay_obj_t *self) {
    return self->rotation;
}
void common_hal_busdisplay_busdisplay_set_rotation(busdisplay_busdisplay_obj_t *self, int rotation) {
    rotation %= 360;
    bool transposed = (self->rotation == 90 || self->rotation == 270);
    bool will_transposed = (rotation == 90 || rotation == 270);
    if (transposed != will_transposed) {
        int tmp = self->width;
        self->width = self->height;
        self->height = tmp;
    }
    self->rotation = rotation;

    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    if (self->lv_display != NULL) {
        // LVGL rotates the opposite way from displayio for the transposed orientations.
        lv_display_rotation_t lv_rotation;
        switch (rotation) {
            case 90:
                lv_rotation = LV_DISPLAY_ROTATION_270;
                break;
            case 270:
                lv_rotation = LV_DISPLAY_ROTATION_90;
                break;
            case 180:
                lv_rotation = LV_DISPLAY_ROTATION_180;
                break;
            default:
                lv_rotation = LV_DISPLAY_ROTATION_0;
                break;
        }
        lv_display_set_rotation(self->lv_display, lv_rotation);
        lv_display_set_default(self->lv_display);
    }

    if (self == &displays[0].display) {
        supervisor_stop_terminal();
        supervisor_start_terminal(self->width, self->height);
    }
    if (self->root_group != NULL) {
        displayio_lvgl_set_root_group(self->root_group);
    }
    lv_obj_t *screen = lv_screen_active();
    if (screen != NULL) {
        lv_obj_invalidate(screen);
    }
    displayio_lvgl_unlock_with_cleanup();
}
bool common_hal_busdisplay_busdisplay_get_auto_refresh(busdisplay_busdisplay_obj_t *self) {
    return self->auto_refresh;
}
void common_hal_busdisplay_busdisplay_set_auto_refresh(busdisplay_busdisplay_obj_t *self, bool auto_refresh) {
    self->auto_refresh = auto_refresh;
}
bool common_hal_busdisplay_busdisplay_refresh(busdisplay_busdisplay_obj_t *self, uint32_t target_ms_per_frame, uint32_t maximum_ms_per_real_frame) {
    if (self->lv_display) {
        // The renderer refreshes on its own thread too, so this has to exclude it.
        displayio_lvgl_lock();
        lv_refr_now(self->lv_display);
        displayio_lvgl_unlock();
    }
    return true;
}
mp_obj_t common_hal_busdisplay_busdisplay_get_bus(busdisplay_busdisplay_obj_t *self) {
    return self->bus.bus;
}
mp_obj_t common_hal_busdisplay_busdisplay_get_root_group(busdisplay_busdisplay_obj_t *self) {
    return self->root_group ? self->root_group : mp_const_none;
}
mp_obj_t common_hal_busdisplay_busdisplay_set_root_group(busdisplay_busdisplay_obj_t *self, displayio_group_t *root_group) {
    self->root_group = root_group;
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    // Group can only be on one display at a time.
    if (root_group != NULL) {
        lv_display_set_default(self->lv_display);
    }
    displayio_lvgl_set_root_group(root_group);
    displayio_lvgl_unlock_with_cleanup();
    return mp_const_none;
}
mp_float_t common_hal_busdisplay_busdisplay_get_brightness(busdisplay_busdisplay_obj_t *self) {
    return self->current_brightness;
}
bool common_hal_busdisplay_busdisplay_set_brightness(busdisplay_busdisplay_obj_t *self, mp_float_t brightness) {
    if (!self->backlight_on_high) {
        brightness = 1.0 - brightness;
    }
    bool ok = busdisplay_backlight_set(&self->backlight, brightness);
    if (ok) {
        self->current_brightness = brightness;
    }
    return ok;
}
bool common_hal_busdisplay_busdisplay_get_dither(busdisplay_busdisplay_obj_t *self) {
    return false;
}
void common_hal_busdisplay_busdisplay_set_dither(busdisplay_busdisplay_obj_t *self, bool dither) {
}
void busdisplay_busdisplay_background(busdisplay_busdisplay_obj_t *self) {
    // The port's LVGL render task owns the pump. Keeping this free of lv_timer_handler()
    // guarantees LVGL only renders on its own tick, a frame boundary, or display.refresh().
    (void)self;
}
void release_busdisplay(busdisplay_busdisplay_obj_t *self) {
    self->deinited = true;
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    displayio_lvgl_set_root_group(NULL);
    if (self->lv_display != NULL) {
        lv_display_delete(self->lv_display);
        self->lv_display = NULL;
    }
    displayio_lvgl_unlock_with_cleanup();
    // Draw buffers are allocated from the port heap, not LVGL's heap, so free them
    // explicitly after the display that referenced them is gone.
    port_free(self->lv_buf1);
    port_free(self->lv_buf2);
    port_free(self->lv_rotation_buf);
    self->lv_buf1 = NULL;
    self->lv_buf2 = NULL;
    self->lv_rotation_buf = NULL;
    self->lv_buf_size = 0;
    self->lv_rotation_buf_size = 0;
    self->root_group = NULL;
    busdisplay_backlight_deinit(&self->backlight);
    self->base.type = &mp_type_NoneType;
}
void reset_busdisplay(busdisplay_busdisplay_obj_t *self) {
    common_hal_busdisplay_busdisplay_set_auto_refresh(self, true);
    circuitpython_splash.x = 0; // reset position in case someone moved it.
    circuitpython_splash.y = 0;
    supervisor_start_terminal(self->width, self->height);
    // Show the splash if it is free, otherwise nothing. Other groups are on the VM heap.
    if (!circuitpython_splash.in_group) {
        nlr_jump_callback_node_t cleanup;
        displayio_lvgl_lock_with_cleanup(&cleanup);
        self->root_group = &circuitpython_splash;
        displayio_lvgl_set_root_group(&circuitpython_splash);
        if (self->lv_display != NULL) {
            lv_refr_now(self->lv_display);
        }
        displayio_lvgl_unlock_with_cleanup();
    } else {
        self->root_group = NULL;
        nlr_jump_callback_node_t cleanup;
        displayio_lvgl_lock_with_cleanup(&cleanup);
        displayio_lvgl_set_root_group(NULL);
        if (self->lv_display != NULL) {
            lv_refr_now(self->lv_display);
        }
        displayio_lvgl_unlock_with_cleanup();
    }
}
void busdisplay_busdisplay_collect_ptrs(busdisplay_busdisplay_obj_t *self) {
    gc_collect_ptr(self->root_group);
    displayio_display_bus_collect_ptrs(&self->bus);
}
