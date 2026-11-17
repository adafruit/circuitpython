// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/epaperdisplay/EPaperDisplay.h"

#include "py/gc.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#if CIRCUITPY_EPAPERDISPLAY_FAST && CIRCUITPY_WATCHDOG
#include "shared-bindings/watchdog/WatchDogTimer.h"
#endif
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/lvgl/lvgl_internal.h"
#include "shared-module/epaperdisplay/lag_drive.h"
#include "supervisor/port.h"
#include "supervisor/port_heap.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/tick.h"

#include "lvgl.h"
#include "lvgl/draw/lv_draw_utils.h"

#if CIRCUITPY_TINYUSB
#include "supervisor/usb.h"
#endif

#include <stdint.h>
#include <string.h>

#define DELAY 0x80

// LVGL draw buffer height. The flush callback converts complete rows, so this only needs to
// cover a useful part of the screen at a time.
#define EPAPER_LVGL_DRAW_BUF_LINES 20

static void epaperdisplay_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
static void epaperdisplay_lvgl_mark_dirty(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *area);
static void epaperdisplay_lvgl_clear_dirty(epaperdisplay_epaperdisplay_obj_t *self);

// lag_drive.c calls these only when it is built. Otherwise they stay static, as before it.
#if CIRCUITPY_EPAPERDISPLAY_FAST
#define LAG_SHARED
#else
#define LAG_SHARED static
#endif

static void epaperdisplay_lvgl_free_buffers(epaperdisplay_epaperdisplay_obj_t *self) {
    port_free(self->lv_buf);
    port_free(self->lv_rotation_buf);
    port_free(self->black_buf);
    port_free(self->color_buf);
    self->lv_buf = NULL;
    self->lv_rotation_buf = NULL;
    self->black_buf = NULL;
    self->color_buf = NULL;
    self->lv_buf_size = 0;
    self->lv_rotation_buf_size = 0;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    epaperdisplay_lag_free(self);
    #endif
}

void common_hal_epaperdisplay_epaperdisplay_construct(epaperdisplay_epaperdisplay_obj_t *self,
    const epaperdisplay_construct_args_t *args) {
    uint16_t rotation = args->rotation % 360;
    uint16_t color_depth = 1;
    bool core_grayscale = true;
    if (args->highlight_color != 0x000000) {
        self->core.colorspace.tricolor = true;
        self->core.colorspace.tricolor_hue = displayio_colorconverter_compute_hue(args->highlight_color);
    } else {
        self->core.colorspace.tricolor = false;
    }
    if (args->highlight_color != 0x000000 && args->highlight_color2 != 0x000000) {
        self->core.colorspace.tricolor = false;
        self->core.colorspace.fourcolor = true;
        self->core.colorspace.fourcolor_hue = displayio_colorconverter_compute_hue(args->highlight_color2);
    } else {
        self->core.colorspace.fourcolor = false;
    }
    self->acep = args->acep || args->spectra6;
    self->core.colorspace.sixcolor = args->spectra6;
    self->core.colorspace.sevencolor = args->acep;
    bool grayscale = args->grayscale;
    if (self->acep) {
        color_depth = 4; // bits. 7 colors + clean
        grayscale = false;
        core_grayscale = false;
    }
    if ((args->highlight_color != 0x000000 || args->highlight_color2 != 0x000000) && args->write_color_ram_command == NO_COMMAND) {
        color_depth = 2;
        core_grayscale = false;
        grayscale = false;
    }

    displayio_display_core_construct(&self->core, args->width, args->height, rotation, color_depth, core_grayscale, true, 1, true, true);
    displayio_display_bus_construct(&self->bus, args->bus, args->ram_width, args->ram_height,
        args->colstart, args->rowstart,
        args->set_column_window_command, args->set_row_window_command, args->set_current_column_command, args->set_current_row_command,
        false /* data_as_commands */, args->always_toggle_chip_select,
        false /* SH1107_addressing */, args->address_little_endian);

    // The converted framebuffer. The panel planes are packed like the controller RAM:
    // most significant pixel first, regardless of the LVGL source format. These allocations
    // outlive the VM, so they come from the port heap.
    self->lv_display = NULL;
    self->lv_color_format = LV_COLOR_FORMAT_RGB565;
    self->lv_buf = NULL;
    self->lv_buf_size = 0;
    self->lv_rotation_buf = NULL;
    self->lv_rotation_buf_size = 0;
    // The constructor's width and height are logical dimensions at the requested rotation.
    // LVGL's original resolution and the converted planes use the controller's native RAM
    // orientation instead, which is fixed for the lifetime of the display.
    bool transposed = rotation == 90 || rotation == 270;
    self->canvas_width = transposed ? args->height : args->width;
    self->canvas_height = transposed ? args->width : args->height;
    self->canvas_depth = color_depth;
    self->canvas_stride = (self->canvas_width * color_depth + 7) / 8;
    self->black_buf = NULL;
    self->color_buf = NULL;
    self->dirty_count = 0;
    self->dirty_full = false;
    self->refresh_in_progress = false;
    self->full_refresh = true;

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    self->lag.slots = NULL;
    self->lag.frames = 0;
    self->lag.busy_deadline = 0;
    self->lag.bus_held = false;
    self->lag.state_lost = false;
    self->lag.slot_overwriting = false;
    self->lag.commit = LAG_COMMIT_NONE;
    self->lag.holding_interrupts = false;
    self->lag.held_exception = MP_OBJ_NULL;
    self->clean_sequence = args->clean_sequence;
    self->clean_sequence_len = args->clean_sequence_len;
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST

    self->write_black_ram_command = args->write_black_ram_command;
    self->black_bits_inverted = args->black_bits_inverted;
    self->write_color_ram_command = args->write_color_ram_command;
    self->color_bits_inverted = args->color_bits_inverted;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    self->write_previous_ram_command = args->write_previous_ram_command;
    #endif
    self->refresh_time = args->refresh_time * 1000;
    self->busy_state = args->busy_state;
    self->refreshing = false;
    self->milliseconds_per_frame = args->seconds_per_frame * 1000;
    self->chip_select = args->always_toggle_chip_select ? CHIP_SELECT_TOGGLE_EVERY_BYTE : CHIP_SELECT_UNTOUCHED;
    self->grayscale = grayscale;

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (args->lag_frames > 0) {
        epaperdisplay_lag_construct(self, args->lag_frames);
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST

    size_t canvas_size = (size_t)self->canvas_stride * self->canvas_height;
    self->black_buf = port_malloc(canvas_size, false);
    if (self->black_buf == NULL) {
        epaperdisplay_lvgl_free_buffers(self);
        self->base.type = &mp_type_NoneType;
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("LVGL buffer alloc failed"));
    }
    memset(self->black_buf, 0, canvas_size);
    if (args->write_color_ram_command != NO_COMMAND && !self->acep) {
        self->color_buf = port_malloc(canvas_size, false);
        if (self->color_buf == NULL) {
            epaperdisplay_lvgl_free_buffers(self);
            self->base.type = &mp_type_NoneType;
            mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("LVGL buffer alloc failed"));
        }
        memset(self->color_buf, 0, canvas_size);
    }

    self->start_sequence = args->start_sequence;
    self->start_sequence_len = args->start_sequence_len;
    self->start_up_time_ms = args->start_up_time * 1000;
    self->stop_sequence = args->stop_sequence;
    self->stop_sequence_len = args->stop_sequence_len;
    self->refresh_sequence = args->refresh_sequence;
    self->refresh_sequence_len = args->refresh_sequence_len;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    self->fast = args->fast;
    self->awake = false;
    self->needs_start = false;
    #endif

    self->busy.base.type = &mp_type_NoneType;
    self->two_byte_sequence_length = args->two_byte_sequence_length;
    if (args->busy_pin != NULL) {
        self->busy.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->busy, args->busy_pin);
    }

    // Clear the color memory if it isn't in use.
    if (args->highlight_color == 0x00 && args->highlight_color2 == 0x00 && args->write_color_ram_command != NO_COMMAND) {
        // TODO: Clear
    }

    // Set up LVGL. The display's buffers and flush callback have to exist before the
    // compositor can see it as the default display.
    displayio_lvgl_port_init();
    displayio_lvgl_lock();
    self->lv_display = lv_display_create(self->canvas_width, self->canvas_height);
    if (self->lv_display == NULL) {
        displayio_lvgl_unlock();
        epaperdisplay_lvgl_free_buffers(self);
        self->base.type = &mp_type_NoneType;
        mp_raise_RuntimeError(MP_ERROR_TEXT("LVGL display create failed"));
    }
    lv_display_set_user_data(self->lv_display, self);
    lv_display_set_color_format(self->lv_display, self->lv_color_format);
    lv_display_set_flush_cb(self->lv_display, epaperdisplay_lvgl_flush_cb);

    uint16_t max_dimension = MAX(args->width, args->height);
    uint16_t lines = MIN(EPAPER_LVGL_DRAW_BUF_LINES, self->canvas_height);
    size_t lv_buf_size = lv_draw_buf_width_to_stride(max_dimension, self->lv_color_format) * lines;
    lv_buf_size = (lv_buf_size + LV_DRAW_BUF_ALIGN - 1) & ~(LV_DRAW_BUF_ALIGN - 1);
    self->lv_buf = port_malloc(lv_buf_size, false);
    self->lv_rotation_buf = port_malloc(lv_buf_size, false);
    if (self->lv_buf == NULL || self->lv_rotation_buf == NULL) {
        lv_display_delete(self->lv_display);
        self->lv_display = NULL;
        displayio_lvgl_unlock();
        epaperdisplay_lvgl_free_buffers(self);
        self->base.type = &mp_type_NoneType;
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("LVGL buffer alloc failed"));
    }
    self->lv_buf_size = lv_buf_size;
    self->lv_rotation_buf_size = lv_buf_size;
    lv_display_set_buffers(self->lv_display, self->lv_buf, NULL, self->lv_buf_size,
        LV_DISPLAY_RENDER_MODE_PARTIAL);

    displayio_lvgl_init();
    lv_display_set_default(self->lv_display);
    displayio_lvgl_register_display(self->lv_display);
    displayio_display_core_set_root_group(&self->core, &circuitpython_splash);
    displayio_lvgl_set_root_group(&circuitpython_splash);
    displayio_lvgl_unlock();

    if (rotation != 0) {
        common_hal_epaperdisplay_epaperdisplay_set_rotation(self, rotation);
    }
}

