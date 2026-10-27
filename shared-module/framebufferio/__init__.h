// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

// Optional frame-boundary callback. The framebuffer invokes it once per frame from a
// task/background context, never an interrupt handler. A display uses it to pace rendering to
// the source (for example a UVC frame or a panel vsync) instead of the autorefresh tick.
typedef void (*framebuffer_frame_callback_t)(void *user_data);
