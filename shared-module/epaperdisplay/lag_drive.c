// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/epaperdisplay/EPaperDisplay.h"

#if CIRCUITPY_EPAPERDISPLAY_FAST

#include "py/gc.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-module/epaperdisplay/lag_drive.h"
#include "supervisor/port_heap.h"
#include "supervisor/shared/tick.h"

#include <stdint.h>
#include <string.h>

// SSD168x only: NOP. These controllers keep the last RAM byte until the next command and lose it to
// a reset, so a RAM write ends with this. Lag drive is only for displays where it is harmless.
#define LAG_RAM_COMMIT_COMMAND 0x7f

static void lag_after_raise(epaperdisplay_epaperdisplay_obj_t *self);
static bool lag_clean(epaperdisplay_epaperdisplay_obj_t *self, uint8_t frames, bool full);

static bool lag_deadline_passed(epaperdisplay_epaperdisplay_obj_t *self) {
    return self->lag.busy_deadline != 0 && supervisor_ticks_ms64() > self->lag.busy_deadline;
}

static void lag_hold_interrupts(epaperdisplay_epaperdisplay_obj_t *self) {
    self->lag.holding_interrupts = true;
    if (self->lag.busy_deadline == 0) {
        self->lag.busy_deadline = supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
    }
    epaperdisplay_epaperdisplay_interrupted(self);
}

void epaperdisplay_lag_release_interrupts(epaperdisplay_epaperdisplay_obj_t *self) {
    if (!self->lag.holding_interrupts) {
        return;
    }
    self->lag.holding_interrupts = false;
    self->lag.busy_deadline = 0;
    if (self->lag.held_exception != MP_OBJ_NULL) {
        mp_sched_exception(self->lag.held_exception);
        self->lag.held_exception = MP_OBJ_NULL;
    }
}

// A transfer in progress raises when something interrupts. On a port with async SPI that can be
// any transfer, however short. While holding, that returns false: the transfer stopped partway,
// so the caller goes back to where the RAM or command can start again.
bool epaperdisplay_lag_send(epaperdisplay_epaperdisplay_obj_t *self, display_byte_type_t type,
    const uint8_t *data, uint32_t length) {
    if (!self->lag.holding_interrupts) {
        self->bus.send(self->bus.bus, type, self->chip_select, data, length);
        return true;
    }
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        self->bus.send(self->bus.bus, type, self->chip_select, data, length);
        nlr_pop();
        return true;
    }
    MP_STATE_THREAD(mp_pending_exception) = nlr.ret_val;
    epaperdisplay_epaperdisplay_interrupted(self);
    return false;
}

// set_region_to_update() has its own transaction, so a raise from it leaves the bus locked.
static bool lag_set_region(epaperdisplay_epaperdisplay_obj_t *self, displayio_area_t *box) {
    for (volatile uint8_t attempt = 0; attempt < 3; attempt++) {
        // It skips its commands if it can't get the bus, which would leave the old window.
        if (!displayio_display_bus_is_free(&self->bus)) {
            return false;
        }
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            self->lag.bus_held = true;
            displayio_display_bus_set_region_to_update(&self->bus, &self->core, box);
            self->lag.bus_held = false;
            nlr_pop();
            return true;
        }
        if (self->lag.bus_held) {
            if (!displayio_display_bus_is_free(&self->bus)) {
                displayio_display_bus_end_transaction(&self->bus);
            }
            self->lag.bus_held = false;
        }
        if (!self->lag.holding_interrupts) {
            nlr_jump(nlr.ret_val);
        }
        MP_STATE_THREAD(mp_pending_exception) = nlr.ret_val;
        epaperdisplay_epaperdisplay_interrupted(self);
    }
    return false;
}