bool common_hal_epaperdisplay_epaperdisplay_set_root_group(epaperdisplay_epaperdisplay_obj_t *self, displayio_group_t *root_group) {
    bool ok = displayio_display_core_set_root_group(&self->core, root_group);
    if (!ok) {
        return false;
    }
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    if (self->lv_display != NULL) {
        lv_display_set_default(self->lv_display);
    }
    displayio_lvgl_set_root_group(root_group);
    self->full_refresh = true;
    self->dirty_full = true;
    displayio_lvgl_unlock_with_cleanup();
    return true;
}

uint16_t common_hal_epaperdisplay_epaperdisplay_get_width(epaperdisplay_epaperdisplay_obj_t *self) {
    return displayio_display_core_get_width(&self->core);
}

uint16_t common_hal_epaperdisplay_epaperdisplay_get_height(epaperdisplay_epaperdisplay_obj_t *self) {
    return displayio_display_core_get_height(&self->core);
}

// The whole physical panel, which is also the coordinate space the flush callback and the
// controller commands use.
void epaperdisplay_lvgl_full_area(epaperdisplay_epaperdisplay_obj_t *self, displayio_area_t *area) {
    area->x1 = 0;
    area->y1 = 0;
    area->x2 = self->canvas_width;
    area->y2 = self->canvas_height;
}

// Clip an area to the panel and grow it to a whole byte of packed pixels. The controller
// addresses packed RAM in whole bytes, so a partial byte at either edge has to be sent too.
bool epaperdisplay_lvgl_clip_area(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *area,
    displayio_area_t *clipped) {
    displayio_area_t whole;
    epaperdisplay_lvgl_full_area(self, &whole);
    if (!displayio_area_compute_overlap(&whole, area, clipped)) {
        return false;
    }
    uint8_t depth = self->canvas_depth;
    if (depth < 8 && self->core.colorspace.pixels_in_byte_share_row) {
        uint8_t pixels_per_byte = 8 / depth * self->core.colorspace.bytes_per_cell;
        clipped->x1 -= clipped->x1 % pixels_per_byte;
        if (clipped->x2 % pixels_per_byte != 0) {
            clipped->x2 += pixels_per_byte - clipped->x2 % pixels_per_byte;
        }
        if (clipped->x1 < 0) {
            clipped->x1 = 0;
        }
        // The packed rows have room for the padding pixels in the last byte. They are written
        // as zero (or white when inverted) and sent with the rest of the row.
        int32_t packed_width = self->canvas_stride * pixels_per_byte;
        if (clipped->x2 > packed_width) {
            clipped->x2 = packed_width;
        }
    }
    return !displayio_area_empty(clipped);
}

