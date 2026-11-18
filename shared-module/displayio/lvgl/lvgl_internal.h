// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Internal declarations shared by the files that implement the LVGL-based
// displayio compositing core (shared-module/displayio/lvgl/).

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/nlr.h"
#include "shared-module/displayio/Group.h"
#include "shared-module/displayio/TileGrid.h"
#include "shared-module/displayio/Bitmap.h"
#include "shared-module/displayio/Palette.h"
#include "shared-module/displayio/OnDiskBitmap.h"
#include "shared-module/displayio/lvgl/ImageRoot.h"
#include "shared-module/displayio/lvgl/Palette.h"

#if CIRCUITPY_DISPLAYIO_LVGL

// Port hook: initialize LVGL itself and its tick source. Implemented by each port's
// common-hal and safe to call repeatedly.
void displayio_lvgl_port_init(void);

// Port-supplied recursion-safe lock around LVGL. The RTOS renders LVGL on its own
// thread (e.g. a Zephyr workqueue or the ESP32 LVGL task), so CircuitPython-side mirror
// mutations must be excluded from rendering. The lock must be recursive: the shared
// wrappers may nest, and the render thread uses the same lock.
void displayio_lvgl_port_lock(void);
void displayio_lvgl_port_unlock(void);

// Port hook: arrange for BusDisplay refreshes to run on the CircuitPython thread when the port
// renders LVGL on a task. Called once per BusDisplay, after its refresh timer exists. Ports that
// render BusDisplay on their own LVGL thread (Zephyr) do nothing.
void displayio_lvgl_port_install_busdisplay_refresh(lv_display_t *display);

// Port hook: a framebuffer reached a frame boundary. May be called from an interrupt handler, so
// ports that have a render task must defer it safely and render on the task. In task context, the
// ESP port renders synchronously.
void displayio_lvgl_port_frame_boundary(void);

// Shared lock wrappers. All display-side (CP thread) LVGL calls must happen inside these.
void displayio_lvgl_lock(void);
void displayio_lvgl_unlock(void);

// CircuitPython-thread mutations that may raise must register an unlock on exception.
// The caller owns the stack node until unlock_with_cleanup(). Do not use these from
// the render task, which has no active Python exception/cleanup stack.
void displayio_lvgl_lock_with_cleanup(nlr_jump_callback_node_t *cleanup);
void displayio_lvgl_unlock_with_cleanup(void);

// Set up LVGL for displayio. Called by the port's display constructor.
void displayio_lvgl_init(void);

// Register displayio's per-display state: the screen background and the refresh hooks, including
// the compose pass that brings every attached TileGrid's buffer up to date before each refresh
// draws anything. Called once per LVGL display, after it is created (and made default).
void displayio_lvgl_register_display(lv_display_t *display);

// True when the displays the compositor drives still want automatic repaints
// (display.auto_refresh). A port's render task consults this so turning auto_refresh off stops
// repaints without also stopping the rest of the port's background work.
bool displayio_lvgl_auto_refresh(void);

// Render displays that can't flush from the LVGL task (currently BusDisplay when the port asks
// for it). Called on the CircuitPython thread by the port's proxy, under the LVGL lock.
void displayio_lvgl_refresh_unsafe_displays(void);

// Render the framebuffer displays that pace themselves from a frame boundary. Called on the
// CircuitPython thread (directly or from the render task after an ISR notification), under the
// LVGL lock.
void displayio_lvgl_refresh_paced_displays(void);

// Attach/detach the compositor's root group. Implemented in lvgl/Group.c. NULL detach.
void displayio_lvgl_set_root_group(displayio_group_t *group);

#endif  // CIRCUITPY_DISPLAYIO_LVGL
