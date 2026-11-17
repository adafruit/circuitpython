// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/displayio/Group.h"

#include "shared-module/displayio/area.h"
#include "shared-module/displayio/bus_core.h"
#include "shared-module/displayio/display_core.h"
#include "common-hal/digitalio/DigitalInOut.h"

#include "lvgl.h"

// Dirty rectangles kept since the last panel refresh. When more than this arrive they are
// merged into one bounding box, which is still correct because every one is inside it.
#define EPAPER_LVGL_MAX_DIRTY 8

#if CIRCUITPY_EPAPERDISPLAY_FAST
// Lag drive state. This is the same algorithm as the legacy backend, but frames are copied
// out of the converted framebuffer instead of being rendered from the displayio group tree.
typedef struct {
    uint8_t **slots;
    uint64_t busy_deadline; // When not 0, stop waiting for BUSY or the bus after this tick.
    // What interrupted while interrupts are held. A send can raise one from the heap, so it is
    // marked in epaperdisplay_epaperdisplay_collect_ptrs().
    mp_obj_t held_exception;
    bool holding_interrupts;
    bool bus_held; // Inside a transaction, so an exception must end it.
    bool last_try; // Shutdown's reset after a stuck wait: the deadline is not renewed.
    bool state_lost; // What the panel received is unknown, so it needs a stored-waveform refresh.
    bool slot_overwriting; // The oldest frame in the ring is being replaced.
    bool commit_drive; // The owed step has pixels to drive.
    uint8_t commit; // What the newest step in the ring still has to send.
    displayio_area_t commit_box;
    uint64_t update_start; // When the running update was sent.
    // What each of the last frames + 1 steps rendered, in pixels.
    displayio_area_t *boxes;
    uint32_t slot_size;
    uint16_t stride;
    uint16_t rows;
    uint8_t frames;
    uint8_t head; // Slot of the newest frame.
    uint8_t box_index; // Entry in boxes for this step.
    uint8_t settle; // Steps owed until every plane pair matches the newest frame.
    bool force_full; // Write everything on the next step.
} epaperdisplay_lag_t;
#endif  // CIRCUITPY_EPAPERDISPLAY_FAST

typedef struct {
    mp_obj_base_t base;
    displayio_display_core_t core;
    displayio_display_bus_t bus;
    digitalio_digitalinout_obj_t busy;
    uint32_t milliseconds_per_frame;
    const uint8_t *start_sequence;
    const uint8_t *stop_sequence;
    const uint8_t *refresh_sequence;
    uint16_t start_sequence_len;
    uint16_t stop_sequence_len;
    uint16_t refresh_sequence_len;
    uint16_t start_up_time_ms;
    uint16_t refresh_time;
    uint16_t write_black_ram_command;
    uint16_t write_color_ram_command;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    uint16_t write_previous_ram_command;
    #endif
    uint8_t hue;
    bool busy_state;
    bool black_bits_inverted;
    bool color_bits_inverted;
    bool refreshing;
    bool grayscale;
    bool acep;
    bool two_byte_sequence_length;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    bool fast;
    bool awake;
    bool needs_start; // update_refresh_mode() changed the start sequence.
    #endif
    display_chip_select_behavior_t chip_select;

    // LVGL display. The draw buffer is RGB565; the flush callback converts it into the
    // packed panel planes below.
    lv_display_t *lv_display;
    lv_color_format_t lv_color_format;
    uint8_t *lv_buf;
    size_t lv_buf_size;
    uint8_t *lv_rotation_buf;
    size_t lv_rotation_buf_size;

    // The converted framebuffer. Each byte holds (8 / canvas_depth) panel pixels, most
    // significant pixel first, exactly as the panel RAM does. black_buf always exists.
    // color_buf exists when the panel has a separate color RAM.
    uint8_t *black_buf;
    uint8_t *color_buf;
    uint16_t canvas_stride;
    uint16_t canvas_width; // Physical panel width, not the rotated display width.
    uint16_t canvas_height;
    uint8_t canvas_depth;

    // Areas the flush callback changed since the last panel refresh.
    displayio_area_t dirty[EPAPER_LVGL_MAX_DIRTY];
    uint8_t dirty_count;
    bool dirty_full;
    bool refresh_in_progress;
    bool full_refresh;

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    const uint8_t *clean_sequence;
    uint32_t clean_sequence_len;
    epaperdisplay_lag_t lag;
    #endif
} epaperdisplay_epaperdisplay_obj_t;

void epaperdisplay_epaperdisplay_change_refresh_mode_parameters(epaperdisplay_epaperdisplay_obj_t *self,
    mp_buffer_info_t *start_sequence, mp_buffer_info_t *refresh_sequence, float seconds_per_frame);
void epaperdisplay_epaperdisplay_background(epaperdisplay_epaperdisplay_obj_t *self);
void epaperdisplay_epaperdisplay_reset(epaperdisplay_epaperdisplay_obj_t *self);
void release_epaperdisplay(epaperdisplay_epaperdisplay_obj_t *self);
#if CIRCUITPY_EPAPERDISPLAY_FAST
// The VM is over: a fast display ends whatever transaction still holds its bus, since its
// stop sequence would wait for the bus forever. Other displays are left alone. Does not raise.
void epaperdisplay_epaperdisplay_end_held_bus(epaperdisplay_epaperdisplay_obj_t *self);
// Waits up to 30 s for the bus so a release can finish the drive. Raises what interrupted it, or
// RuntimeError if the bus stays held, with the display and its drive unchanged. Only call it where
// an exception can be caught. Doesn't allocate.
void epaperdisplay_epaperdisplay_prepare_release(epaperdisplay_epaperdisplay_obj_t *self);
#endif  // CIRCUITPY_EPAPERDISPLAY_FAST
size_t maybe_refresh_epaperdisplay(void);

void epaperdisplay_epaperdisplay_collect_ptrs(epaperdisplay_epaperdisplay_obj_t *self);

// LVGL backend helpers shared with lag_drive.c. All operate on physical panel coordinates.
bool epaperdisplay_lvgl_start_refresh(epaperdisplay_epaperdisplay_obj_t *self);
void epaperdisplay_lvgl_finish_refresh(epaperdisplay_epaperdisplay_obj_t *self);
void epaperdisplay_lvgl_full_area(epaperdisplay_epaperdisplay_obj_t *self, displayio_area_t *area);
bool epaperdisplay_lvgl_clip_area(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *area,
    displayio_area_t *clipped);
