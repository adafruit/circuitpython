// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/displayio/TileGrid.h"

#include "py/runtime.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/OnDiskBitmap.h"
#include "shared-bindings/displayio/Palette.h"
#if CIRCUITPY_TILEPALETTEMAPPER
#include "shared-bindings/tilepalettemapper/TilePaletteMapper.h"
#endif

#include "supervisor/shared/serial.h"

void common_hal_displayio_tilegrid_construct(displayio_tilegrid_t *self, mp_obj_t bitmap,
    uint16_t bitmap_width_in_tiles, uint16_t bitmap_height_in_tiles,
    mp_obj_t pixel_shader, uint16_t width, uint16_t height,
    uint16_t tile_width, uint16_t tile_height, uint16_t x, uint16_t y, uint16_t default_tile) {

    uint32_t total_tiles = width * height;
    self->bitmap_width_in_tiles = bitmap_width_in_tiles;
    self->tiles_in_bitmap = bitmap_width_in_tiles * bitmap_height_in_tiles;

    // Determine if we need uint16_t or uint8_t for tile indices
    bool use_uint16 = self->tiles_in_bitmap > 255;

    // Sprites will only have one tile so save a little memory by inlining values in the pointer.
    uint8_t inline_tiles = sizeof(void *) / (use_uint16 ? sizeof(uint16_t) : sizeof(uint8_t));

    if (total_tiles <= inline_tiles) {
        self->tiles = 0;
        // Pack values into the pointer since there are only a few.
        if (use_uint16) {
            for (uint32_t i = 0; i < inline_tiles && i < total_tiles; i++) {
                ((uint16_t *)&self->tiles)[i] = default_tile;
            }
        } else {
            for (uint32_t i = 0; i < inline_tiles && i < total_tiles; i++) {
                ((uint8_t *)&self->tiles)[i] = (uint8_t)default_tile;
            }
        }
        self->inline_tiles = true;
    } else {
        if (use_uint16) {
            uint16_t *tiles16 = (uint16_t *)m_malloc_without_collect(total_tiles * sizeof(uint16_t));
            for (uint32_t i = 0; i < total_tiles; i++) {
                tiles16[i] = default_tile;
            }
            self->tiles = tiles16;
        } else {
            uint8_t *tiles8 = (uint8_t *)m_malloc_without_collect(total_tiles);
            for (uint32_t i = 0; i < total_tiles; i++) {
                tiles8[i] = (uint8_t)default_tile;
            }
            self->tiles = tiles8;
        }
        self->inline_tiles = false;
    }

    self->width_in_tiles = width;
    self->height_in_tiles = height;
    self->x = x;
    self->y = y;
    self->pixel_width = width * tile_width;
    self->pixel_height = height * tile_height;
    self->tile_width = tile_width;
    self->tile_height = tile_height;
    self->bitmap = bitmap;
    self->pixel_shader = pixel_shader;
    self->in_group = false;
    self->hidden = false;
    self->hidden_by_parent = false;
    self->previous_area.x1 = 0xffff;
    self->previous_area.x2 = self->previous_area.x1;
    self->flip_x = false;
    self->flip_y = false;
    self->transpose_xy = false;
    self->absolute_transform = NULL;
    #if CIRCUITPY_TILEPALETTEMAPPER
    if (mp_obj_is_type(self->pixel_shader, &tilepalettemapper_tilepalettemapper_type)) {
        tilepalettemapper_tilepalettemapper_bind(self->pixel_shader, self);
    }
    #endif
}


bool common_hal_displayio_tilegrid_get_hidden(displayio_tilegrid_t *self) {
    return self->hidden;
}

bool displayio_tilegrid_get_rendered_hidden(displayio_tilegrid_t *self) {
    return self->rendered_hidden;
}

void common_hal_displayio_tilegrid_set_hidden(displayio_tilegrid_t *self, bool hidden) {
    self->hidden = hidden;
    self->rendered_hidden = false;
    if (!hidden) {
        self->full_change = true;
    }
}

void displayio_tilegrid_set_hidden_by_parent(displayio_tilegrid_t *self, bool hidden) {
    self->hidden_by_parent = hidden;
    self->rendered_hidden = false;
    if (!hidden) {
        self->full_change = true;
    }
}