bool epaperdisplay_lvgl_start_refresh(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->refresh_in_progress) {
        return false;
    }
    self->refresh_in_progress = true;
    self->core.last_refresh = supervisor_ticks_ms64();
    return true;
}

void epaperdisplay_lvgl_finish_refresh(epaperdisplay_epaperdisplay_obj_t *self) {
    self->full_refresh = false;
    self->refresh_in_progress = false;
    self->core.last_refresh = supervisor_ticks_ms64();
    epaperdisplay_lvgl_clear_dirty(self);
}

static void epaperdisplay_lvgl_clear_dirty(epaperdisplay_epaperdisplay_obj_t *self) {
    self->dirty_count = 0;
    self->dirty_full = false;
}

static bool epaperdisplay_lvgl_area_contains(const displayio_area_t *outer, const displayio_area_t *inner) {
    return outer->x1 <= inner->x1 && outer->y1 <= inner->y1 &&
           outer->x2 >= inner->x2 && outer->y2 >= inner->y2;
}

// Keep the areas LVGL flushed since the panel was last updated. Sending them individually
// preserves more of the panel's partial-update window than one screen-sized box would.
static void epaperdisplay_lvgl_mark_dirty(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *area) {
    if (self->dirty_full) {
        return;
    }
    displayio_area_t clipped;
    if (!epaperdisplay_lvgl_clip_area(self, area, &clipped)) {
        return;
    }
    for (uint8_t i = 0; i < self->dirty_count; i++) {
        if (epaperdisplay_lvgl_area_contains(&self->dirty[i], &clipped)) {
            return;
        }
    }
    if (self->dirty_count < EPAPER_LVGL_MAX_DIRTY) {
        self->dirty[self->dirty_count++] = clipped;
        return;
    }
    // Too many small changes. Merge everything, including this one, into a single box.
    displayio_area_t merged = clipped;
    for (uint8_t i = 0; i < self->dirty_count; i++) {
        displayio_area_union(&merged, &self->dirty[i], &merged);
    }
    self->dirty[0] = merged;
    self->dirty_count = 1;
}

// Store one converted panel pixel in a packed plane. Bits run most significant first and each
// row starts on a byte boundary, which is what the controllers and the legacy compositor write.
static void epaperdisplay_lvgl_store_pixel(epaperdisplay_epaperdisplay_obj_t *self, uint8_t *buf,
    uint16_t x, uint16_t y, uint8_t value, bool inverted) {
    uint8_t depth = self->canvas_depth;
    uint8_t pixels_per_byte = 8 / depth;
    uint8_t mask = (1u << depth) - 1u;
    value &= mask;
    if (inverted) {
        value ^= mask;
    }
    uint16_t byte_index = y * self->canvas_stride + x / pixels_per_byte;
    uint8_t shift = (pixels_per_byte - 1 - (x % pixels_per_byte)) * depth;
    buf[byte_index] = (buf[byte_index] & ~(mask << shift)) | (value << shift);
}

static uint32_t epaperdisplay_lvgl_rgb565_to_rgb888(uint16_t pixel) {
    uint32_t r8 = (pixel >> 11) << 3;
    uint32_t g8 = ((pixel >> 5) << 2) & 0xff;
    uint32_t b8 = (pixel << 3) & 0xff;
    return (r8 << 16) | (g8 << 8) | b8;
}

static void epaperdisplay_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    epaperdisplay_epaperdisplay_obj_t *self = lv_display_get_user_data(disp);
    lv_display_rotation_t rotation = lv_display_get_rotation(disp);
    const uint8_t *src = px_map;
    lv_area_t rotated_area;
    uint32_t src_stride = lv_draw_buf_width_to_stride(lv_area_get_width(area), self->lv_color_format);

    if (rotation != LV_DISPLAY_ROTATION_0) {
        rotated_area = *area;
        lv_display_rotate_area(disp, &rotated_area);
        uint32_t dest_stride = lv_draw_buf_width_to_stride(
            lv_area_get_width(&rotated_area), self->lv_color_format);
        if ((size_t)dest_stride * lv_area_get_height(&rotated_area) > self->lv_rotation_buf_size) {
            // The rotation buffer is sized for the longest edge. A larger flush means the
            // display geometry changed without reconfiguring; drop this frame rather than
            // writing past the buffer.
            lv_display_flush_ready(disp);
            return;
        }
        lv_color_format_t rotate_format = self->lv_color_format;
        if (rotate_format == LV_COLOR_FORMAT_RGB565_SWAPPED) {
            rotate_format = LV_COLOR_FORMAT_RGB565;
        }
        lv_draw_rotate(px_map, self->lv_rotation_buf, lv_area_get_width(area),
            lv_area_get_height(area), src_stride, dest_stride, rotation, rotate_format);
        area = &rotated_area;
        src = self->lv_rotation_buf;
        src_stride = dest_stride;
    }

    _displayio_colorspace_t black_cs = self->core.colorspace;
    if (!self->acep) {
        black_cs.grayscale = true;
        black_cs.grayscale_bit = 7;
    }
    _displayio_colorspace_t color_cs = self->core.colorspace;
    bool color_convert = false;
    if (self->color_buf != NULL) {
        if (self->grayscale) {
            color_cs.grayscale = true;
            color_cs.grayscale_bit = 6;
            color_convert = true;
        } else if (color_cs.tricolor) {
            color_cs.grayscale = false;
            color_convert = true;
        } else if (color_cs.sixcolor || color_cs.sevencolor) {
            color_convert = true;
        }
    }

    for (int32_t y = area->y1; y <= area->y2; y++) {
        // px_map (or the rotated copy) starts at the flush area's first pixel.
        const uint8_t *row = src + (y - area->y1) * src_stride;
        for (int32_t x = area->x1; x <= area->x2; x++) {
            uint16_t source_pixel;
            memcpy(&source_pixel, row, sizeof(source_pixel));
            row += sizeof(source_pixel);
            uint32_t rgb = epaperdisplay_lvgl_rgb565_to_rgb888(source_pixel);
            displayio_input_pixel_t input_pixel = {
                .pixel = rgb,
                .x = x,
                .y = y,
                .tile_x = x,
                .tile_y = y,
            };
            displayio_output_pixel_t output_pixel;
            output_pixel.pixel = 0;
            output_pixel.opaque = false;
            displayio_convert_color(&black_cs, black_cs.dither, &input_pixel, &output_pixel);
            epaperdisplay_lvgl_store_pixel(self, self->black_buf, x, y,
                output_pixel.pixel, self->black_bits_inverted);

            if (self->color_buf != NULL) {
                if (color_convert) {
                    displayio_convert_color(&color_cs, color_cs.dither, &input_pixel, &output_pixel);
                } else {
                    output_pixel.pixel = 0;
                }
                epaperdisplay_lvgl_store_pixel(self, self->color_buf, x, y,
                    output_pixel.pixel, self->color_bits_inverted);
            }
        }
    }

    // LVGL area coordinates are inclusive; displayio's are exclusive.
    displayio_area_t dirty_area = {
        .x1 = area->x1,
        .y1 = area->y1,
        .x2 = area->x2 + 1,
        .y2 = area->y2 + 1,
    };
    epaperdisplay_lvgl_mark_dirty(self, &dirty_area);
    lv_display_flush_ready(disp);
}