// Lag drive. Plane 0x26 holds frame N-lag instead of N-1. Each step drives a pixel toward black
// where N is black and N-lag is white, toward white where N is white and N-lag is black, and not
// at all where they match.
//
// That keeps the drive balanced without counting. Each frame is used twice: pixels are driven
// toward it in the step where it is N, and away from it lag steps later, where it is N-lag. So a
// pixel that ends the way it started has been driven the same amount toward black and toward
// white. For example, lag 3 and a pixel that is black for 2 steps (both planes start white):
//
//   step        1  2  3  4  5  6
//   frame N     W  B  B  W  W  W
//   frame N-3   W  W  W  W  B  B
//   drive       -  B  B  -  W  W     2 toward black, 2 toward white
//
// A change that holds for lag steps or more is driven lag steps, so a bigger lag drives harder.
// When frames stop changing, lag more steps finish the drive. Each frame is rendered once into
// the ring and the planes are sent from there.
//
// The ring has lag slots, N-1 to N-lag, and N replaces N-lag in its slot. The box to send must
// be known before that, so it comes from the areas that were rendered, not from comparing frames.
// Frames N and N-lag can only differ inside the areas of the last lag steps. Sending the areas of
// the last lag + 1 steps also covers what the previous step left unequal, so the planes are
// equal outside the box: a byte there is either outside the last box too, so equal by induction,
// or was last written with frames that have not changed there since.

void epaperdisplay_lag_construct(epaperdisplay_epaperdisplay_obj_t *self, uint8_t lag_frames) {
    self->lag.stride = (self->core.area.x2 + 7) / 8;
    self->lag.rows = self->core.area.y2;
    self->lag.slot_size = self->lag.stride * self->lag.rows;
    self->lag.frames = lag_frames;
    self->lag.head = 0;
    self->lag.box_index = 0;
    self->lag.settle = 0;
    // The panel's RAM is unknown, so the first step writes everything.
    self->lag.force_full = true;
    size_t meta_size = lag_frames * sizeof(uint8_t *) + (lag_frames + 1) * sizeof(displayio_area_t);
    self->lag.slots = port_malloc(meta_size, false);
    bool ok = self->lag.slots != NULL;
    if (ok) {
        memset(self->lag.slots, 0, meta_size);
        self->lag.boxes = (displayio_area_t *)(self->lag.slots + lag_frames);
    }
    for (uint8_t i = 0; ok && i < lag_frames; i++) {
        self->lag.slots[i] = port_malloc(self->lag.slot_size, true);
        ok = self->lag.slots[i] != NULL;
    }
    if (!ok) {
        epaperdisplay_lag_free(self);
        self->base.type = &mp_type_NoneType;
        m_malloc_fail(self->lag.slot_size);
    }
    // History starts white, the state a cleared panel is in.
    for (uint8_t i = 0; i < lag_frames; i++) {
        memset(self->lag.slots[i], self->black_bits_inverted ? 0x00 : 0xff, self->lag.slot_size);
    }
}

void epaperdisplay_lag_free(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag.slots != NULL) {
        for (uint8_t i = 0; i < self->lag.frames; i++) {
            if (self->lag.slots[i] != NULL) {
                port_free(self->lag.slots[i]);
            }
        }
        port_free(self->lag.slots);
        self->lag.slots = NULL;
    }
    self->lag.frames = 0;
}

static void lag_render_area(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *area, uint8_t *frame, bool white) {
    displayio_area_t clipped;
    if (!displayio_display_core_clip_area(&self->core, area, &clipped)) {
        return;
    }
    if (white) {
        for (int16_t row = clipped.y1; row < clipped.y2; row++) {
            memset(frame + row * self->lag.stride + clipped.x1 / 8, self->black_bits_inverted ? 0x00 : 0xff,
                displayio_area_width(&clipped) / 8);
        }
        return;
    }
    uint16_t width = displayio_area_width(&clipped);
    uint16_t bytes_per_row = width / 8;
    uint16_t rows_per_buffer = MAX(1, 128 * 32 / width);
    size_t words = (rows_per_buffer * width + 31) / 32;
    uint32_t buffer[words];
    uint32_t mask[words];

    self->core.colorspace.grayscale = true;
    self->core.colorspace.grayscale_bit = 7;
    for (int16_t y = clipped.y1; y < clipped.y2; y += rows_per_buffer) {
        displayio_area_t subrectangle = {
            .x1 = clipped.x1,
            .y1 = y,
            .x2 = clipped.x2,
            .y2 = MIN(y + rows_per_buffer, clipped.y2),
        };
        memset(mask, 0, sizeof(mask));
        memset(buffer, 0, sizeof(buffer));
        displayio_display_core_fill_area(&self->core, &subrectangle, mask, buffer);
        if (self->black_bits_inverted) {
            for (size_t k = 0; k < words; k++) {
                buffer[k] = ~buffer[k];
            }
        }
        const uint8_t *in = (const uint8_t *)buffer;
        for (int16_t row = subrectangle.y1; row < subrectangle.y2; row++) {
            memcpy(frame + row * self->lag.stride + clipped.x1 / 8, in, bytes_per_row);
            in += bytes_per_row;
        }
        // Run background tasks like a regular refresh does.
        RUN_BACKGROUND_TASKS;
    }
}