bool displayio_tilegrid_get_previous_area(displayio_tilegrid_t *self, displayio_area_t *area) {
    if (self->previous_area.x1 == self->previous_area.x2) {
        return false;
    }
    displayio_area_copy(&self->previous_area, area);
    return true;
}

static void _update_current_x(displayio_tilegrid_t *self) {
    uint16_t width;
    if (self->transpose_xy) {
        width = self->pixel_height;
    } else {
        width = self->pixel_width;
    }

    // If there's no transform, substitute an identity transform so the calculations will work.
    const displayio_buffer_transform_t *absolute_transform =
        self->absolute_transform == NULL
        ? &null_transform
        : self->absolute_transform;

    if (absolute_transform->transpose_xy) {
        self->current_area.y1 = absolute_transform->y + absolute_transform->dy * self->x;
        self->current_area.y2 = absolute_transform->y + absolute_transform->dy * (self->x + width);
        if (self->current_area.y2 < self->current_area.y1) {
            int16_t temp = self->current_area.y2;
            self->current_area.y2 = self->current_area.y1;
            self->current_area.y1 = temp;
        }
    } else {
        self->current_area.x1 = absolute_transform->x + absolute_transform->dx * self->x;
        self->current_area.x2 = absolute_transform->x + absolute_transform->dx * (self->x + width);
        if (self->current_area.x2 < self->current_area.x1) {
            int16_t temp = self->current_area.x2;
            self->current_area.x2 = self->current_area.x1;
            self->current_area.x1 = temp;
        }
    }
}

static void _update_current_y(displayio_tilegrid_t *self) {
    uint16_t height;
    if (self->transpose_xy) {
        height = self->pixel_width;
    } else {
        height = self->pixel_height;
    }

    // If there's no transform, substitute an identity transform so the calculations will work.
    const displayio_buffer_transform_t *absolute_transform =
        self->absolute_transform == NULL
        ? &null_transform
        : self->absolute_transform;

    if (absolute_transform->transpose_xy) {
        self->current_area.x1 = absolute_transform->x + absolute_transform->dx * self->y;
        self->current_area.x2 = absolute_transform->x + absolute_transform->dx * (self->y + height);
        if (self->current_area.x2 < self->current_area.x1) {
            int16_t temp = self->current_area.x2;
            self->current_area.x2 = self->current_area.x1;
            self->current_area.x1 = temp;
        }
    } else {
        self->current_area.y1 = absolute_transform->y + absolute_transform->dy * self->y;
        self->current_area.y2 = absolute_transform->y + absolute_transform->dy * (self->y + height);
        if (self->current_area.y2 < self->current_area.y1) {
            int16_t temp = self->current_area.y2;
            self->current_area.y2 = self->current_area.y1;
            self->current_area.y1 = temp;
        }
    }
}

void displayio_tilegrid_update_transform(displayio_tilegrid_t *self,
    const displayio_buffer_transform_t *absolute_transform) {
    self->in_group = absolute_transform != NULL;
    self->absolute_transform = absolute_transform;
    if (absolute_transform != NULL) {
        self->moved = true;

        _update_current_x(self);
        _update_current_y(self);
    }
}

mp_int_t common_hal_displayio_tilegrid_get_x(displayio_tilegrid_t *self) {
    return self->x;
}
void common_hal_displayio_tilegrid_set_x(displayio_tilegrid_t *self, mp_int_t x) {
    if (self->x == x) {
        return;
    }

    self->moved = true;

    self->x = x;
    if (self->absolute_transform != NULL) {
        _update_current_x(self);
    }
}
mp_int_t common_hal_displayio_tilegrid_get_y(displayio_tilegrid_t *self) {
    return self->y;
}

void common_hal_displayio_tilegrid_set_y(displayio_tilegrid_t *self, mp_int_t y) {
    if (self->y == y) {
        return;
    }
    self->moved = true;
    self->y = y;
    if (self->absolute_transform != NULL) {
        _update_current_y(self);
    }
}

mp_obj_t common_hal_displayio_tilegrid_get_pixel_shader(displayio_tilegrid_t *self) {
    return self->pixel_shader;
}

