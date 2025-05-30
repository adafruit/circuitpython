// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT


#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-module/displayio/Palette.h"
#include "shared-module/displayio/TileGrid.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_t pixel_shader;
    mp_obj_t tilegrid;
    uint16_t width_in_tiles;
    uint16_t height_in_tiles;
    uint16_t input_color_count;
    uint32_t **tile_mappings;
} tilepalettemapper_tilepalettemapper_t;

void tilepalettemapper_tilepalettemapper_get_color(tilepalettemapper_tilepalettemapper_t *self, const _displayio_colorspace_t *colorspace, displayio_input_pixel_t *input_pixel, displayio_output_pixel_t *output_color, uint16_t x_tile_index, uint16_t y_tile_index);
void tilepalettemapper_tilepalettemapper_bind(tilepalettemapper_tilepalettemapper_t *self, displayio_tilegrid_t *tilegrid);
