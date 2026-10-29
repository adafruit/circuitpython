// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/web_workflow/display_stream.h"

#if CIRCUITPY_WEB_WORKFLOW && CIRCUITPY_DISPLAYIO

#include <stdio.h>
#include <string.h>

#include "py/mperrno.h"
#include "shared-bindings/socketpool/Socket.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"
#include "supervisor/shared/web_workflow/websocket.h"
#include "supervisor/workflow.h"

#if CIRCUITPY_BUSDISPLAY
#include "shared-bindings/busdisplay/BusDisplay.h"
#endif
#if CIRCUITPY_FRAMEBUFFERIO
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#endif

// Room for one message: the websocket header, the rectangle header and its pixels.
#define MESSAGE_SIZE (4096)
#if CIRCUITPY_BUSDISPLAY
// Room for rows rendered from a display without a framebuffer, and the mask
// rendering needs for them. Pixels are a byte or more there.
#define RENDER_SIZE (2048)
#define RENDER_MASK_WORDS (RENDER_SIZE / 32 + 1)
#else
#define RENDER_SIZE (0)
#define RENDER_MASK_WORDS (0)
#endif
#define BUFFER_SIZE (MESSAGE_SIZE + RENDER_SIZE + RENDER_MASK_WORDS * sizeof(uint32_t))
#define WS_HEADER_SIZE (4)
#define RECT_HEADER_SIZE (10)
#define PAYLOAD_SIZE (MESSAGE_SIZE - WS_HEADER_SIZE)
#define MAX_ROWS (1024)
// How long one background pass may spend sending.
#define PASS_BUDGET_MS (8)
// How often the connection is checked. Each check can cost a command to the
// network co-processor, so it isn't done every pass.
#define POLL_MS (250)
// Passes that send start at least this far apart, so the stream takes at most
// PASS_BUDGET_MS of every PASS_INTERVAL_MS however fast the display changes or
// however slowly the client reads. Changes in between are sent together.
#define PASS_INTERVAL_MS (20)

#define ENCODING_RAW (0)
#define ENCODING_RLE (1)

static websocket_t ws;
// Set while a client is connected and the message buffer is allocated.
static bool streaming;
static uint8_t *message;
// The part of message still to send.
static size_t message_start;
static size_t message_end;
#if CIRCUITPY_BUSDISPLAY
// In the same allocation, after message.
static uint32_t *render_buffer;
static uint32_t *render_mask;
#endif

// The display being streamed and what the client was told about it. target_fb
// is set when it's read from a framebuffer, otherwise rows are rendered.
static displayio_display_core_t *target;
#if CIRCUITPY_FRAMEBUFFERIO
static framebufferio_framebufferdisplay_obj_t *target_fb;
#endif
static const char *format;
static uint16_t width;
static uint16_t height;
static uint8_t depth;
static void *pixels;
static bool hello_needed;
// When the connection was last checked.
static uint64_t last_poll_ms;
// When the last pass that sent started.
static uint64_t last_pass_ms;
// Ticks left until the next pass is requested, and whether the tick is enabled for it.
static volatile uint8_t pass_countdown;
static bool tick_enabled;

// Rows written since they were last sent, and the columns written in any of them.
static uint8_t dirty_rows[MAX_ROWS / 8];
static uint16_t dirty_x1;
static uint16_t dirty_x2;
// Where the search for dirty rows continues, so the top doesn't starve the bottom.
static uint16_t next_row;

void display_stream_init(void) {
    socketpool_socket_reset(&ws.socket);
}

static void _mark_dirty(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    for (uint16_t y = y1; y < y2 && y < MAX_ROWS; y++) {
        dirty_rows[y / 8] |= 1 << (y % 8);
    }
    dirty_x1 = MIN(dirty_x1, x1);
    dirty_x2 = MAX(dirty_x2, x2);
}

static void _mark_all_dirty(void) {
    _mark_dirty(0, 0, width, height);
}

// Some ports only run the web workflow when a socket has data, so the tick asks
// for the next pass, PASS_INTERVAL_MS after the last one.
static void _pass_later(void) {
    if (pass_countdown > 0) {
        return;
    }
    if (!tick_enabled) {
        supervisor_enable_tick();
        tick_enabled = true;
    }
    uint64_t since = supervisor_ticks_ms64() - last_pass_ms;
    pass_countdown = since >= PASS_INTERVAL_MS ? 1 : PASS_INTERVAL_MS - since;
}

static void _no_pass_needed(void) {
    pass_countdown = 0;
    if (tick_enabled) {
        supervisor_disable_tick();
        tick_enabled = false;
    }
}

void display_stream_tick(void) {
    if (pass_countdown > 0 && --pass_countdown == 0) {
        supervisor_workflow_request_background();
    }
}

