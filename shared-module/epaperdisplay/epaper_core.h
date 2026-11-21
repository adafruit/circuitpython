// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-module/displayio/area.h"
#include "shared-module/displayio/bus_core.h"
#include "shared-module/displayio/display_core.h"

// Defined by the selected implementation; only the pointer is needed by the common core.
typedef struct epaperdisplay_epaperdisplay_obj_t epaperdisplay_epaperdisplay_obj_t;
typedef struct epaperdisplay_construct_args_t epaperdisplay_construct_args_t;

#if CIRCUITPY_EPAPERDISPLAY_FAST
// Lag drive state shared by both backends. FAST writes the last frame to the previous RAM, so each
// changed pixel is driven once. Lag writes the frame from frames refreshes ago, so a changed pixel
// is driven frames times, the same number each way. The ring is only used when frames > 0.
typedef struct {
    uint8_t **slots;
    uint64_t busy_deadline; // When not 0, stop waiting for BUSY or the bus after this tick.
    // What interrupted while interrupts are held. A send can raise one from the heap, so it is
    // marked in epaperdisplay_epaper_core_collect_ptrs().
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
#endif

// State and operations shared by the legacy and LVGL EPaperDisplay implementations. Keeping this
// separate from epaperdisplay_epaperdisplay_obj_t lets the common code and lag drive operate
// without knowing either implementation's object layout.
typedef struct {
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

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    const uint8_t *clean_sequence;
    uint32_t clean_sequence_len;
    epaperdisplay_lag_t lag;
    epaperdisplay_epaperdisplay_obj_t *display;
    #endif
} epaperdisplay_epaper_core_t;

void epaperdisplay_epaper_core_construct(epaperdisplay_epaper_core_t *self,
    const epaperdisplay_construct_args_t *args);

uint16_t epaperdisplay_epaper_core_get_width(epaperdisplay_epaper_core_t *self);
uint16_t epaperdisplay_epaper_core_get_height(epaperdisplay_epaper_core_t *self);
uint16_t epaperdisplay_epaper_core_get_rotation(epaperdisplay_epaper_core_t *self);
mp_obj_t epaperdisplay_epaper_core_get_root_group(epaperdisplay_epaper_core_t *self);
mp_obj_t epaperdisplay_epaper_core_get_bus(epaperdisplay_epaper_core_t *self);
uint32_t epaperdisplay_epaper_core_get_time_to_refresh(epaperdisplay_epaper_core_t *self);
bool epaperdisplay_epaper_core_get_busy(epaperdisplay_epaper_core_t *self);
void epaperdisplay_epaper_core_background(epaperdisplay_epaper_core_t *self);
void epaperdisplay_epaper_core_change_refresh_mode_parameters(epaperdisplay_epaper_core_t *self,
    mp_buffer_info_t *start_sequence, mp_buffer_info_t *refresh_sequence, float seconds_per_frame);
void epaperdisplay_epaper_core_collect_ptrs(epaperdisplay_epaper_core_t *self);

// Waits for BUSY to clear or an interrupt. On a lag display the wait is bounded.
void epaperdisplay_epaper_core_wait_for_busy(epaperdisplay_epaper_core_t *self);
// Sends a start, stop or refresh sequence. True if every command went out.
bool epaperdisplay_epaper_core_send_command_sequence(epaperdisplay_epaper_core_t *self,
    bool should_wait_for_busy, const uint8_t *sequence, uint32_t sequence_len);
// Delays, but while interrupts are held an interrupt doesn't cut it short.
void epaperdisplay_epaper_core_delay_ms(epaperdisplay_epaper_core_t *self, uint32_t delay_ms);

#if CIRCUITPY_EPAPERDISPLAY_FAST
void epaperdisplay_epaper_core_prepare_release(epaperdisplay_epaper_core_t *self);
void epaperdisplay_epaper_core_end_held_bus(epaperdisplay_epaper_core_t *self);
void epaperdisplay_epaper_core_send_stop_sequence_guarded(epaperdisplay_epaper_core_t *self);
bool epaperdisplay_epaper_core_wait_for_update_end(epaperdisplay_epaper_core_t *self);
bool epaperdisplay_epaper_core_interrupted(epaperdisplay_epaper_core_t *self);
bool epaperdisplay_epaper_core_wait_for_update_start(epaperdisplay_epaper_core_t *self);
// Backend operations needed by lag drive. Implementations can use display to reach their own state.
void epaperdisplay_backend_full_area(epaperdisplay_epaper_core_t *self, displayio_area_t *area);
bool epaperdisplay_backend_clip_area(epaperdisplay_epaper_core_t *self, const displayio_area_t *area,
    displayio_area_t *clipped);
void epaperdisplay_backend_render_area(epaperdisplay_epaper_core_t *self, const displayio_area_t *area,
    uint8_t *frame, uint16_t frame_stride, bool white);
void epaperdisplay_backend_finish_refresh(epaperdisplay_epaper_core_t *self);
void epaperdisplay_backend_set_full_refresh(epaperdisplay_epaper_core_t *self, bool full_refresh);
void epaperdisplay_backend_set_refresh_in_progress(epaperdisplay_epaper_core_t *self, bool in_progress);
// Implemented by the selected backend. A fast display that is already awake skips the start
// sequence unless it changed.
void epaperdisplay_epaper_core_start_refresh(epaperdisplay_epaper_core_t *self);
#endif