void common_hal_displayio_tilegrid_set_pixel_shader(displayio_tilegrid_t *self, mp_obj_t pixel_shader) {
    self->pixel_shader = pixel_shader;
    self->full_change = true;
    #if CIRCUITPY_TILEPALETTEMAPPER
    if (mp_obj_is_type(self->pixel_shader, &tilepalettemapper_tilepalettemapper_type)) {
        tilepalettemapper_tilepalettemapper_bind(self->pixel_shader, self);
    }
    #endif
}

mp_obj_t common_hal_displayio_tilegrid_get_bitmap(displayio_tilegrid_t *self) {
    return self->bitmap;
}

void common_hal_displayio_tilegrid_set_bitmap(displayio_tilegrid_t *self, mp_obj_t bitmap) {
    self->bitmap = bitmap;
    self->full_change = true;
}

uint16_t common_hal_displayio_tilegrid_get_width(displayio_tilegrid_t *self) {
    return self->width_in_tiles;
}

uint16_t common_hal_displayio_tilegrid_get_height(displayio_tilegrid_t *self) {
    return self->height_in_tiles;
}

uint16_t common_hal_displayio_tilegrid_get_tile_width(displayio_tilegrid_t *self) {
    return self->tile_width;
}

uint16_t common_hal_displayio_tilegrid_get_tile_height(displayio_tilegrid_t *self) {
    return self->tile_height;
}

uint16_t common_hal_displayio_tilegrid_get_tile(displayio_tilegrid_t *self, uint16_t x, uint16_t y) {
    void *tiles = self->tiles;
    if (self->inline_tiles) {
        tiles = &self->tiles;
    }
    if (tiles == NULL) {
        return 0;
    }

    uint32_t index = y * self->width_in_tiles + x;
    if (self->tiles_in_bitmap > 255) {
        return ((uint16_t *)tiles)[index];
    } else {
        return ((uint8_t *)tiles)[index];
    }
}

void displayio_tilegrid_mark_tile_dirty(displayio_tilegrid_t *self, uint16_t x, uint16_t y) {
    displayio_area_t temp_area;
    displayio_area_t *tile_area;
    if (!self->partial_change) {
        tile_area = &self->dirty_area;
    } else {
        tile_area = &temp_area;
    }
    int16_t tx = (x - self->top_left_x) % self->width_in_tiles;
    if (tx < 0) {
        tx += self->width_in_tiles;
    }
    tile_area->x1 = tx * self->tile_width;
    tile_area->x2 = tile_area->x1 + self->tile_width;
    int16_t ty = (y - self->top_left_y) % self->height_in_tiles;
    if (ty < 0) {
        ty += self->height_in_tiles;
    }
    tile_area->y1 = ty * self->tile_height;
    tile_area->y2 = tile_area->y1 + self->tile_height;

    if (self->partial_change) {
        displayio_area_union(&self->dirty_area, &temp_area, &self->dirty_area);
    }
    self->partial_change = true;
}

void common_hal_displayio_tilegrid_set_tile(displayio_tilegrid_t *self, uint16_t x, uint16_t y, uint16_t tile_index) {
    if (tile_index >= self->tiles_in_bitmap) {
        mp_raise_ValueError(MP_ERROR_TEXT("Tile index out of bounds"));
    }

    void *tiles = self->tiles;
    if (self->inline_tiles) {
        tiles = &self->tiles;
    }
    if (tiles == NULL) {
        return;
    }

    uint32_t index = y * self->width_in_tiles + x;
    if (self->tiles_in_bitmap > 255) {
        ((uint16_t *)tiles)[index] = tile_index;
    } else {
        ((uint8_t *)tiles)[index] = (uint8_t)tile_index;
    }
    displayio_tilegrid_mark_tile_dirty(self, x, y);
}