static bool lag_send_ram_commit(epaperdisplay_epaperdisplay_obj_t *self) {
    const uint8_t command = LAG_RAM_COMMIT_COMMAND;
    return epaperdisplay_lag_send(self, DISPLAY_COMMAND, &command, 1);
}

// Rows are gathered so a narrow box doesn't cost one SPI transfer per row.
static bool lag_send_plane(epaperdisplay_epaperdisplay_obj_t *self, uint8_t command, const uint8_t *frame, const displayio_area_t *box) {
    if (!epaperdisplay_lag_send(self, DISPLAY_COMMAND, &command, 1)) {
        return false;
    }
    uint16_t bytes_per_row = displayio_area_width(box) / 8;
    const uint8_t *in = frame + box->y1 * self->lag.stride + box->x1 / 8;
    if (bytes_per_row == self->lag.stride) {
        return epaperdisplay_lag_send(self, DISPLAY_DATA, in, displayio_area_height(box) * bytes_per_row) && lag_send_ram_commit(self);
    }
    uint8_t chunk[512];
    size_t used = 0;
    for (int16_t y = box->y1; y < box->y2; y++) {
        if (used + bytes_per_row > sizeof(chunk)) {
            if (!epaperdisplay_lag_send(self, DISPLAY_DATA, chunk, used)) {
                return false;
            }
            used = 0;
        }
        memcpy(chunk + used, in, bytes_per_row);
        used += bytes_per_row;
        in += self->lag.stride;
    }
    return epaperdisplay_lag_send(self, DISPLAY_DATA, chunk, used) && lag_send_ram_commit(self);
}

// A send stopped by a held interrupt leaves the RAM pointer partway, so start over from the region.
static bool lag_write_plane(epaperdisplay_epaperdisplay_obj_t *self, uint8_t command, const uint8_t *frame, displayio_area_t *box) {
    for (uint8_t attempt = 0; attempt < 3; attempt++) {
        if (!lag_set_region(self, box) || !displayio_display_bus_begin_transaction(&self->bus)) {
            return false;
        }
        self->lag.bus_held = true;
        const bool sent = lag_send_plane(self, command, frame, box);
        displayio_display_bus_end_transaction(&self->bus);
        self->lag.bus_held = false;
        if (sent) {
            return true;
        }
    }
    return false;
}

// Finish a step whose frame is already in the ring: frame N to its plane, then the update. Each
// part stays owed until it has gone out, so a pixel is only counted as driven if it was. Interrupts
// wait, since a send they stop can't be told from one that arrived. False if the bus can't be had.
static bool lag_commit_step(epaperdisplay_epaperdisplay_obj_t *self) {
    const bool was_holding = self->lag.holding_interrupts;
    lag_hold_interrupts(self);
    bool ok = true;
    if (self->lag.commit == LAG_COMMIT_PLANE) {
        ok = lag_write_plane(self, self->write_black_ram_command, self->lag.slots[self->lag.head], &self->lag.commit_box);
        if (ok) {
            self->lag.commit = LAG_COMMIT_ACTIVATE;
        }
    }
    if (ok && self->lag.commit == LAG_COMMIT_ACTIVATE) {
        // Nothing can differ when the box is empty, so an update would only take time.
        if (self->lag.commit_drive) {
            ok = epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->refresh_sequence, self->refresh_sequence_len);
            if (ok) {
                epaperdisplay_epaperdisplay_wait_for_update_start(self);
                supervisor_enable_tick();
                self->refreshing = true;
                self->lag.update_start = supervisor_ticks_ms64();
            }
        }
        if (ok) {
            self->lag.commit = LAG_COMMIT_NONE;
        }
    }
    if (!was_holding) {
        epaperdisplay_lag_release_interrupts(self);
    }
    return ok;
}