#if CIRCUITPY_EPAPERDISPLAY_FAST
// BUSY can take a moment to rise after an update command, and reading it low before then would
// look like a finished update. If it never rises, nothing has started or it was too quick to see,
// so this returns false after 2 ms. Short enough that an interrupt doesn't have to end it.
bool epaperdisplay_epaperdisplay_wait_for_update_start(epaperdisplay_epaperdisplay_obj_t *self) {
    for (uint8_t wait = 0; wait < 200; wait++) {
        if (common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state) {
            return true;
        }
        common_hal_mcu_delay_us(10);
        RUN_BACKGROUND_TASKS;
        port_task_yield();
    }
    return false;
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

#if CIRCUITPY_EPAPERDISPLAY_FAST
// When a wait for BUSY or the bus gives up, or 0 for never. Lag drive always has an end, so a
// stuck panel can't hold up a caller that holds interrupts or has no way to be interrupted.
static uint64_t display_wait_deadline(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag.busy_deadline != 0 || self->lag.frames == 0) {
        return self->lag.busy_deadline;
    }
    return supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
}

// Between hold and release a waveform or a panel reset is under way, and cutting it short leaves
// the panel dark. Whatever interrupts (Ctrl-C, a reload, a watchdog) waits for the release.
bool epaperdisplay_epaperdisplay_interrupted(epaperdisplay_epaperdisplay_obj_t *self) {
    if (!self->lag.holding_interrupts) {
        return mp_hal_is_interrupted();
    }
    if (mp_hal_is_interrupted()) {
        if (self->lag.held_exception == MP_OBJ_NULL) {
            self->lag.held_exception = MP_STATE_THREAD(mp_pending_exception);
        }
        MP_STATE_THREAD(mp_pending_exception) = MP_OBJ_NULL;
    }
    #if CIRCUITPY_WATCHDOG
    // A stored waveform takes seconds, and a reset mid-waveform leaves the panel dark. These waits
    // are bounded, so a hang still ends.
    if (common_hal_watchdog_get_mode(&common_hal_mcu_watchdogtimer_obj) != WATCHDOGMODE_NONE) {
        common_hal_watchdog_feed(&common_hal_mcu_watchdogtimer_obj);
    }
    #endif
    return false;
}

static bool display_send_command(epaperdisplay_epaperdisplay_obj_t *self, const uint8_t *command,
    const uint8_t *data, uint32_t length) {
    for (uint8_t attempt = 0; attempt < 3; attempt++) {
        if (epaperdisplay_lag_send(self, DISPLAY_COMMAND, command, 1) && epaperdisplay_lag_send(self, DISPLAY_DATA, data, length)) {
            return true;
        }
        // The send stopped, and it may have arrived. If it was an update, sending it again would
        // drive every pixel a second time. BUSY high says it landed. A command with no data could
        // be an update that BUSY hasn't shown yet, so don't risk it: the drive is now unknown.
        if (epaperdisplay_epaperdisplay_wait_for_update_start(self)) {
            return true;
        }
        if (length == 0) {
            self->lag.state_lost = true;
            return false;
        }
    }
    return false;
}

// A delay ends early when interrupted, which a held interrupt must not do.
void epaperdisplay_epaperdisplay_delay_ms(epaperdisplay_epaperdisplay_obj_t *self, uint32_t delay_ms) {
    uint64_t start = supervisor_ticks_ms64();
    uint32_t elapsed = 0;
    while (true) {
        common_hal_time_delay_ms(delay_ms - elapsed);
        elapsed = supervisor_ticks_ms64() - start;
        if (!self->lag.holding_interrupts || elapsed >= delay_ms) {
            return;
        }
        epaperdisplay_epaperdisplay_interrupted(self);
    }
}
#else // CIRCUITPY_EPAPERDISPLAY_FAST
#define display_wait_deadline(self) ((uint64_t)0)
#define epaperdisplay_epaperdisplay_interrupted(self) mp_hal_is_interrupted()
#define display_send_command(self, command, data, length) ( \
    (self)->bus.send((self)->bus.bus, DISPLAY_COMMAND, (self)->chip_select, command, 1), \
    (self)->bus.send((self)->bus.bus, DISPLAY_DATA, (self)->chip_select, data, length), \
    true)
#define epaperdisplay_epaperdisplay_delay_ms(self, delay_ms) common_hal_time_delay_ms(delay_ms)
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

LAG_SHARED void epaperdisplay_epaperdisplay_wait_for_busy(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->busy.base.type == &mp_type_NoneType) {
        return;
    }
    const uint64_t deadline = display_wait_deadline(self);
    while (common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state &&
           !epaperdisplay_epaperdisplay_interrupted(self)) {
        if (deadline != 0 && supervisor_ticks_ms64() > deadline) {
            return;
        }
        RUN_BACKGROUND_TASKS;
        port_task_yield();
    }
}