void common_hal_displayio_tilegrid_set_all_tiles(displayio_tilegrid_t *self, uint16_t tile_index) {
    if (tile_index >= self->tiles_in_bitmap) {
        mp_raise_ValueError(MP_ERROR_TEXT("Tile index out of bounds"));
    }

    void *tiles = self->tiles;
    if (self->inline_tiles) {
        tiles = &self->tiles;
    }
    if (tiles == NULL) {
        return;
    }

    if (self->tiles_in_bitmap > 255) {
        uint16_t *tiles16 = (uint16_t *)tiles;
        for (uint16_t y = 0; y < self->height_in_tiles; y++) {
            for (uint16_t x = 0; x < self->width_in_tiles; x++) {
                tiles16[y * self->width_in_tiles + x] = tile_index;
            }
        }
    } else {
        uint8_t *tiles8 = (uint8_t *)tiles;
        for (uint16_t y = 0; y < self->height_in_tiles; y++) {
            for (uint16_t x = 0; x < self->width_in_tiles; x++) {
                tiles8[y * self->width_in_tiles + x] = (uint8_t)tile_index;
            }
        }
    }

    self->full_change = true;
}

bool common_hal_displayio_tilegrid_get_flip_x(displayio_tilegrid_t *self) {
    return self->flip_x;
}

void common_hal_displayio_tilegrid_set_flip_x(displayio_tilegrid_t *self, bool flip_x) {
    if (self->flip_x == flip_x) {
        return;
    }
    self->flip_x = flip_x;
    self->full_change = true;
}

bool common_hal_displayio_tilegrid_get_flip_y(displayio_tilegrid_t *self) {
    return self->flip_y;
}

void common_hal_displayio_tilegrid_set_flip_y(displayio_tilegrid_t *self, bool flip_y) {
    if (self->flip_y == flip_y) {
        return;
    }
    self->flip_y = flip_y;
    self->full_change = true;
}

bool common_hal_displayio_tilegrid_get_transpose_xy(displayio_tilegrid_t *self) {
    return self->transpose_xy;
}

void common_hal_displayio_tilegrid_set_transpose_xy(displayio_tilegrid_t *self, bool transpose_xy) {
    if (self->transpose_xy == transpose_xy) {
        return;
    }
    self->transpose_xy = transpose_xy;

    // Square TileGrids do not change dimensions when transposed.
    if (self->pixel_width == self->pixel_height) {
        self->full_change = true;
        return;
    }

    _update_current_x(self);
    _update_current_y(self);

    self->moved = true;
}

bool common_hal_displayio_tilegrid_contains(displayio_tilegrid_t *self, uint16_t x, uint16_t y) {
    uint16_t right_edge = self->x + (self->width_in_tiles * self->tile_width);
    uint16_t bottom_edge = self->y + (self->height_in_tiles * self->tile_height);
    return x >= self->x && x < right_edge &&
           y >= self->y && y < bottom_edge;
}

void common_hal_displayio_tilegrid_set_top_left(displayio_tilegrid_t *self, uint16_t x, uint16_t y) {
    self->top_left_x = x;
    self->top_left_y = y;
    self->full_change = true;
}

