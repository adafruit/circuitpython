// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// The EPaperDisplay implementation has two backends selected at build time with
// CIRCUITPY_DISPLAYIO_LVGL:
//
//   legacy/ - renders with the scanline compositor through display_core.
//   lvgl/   - renders with LVGL, then converts the composed RGB pixels into the
//             packed pixel planes the panel understands. The converted planes are
//             kept in a framebuffer of their own so partial updates and the fast
//             lag drive do not have to read pixels back from the panel.
#if CIRCUITPY_DISPLAYIO_LVGL
#include "shared-module/epaperdisplay/lvgl/EPaperDisplay.h"
#else
#include "shared-module/epaperdisplay/legacy/EPaperDisplay.h"
#endif

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
