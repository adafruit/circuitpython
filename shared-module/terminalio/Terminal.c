// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/terminalio/Terminal.h"

#include "shared-module/fontio/BuiltinFont.h"
#include "shared-bindings/displayio/TileGrid.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/terminalio/Terminal.h"

#if CIRCUITPY_STATUS_BAR
#include "shared-bindings/supervisor/__init__.h"
#include "shared-bindings/supervisor/StatusBar.h"
#endif

void terminalio_terminal_clear_status_bar(terminalio_terminal_obj_t *self) {
    if (self->status_bar) {
        common_hal_displayio_tilegrid_set_all_tiles(self->status_bar, 0);
    }
}

void common_hal_terminalio_terminal_construct(terminalio_terminal_obj_t *self,
    displayio_tilegrid_t *scroll_area, const fontio_builtinfont_t *font,
    displayio_tilegrid_t *status_bar) {
    self->cursor_x = 0;
    self->cursor_y = 0;
    self->font = font;
    self->scroll_area = scroll_area;
    self->status_bar = status_bar;
    self->status_x = 0;
    self->status_y = 0;
    self->first_row = 0;
    self->vt_scroll_top = 0;
    self->vt_scroll_end = self->scroll_area->height_in_tiles - 1;
    common_hal_displayio_tilegrid_set_all_tiles(self->scroll_area, 0);
    if (self->status_bar) {
        common_hal_displayio_tilegrid_set_all_tiles(self->status_bar, 0);
    }

    common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, 1);
}