static void _stop(void) {
    _no_pass_needed();
    if (!common_hal_socketpool_socket_get_closed(&ws.socket)) {
        common_hal_socketpool_socket_close(&ws.socket);
    }
    if (message != NULL) {
        port_free(message);
        message = NULL;
    }
    streaming = false;
    target = NULL;
    #if CIRCUITPY_FRAMEBUFFERIO
    target_fb = NULL;
    #endif
}

void display_stream_handoff(socketpool_socket_obj_t *socket) {
    if (streaming && message_start < message_end) {
        // The close frame can't go in the middle of a message, so finish it if the
        // socket takes it now. A client that stopped reading is closed without one
        // rather than holding everything up.
        int left = message_end - message_start;
        if (socketpool_socket_send(&ws.socket, message + message_start, left) < left) {
            common_hal_socketpool_socket_close(&ws.socket);
        }
    }
    if (message != NULL) {
        port_free(message);
        message = NULL;
    }
    streaming = false;
    websocket_take_socket(&ws, socket);
    uint32_t *buffer = port_malloc(BUFFER_SIZE, false);
    message = (uint8_t *)buffer;
    if (message == NULL) {
        _stop();
        return;
    }
    #if CIRCUITPY_BUSDISPLAY
    render_buffer = buffer + MESSAGE_SIZE / sizeof(uint32_t);
    render_mask = render_buffer + RENDER_SIZE / sizeof(uint32_t);
    #endif
    message_start = 0;
    message_end = 0;
    target = NULL;
    #if CIRCUITPY_FRAMEBUFFERIO
    target_fb = NULL;
    #endif
    format = NULL;
    hello_needed = true;
    last_poll_ms = supervisor_ticks_ms64();
    last_pass_ms = 0;
    streaming = true;
}

void display_stream_area_updated(displayio_display_core_t *core, const displayio_area_t *area) {
    if (!streaming) {
        return;
    }
    if (core == target) {
        _mark_dirty(area->x1, area->y1, area->x2, area->y2);
    }
    // Another display may be a new one to switch to.
    _pass_later();
}

static const char *_format_of(const _displayio_colorspace_t *colorspace) {
    if (colorspace->depth < 8 && !colorspace->pixels_in_byte_share_row) {
        return NULL;
    }
    switch (colorspace->depth) {
        case 32:
            return "bgrx8888";
        case 16:
            return colorspace->reverse_bytes_in_word ? "rgb565be" : "rgb565le";
        case 8:
            return colorspace->grayscale ? "gray8" : "rgb332";
        case 4:
            return colorspace->grayscale ? "gray4" : "rgbd4";
        case 2:
            return "gray2";
        case 1:
            return "gray1";
    }
    return NULL;
}

// Finds the display to stream and notices when it changes.
static void _check_display(void) {
    displayio_display_core_t *core = NULL;
    void *fb = NULL;
    void *new_pixels = NULL;
    for (size_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT && core == NULL; i++) {
        const mp_obj_type_t *type = displays[i].display_base.type;
        #if CIRCUITPY_FRAMEBUFFERIO
        if (type == &framebufferio_framebufferdisplay_type) {
            fb = &displays[i].framebuffer_display;
            core = &displays[i].framebuffer_display.core;
            new_pixels = displays[i].framebuffer_display.bufinfo.buf;
        }
        #endif
        #if CIRCUITPY_BUSDISPLAY
        if (type == &busdisplay_busdisplay_type) {
            core = &displays[i].display.core;
        }
        #endif
        (void)type;
    }
    const char *new_format = NULL;
    uint16_t new_width = 0;
    uint16_t new_height = 0;
    if (core != NULL) {
        // The core's area is the panel's size, whatever the rotation.
        new_width = core->area.x2;
        new_height = core->area.y2;
        new_format = _format_of(&core->colorspace);
        size_t row_bytes = (new_width * core->colorspace.depth + 7) / 8;
        if (new_height > MAX_ROWS || RECT_HEADER_SIZE + row_bytes + row_bytes / 128 + 1 > PAYLOAD_SIZE) {
            new_format = NULL;
        }
        if (fb != NULL ? new_pixels == NULL :
            core->colorspace.depth < 8 || row_bytes > RENDER_SIZE) {
            new_format = NULL;
        }
        if (new_format == NULL) {
            core = NULL;
            fb = NULL;
            new_pixels = NULL;
        }
    }
    if (core == target &&
        (core == NULL || (new_format == format && new_width == width && new_height == height &&
                          new_pixels == pixels))) {
        return;
    }
    target = core;
    #if CIRCUITPY_FRAMEBUFFERIO
    target_fb = fb;
    #endif
    format = new_format;
    width = new_width;
    height = new_height;
    depth = core != NULL ? core->colorspace.depth : 0;
    pixels = new_pixels;
    memset(dirty_rows, 0, sizeof(dirty_rows));
    dirty_x1 = UINT16_MAX;
    dirty_x2 = 0;
    next_row = 0;
    _mark_all_dirty();
    hello_needed = true;
}