bool displayio_tilegrid_fill_area(displayio_tilegrid_t *self,
    const _displayio_colorspace_t *colorspace, const displayio_area_t *area,
    uint32_t *mask, uint32_t *buffer) {
    // If no tiles are present we have no impact.
    void *tiles = self->tiles;
    if (self->inline_tiles) {
        tiles = &self->tiles;
    }
    if (tiles == NULL) {
        return false;
    }

    bool hidden = self->hidden || self->hidden_by_parent;
    if (hidden) {
        return false;
    }

    displayio_area_t overlap;
    if (!displayio_area_compute_overlap(area, &self->current_area, &overlap)) {
        return false;
    }

    int16_t x_stride = 1;
    int16_t y_stride = displayio_area_width(area);

    bool flip_x = self->flip_x;
    bool flip_y = self->flip_y;
    if (self->transpose_xy != self->absolute_transform->transpose_xy) {
        bool temp_flip = flip_x;
        flip_x = flip_y;
        flip_y = temp_flip;
    }

    // How many pixels are outside of our area between us and the start of the row.
    uint16_t start = 0;
    if ((self->absolute_transform->dx < 0) != flip_x) {
        start += (area->x2 - area->x1 - 1) * x_stride;
        x_stride *= -1;
    }
    if ((self->absolute_transform->dy < 0) != flip_y) {
        start += (area->y2 - area->y1 - 1) * y_stride;
        y_stride *= -1;
    }

    // Track if this layer finishes filling in the given area. We can ignore any remaining
    // layers at that point.
    bool full_coverage = displayio_area_equal(area, &overlap);

    // TODO(tannewt): Skip coverage tracking if all pixels outside the overlap have already been
    // set and our palette is all opaque.

    // TODO(tannewt): Check to see if the pixel_shader has any transparency. If it doesn't then we
    // can either return full coverage or bulk update the mask.
    displayio_area_t transformed;
    displayio_area_transform_within(flip_x != (self->absolute_transform->dx < 0), flip_y != (self->absolute_transform->dy < 0), self->transpose_xy != self->absolute_transform->transpose_xy,
        &overlap,
        &self->current_area,
        &transformed);

    int16_t start_x = (transformed.x1 - self->current_area.x1);
    int16_t end_x = (transformed.x2 - self->current_area.x1);
    int16_t start_y = (transformed.y1 - self->current_area.y1);
    int16_t end_y = (transformed.y2 - self->current_area.y1);

    int16_t y_shift = 0;
    int16_t x_shift = 0;
    if ((self->absolute_transform->dx < 0) != flip_x) {
        x_shift = area->x2 - overlap.x2;
    } else {
        x_shift = overlap.x1 - area->x1;
    }
    if ((self->absolute_transform->dy < 0) != flip_y) {
        y_shift = area->y2 - overlap.y2;
    } else {
        y_shift = overlap.y1 - area->y1;
    }

    // This untransposes x and y so it aligns with bitmap rows.
    if (self->transpose_xy != self->absolute_transform->transpose_xy) {
        int16_t temp_stride = x_stride;
        x_stride = y_stride;
        y_stride = temp_stride;
        int16_t temp_shift = x_shift;
        x_shift = y_shift;
        y_shift = temp_shift;
    }

    displayio_input_pixel_t input_pixel;
    displayio_output_pixel_t output_pixel;

    for (input_pixel.y = start_y; input_pixel.y < end_y; ++input_pixel.y) {
        int16_t row_start = start + (input_pixel.y - start_y + y_shift) * y_stride; // in pixels
        int16_t local_y = input_pixel.y / self->absolute_transform->scale;
        for (input_pixel.x = start_x; input_pixel.x < end_x; ++input_pixel.x) {
            // Compute the destination pixel in the buffer and mask based on the transformations.
            int16_t offset = row_start + (input_pixel.x - start_x + x_shift) * x_stride; // in pixels

            // This is super useful for debugging out of range accesses. Uncomment to use.
            // if (offset < 0 || offset >= (int32_t) displayio_area_size(area)) {
            //     asm("bkpt");
            // }

            // Check the mask first to see if the pixel has already been set.
            if ((mask[offset / 32] & (1 << (offset % 32))) != 0) {
                continue;
            }
            int16_t local_x = input_pixel.x / self->absolute_transform->scale;
            uint16_t x_tile_index = (local_x / self->tile_width + self->top_left_x) % self->width_in_tiles;
            uint16_t y_tile_index = (local_y / self->tile_height + self->top_left_y) % self->height_in_tiles;
            uint16_t tile_location = y_tile_index * self->width_in_tiles + x_tile_index;

            if (self->tiles_in_bitmap > 255) {
                input_pixel.tile = ((uint16_t *)tiles)[tile_location];
            } else {
                input_pixel.tile = ((uint8_t *)tiles)[tile_location];
            }
            input_pixel.tile_x = (input_pixel.tile % self->bitmap_width_in_tiles) * self->tile_width + local_x % self->tile_width;
            input_pixel.tile_y = (input_pixel.tile / self->bitmap_width_in_tiles) * self->tile_height + local_y % self->tile_height;

            output_pixel.pixel = 0;
            input_pixel.pixel = 0;

            // We always want to read bitmap pixels by row first and then transpose into the destination
            // buffer because most bitmaps are row associated.
            if (mp_obj_is_type(self->bitmap, &displayio_bitmap_type)) {
                input_pixel.pixel = common_hal_displayio_bitmap_get_pixel(self->bitmap, input_pixel.tile_x, input_pixel.tile_y);
            } else if (mp_obj_is_type(self->bitmap, &displayio_ondiskbitmap_type)) {
                input_pixel.pixel = common_hal_displayio_ondiskbitmap_get_pixel(self->bitmap, input_pixel.tile_x, input_pixel.tile_y);
            }

            output_pixel.opaque = true;
            #if CIRCUITPY_TILEPALETTEMAPPER
            if (mp_obj_is_type(self->pixel_shader, &tilepalettemapper_tilepalettemapper_type)) {
                tilepalettemapper_tilepalettemapper_get_color(self->pixel_shader, colorspace, &input_pixel, &output_pixel, x_tile_index, y_tile_index);
            }
            #endif
            if (self->pixel_shader == mp_const_none) {
                output_pixel.pixel = input_pixel.pixel;
            } else if (mp_obj_is_type(self->pixel_shader, &displayio_palette_type)) {
                displayio_palette_get_color(self->pixel_shader, colorspace, &input_pixel, &output_pixel);
            } else if (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type)) {
                displayio_colorconverter_convert(self->pixel_shader, colorspace, &input_pixel, &output_pixel);
            }
            if (!output_pixel.opaque) {
                // A pixel is transparent so we haven't fully covered the area ourselves.
                full_coverage = false;
            } else {
                mask[offset / 32] |= 1 << (offset % 32);
                if (colorspace->depth == 16) {
                    *(((uint16_t *)buffer) + offset) = output_pixel.pixel;
                } else if (colorspace->depth == 32) {
                    *(((uint32_t *)buffer) + offset) = output_pixel.pixel;
                } else if (colorspace->depth == 24) {
                    memcpy(((uint8_t *)buffer) + offset * 3, &output_pixel.pixel, 3);
                } else if (colorspace->depth == 8) {
                    *(((uint8_t *)buffer) + offset) = output_pixel.pixel;
                } else if (colorspace->depth < 8) {
                    uint8_t pixels_per_byte = 8 / colorspace->depth;

                    // Reorder the offsets to pack multiple rows into a byte (meaning they share a column).
                    if (!colorspace->pixels_in_byte_share_row) {
                        uint16_t width = displayio_area_width(area);
                        uint16_t row = offset / width;
                        uint16_t col = offset % width;
                        // Dividing by pixels_per_byte does truncated division even if we multiply it back out.
                        offset = col * pixels_per_byte + (row / pixels_per_byte) * pixels_per_byte * width + row % pixels_per_byte;
                        // Also useful for validating that the bitpacking worked correctly.
                        // if (offset > displayio_area_size(area)) {
                        //     asm("bkpt");
                        // }
                    }
                    uint8_t shift = (offset % pixels_per_byte) * colorspace->depth;
                    if (colorspace->reverse_pixels_in_byte) {
                        // Reverse the shift by subtracting it from the leftmost shift.
                        shift = (pixels_per_byte - 1) * colorspace->depth - shift;
                    }
                    ((uint8_t *)buffer)[offset / pixels_per_byte] |= output_pixel.pixel << shift;
                }
            }
        }
    }
    return full_coverage;
}

