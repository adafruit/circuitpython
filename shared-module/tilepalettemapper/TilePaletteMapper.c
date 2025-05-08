// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include <stdlib.h>
#include "py/runtime.h"
#include "shared-bindings/tilepalettemapper/TilePaletteMapper.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/ColorConverter.h"

void common_hal_tilepalettemapper_tilepalettemapper_construct(tilepalettemapper_tilepalettemapper_t *self,
    mp_obj_t pixel_shader, uint16_t input_color_count, uint16_t width, uint16_t height) {

    self->pixel_shader = pixel_shader;
    self->width_in_tiles = width;
    self->height_in_tiles = height;
    self->input_color_count = input_color_count;
    self->needs_refresh = false;
    int mappings_len = width * height;
    self->tile_mappings = (uint32_t **)m_malloc(mappings_len * sizeof(uint32_t *));
    for (int i = 0; i < mappings_len; i++) {
        self->tile_mappings[i] = (uint32_t *)m_malloc_without_collect(input_color_count * sizeof(uint32_t));
        if (mp_obj_is_type(self->pixel_shader, &displayio_palette_type)) {
            for (uint16_t j = 0; j < input_color_count; j++) {
                self->tile_mappings[i][j] = j;
            }
        } else if (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type)) {
            for (uint16_t j = 0; j < input_color_count; j++) {
                self->tile_mappings[i][j] = 0;
            }
        }
    }
}

uint16_t common_hal_tilepalettemapper_tilepalettemapper_get_width(tilepalettemapper_tilepalettemapper_t *self) {
    return self->width_in_tiles;
}

uint16_t common_hal_tilepalettemapper_tilepalettemapper_get_height(tilepalettemapper_tilepalettemapper_t *self) {
    return self->height_in_tiles;
}

mp_obj_t common_hal_tilepalettemapper_tilepalettemapper_get_pixel_shader(tilepalettemapper_tilepalettemapper_t *self) {
    return self->pixel_shader;
}

mp_obj_t common_hal_tilepalettemapper_tilepalettemapper_get_mapping(tilepalettemapper_tilepalettemapper_t *self, uint16_t x, uint16_t y) {
    int index = x + y * self->width_in_tiles;
    mp_obj_t result[self->input_color_count];
    for (uint32_t i = 0; i < self->input_color_count; i++) {
        result[i] = mp_obj_new_int(self->tile_mappings[index][i]);
    }
    return mp_obj_new_tuple(self->input_color_count, result);
}

void common_hal_tilepalettemapper_tilepalettemapper_set_mapping(tilepalettemapper_tilepalettemapper_t *self, uint16_t x, uint16_t y, size_t len, mp_obj_t *items) {
    uint32_t palette_max;
    if (mp_obj_is_type(self->pixel_shader, &displayio_palette_type)) {
        palette_max = common_hal_displayio_palette_get_len(self->pixel_shader) - 1;
    } else { // colorconverter type
        palette_max = 0xFFFFFF;
    }

    for (uint16_t i = 0; i < MIN(len, self->input_color_count); i++) {
        int mapping_val = mp_arg_validate_type_int(items[i], MP_QSTR_mapping_value);
        mp_arg_validate_int_range(mapping_val, 0, palette_max, MP_QSTR_mapping_value);
        self->tile_mappings[y * self->width_in_tiles + x][i] = mapping_val;
    }
    self->needs_refresh = true;
}

void tilepalettemapper_tilepalettemapper_get_color(tilepalettemapper_tilepalettemapper_t *self, const _displayio_colorspace_t *colorspace, displayio_input_pixel_t *input_pixel, displayio_output_pixel_t *output_color, uint16_t x_tile_index, uint16_t y_tile_index) {
    if (x_tile_index >= self->width_in_tiles || y_tile_index >= self->height_in_tiles) {
        if (mp_obj_is_type(self->pixel_shader, &displayio_palette_type)) {
            displayio_palette_get_color(self->pixel_shader, colorspace, input_pixel, output_color);
        } else if (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type)) {
            displayio_colorconverter_convert(self->pixel_shader, colorspace, input_pixel, output_color);
        }
        return;
    }
    uint16_t tile_index = y_tile_index * self->width_in_tiles + x_tile_index;
    uint32_t mapped_index = self->tile_mappings[tile_index][input_pixel->pixel];
    displayio_input_pixel_t tmp_pixel;
    tmp_pixel.pixel = mapped_index;
    if (mp_obj_is_type(self->pixel_shader, &displayio_palette_type)) {
        displayio_palette_get_color(self->pixel_shader, colorspace, &tmp_pixel, output_color);
    } else if (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type)) {
        displayio_colorconverter_convert(self->pixel_shader, colorspace, &tmp_pixel, output_color);
    }

}

bool tilepalettemapper_tilepalettemapper_needs_refresh(tilepalettemapper_tilepalettemapper_t *self) {
    return self->needs_refresh;
}

void tilepalettemapper_tilepalettemapper_finish_refresh(tilepalettemapper_tilepalettemapper_t *self) {
    self->needs_refresh = false;
}
