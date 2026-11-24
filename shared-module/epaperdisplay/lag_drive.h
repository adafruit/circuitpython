// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/epaperdisplay/epaper_core.h"

#if CIRCUITPY_EPAPERDISPLAY_FAST
// Longer than one update or stored-waveform refresh takes. This bounds each wait for an update on
// a lag display, and the wait before writing the previous RAM. A stock display's waits have no bound.
#define EPAPER_WAIT_TIMEOUT_MS 30000

// What the newest lag step still has to send: nothing, its plane, or the update that shows it.
enum { LAG_COMMIT_NONE, LAG_COMMIT_PLANE, LAG_COMMIT_ACTIVATE };

// Allocates the frame ring outside the VM heap. Raises MemoryError if it can't.
void epaperdisplay_lag_construct(epaperdisplay_epaper_core_t *self, mp_obj_base_t *base, uint8_t lag_frames);
// Sends the rest of the drive owed. Returns early if interrupted or the panel is stuck.
void epaperdisplay_lag_flush(epaperdisplay_epaper_core_t *self);
// Frees the frame ring.
void epaperdisplay_lag_free(epaperdisplay_epaper_core_t *self);
// Marks a panel BUSY for longer than EPAPER_WAIT_TIMEOUT_MS as lost, for the next refresh to reset.
void epaperdisplay_lag_give_up_if_stuck(epaperdisplay_epaper_core_t *self);
// Abandons the step in progress, ends any hold on interrupts, then raises. An interrupt saved
// during the hold is raised instead of raised.
MP_NORETURN void epaperdisplay_lag_raise(epaperdisplay_epaper_core_t *self, mp_obj_t raised);
// Sends one lag step for areas, or only the owed drive when there are none. False if the step
// couldn't be sent: interrupted, the panel is stuck, or the start sequence changed. The areas
// stay marked and the drive stays owed.
// LVGL builds keep dirty areas in an array, because displayio_area_t is lv_area_t and has no list
// link there. Other builds ignore area_count. NULL must be paired with area_count == 0.
bool epaperdisplay_lag_refresh(epaperdisplay_epaper_core_t *self, const displayio_area_t *areas,
    uint8_t area_count);
// Ends a hold on interrupts and schedules any interrupt saved during it.
void epaperdisplay_lag_release_interrupts(epaperdisplay_epaper_core_t *self);
// Sends bytes on the bus. False only while interrupts are held and the transfer was interrupted.
bool epaperdisplay_lag_send(epaperdisplay_epaper_core_t *self, display_byte_type_t type,
    const uint8_t *data, uint32_t length);
// Leaves the panel white and balanced when the display goes away. Doesn't raise or allocate.
void epaperdisplay_lag_shutdown(epaperdisplay_epaper_core_t *self);
// Brings every pixel to white with balanced drive, optionally using the clean sequence first.
bool epaperdisplay_epaper_core_clean(epaperdisplay_epaper_core_t *self, uint8_t frames, bool full);
#endif // CIRCUITPY_EPAPERDISPLAY_FAST
