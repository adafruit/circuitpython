// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mpconfig.h"

#if CIRCUITPY_WEB_WORKFLOW && CIRCUITPY_DISPLAYIO

#include "shared-bindings/socketpool/Socket.h"
#include "shared-module/displayio/area.h"
#include "shared-module/displayio/display_core.h"

// Streams the first display to one websocket client at /cp/display/. Framebuffer
// displays are read from their framebuffer. Bus displays have none, so changed
// rows are rendered again from the display's group.
//
// The client first gets a text message describing the display:
//   {"width": 320, "height": 240, "format": "rgb565le", "lsb_first": true}
// format is one of rgb565le, rgb565be, rgb332, bgrx8888, rgbd4, gray1, gray2,
// gray4, gray8, or none when there is no display to stream. lsb_first says where
// the leftmost pixel of a byte is when pixels are smaller than a byte. A new one
// is sent when the display changes.
//
// Then each binary message is a rectangle of the display in its own format:
//   uint8 type (1), uint8 encoding (0 raw, 1 run length), uint16 x, y, w, h
// (little endian), then the pixels row by row. Rows smaller than a byte are
// padded to whole bytes. With run length encoding the data is a series of
// control bytes in units of one pixel (one byte when pixels are smaller):
// c < 128 is followed by c + 1 units to copy, c >= 128 by one unit to repeat
// c - 126 times.
//
// The client may send the text "f" to have the whole display sent again.
// There is one client at a time: a new one takes over, and the old one is closed
// with code 4000 (WEBSOCKET_CLOSE_REPLACED).

void display_stream_init(void);
// Takes the socket of a websocket upgrade request for /cp/display/.
void display_stream_handoff(socketpool_socket_obj_t *socket);
void display_stream_background(void);
// Called every tick while the stream has the tick enabled.
void display_stream_tick(void);
// Called by a display after it refreshed area.
void display_stream_area_updated(displayio_display_core_t *core, const displayio_area_t *area);

#endif