static void _finish_message(uint8_t opcode, size_t payload_len) {
    uint8_t *payload = message + WS_HEADER_SIZE;
    if (payload_len <= 125) {
        message_start = WS_HEADER_SIZE - 2;
        message[message_start + 1] = payload_len;
    } else {
        message_start = 0;
        message[1] = 126;
        message[2] = payload_len >> 8;
        message[3] = payload_len & 0xff;
    }
    message[message_start] = 0x80 | opcode;
    message_end = payload + payload_len - message;
}

static void _build_hello(void) {
    const _displayio_colorspace_t *colorspace = target != NULL ? &target->colorspace : NULL;
    int len = snprintf((char *)message + WS_HEADER_SIZE, PAYLOAD_SIZE,
        "{\"width\": %d, \"height\": %d, \"format\": \"%s\", \"lsb_first\": %s}",
        width, height, format != NULL ? format : "none",
        colorspace != NULL && colorspace->reverse_pixels_in_byte ? "false" : "true");
    _finish_message(0x1, len);
    hello_needed = false;
}

// Run length encodes one row of units into out. Returns the length written.
static size_t _encode_row(const uint8_t *src, size_t units, size_t unit, uint8_t *out) {
    // A run of single bytes only saves space from three on.
    size_t min_run = unit == 1 ? 3 : 2;
    uint8_t *start = out;
    size_t i = 0;
    while (i < units) {
        size_t run = 1;
        while (i + run < units && run < 129 && memcmp(src + i * unit, src + (i + run) * unit, unit) == 0) {
            run++;
        }
        if (run >= min_run) {
            *out++ = 0x80 | (run - 2);
            memcpy(out, src + i * unit, unit);
            out += unit;
            i += run;
            continue;
        }
        size_t literal_start = i;
        size_t count = 0;
        while (i < units && count < 128) {
            size_t same = 1;
            while (same < min_run && i + same < units &&
                   memcmp(src + i * unit, src + (i + same) * unit, unit) == 0) {
                same++;
            }
            if (same >= min_run) {
                break;
            }
            i++;
            count++;
        }
        *out++ = count - 1;
        memcpy(out, src + literal_start * unit, count * unit);
        out += count * unit;
    }
    return out - start;
}

static bool _row_dirty(uint16_t y) {
    return dirty_rows[y / 8] & (1 << (y % 8));
}

// How many rows _read_rows can give at once.
static uint16_t _rows_at_once(size_t row_bytes) {
    #if CIRCUITPY_FRAMEBUFFERIO
    if (target_fb != NULL) {
        return UINT16_MAX;
    }
    #endif
    return RENDER_SIZE / row_bytes;
}

// Returns rows [y, y + count) of columns [x1, x2), each stride bytes after the
// last: straight from the framebuffer, or rendered from the display's group.
static const uint8_t *_read_rows(uint16_t y, uint16_t count, uint16_t x1, uint16_t x2, size_t *stride) {
    #if CIRCUITPY_FRAMEBUFFERIO
    if (target_fb != NULL) {
        *stride = target_fb->row_stride;
        return (const uint8_t *)pixels + target_fb->first_pixel_offset + y * *stride + x1 * depth / 8;
    }
    #endif
    #if CIRCUITPY_BUSDISPLAY
    displayio_area_t area = { .x1 = x1, .y1 = y, .x2 = x2, .y2 = y + count, .next = NULL };
    *stride = (x2 - x1) * depth / 8;
    memset(render_buffer, 0, count * *stride);
    memset(render_mask, 0, RENDER_MASK_WORDS * sizeof(uint32_t));
    displayio_display_core_fill_area(target, &area, render_mask, render_buffer);
    return (const uint8_t *)render_buffer;
    #else
    return NULL;
    #endif
}

