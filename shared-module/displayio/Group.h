// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/objlist.h"
#include "shared-module/displayio/Palette.h"

// The compositing core (Group and TileGrid) has two implementations, selected at build
// time with CIRCUITPY_DISPLAYIO_LVGL:
//
//   legacy/ — the original scanline compositing core used by display_core.c and every
//     BusDisplay/EPaperDisplay/FramebufferDisplay variant.
//   lvgl/  — an implementation that mirrors Groups and TileGrids into an LVGL object tree
//     which is rendered by LVGL. Groups are plain lv_obj containers. A TileGrid is a
//     single lv_image (single-tile sprites) or an lv_obj container with one lv_image per
//     tile. Images share the source bitmap's pixel data directly when the source format
//     matches the display format; otherwise a custom LVGL image decoder converts tiles.
#if CIRCUITPY_DISPLAYIO_LVGL
#include "shared-module/displayio/lvgl/Group.h"
// The protocol slot an LVGL-backed Group exposes to its parent Group. Legacy Groups are found
// by type, so the slot stays empty there. See displayio/lvgl/lvgl_protocol.h.
#define DISPLAYIO_GROUP_LVGL_PROTOCOL &displayio_lvgl_group_protocol
#else
#include "shared-module/displayio/legacy/Group.h"
#define DISPLAYIO_GROUP_LVGL_PROTOCOL NULL
#endif
