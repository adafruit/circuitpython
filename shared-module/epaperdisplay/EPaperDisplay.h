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
    uint16_t write_previous_ram_command;
    uint8_t hue;
    bool busy_state;
    bool black_bits_inverted;
    bool color_bits_inverted;
    bool refreshing;
    bool grayscale;
    bool acep;
    bool two_byte_sequence_length;
    bool keep_awake;
    bool awake;
    bool needs_start; // update_refresh_mode() changed the start sequence.
    display_chip_select_behavior_t chip_select;
    #if CIRCUITPY_EPAPERDISPLAY_LAG
    // Lag drive. Only used when lag_frames > 0. The ring has lag_frames slots: frames N-1 to
    // N-lag_frames.
    uint8_t **lag_slots;
    const uint8_t *clean_sequence;
    uint32_t clean_sequence_len;
    uint64_t lag_busy_deadline; // When not 0, stop waiting for BUSY or the bus after this tick.
    // What interrupted while interrupts are held. A send can raise one from the heap, so it is
    // marked in epaperdisplay_epaperdisplay_collect_ptrs().
    mp_obj_t lag_held_exception;
    bool lag_holding_interrupts;
    bool lag_bus_held; // Inside a transaction, so an exception must end it.
    bool lag_state_lost; // What the panel received is unknown, so it needs a stored-waveform refresh.
    bool lag_slot_overwriting; // The oldest frame in the ring is being replaced.
    bool lag_commit_drive; // The owed step has pixels to drive.
    uint8_t lag_commit; // What the newest step in the ring still has to send.
    displayio_area_t lag_commit_box;
    uint64_t lag_update_start; // When the running update was sent.
    // What each of the last lag_frames + 1 steps rendered, in pixels.
    displayio_area_t *lag_boxes;
    uint32_t lag_slot_size;
    uint16_t lag_stride;
    uint16_t lag_rows;
    uint8_t lag_frames;
    uint8_t lag_head; // Slot of the newest frame.
    uint8_t lag_box_index; // Entry in lag_boxes for this step.
    uint8_t lag_settle; // Steps owed until every plane pair matches the newest frame.
    bool lag_force_full; // Write everything on the next step.
    #endif
} epaperdisplay_epaperdisplay_obj_t;

void epaperdisplay_epaperdisplay_change_refresh_mode_parameters(epaperdisplay_epaperdisplay_obj_t *self,
    mp_buffer_info_t *start_sequence, mp_buffer_info_t *refresh_sequence, float seconds_per_frame);
void epaperdisplay_epaperdisplay_background(epaperdisplay_epaperdisplay_obj_t *self);
void epaperdisplay_epaperdisplay_reset(epaperdisplay_epaperdisplay_obj_t *self);
void release_epaperdisplay(epaperdisplay_epaperdisplay_obj_t *self);
// The VM is over: a keep_awake display ends whatever transaction still holds its bus, since its
// stop sequence would wait for the bus forever. Other displays are left alone. Does not raise.
void epaperdisplay_epaperdisplay_end_held_bus(epaperdisplay_epaperdisplay_obj_t *self);
size_t maybe_refresh_epaperdisplay(void);

void epaperdisplay_epaperdisplay_collect_ptrs(epaperdisplay_epaperdisplay_obj_t *self);