// Builds a message from the next dirty rows, adding rows until it's full or
// end_ms. Returns false when there are none.
static bool _build_rect(uint64_t end_ms) {
    uint16_t y = next_row;
    uint16_t searched = 0;
    while (searched < height && !_row_dirty(y)) {
        y = y + 1 < height ? y + 1 : 0;
        searched++;
    }
    if (searched == height) {
        dirty_x1 = UINT16_MAX;
        dirty_x2 = 0;
        return false;
    }

    uint16_t x1 = MIN(dirty_x1, width);
    uint16_t x2 = MIN(dirty_x2, width);
    if (depth < 8) {
        // Whole bytes only.
        uint16_t per_byte = 8 / depth;
        x1 = x1 / per_byte * per_byte;
        x2 = MIN((x2 + per_byte - 1) / per_byte * per_byte, width);
    }
    if (x1 >= x2) {
        x1 = 0;
        x2 = width;
    }

    size_t unit = depth >= 8 ? depth / 8 : 1;
    size_t row_bytes = ((x2 - x1) * depth + 7) / 8;
    size_t units = row_bytes / unit;
    size_t worst_row = row_bytes + units / 128 + 1;
    uint16_t at_once = _rows_at_once(row_bytes);

    uint8_t *header = message + WS_HEADER_SIZE;
    uint8_t *data = header + RECT_HEADER_SIZE;
    size_t room = PAYLOAD_SIZE - RECT_HEADER_SIZE;
    size_t encoded = 0;
    uint16_t rows = 0;
    // Rendered rows can take a while, and a message of rows that encode small
    // holds many of them.
    while (y + rows < height && _row_dirty(y + rows) && encoded + worst_row <= room &&
           (rows == 0 || supervisor_ticks_ms64() < end_ms)) {
        // The dirty rows that are sure to fit, cleared before they are read so a
        // write after that marks them again.
        uint16_t count = 0;
        while (count < at_once && y + rows + count < height && _row_dirty(y + rows + count) &&
               encoded + (count + 1) * worst_row <= room) {
            uint16_t row = y + rows + count;
            dirty_rows[row / 8] &= ~(1 << (row % 8));
            count++;
        }
        size_t stride;
        const uint8_t *src = _read_rows(y + rows, count, x1, x2, &stride);
        for (uint16_t i = 0; i < count; i++) {
            encoded += _encode_row(src + i * stride, units, unit, data + encoded);
        }
        rows += count;
    }
    uint8_t encoding = ENCODING_RLE;
    if (encoded > rows * row_bytes) {
        // Noisy pixels: send them as they are.
        encoding = ENCODING_RAW;
        for (uint16_t done = 0; done < rows;) {
            uint16_t count = MIN(at_once, rows - done);
            size_t stride;
            const uint8_t *src = _read_rows(y + done, count, x1, x2, &stride);
            for (uint16_t i = 0; i < count; i++) {
                memcpy(data + (done + i) * row_bytes, src + i * stride, row_bytes);
            }
            done += count;
        }
        encoded = rows * row_bytes;
    }
    next_row = y + rows < height ? y + rows : 0;

    uint16_t w = x2 - x1;
    header[0] = 1;
    header[1] = encoding;
    header[2] = x1 & 0xff;
    header[3] = x1 >> 8;
    header[4] = y & 0xff;
    header[5] = y >> 8;
    header[6] = w & 0xff;
    header[7] = w >> 8;
    header[8] = rows & 0xff;
    header[9] = rows >> 8;
    _finish_message(0x2, RECT_HEADER_SIZE + encoded);
    return true;
}

static void _read_commands(void) {
    uint8_t c;
    while (websocket_read_payload_byte(&ws, &c)) {
        if (c == 'f' && target != NULL) {
            _mark_all_dirty();
        }
    }
}

void display_stream_background(void) {
    if (!streaming) {
        return;
    }
    // A CLOSE from the client closes the socket without asking the co-processor.
    if (common_hal_socketpool_socket_get_closed(&ws.socket)) {
        _stop();
        return;
    }
    uint64_t start = supervisor_ticks_ms64();
    if (start - last_poll_ms >= POLL_MS) {
        last_poll_ms = start;
        if (!common_hal_socketpool_socket_get_connected(&ws.socket)) {
            _stop();
            return;
        }
    }
    // Some ports wake the web workflow until a socket's data is read, so read
    // what the client sent every time. Control frames are answered with sends
    // of their own, so not in the middle of a message.
    if (message_start >= message_end) {
        _read_commands();
        if (!streaming || common_hal_socketpool_socket_get_closed(&ws.socket)) {
            _stop();
            return;
        }
    }
    if (start - last_pass_ms < PASS_INTERVAL_MS) {
        // Too soon after the last one, whatever woke the web workflow.
        _pass_later();
        return;
    }
    last_pass_ms = start;
    do {
        if (message_start < message_end) {
            int sent = socketpool_socket_send(&ws.socket, message + message_start, message_end - message_start);
            if (sent == -MP_EAGAIN) {
                break;
            }
            if (sent < 0) {
                _stop();
                return;
            }
            message_start += sent;
            continue;
        }
        _check_display();
        if (hello_needed) {
            _build_hello();
        } else if (target == NULL || !_build_rect(start + PASS_BUDGET_MS)) {
            break;
        }
    } while (supervisor_ticks_ms64() - start < PASS_BUDGET_MS);
    // Come back for what's left: the rest of a message the socket couldn't take
    // yet, or rows the budget ran out on.
    if (message_start < message_end || (target != NULL && dirty_x1 != UINT16_MAX)) {
        _pass_later();
    } else {
        _no_pass_needed();
    }
}

#endif