// True if every command went out. Lag drive counts an update only if its last command did.
LAG_SHARED bool epaperdisplay_epaperdisplay_send_command_sequence(epaperdisplay_epaperdisplay_obj_t *self,
    bool should_wait_for_busy, const uint8_t *sequence, uint32_t sequence_len) {
    uint32_t i = 0;
    while (i < sequence_len) {
        const uint8_t *cmd = sequence + i;
        uint8_t data_size = *(cmd + 1);
        bool delay = (data_size & DELAY) != 0;
        const uint8_t *data = cmd + 2;
        data_size &= ~DELAY;
        if (self->two_byte_sequence_length) {
            data_size = ((data_size & ~DELAY) << 8) + *(cmd + 2);
            data = cmd + 3;
        }
        bool have_bus;
        const uint64_t deadline = display_wait_deadline(self);
        while (!(have_bus = displayio_display_bus_begin_transaction(&self->bus)) &&
               !epaperdisplay_epaperdisplay_interrupted(self) && (deadline == 0 || supervisor_ticks_ms64() <= deadline)) {
            RUN_BACKGROUND_TASKS;
        }
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        if (!have_bus && self->lag.frames > 0) {
            return false;
        }
        self->lag.bus_held = have_bus;
        #endif // CIRCUITPY_EPAPERDISPLAY_FAST
        const bool sent = display_send_command(self, cmd, data, data_size);
        displayio_display_bus_end_transaction(&self->bus);
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        self->lag.bus_held = false;
        #endif // CIRCUITPY_EPAPERDISPLAY_FAST
        if (!sent) {
            return false;
        }
        uint16_t delay_length_ms = 0;
        if (delay) {
            data_size++;
            delay_length_ms = *(cmd + 1 + data_size + self->two_byte_sequence_length);
            if (delay_length_ms == 255) {
                delay_length_ms = 500;
            }
        }
        epaperdisplay_epaperdisplay_delay_ms(self, delay_length_ms);
        if (should_wait_for_busy) {
            #if CIRCUITPY_EPAPERDISPLAY_FAST
            if (self->lag.frames > 0) {
                epaperdisplay_epaperdisplay_wait_for_update_start(self);
            }
            #endif
            epaperdisplay_epaperdisplay_wait_for_busy(self);
        }
        #if !CIRCUITPY_EPAPERDISPLAY_FAST
        if (mp_hal_is_interrupted()) {
            return true;
        }
        #endif
        i += 2 + data_size;
        if (self->two_byte_sequence_length) {
            i++;
        }
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        if (epaperdisplay_epaperdisplay_interrupted(self)) {
            return i >= sequence_len;
        }
        #endif
    }
    return true;
}

void epaperdisplay_epaperdisplay_change_refresh_mode_parameters(epaperdisplay_epaperdisplay_obj_t *self,
    mp_buffer_info_t *start_sequence, mp_buffer_info_t *refresh_sequence, float seconds_per_frame) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->lag.frames > 0) {
        // Finish the drive with the waveform that started it, and rewrite both planes after.
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            epaperdisplay_lag_flush(self);
            epaperdisplay_epaperdisplay_wait_for_busy(self);
            nlr_pop();
        } else {
            epaperdisplay_lag_raise(self, nlr.ret_val);
        }
        if (self->lag.settle > 0 || self->lag.commit != LAG_COMMIT_NONE) {
            // Still owed: keep the mode that started it.
            if (!mp_hal_is_interrupted()) {
                mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_display_bus);
            }
            return;
        }
        self->lag.force_full = true;
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
    self->start_sequence = (uint8_t *)start_sequence->buf;
    self->start_sequence_len = start_sequence->len;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    // NULL keeps the current refresh sequence.
    if (refresh_sequence != NULL) {
        self->refresh_sequence = (uint8_t *)refresh_sequence->buf;
        self->refresh_sequence_len = refresh_sequence->len;
    }
    #endif
    self->milliseconds_per_frame = seconds_per_frame * 1000;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    self->needs_start = true;
    #endif
}

#if CIRCUITPY_EPAPERDISPLAY_FAST
// For a panel kept awake. Ctrl-C raises from a send in progress, and that transaction is still
// open. The caller still has things to do first, so the interrupt raises later.
static void send_stop_sequence_guarded(epaperdisplay_epaperdisplay_obj_t *self) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
        nlr_pop();
    } else {
        if (!displayio_display_bus_is_free(&self->bus)) {
            displayio_display_bus_end_transaction(&self->bus);
        }
        mp_sched_exception(nlr.ret_val);
    }
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

LAG_SHARED void epaperdisplay_epaperdisplay_start_refresh(epaperdisplay_epaperdisplay_obj_t *self) {
    if (!displayio_display_bus_is_free(&self->bus)) {
        // Can't acquire display bus; skip updating this display. Try next display.
        return;
    }
    if (!epaperdisplay_lvgl_start_refresh(self)) {
        // Refresh on this display already in progress.
        return;
    }

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->fast && self->awake) {
        if (!self->needs_start) {
            // The panel is still configured from the previous refresh.
            return;
        }
        epaperdisplay_epaperdisplay_wait_for_busy(self);
        // SSD168x controllers commit the last RAM byte when the next command arrives, so a reset
        // can't come straight after a RAM write.
        send_stop_sequence_guarded(self);
        self->awake = false;
    }
    #endif

    // run start sequence
    self->bus.bus_reset(self->bus.bus);

    epaperdisplay_epaperdisplay_delay_ms(self, self->start_up_time_ms);

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (!epaperdisplay_epaperdisplay_send_command_sequence(self, true, self->start_sequence, self->start_sequence_len) ||
        epaperdisplay_epaperdisplay_interrupted(self)) {
        return;
    }
    self->awake = true;
    self->needs_start = false;
    #else
    epaperdisplay_epaperdisplay_send_command_sequence(self, true, self->start_sequence, self->start_sequence_len);
    #endif
}

uint32_t common_hal_epaperdisplay_epaperdisplay_get_time_to_refresh(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->core.last_refresh == 0) {
        return 0;
    }
    // Refresh at seconds per frame rate.
    uint32_t elapsed_time = supervisor_ticks_ms64() - self->core.last_refresh;
    if (elapsed_time > self->milliseconds_per_frame) {
        return 0;
    }
    return self->milliseconds_per_frame - elapsed_time;
}

static bool epaperdisplay_epaperdisplay_refresh_area(epaperdisplay_epaperdisplay_obj_t *self,
    const displayio_area_t *area, bool write_previous);

#if CIRCUITPY_EPAPERDISPLAY_FAST
// False if the panel is still updating when the wait is over.
static bool wait_for_update_end(epaperdisplay_epaperdisplay_obj_t *self) {
    epaperdisplay_epaperdisplay_wait_for_update_start(self);
    const uint64_t deadline = supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
    bool busy;
    while ((busy = common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state) &&
           !mp_hal_is_interrupted() && supervisor_ticks_ms64() <= deadline) {
        RUN_BACKGROUND_TASKS;
        port_task_yield();
    }
    return !busy;
}