size_t common_hal_terminalio_terminal_write(terminalio_terminal_obj_t *self, const byte *data, size_t len, int *errcode) {
    #define SCRNMOD(x) (((x) + (self->scroll_area->top_left_y)) % (self->scroll_area->height_in_tiles))

    // Make sure the terminal is initialized before we do anything with it.
    if (self->scroll_area == NULL) {
        return len;
    }

    #if CIRCUITPY_TERMINALIO_VT100
    uint32_t _select_color(uint16_t ascii_color) {
        uint32_t color_value = 0;
        if ((ascii_color & 1) > 0) {
            color_value += 0xff0000;
        }
        if ((ascii_color & 2) > 0) {
            color_value += 0x00ff00;
        }
        if ((ascii_color & 4) > 0) {
            color_value += 0x0000ff;
        }

        return color_value;
    }

    displayio_palette_t *terminal_palette = self->scroll_area->pixel_shader;
    #endif

    const byte *i = data;
    uint16_t start_y = self->cursor_y;
    while (i < data + len) {
        unichar c = utf8_get_char(i);
        i = utf8_next_char(i);
        if (self->in_osc_command) {
            if (c == 0x1b && i[0] == '\\') {
                self->in_osc_command = false;
                self->status_x = 0;
                self->status_y = 0;
                i += 1;
            } else if (
                self->osc_command == 0 &&
                self->status_bar != NULL &&
                self->status_y < self->status_bar->height_in_tiles) {
                uint8_t tile_index = fontio_builtinfont_get_glyph_index(self->font, c);
                if (tile_index != 0xff) {
                    // Clear the tile grid before we start putting new info.
                    if (self->status_x == 0 && self->status_y == 0) {
                        common_hal_displayio_tilegrid_set_all_tiles(self->status_bar, 0);
                    }
                    common_hal_displayio_tilegrid_set_tile(self->status_bar, self->status_x, self->status_y, tile_index);
                    self->status_x++;
                    if (self->status_x >= self->status_bar->width_in_tiles) {
                        self->status_y++;
                        self->status_x %= self->status_bar->width_in_tiles;
                    }
                }
            }
            continue;
        }
        // Always handle ASCII.
        if (c < 128) {
            if (c >= 0x20 && c <= 0x7e) {
                uint8_t tile_index = fontio_builtinfont_get_glyph_index(self->font, c);
                common_hal_displayio_tilegrid_set_tile(self->scroll_area, self->cursor_x, self->cursor_y, tile_index);
                self->cursor_x++;
            } else if (c == '\r') {
                self->cursor_x = 0;
            } else if (c == '\n') {
                self->cursor_y++;
                // Commands below are used by MicroPython in the REPL
            } else if (c == '\b') {
                if (self->cursor_x > 0) {
                    self->cursor_x--;
                }
            } else if (c == 0x1b) {
                // Handle commands of the form [ESC].<digits><command-char> where . is not yet known.
                int16_t vt_args[3] = {0, 0, 0};
                uint8_t j = 1;
                #if CIRCUITPY_TERMINALIO_VT100
                uint8_t n_args = 1;
                #endif
                for (; j < 6; j++) {
                    if ('0' <= i[j] && i[j] <= '9') {
                        vt_args[0] = vt_args[0] * 10 + (i[j] - '0');
                    } else {
                        c = i[j];
                        break;
                    }
                }
                if (i[0] == '[') {
                    for (uint8_t i_args = 1; i_args < 3 && c == ';'; i_args++) {
                        vt_args[i_args] = 0;
                        for (++j; j < 12; j++) {
                            if ('0' <= i[j] && i[j] <= '9') {
                                vt_args[i_args] = vt_args[i_args] * 10 + (i[j] - '0');
                                #if CIRCUITPY_TERMINALIO_VT100
                                n_args = i_args + 1;
                                #endif
                            } else {
                                c = i[j];
                                break;
                            }
                        }
                    }
                    if (c == '?') {
                        #if CIRCUITPY_TERMINALIO_VT100
                        if (i[2] == '2' && i[3] == '5') {
                            // cursor visibility commands
                            if (i[4] == 'h') {
                                // make cursor visible
                                // not implemented yet
                            } else if (i[4] == 'l') {
                                // make cursor invisible
                                // not implemented yet
                            }
                        }
                        i += 5;
                        #endif
                    } else {
                        if (c == 'K') {
                            int16_t clr_start = self->cursor_x;
                            int16_t clr_end = self->scroll_area->width_in_tiles;
                            #if CIRCUITPY_TERMINALIO_VT100
                            if (vt_args[0] == 1) {
                                clr_start = 0;
                                clr_end = self->cursor_x;
                            } else if (vt_args[0] == 2) {
                                clr_start = 0;
                            }
                            #endif
                            // Clear the (start/rest/all) of the line.
                            for (uint16_t k = clr_start; k < clr_end; k++) {
                                common_hal_displayio_tilegrid_set_tile(self->scroll_area, k, self->cursor_y, 0);
                            }
                        } else if (c == 'D') {
                            if (vt_args[0] > self->cursor_x) {
                                self->cursor_x = 0;
                            } else {
                                self->cursor_x -= vt_args[0];
                            }
                        } else if (c == 'J') {
                            if (vt_args[0] == 2) {
                                common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, 0);
                                self->cursor_x = self->cursor_y = start_y = 0;
                                common_hal_displayio_tilegrid_set_all_tiles(self->scroll_area, 0);
                            }
                        } else if (c == 'H') {
                            if (vt_args[0] > 0) {
                                vt_args[0]--;
                            }
                            if (vt_args[1] > 0) {
                                vt_args[1]--;
                            }
                            if (vt_args[0] >= self->scroll_area->height_in_tiles) {
                                vt_args[0] = self->scroll_area->height_in_tiles - 1;
                            }
                            if (vt_args[1] >= self->scroll_area->width_in_tiles) {
                                vt_args[1] = self->scroll_area->width_in_tiles - 1;
                            }
                            vt_args[0] = SCRNMOD(vt_args[0]);
                            self->cursor_x = vt_args[1];
                            self->cursor_y = vt_args[0];
                            start_y = self->cursor_y;
                        #if CIRCUITPY_TERMINALIO_VT100
                        } else if (c == 'm') {
                            for (uint8_t i_args = 0; i_args < n_args; i_args++) {
                                if ((vt_args[i_args] >= 40 && vt_args[i_args] <= 47) || (vt_args[i_args] >= 30 && vt_args[i_args] <= 37)) {
                                    common_hal_displayio_palette_set_color(terminal_palette, 1 - (vt_args[i_args] / 40), _select_color(vt_args[i_args] % 10));
                                }
                                if (vt_args[i_args] == 0) {
                                    common_hal_displayio_palette_set_color(terminal_palette, 0, 0x000000);
                                    common_hal_displayio_palette_set_color(terminal_palette, 1, 0xffffff);
                                }
                            }
                        } else if (c == 'r') {
                            if (vt_args[0] < vt_args[1] && vt_args[0] >= 1 && vt_args[1] <= self->scroll_area->height_in_tiles) {
                                self->vt_scroll_top = vt_args[0] - 1;
                                self->vt_scroll_end = vt_args[1] - 1;
                            } else {
                                self->vt_scroll_top = 0;
                                self->vt_scroll_end = self->scroll_area->height_in_tiles - 1;
                            }
                            self->cursor_x = 0;
                            self->cursor_y = self->scroll_area->top_left_y % self->scroll_area->height_in_tiles;
                            start_y = self->cursor_y;
                        #endif
                        }
                        i += j + 1;
                    }
                #if CIRCUITPY_TERMINALIO_VT100
                } else if (i[0] == 'M') {
                    if (self->cursor_y != SCRNMOD(self->vt_scroll_top)) {
                        if (self->cursor_y > 0) {
                            self->cursor_y = self->cursor_y - 1;
                        } else {
                            self->cursor_y = self->scroll_area->height_in_tiles - 1;
                        }
                    } else {
                        if (self->vt_scroll_top != 0 || self->vt_scroll_end != self->scroll_area->height_in_tiles) {
                            // Scroll range defined, manually move tiles to perform scroll
                            for (int16_t irow = self->vt_scroll_end - 1; irow >= self->vt_scroll_top; irow--) {
                                for (int16_t icol = 0; icol < self->scroll_area->width_in_tiles; icol++) {
                                    common_hal_displayio_tilegrid_set_tile(self->scroll_area, icol, SCRNMOD(irow + 1), common_hal_displayio_tilegrid_get_tile(self->scroll_area, icol, SCRNMOD(irow)));
                                }
                            }
                            for (int16_t icol = 0; icol < self->scroll_area->width_in_tiles; icol++) {
                                common_hal_displayio_tilegrid_set_tile(self->scroll_area, icol, self->cursor_y, 0);
                            }
                        } else {
                            // Full screen scroll, just set new top_y pointer and clear row
                            if (self->cursor_y > 0) {
                                common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, self->cursor_y - 1);
                            } else {
                                common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, self->scroll_area->height_in_tiles - 1);
                            }
                            for (uint16_t icol = 0; icol < self->scroll_area->width_in_tiles; icol++) {
                                common_hal_displayio_tilegrid_set_tile(self->scroll_area, icol, self->scroll_area->top_left_y, 0);
                            }
                            self->cursor_y = self->scroll_area->top_left_y;
                        }
                        self->cursor_x = 0;
                    }
                    start_y = self->cursor_y;
                    i++;
                } else if (i[0] == 'D') {
                    self->cursor_y++;
                    i++;
                #endif
                } else if (i[0] == ']' && c == ';') {
                    self->in_osc_command = true;
                    self->osc_command = vt_args[0];
                    i += j + 1;
                }
            }
        } else {
            uint8_t tile_index = fontio_builtinfont_get_glyph_index(self->font, c);
            if (tile_index != 0xff) {
                common_hal_displayio_tilegrid_set_tile(self->scroll_area, self->cursor_x, self->cursor_y, tile_index);
                self->cursor_x++;

            }
        }
        if (self->cursor_x >= self->scroll_area->width_in_tiles) {
            self->cursor_y++;
            self->cursor_x %= self->scroll_area->width_in_tiles;
        }
        if (self->cursor_y >= self->scroll_area->height_in_tiles) {
            self->cursor_y %= self->scroll_area->height_in_tiles;
        }
        if (self->cursor_y != start_y) {
            if (((self->cursor_y + self->scroll_area->height_in_tiles) - 1) % self->scroll_area->height_in_tiles == SCRNMOD(self->vt_scroll_end)) {
                #if CIRCUITPY_TERMINALIO_VT100
                if (self->vt_scroll_top != 0 || self->vt_scroll_end != self->scroll_area->height_in_tiles) {
                    // Scroll range defined, manually move tiles to perform scroll
                    self->cursor_y = SCRNMOD(self->vt_scroll_end);

                    for (int16_t irow = self->vt_scroll_top; irow < self->vt_scroll_end; irow++) {
                        for (int16_t icol = 0; icol < self->scroll_area->width_in_tiles; icol++) {
                            common_hal_displayio_tilegrid_set_tile(self->scroll_area, icol, SCRNMOD(irow), common_hal_displayio_tilegrid_get_tile(self->scroll_area, icol, SCRNMOD(irow + 1)));
                        }
                    }
                }
                #endif
                if (self->vt_scroll_top == 0 && self->vt_scroll_end == self->scroll_area->height_in_tiles) {
                    // Full screen scroll, just set new top_y pointer
                    common_hal_displayio_tilegrid_set_top_left(self->scroll_area, 0, (self->cursor_y + self->scroll_area->height_in_tiles + 1) % self->scroll_area->height_in_tiles);
                }
                // clear the new row in case of scroll up
                for (int16_t icol = 0; icol < self->scroll_area->width_in_tiles; icol++) {
                    common_hal_displayio_tilegrid_set_tile(self->scroll_area, icol, self->cursor_y, 0);
                }
                self->cursor_x = 0;
            }
            start_y = self->cursor_y;
        }
    }
    return i - data;
}

bool common_hal_terminalio_terminal_ready_to_tx(terminalio_terminal_obj_t *self) {
    return self->scroll_area != NULL;
}
