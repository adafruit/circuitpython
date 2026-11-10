// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/displayio/Group.h"

#include "shared-module/displayio/area.h"
#include "shared-module/displayio/bus_core.h"
#include "shared-module/displayio/display_core.h"
#include "common-hal/digitalio/DigitalInOut.h"

#if CIRCUITPY_EPAPERDISPLAY_FAST
// Lag drive, built on FAST's write_previous_ram_command. FAST writes the last frame to the
// previous RAM, so each changed pixel is driven once. Lag writes the frame from frames
// refreshes ago, so a changed pixel is driven frames times, the same number each way.
// The ring holds those frames. Only used when frames > 0.
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
#endif

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
#endif
size_t maybe_refresh_epaperdisplay(void);

void epaperdisplay_epaperdisplay_collect_ptrs(epaperdisplay_epaperdisplay_obj_t *self);