static void epaperdisplay_epaperdisplay_finish_refresh(epaperdisplay_epaperdisplay_obj_t *self,
    const displayio_area_t *areas, uint8_t area_count) {
    // Actually refresh the display now that all pixel RAM has been updated.
    epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->refresh_sequence, self->refresh_sequence_len);
    const uint64_t refresh_start = supervisor_ticks_ms64();
    bool previous_written = true;
    mp_obj_t raised = MP_OBJ_NULL;

    if (self->write_previous_ram_command != NO_COMMAND && area_count > 0) {
        // Copy the new image into the previous image RAM so the next partial update compares
        // against what is on screen. The controller ignores RAM writes while it is updating.
        if (self->busy.base.type == &mp_type_NoneType) {
            common_hal_time_delay_ms(self->refresh_time);
        } else {
            previous_written = wait_for_update_end(self);
        }
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            for (uint8_t i = 0; i < area_count && previous_written; i++) {
                previous_written = !mp_hal_is_interrupted() &&
                    epaperdisplay_epaperdisplay_refresh_area(self, &areas[i], true);
            }
            nlr_pop();
        } else {
            // Ctrl-C raises from a transfer in progress, and that transaction is still open.
            raised = nlr.ret_val;
            previous_written = false;
            if (!displayio_display_bus_is_free(&self->bus)) {
                displayio_display_bus_end_transaction(&self->bus);
            }
        }
    }

    supervisor_enable_tick();
    self->refreshing = true;

    epaperdisplay_lvgl_finish_refresh(self);
    // The update began before the wait above, and refresh_time and seconds_per_frame count from it.
    self->core.last_refresh = refresh_start;
    if (!previous_written) {
        // The previous RAM is stale, so the next update has to write everything.
        self->full_refresh = true;
    }
    if (raised != MP_OBJ_NULL) {
        nlr_jump(raised);
    }
}
#else
static void epaperdisplay_epaperdisplay_finish_refresh(epaperdisplay_epaperdisplay_obj_t *self,
    const displayio_area_t *areas, uint8_t area_count) {
    // Actually refresh the display now that all pixel RAM has been updated.
    epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->refresh_sequence, self->refresh_sequence_len);

    supervisor_enable_tick();
    self->refreshing = true;

    epaperdisplay_lvgl_finish_refresh(self);
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

mp_obj_t common_hal_epaperdisplay_epaperdisplay_get_bus(epaperdisplay_epaperdisplay_obj_t *self) {
    return self->bus.bus;
}

void common_hal_epaperdisplay_epaperdisplay_set_rotation(epaperdisplay_epaperdisplay_obj_t *self, int rotation) {
    // Only the logical dimensions change here. The canvas and LVGL's original resolution stay
    // in the controller's native RAM orientation established by the constructor.
    bool transposed = (self->core.rotation == 90 || self->core.rotation == 270);
    bool will_transposed = (rotation == 90 || rotation == 270);
    if (transposed != will_transposed) {
        int tmp = self->core.width;
        self->core.width = self->core.height;
        self->core.height = tmp;
    }
    displayio_display_core_set_rotation(&self->core, rotation);
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
        nlr_jump_callback_node_t cleanup;
        displayio_lvgl_lock_with_cleanup(&cleanup);
        lv_display_set_rotation(self->lv_display, lv_rotation);
        lv_display_set_default(self->lv_display);
        self->full_refresh = true;
        self->dirty_full = true;
        displayio_lvgl_unlock_with_cleanup();
    }
    if (self == &displays[0].epaper_display) {
        supervisor_stop_terminal();
        supervisor_start_terminal(self->core.width, self->core.height);
    }
    if (self->core.current_group != NULL) {
        nlr_jump_callback_node_t cleanup;
        displayio_lvgl_lock_with_cleanup(&cleanup);
        displayio_lvgl_set_root_group(self->core.current_group);
        displayio_lvgl_unlock_with_cleanup();
    }
}

uint16_t common_hal_epaperdisplay_epaperdisplay_get_rotation(epaperdisplay_epaperdisplay_obj_t *self) {
    return self->core.rotation;
}

mp_obj_t common_hal_epaperdisplay_epaperdisplay_get_root_group(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->core.current_group == NULL) {
        return mp_const_none;
    }
    return self->core.current_group;
}

// Send one packed plane from the converted framebuffer. The area must already be clipped to
// whole bytes of pixels.
static bool epaperdisplay_lvgl_send_plane(epaperdisplay_epaperdisplay_obj_t *self, uint8_t command,
    const uint8_t *buf, const displayio_area_t *area) {
    displayio_area_t region = *area;
    if (self->bus.row_command != NO_COMMAND) {
        displayio_display_bus_set_region_to_update(&self->bus, &self->core.colorspace, &region);
    }
    if (!displayio_display_bus_begin_transaction(&self->bus)) {
        return false;
    }
    self->bus.send(self->bus.bus, DISPLAY_COMMAND, self->chip_select, &command, 1);
    displayio_display_bus_end_transaction(&self->bus);

    uint16_t bytes_per_row = displayio_area_width(area) * self->canvas_depth / 8;
    uint16_t first_byte = area->x1 * self->canvas_depth / 8;
    uint8_t chunk[512];
    size_t used = 0;
    for (int16_t y = area->y1; y < area->y2; y++) {
        const uint8_t *row = buf + y * self->canvas_stride + first_byte;
        if (bytes_per_row > sizeof(chunk)) {
            if (!displayio_display_bus_begin_transaction(&self->bus)) {
                return false;
            }
            self->bus.send(self->bus.bus, DISPLAY_DATA, self->chip_select, row, bytes_per_row);
            displayio_display_bus_end_transaction(&self->bus);
        } else {
            if (used + bytes_per_row > sizeof(chunk)) {
                if (!displayio_display_bus_begin_transaction(&self->bus)) {
                    return false;
                }
                self->bus.send(self->bus.bus, DISPLAY_DATA, self->chip_select, chunk, used);
                displayio_display_bus_end_transaction(&self->bus);
                used = 0;
            }
            memcpy(chunk + used, row, bytes_per_row);
            used += bytes_per_row;
        }
        // Run background tasks so they can run during an explicit refresh.
        // Auto-refresh won't run background tasks here because it is a background task itself.
        RUN_BACKGROUND_TASKS;

        // Run USB background tasks so they can run during an implicit refresh.
        #if CIRCUITPY_TINYUSB
        usb_background();
        #endif
    }
    if (used > 0) {
        if (!displayio_display_bus_begin_transaction(&self->bus)) {
            return false;
        }
        self->bus.send(self->bus.bus, DISPLAY_DATA, self->chip_select, chunk, used);
        displayio_display_bus_end_transaction(&self->bus);
    }
    return true;
}