// Send one step: frame N-lag to the previous plane, then frame N, rendered from areas (may be
// NULL) over its slot, or white in those areas without rendering, to the other. The history
// doesn't advance if the bus can't be had before the slot is touched, so a retry does the same
// step. Once it has, the step is owed and true is returned, whatever happens to the rest.
static bool lag_step(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas, bool white) {
    uint8_t lag = self->lag.frames;
    uint8_t next = (self->lag.head + 1) % lag;
    uint8_t *frame = self->lag.slots[next];
    displayio_area_t *boxes = self->lag.boxes;
    uint8_t index = self->lag.box_index;
    bool changed = areas != NULL;

    for (const displayio_area_t *area = areas; area != NULL; area = area->next) {
        displayio_area_t clipped;
        if (displayio_display_core_clip_area(&self->core, area, &clipped)) {
            displayio_area_union(&boxes[index], &clipped, &boxes[index]);
        }
    }
    // The oldest entry is lag steps back: only needed to make the planes equal again.
    uint8_t oldest = (index + 1) % (lag + 1);
    displayio_area_t drive_box = {0};
    for (uint8_t i = 0; i <= lag; i++) {
        if (i != oldest) {
            displayio_area_union(&drive_box, &boxes[i], &drive_box);
        }
    }
    displayio_area_t write_box;
    if (self->lag.force_full) {
        displayio_display_core_clip_area(&self->core, &self->core.area, &write_box);
    } else {
        displayio_area_union(&drive_box, &boxes[oldest], &write_box);
    }
    bool write = !displayio_area_empty(&write_box);

    if (write && !lag_write_plane(self, self->write_previous_ram_command, frame, &write_box)) {
        return false;
    }
    // N-1 and N-lag only differ inside the box.
    self->lag.slot_overwriting = true;
    if (write && next != self->lag.head) {
        const uint8_t *in = self->lag.slots[self->lag.head] + write_box.y1 * self->lag.stride + write_box.x1 / 8;
        uint8_t *out = frame + write_box.y1 * self->lag.stride + write_box.x1 / 8;
        uint16_t bytes_per_row = displayio_area_width(&write_box) / 8;
        for (int16_t y = write_box.y1; y < write_box.y2; y++) {
            memcpy(out, in, bytes_per_row);
            in += self->lag.stride;
            out += self->lag.stride;
        }
    }
    for (const displayio_area_t *area = areas; area != NULL; area = area->next) {
        lag_render_area(self, area, frame, white);
    }
    // The slot no longer holds N-lag, so from here the step has to finish or that drive is lost.
    self->lag.slot_overwriting = false;
    self->lag.head = next;
    self->lag.box_index = oldest;
    boxes[oldest] = (displayio_area_t) {0};
    if (changed) {
        self->lag.settle = lag;
    } else if (self->lag.settle > 0) {
        self->lag.settle--;
    }
    self->lag.force_full = false;
    self->lag.commit_box = write_box;
    self->lag.commit_drive = !displayio_area_empty(&drive_box);
    self->lag.commit = write ? LAG_COMMIT_PLANE : LAG_COMMIT_ACTIVATE;
    lag_commit_step(self);
    return true;
}

static bool lag_refresh_unguarded(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas) {
    if (!self->awake || self->lag.state_lost) {
        // The panel may still hold anything, including ghosts from a run that lost power.
        if (!lag_clean(self, 0, true) || mp_hal_is_interrupted()) {
            return false;
        }
        areas = &self->core.area;
    }
    if (self->lag.commit != LAG_COMMIT_NONE) {
        // The areas stay marked, so the next refresh draws them.
        return lag_commit_step(self);
    }
    if (areas == NULL && self->lag.settle == 0) {
        return true;
    }
    if (self->needs_start) {
        lag_hold_interrupts(self);
        epaperdisplay_epaperdisplay_start_refresh(self);
        epaperdisplay_lag_release_interrupts(self);
    } else {
        epaperdisplay_epaperdisplay_start_refresh(self);
    }
    bool ok = !self->needs_start && lag_step(self, areas, false);
    if (ok) {
        displayio_display_core_finish_refresh(&self->core);
    } else {
        self->core.refresh_in_progress = false;
    }
    return ok;
}

// An update takes far less than the limit, so a panel still BUSY is stuck, and only a reset can
// help. That has to wait for the next refresh or the shutdown, since the bus may be in use here.
// Until then a script polling busy can go on.
void epaperdisplay_lag_give_up_if_stuck(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag.frames > 0 && !self->lag.holding_interrupts &&
        supervisor_ticks_ms64() - self->lag.update_start > EPAPER_WAIT_TIMEOUT_MS) {
        supervisor_disable_tick();
        self->refreshing = false;
        self->awake = false;
        self->lag.state_lost = true;
    }
}

