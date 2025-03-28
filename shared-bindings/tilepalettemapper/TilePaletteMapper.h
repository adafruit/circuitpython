// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include "shared-module/tilepalettemapper/TilePaletteMapper.h"

extern const mp_obj_type_t tilepalettemapper_tilepalettemapper_type;

void common_hal_tilepalettemapper_tilepalettemapper_construct(tilepalettemapper_tilepalettemapper_t *self,
    mp_obj_t paltte, uint16_t input_color_count, uint16_t bitmap_width_in_tiles, uint16_t bitmap_height_in_tiles);


uint16_t common_hal_tilepalettemapper_tilepalettemapper_get_width(tilepalettemapper_tilepalettemapper_t *self);
uint16_t common_hal_tilepalettemapper_tilepalettemapper_get_height(tilepalettemapper_tilepalettemapper_t *self);
mp_obj_t common_hal_tilepalettemapper_tilepalettemapper_get_pixel_shader(tilepalettemapper_tilepalettemapper_t *self);
mp_obj_t common_hal_tilepalettemapper_tilepalettemapper_get_mapping(tilepalettemapper_tilepalettemapper_t *self, uint16_t x, uint16_t y);
void common_hal_tilepalettemapper_tilepalettemapper_set_mapping(tilepalettemapper_tilepalettemapper_t *self, uint16_t x, uint16_t y, size_t len, mp_obj_t *items);