static bool epaperdisplay_epaperdisplay_refresh_area(epaperdisplay_epaperdisplay_obj_t *self,
    const displayio_area_t *area, bool write_previous) {
    displayio_area_t clipped;
    // Clip the area to the display by overlapping the areas. If there is no overlap then we're done.
    if (!epaperdisplay_lvgl_clip_area(self, area, &clipped)) {
        return true;
    }
    if (write_previous) {
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        if (self->write_previous_ram_command != NO_COMMAND) {
            return epaperdisplay_lvgl_send_plane(self, self->write_previous_ram_command,
                self->black_buf, &clipped);
        }
        #endif
        return true;
    }
    if (!epaperdisplay_lvgl_send_plane(self, self->write_black_ram_command, self->black_buf, &clipped)) {
        return false;
    }
    if (self->write_color_ram_command != NO_COMMAND && self->color_buf != NULL) {
        if (!epaperdisplay_lvgl_send_plane(self, self->write_color_ram_command, self->color_buf, &clipped)) {
            return false;
        }
    }
    return true;
}

static bool _clean_area(epaperdisplay_epaperdisplay_obj_t *self) {
    uint16_t width = self->canvas_width;
    uint16_t height = self->canvas_height;

    uint8_t buffer[width / 2];
    memset(buffer, 0x77, width / 2);

    uint8_t write_command = self->write_black_ram_command;
    if (!displayio_display_bus_begin_transaction(&self->bus)) {
        return false;
    }
    self->bus.send(self->bus.bus, DISPLAY_COMMAND, self->chip_select, &write_command, 1);
    displayio_display_bus_end_transaction(&self->bus);

    for (uint16_t j = 0; j < height; j++) {
        if (!displayio_display_bus_begin_transaction(&self->bus)) {
            // Can't acquire display bus; skip the rest of the data. Try next display.
            return false;
        }
        self->bus.send(self->bus.bus, DISPLAY_DATA, self->chip_select, buffer, width / 2);
        displayio_display_bus_end_transaction(&self->bus);

        // TODO(tannewt): Make refresh displays faster so we don't starve other
        // background tasks.
        #if CIRCUITPY_TINYUSB
        usb_background();
        #endif
    }

    return true;
}

bool common_hal_epaperdisplay_epaperdisplay_refresh(epaperdisplay_epaperdisplay_obj_t *self) {

    if (self->refreshing && self->busy.base.type == &digitalio_digitalinout_type) {
        if (common_hal_digitalio_digitalinout_get_value(&self->busy) != self->busy_state) {
            supervisor_disable_tick();
            self->refreshing = false;
            #if CIRCUITPY_EPAPERDISPLAY_FAST
            if (!self->fast) {
                // Run stop sequence but don't wait for busy because busy is set when sleeping.
                epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            }
            #else
            // Run stop sequence but don't wait for busy because busy is set when sleeping.
            epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            #endif
        } else {
            #if CIRCUITPY_EPAPERDISPLAY_FAST
            epaperdisplay_lag_give_up_if_stuck(self);
            #endif
            return false;
        }
    }
    if (self->core.current_group == NULL) {
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        // Nothing to draw, but the drive already started is still owed.
        if (self->lag.frames > 0 && self->awake) {
            nlr_jump_callback_node_t cleanup;
            displayio_lvgl_lock_with_cleanup(&cleanup);
            bool ok = epaperdisplay_lag_refresh(self, NULL, 0);
            displayio_lvgl_unlock_with_cleanup();
            return ok;
        }
        #endif
        return true;
    }
    // Refresh at seconds per frame rate.
    if (common_hal_epaperdisplay_epaperdisplay_get_time_to_refresh(self) > 0) {
        return false;
    }
    if (!displayio_display_bus_is_free(&self->bus)) {
        // Can't acquire display bus; skip updating this display. Try next display.
        return false;
    }

    // Hold the LVGL lock from the compose and flush that brings the converted framebuffer up to
    // date through the transfers that read it. The render task otherwise shares this display.
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);

    if (self->full_refresh && self->lv_display != NULL) {
        self->dirty_full = true;
        lv_obj_t *screen = lv_display_get_screen_active(self->lv_display);
        if (screen != NULL) {
            lv_obj_invalidate(screen);
        }
    }
    if (self->lv_display != NULL) {
        lv_refr_now(self->lv_display);
    }

    displayio_area_t areas[EPAPER_LVGL_MAX_DIRTY];
    uint8_t area_count = 0;
    if (self->dirty_full || self->bus.row_command == NO_COMMAND) {
        // Without a RAM window, a short transfer would start at the controller's current cursor
        // instead of the dirty area's coordinates, leaving a mixture of old and new frames.
        epaperdisplay_lvgl_full_area(self, &areas[0]);
        area_count = 1;
    } else if (self->dirty_count > 0) {
        memcpy(areas, self->dirty, self->dirty_count * sizeof(areas[0]));
        area_count = self->dirty_count;
    }

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->lag.frames > 0) {
        bool ok = epaperdisplay_lag_refresh(self, area_count > 0 ? areas : NULL, area_count);
        displayio_lvgl_unlock_with_cleanup();
        return ok;
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST

    if (area_count == 0) {
        displayio_lvgl_unlock_with_cleanup();
        return true;
    }

    if (self->acep) {
        epaperdisplay_epaperdisplay_start_refresh(self);
        _clean_area(self);
        epaperdisplay_epaperdisplay_finish_refresh(self, NULL, 0);
        while (self->refreshing && !mp_hal_is_interrupted()) {
            RUN_BACKGROUND_TASKS;
        }
    }
    if (mp_hal_is_interrupted()) {
        displayio_lvgl_unlock_with_cleanup();
        return false;
    }

    epaperdisplay_epaperdisplay_start_refresh(self);
    for (uint8_t i = 0; i < area_count; i++) {
        epaperdisplay_epaperdisplay_refresh_area(self, &areas[i], false);
    }
    epaperdisplay_epaperdisplay_finish_refresh(self, areas, area_count);
    displayio_lvgl_unlock_with_cleanup();
    return true;
}