// False if the update is still running: interrupted, or the panel is stuck. RAM writes and a reset
// would land on that update, so nothing more can be sent.
static bool lag_wait_idle(epaperdisplay_epaperdisplay_obj_t *self) {
    epaperdisplay_epaperdisplay_wait_for_busy(self);
    return common_hal_digitalio_digitalinout_get_value(&self->busy) != self->busy_state;
}

// The deadline bounds each piece of work, not the whole drive, which can be 32 updates and a clean.
// Only called after progress, so a bus that is never free still runs out.
static void lag_renew_deadline(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag.busy_deadline != 0 && !self->lag.last_try) {
        self->lag.busy_deadline = supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
    }
}

// Give every changed pixel the rest of its drive so the panel is left DC balanced.
void epaperdisplay_lag_flush(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag.frames == 0 || !self->awake) {
        return;
    }
    while (self->lag.settle > 0 || self->lag.commit != LAG_COMMIT_NONE) {
        if (!lag_wait_idle(self) || epaperdisplay_epaperdisplay_interrupted(self)) {
            return;
        }
        if (self->refreshing) {
            supervisor_disable_tick();
            self->refreshing = false;
        }
        if (self->lag.commit != LAG_COMMIT_NONE ? !lag_commit_step(self) : !lag_step(self, NULL, false)) {
            return;
        }
        lag_renew_deadline(self);
    }
}

bool epaperdisplay_lag_refresh(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        bool ok = lag_refresh_unguarded(self, areas);
        nlr_pop();
        return ok;
    }
    epaperdisplay_lag_raise(self, nlr.ret_val);
}

// Fill a whole RAM plane with one color from a small buffer.
static bool lag_send_solid(epaperdisplay_epaperdisplay_obj_t *self, uint8_t command, bool white) {
    if (!epaperdisplay_lag_send(self, DISPLAY_COMMAND, &command, 1)) {
        return false;
    }
    uint8_t chunk[512];
    memset(chunk, white != self->black_bits_inverted ? 0xff : 0x00, sizeof(chunk));
    size_t remaining = self->lag.slot_size;
    while (remaining > 0) {
        size_t count = MIN(remaining, sizeof(chunk));
        if (!epaperdisplay_lag_send(self, DISPLAY_DATA, chunk, count)) {
            return false;
        }
        remaining -= count;
    }
    return lag_send_ram_commit(self);
}

static bool lag_write_solid(epaperdisplay_epaperdisplay_obj_t *self, uint8_t command, bool white) {
    displayio_area_t box;
    displayio_display_core_clip_area(&self->core, &self->core.area, &box);
    for (uint8_t attempt = 0; attempt < 3; attempt++) {
        if (!lag_set_region(self, &box) || !displayio_display_bus_begin_transaction(&self->bus)) {
            return false;
        }
        self->lag.bus_held = true;
        const bool sent = lag_send_solid(self, command, white);
        displayio_display_bus_end_transaction(&self->bus);
        self->lag.bus_held = false;
        if (sent) {
            return true;
        }
    }
    return false;
}

// Updates with the planes as they are. Background tasks run, so a video keeps decoding. Returns how
// many went out.
static uint8_t lag_wipe_phase(epaperdisplay_epaperdisplay_obj_t *self, uint8_t frames) {
    for (uint8_t i = 0; i < frames; i++) {
        lag_renew_deadline(self);
        if (!epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->refresh_sequence, self->refresh_sequence_len)) {
            return i;
        }
        epaperdisplay_epaperdisplay_wait_for_update_start(self);
        if (!lag_wait_idle(self)) {
            return i + 1; // A stuck panel: writes would be dropped, so nothing more can help.
        }
    }
    return frames;
}

