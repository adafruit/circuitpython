// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/epaperdisplay/EPaperDisplay.h"

// Shared by EPaperDisplay.c and lag_drive.c only.

// Sends a start, stop or refresh sequence. True if every command went out.
bool epaperdisplay_epaperdisplay_send_command_sequence(epaperdisplay_epaperdisplay_obj_t *self,
    bool should_wait_for_busy, const uint8_t *sequence, uint32_t sequence_len);
// Runs the start sequence. A fast display that is already awake skips it unless the start
// sequence changed.
void epaperdisplay_epaperdisplay_start_refresh(epaperdisplay_epaperdisplay_obj_t *self);
// Waits for BUSY to clear or an interrupt. On a lag display the wait is bounded.
void epaperdisplay_epaperdisplay_wait_for_busy(epaperdisplay_epaperdisplay_obj_t *self);

#if CIRCUITPY_EPAPERDISPLAY_FAST
// Longer than one update or stored-waveform refresh takes. This bounds each wait for an update on
// a lag display, and the wait before writing the previous RAM. A stock display's waits have no bound.
#define EPAPER_WAIT_TIMEOUT_MS 30000

// What the newest lag step still has to send: nothing, its plane, or the update that shows it.
enum { LAG_COMMIT_NONE, LAG_COMMIT_PLANE, LAG_COMMIT_ACTIVATE };

// Delays, but while interrupts are held an interrupt doesn't cut it short.
void epaperdisplay_epaperdisplay_delay_ms(epaperdisplay_epaperdisplay_obj_t *self, uint32_t delay_ms);
// True if interrupted. While interrupts are held it saves the interrupt for the release and
// returns false.
bool epaperdisplay_epaperdisplay_interrupted(epaperdisplay_epaperdisplay_obj_t *self);
// True once BUSY rises after an update command. False if it doesn't within 2 ms.
bool epaperdisplay_epaperdisplay_wait_for_update_start(epaperdisplay_epaperdisplay_obj_t *self);

// Allocates the frame ring outside the VM heap. Raises MemoryError if it can't.
void epaperdisplay_lag_construct(epaperdisplay_epaperdisplay_obj_t *self, uint8_t lag_frames);
// Sends the rest of the drive owed. Returns early if interrupted or the panel is stuck.
void epaperdisplay_lag_flush(epaperdisplay_epaperdisplay_obj_t *self);
// Frees the frame ring.
void epaperdisplay_lag_free(epaperdisplay_epaperdisplay_obj_t *self);
// Marks a panel BUSY for longer than EPAPER_WAIT_TIMEOUT_MS as lost, for the next refresh to reset.
void epaperdisplay_lag_give_up_if_stuck(epaperdisplay_epaperdisplay_obj_t *self);
// Abandons the step in progress, ends any hold on interrupts, then raises. An interrupt saved
// during the hold is raised instead of raised.
MP_NORETURN void epaperdisplay_lag_raise(epaperdisplay_epaperdisplay_obj_t *self, mp_obj_t raised);
// Sends one lag step for areas, or only the owed drive when areas is NULL. False if the step
// couldn't be sent: interrupted, the panel is stuck, or the start sequence changed. The areas
// stay marked and the drive stays owed.
bool epaperdisplay_lag_refresh(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas);
// Ends a hold on interrupts and schedules any interrupt saved during it.
void epaperdisplay_lag_release_interrupts(epaperdisplay_epaperdisplay_obj_t *self);
// Sends bytes on the bus. False only while interrupts are held and the transfer was interrupted.
bool epaperdisplay_lag_send(epaperdisplay_epaperdisplay_obj_t *self, display_byte_type_t type,
    const uint8_t *data, uint32_t length);
// Leaves the panel white and balanced when the display goes away. Doesn't raise or allocate.
void epaperdisplay_lag_shutdown(epaperdisplay_epaperdisplay_obj_t *self);
#endif // CIRCUITPY_EPAPERDISPLAY_FAST