void displayio_tilegrid_finish_refresh(displayio_tilegrid_t *self) {
    bool first_draw = self->previous_area.x1 == self->previous_area.x2;
    bool hidden = self->hidden || self->hidden_by_parent;
    if (!first_draw && hidden) {
        self->previous_area.x2 = self->previous_area.x1;
    } else if (self->moved || first_draw) {
        displayio_area_copy(&self->current_area, &self->previous_area);
    }

    self->moved = false;
    self->full_change = false;
    self->partial_change = false;
    if (mp_obj_is_type(self->pixel_shader, &displayio_palette_type)) {
        displayio_palette_finish_refresh(self->pixel_shader);
    } else if (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type)) {
        displayio_colorconverter_finish_refresh(self->pixel_shader);
    }
    if (mp_obj_is_type(self->bitmap, &displayio_bitmap_type)) {
        displayio_bitmap_finish_refresh(self->bitmap);
    } else if (mp_obj_is_type(self->bitmap, &displayio_ondiskbitmap_type)) {
        // OnDiskBitmap changes will trigger a complete reload so no need to
        // track changes.
    }
    // TODO(tannewt): We could double buffer changes to position and move them over here.
    // That way they won't change during a refresh and tear.
}

displayio_area_t *displayio_tilegrid_get_refresh_areas(displayio_tilegrid_t *self, displayio_area_t *tail) {
    bool first_draw = self->previous_area.x1 == self->previous_area.x2;
    bool hidden = self->hidden || self->hidden_by_parent;
    // Check hidden first because it trumps all other changes.
    if (hidden) {
        self->rendered_hidden = true;
        if (!first_draw) {
            self->previous_area.next = tail;
            return &self->previous_area;
        } else {
            return tail;
        }
    } else if (self->moved && !first_draw) {
        displayio_area_union(&self->previous_area, &self->current_area, &self->dirty_area);
        if (displayio_area_size(&self->dirty_area) <= 2U * self->pixel_width * self->pixel_height) {
            self->dirty_area.next = tail;
            return &self->dirty_area;
        }
        self->previous_area.next = tail;
        self->current_area.next = &self->previous_area;
        return &self->current_area;
    }

    // If we have an in-memory bitmap, then check it for modifications.
    if (mp_obj_is_type(self->bitmap, &displayio_bitmap_type)) {
        displayio_area_t *refresh_area = displayio_bitmap_get_refresh_areas(self->bitmap, tail);
        if (refresh_area != tail) {
            // Special case a TileGrid that shows a full bitmap and use its
            // dirty area. Copy it to ours so we can transform it.
            if (self->tiles_in_bitmap == 1) {
                displayio_area_copy(refresh_area, &self->dirty_area);
                self->partial_change = true;
            } else {
                self->full_change = true;
            }
        }
    }

    self->full_change = self->full_change ||
        (mp_obj_is_type(self->pixel_shader, &displayio_palette_type) &&
            displayio_palette_needs_refresh(self->pixel_shader)) ||
        (mp_obj_is_type(self->pixel_shader, &displayio_colorconverter_type) &&
            displayio_colorconverter_needs_refresh(self->pixel_shader));

    if (self->full_change || first_draw) {
        self->current_area.next = tail;
        return &self->current_area;
    }

    if (self->partial_change) {
        int16_t x = self->x;
        int16_t y = self->y;
        if (self->absolute_transform->transpose_xy) {
            int16_t temp = y;
            y = x;
            x = temp;
        }
        int16_t x1 = self->dirty_area.x1;
        int16_t x2 = self->dirty_area.x2;
        if (self->flip_x) {
            x1 = self->pixel_width - x1;
            x2 = self->pixel_width - x2;
        }
        int16_t y1 = self->dirty_area.y1;
        int16_t y2 = self->dirty_area.y2;
        if (self->flip_y) {
            y1 = self->pixel_height - y1;
            y2 = self->pixel_height - y2;
        }
        if (self->transpose_xy != self->absolute_transform->transpose_xy) {
            int16_t temp1 = y1, temp2 = y2;
            y1 = x1;
            x1 = temp1;
            y2 = x2;
            x2 = temp2;
        }
        self->dirty_area.x1 = self->absolute_transform->x + self->absolute_transform->dx * (x + x1);
        self->dirty_area.y1 = self->absolute_transform->y + self->absolute_transform->dy * (y + y1);
        self->dirty_area.x2 = self->absolute_transform->x + self->absolute_transform->dx * (x + x2);
        self->dirty_area.y2 = self->absolute_transform->y + self->absolute_transform->dy * (y + y2);
        if (self->dirty_area.y2 < self->dirty_area.y1) {
            int16_t temp = self->dirty_area.y2;
            self->dirty_area.y2 = self->dirty_area.y1;
            self->dirty_area.y1 = temp;
        }
        if (self->dirty_area.x2 < self->dirty_area.x1) {
            int16_t temp = self->dirty_area.x2;
            self->dirty_area.x2 = self->dirty_area.x1;
            self->dirty_area.x1 = temp;
        }

        self->dirty_area.next = tail;
        return &self->dirty_area;
    }
    return tail;
}