// Both planes must be white. Every pixel gets frames toward black, then the same number toward white,
// and both planes are white again at the end. The caller holds interrupts, so the phases stay
// together. If that can't be done the drive is unknown, and only a stored-waveform refresh is safe.
static void lag_wipe(epaperdisplay_epaperdisplay_obj_t *self, uint8_t frames) {
    bool ok = lag_write_solid(self, self->write_black_ram_command, false);
    if (ok) {
        const uint8_t toward_black = lag_wipe_phase(self, frames);
        lag_renew_deadline(self);
        ok = toward_black == frames && lag_wait_idle(self) &&
            lag_write_solid(self, self->write_previous_ram_command, false) &&
            lag_write_solid(self, self->write_black_ram_command, true) &&
            lag_wipe_phase(self, frames) == frames &&
            lag_write_solid(self, self->write_previous_ram_command, true);
    }
    if (!ok) {
        self->lag.state_lost = true;
    }
}

// A normal step to white, then the owed steps. Everything changed so far gets all of its drive,
// and the ring and both planes end up white with nothing owed. False if interrupted first, with
// the drive still owed.
static bool lag_to_white(epaperdisplay_epaperdisplay_obj_t *self) {
    while (true) {
        if (!lag_wait_idle(self) || epaperdisplay_epaperdisplay_interrupted(self)) {
            return false;
        }
        if (self->refreshing) {
            supervisor_disable_tick();
            self->refreshing = false;
        }
        if (self->lag.commit == LAG_COMMIT_NONE) {
            break;
        }
        if (!lag_commit_step(self)) {
            return false;
        }
    }
    displayio_area_t whole = self->core.area;
    whole.next = NULL;
    if (!lag_step(self, &whole, true)) {
        return false;
    }
    lag_renew_deadline(self);
    epaperdisplay_lag_flush(self);
    if (self->lag.settle > 0 || self->lag.commit != LAG_COMMIT_NONE || !lag_wait_idle(self)) {
        return false;
    }
    if (self->refreshing) {
        supervisor_disable_tick();
        self->refreshing = false;
    }
    return true;
}

// An exception came out of a bus send (Ctrl-C during a long transfer raises) or a render. Without
// this the bus stays locked and chip select low, and everything after waits for it. The next
// step writes all of both planes, since this one may have stopped partway through either.
static void lag_abandon_step(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag.bus_held) {
        displayio_display_bus_end_transaction(&self->bus);
        self->lag.bus_held = false;
    }
    self->core.refresh_in_progress = false;
    self->lag.force_full = true;
    if (self->lag.slot_overwriting) {
        // The drive it held can't be sent any more.
        self->lag.slot_overwriting = false;
        self->lag.state_lost = true;
    }
}

static void lag_after_raise(epaperdisplay_epaperdisplay_obj_t *self) {
    lag_abandon_step(self);
    epaperdisplay_lag_release_interrupts(self);
}

// Raise once: what was held, if anything, instead of both that and this.
MP_NORETURN void epaperdisplay_lag_raise(epaperdisplay_epaperdisplay_obj_t *self, mp_obj_t raised) {
    if (self->lag.held_exception != MP_OBJ_NULL) {
        raised = self->lag.held_exception;
        self->lag.held_exception = MP_OBJ_NULL;
    }
    lag_after_raise(self);
    nlr_jump(raised);
}

static bool lag_wake(epaperdisplay_epaperdisplay_obj_t *self) {
    self->awake = false;
    self->bus.bus_reset(self->bus.bus);
    epaperdisplay_epaperdisplay_delay_ms(self, self->start_up_time_ms);
    if (!epaperdisplay_epaperdisplay_send_command_sequence(self, true, self->start_sequence, self->start_sequence_len)) {
        return false;
    }
    self->awake = true;
    self->needs_start = false;
    return true;
}

// What the panel received is unknown, so paying back could drive pixels the wrong way. Forget the
// drive and let the stored waveform, which is balanced by itself, bring the panel to white.
static bool lag_recover(epaperdisplay_epaperdisplay_obj_t *self) {
    // An update that may have started shouldn't be cut. A stuck panel has already been waited for.
    if (self->awake) {
        epaperdisplay_epaperdisplay_wait_for_busy(self);
    }
    if (!lag_wake(self) || !lag_wait_idle(self)) {
        return false;
    }
    lag_renew_deadline(self);
    if (!lag_write_solid(self, self->write_previous_ram_command, true) ||
        !lag_write_solid(self, self->write_black_ram_command, true) ||
        !epaperdisplay_epaperdisplay_send_command_sequence(self, true, self->clean_sequence, self->clean_sequence_len) ||
        !lag_wait_idle(self)) {
        return false;
    }
    lag_renew_deadline(self);
    if (!lag_wake(self)) {
        return false;
    }
    for (uint8_t i = 0; i < self->lag.frames; i++) {
        memset(self->lag.slots[i], self->black_bits_inverted ? 0x00 : 0xff, self->lag.slot_size);
    }
    memset(self->lag.boxes, 0, (self->lag.frames + 1) * sizeof(displayio_area_t));
    self->lag.settle = 0;
    self->lag.commit = LAG_COMMIT_NONE;
    self->lag.force_full = false;
    self->lag.state_lost = false;
    return true;
}

