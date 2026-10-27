// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-module/displayio/Palette.h"

// See Group.h for the compositing core backend selection.
#if CIRCUITPY_DISPLAYIO_LVGL
#include "shared-module/displayio/lvgl/TileGrid.h"
// The protocol slot an LVGL-backed TileGrid exposes to its Group. Legacy TileGrids are found
// by type, so the slot stays empty there. See displayio/lvgl/lvgl_protocol.h.
#define DISPLAYIO_TILEGRID_LVGL_PROTOCOL &displayio_lvgl_tilegrid_protocol
#else
#include "shared-module/displayio/legacy/TileGrid.h"
#define DISPLAYIO_TILEGRID_LVGL_PROTOCOL NULL
#endif