void epaperdisplay_epaperdisplay_background(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->refreshing) {
        bool refresh_done = false;
        if (self->busy.base.type == &digitalio_digitalinout_type) {
            bool busy = common_hal_digitalio_digitalinout_get_value(&self->busy);
            refresh_done = busy != self->busy_state;
        } else {
            refresh_done = supervisor_ticks_ms64() - self->core.last_refresh > self->refresh_time;
        }
        if (refresh_done) {
            supervisor_disable_tick();
            self->refreshing = false;
            #if CIRCUITPY_EPAPERDISPLAY_FAST
            if (!self->fast) {
                // Run stop sequence but don't wait for busy because busy is set when sleeping.
                epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            }
            #else
            // Run stop sequence but don't wait for busy because busy is set when sleeping.
            epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            #endif
        }
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        else {
            epaperdisplay_lag_give_up_if_stuck(self);
        }
        #endif
    }
}

bool common_hal_epaperdisplay_epaperdisplay_get_busy(epaperdisplay_epaperdisplay_obj_t *self) {
    epaperdisplay_epaperdisplay_background(self);
    return self->refreshing;
}

void release_epaperdisplay(epaperdisplay_epaperdisplay_obj_t *self) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    epaperdisplay_lag_shutdown(self);
    epaperdisplay_lag_free(self);
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->refreshing) {
        epaperdisplay_epaperdisplay_wait_for_busy(self);
        supervisor_disable_tick();
        self->refreshing = false;
        // Run stop sequence but don't wait for busy because busy is set when sleeping.
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        if (self->fast) {
            send_stop_sequence_guarded(self);
        } else {
            epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
        }
        #else
        epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
        #endif
    }
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    else if (self->fast && self->awake) {
        send_stop_sequence_guarded(self);
    }
    self->awake = false;
    #endif
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    epaperdisplay_lag_release_interrupts(self);
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST

    if (self->lv_display != NULL) {
        nlr_jump_callback_node_t cleanup;
        displayio_lvgl_lock_with_cleanup(&cleanup);
        displayio_lvgl_set_root_group(NULL);
        lv_display_delete(self->lv_display);
        self->lv_display = NULL;
        displayio_lvgl_unlock_with_cleanup();
    }
    // These allocations are from the port heap and outlive the VM, so free them explicitly.
    port_free(self->lv_buf);
    port_free(self->lv_rotation_buf);
    self->lv_buf = NULL;
    self->lv_rotation_buf = NULL;
    self->lv_buf_size = 0;
    self->lv_rotation_buf_size = 0;
    port_free(self->black_buf);
    port_free(self->color_buf);
    self->black_buf = NULL;
    self->color_buf = NULL;
    self->dirty_count = 0;
    self->dirty_full = false;
    self->refresh_in_progress = false;
    release_display_core(&self->core);
    if (self->busy.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->busy);
    }
}

#if CIRCUITPY_EPAPERDISPLAY_FAST
void epaperdisplay_epaperdisplay_prepare_release(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag.frames == 0 || (!self->awake && !self->lag.state_lost)) {
        return;
    }
    const uint64_t deadline = supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
    while (!displayio_display_bus_is_free(&self->bus)) {
        mp_handle_pending(true);
        if (supervisor_ticks_ms64() > deadline) {
            mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_display_bus);
        }
        RUN_BACKGROUND_TASKS;
    }
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

#if CIRCUITPY_EPAPERDISPLAY_FAST
void epaperdisplay_epaperdisplay_end_held_bus(epaperdisplay_epaperdisplay_obj_t *self) {
    // The VM is over, so whatever holds the bus can never let it go: end it here.
    if (self->fast && !displayio_display_bus_is_free(&self->bus)) {
        displayio_display_bus_end_transaction(&self->bus);
        self->lag.bus_held = false;
    }
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

void epaperdisplay_epaperdisplay_reset(epaperdisplay_epaperdisplay_obj_t *self) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->lag.frames > 0) {
        // Only a primary display gets here; reset_displays() has released the others. The heap is
        // still here but the sequences won't be, and the display would otherwise persist to draw
        // the splash with them. Finish now and stay asleep. Its bus still holds the pins until
        // the next VM ends or release_displays().
        epaperdisplay_epaperdisplay_end_held_bus(self);
        release_epaperdisplay(self);
        self->base.type = &mp_type_NoneType;
        return;
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
    displayio_display_core_set_root_group(&self->core, &circuitpython_splash);
    if (self->lv_display != NULL) {
        nlr_jump_callback_node_t cleanup;
        displayio_lvgl_lock_with_cleanup(&cleanup);
        self->full_refresh = true;
        self->dirty_full = true;
        lv_display_set_default(self->lv_display);
        displayio_lvgl_set_root_group(&circuitpython_splash);
        lv_obj_t *screen = lv_display_get_screen_active(self->lv_display);
        if (screen != NULL) {
            lv_obj_invalidate(screen);
        }
        displayio_lvgl_unlock_with_cleanup();
    } else {
        self->full_refresh = true;
        self->dirty_full = true;
    }
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    // Let the panel sleep after the next refresh when code isn't running.
    if (self->fast && self->awake && !self->refreshing) {
        // Nothing can catch a raise from a send. A reload may be pending.
        epaperdisplay_epaperdisplay_end_held_bus(self);
        send_stop_sequence_guarded(self);
        self->awake = false;
    }
    self->fast = false;
    #endif
}

void epaperdisplay_epaperdisplay_collect_ptrs(epaperdisplay_epaperdisplay_obj_t *self) {
    displayio_display_core_collect_ptrs(&self->core);
    displayio_display_bus_collect_ptrs(&self->bus);
    gc_collect_ptr((void *)self->start_sequence);
    gc_collect_ptr((void *)self->stop_sequence);
    gc_collect_ptr((void *)self->refresh_sequence);
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    gc_collect_ptr((void *)self->clean_sequence);
    gc_collect_ptr(self->lag.held_exception);
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
}

size_t maybe_refresh_epaperdisplay(void) {
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].epaper_display.base.type != &epaperdisplay_epaperdisplay_type ||
            displays[i].epaper_display.core.current_group != &circuitpython_splash) {
            // Skip regular displays and those not showing the splash.
            continue;
        }
        epaperdisplay_epaperdisplay_obj_t *display = &displays[i].epaper_display;
        size_t time_to_refresh = common_hal_epaperdisplay_epaperdisplay_get_time_to_refresh(display);
        if (time_to_refresh > 0) {
            return time_to_refresh;
        }
        if (common_hal_epaperdisplay_epaperdisplay_refresh(display)) {
            return 0;
        }
        // If we could refresh but it failed, then we want to retry.
        return 1;
    }
    // Return 0 if no ePaper displays are available to pretend it was updated.
    return 0;
}