// A clean can't stop partway, so an interrupt raises when it ends. False if the bus can't be had.
static bool lag_clean(epaperdisplay_epaperdisplay_obj_t *self, uint8_t frames, bool full) {
    if (!displayio_display_bus_is_free(&self->bus)) {
        return false;
    }
    lag_hold_interrupts(self);
    self->core.full_refresh = true;
    bool ok;
    if (self->lag.state_lost) {
        ok = lag_recover(self);
    } else {
        ok = self->awake || lag_wake(self);
        ok = ok && lag_to_white(self);
        if (ok && full) {
            lag_renew_deadline(self);
            self->lag.force_full = true;
            ok = epaperdisplay_epaperdisplay_send_command_sequence(self, true, self->clean_sequence, self->clean_sequence_len) && lag_wait_idle(self);
            if (ok) {
                lag_renew_deadline(self);
                ok = lag_wake(self);
                lag_renew_deadline(self);
            }
        }
    }
    if (ok && frames > 0) {
        lag_wipe(self, frames);
    }
    epaperdisplay_lag_release_interrupts(self);
    return ok;
}

bool common_hal_epaperdisplay_epaperdisplay_clean(epaperdisplay_epaperdisplay_obj_t *self, uint8_t frames, bool full) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        bool ok = lag_clean(self, frames, full);
        nlr_pop();
        return ok;
    }
    epaperdisplay_lag_raise(self, nlr.ret_val);
}

// Leave the panel white and balanced when the display goes away: release, or the end of the VM,
// where nothing can catch an exception. Interrupts stay held until release_epaperdisplay() has sent
// the stop sequence. All of it is bounded so a stuck panel can't hang shutdown. Doesn't allocate.
void epaperdisplay_lag_shutdown(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag.frames == 0 || (!self->awake && !self->lag.state_lost)) {
        return; // Nothing was drawn.
    }
    lag_after_raise(self); // Anything still held from before is ours to end.
    lag_hold_interrupts(self);
    volatile bool done = false;
    // Each raise abandons the step and the loop starts it again, up to 4 times.
    volatile uint8_t raises = 0;
    while (!done && !lag_deadline_passed(self) && raises < 4) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            if (self->lag.state_lost) {
                done = lag_recover(self);
            } else if (lag_to_white(self)) {
                lag_renew_deadline(self);
                done = epaperdisplay_epaperdisplay_send_command_sequence(self, true, self->clean_sequence, self->clean_sequence_len);
            }
            nlr_pop();
            RUN_BACKGROUND_TASKS; // Whatever holds the bus may let it go.
        } else {
            lag_abandon_step(self);
            raises++;
            if (self->lag.held_exception == MP_OBJ_NULL) {
                self->lag.held_exception = nlr.ret_val;
            }
        }
    }
    epaperdisplay_epaperdisplay_wait_for_busy(self);
    if (self->refreshing) {
        supervisor_disable_tick();
        self->refreshing = false;
    }
    // BUSY outlasted the wait. One reset may free the panel for the stored waveform. If BUSY stays
    // high after it, nothing more is sent, and the deadline isn't renewed, so this is the last wait.
    if (!done && raises < 4 && common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state) {
        self->lag.busy_deadline = supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
        self->lag.last_try = true;
        self->awake = false;
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            self->bus.bus_reset(self->bus.bus);
            epaperdisplay_epaperdisplay_delay_ms(self, self->start_up_time_ms);
            if (lag_wait_idle(self)) {
                lag_recover(self);
            }
            nlr_pop();
        } else {
            lag_abandon_step(self);
            if (self->lag.held_exception == MP_OBJ_NULL) {
                self->lag.held_exception = nlr.ret_val;
            }
        }
        self->lag.last_try = false;
    }
}

#endif // CIRCUITPY_EPAPERDISPLAY_FAST
