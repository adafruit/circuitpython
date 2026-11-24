// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/epaperdisplay/EPaperDisplay.h"

#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/epaperdisplay/lag_drive.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/tick.h"

#if CIRCUITPY_TINYUSB
#include "supervisor/usb.h"
#endif

#include <stdint.h>
#include <string.h>

// lag_drive.c calls these only when it is built. Otherwise they stay static, as before it.
#if CIRCUITPY_EPAPERDISPLAY_FAST
#define LAG_SHARED
#else
#define LAG_SHARED static
#endif

void common_hal_epaperdisplay_epaperdisplay_construct(epaperdisplay_epaperdisplay_obj_t *self,
    const epaperdisplay_construct_args_t *args) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    self->epaper.display = self;
    #endif
    epaperdisplay_epaper_core_construct(&self->epaper, args);

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (args->lag_frames > 0) {
        epaperdisplay_lag_construct(&self->epaper, &self->base, args->lag_frames);
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST

    // Set the group after initialization otherwise we may send pixels while we delay in
    // initialization.
    common_hal_epaperdisplay_epaperdisplay_set_root_group(self, &circuitpython_splash);
}

bool common_hal_epaperdisplay_epaperdisplay_set_root_group(epaperdisplay_epaperdisplay_obj_t *self, displayio_group_t *root_group) {
    return displayio_display_core_set_root_group(&self->epaper.core, root_group);
}

static const displayio_area_t *epaperdisplay_epaperdisplay_get_refresh_areas(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->epaper.core.full_refresh) {
        self->epaper.core.area.next = NULL;
        return &self->epaper.core.area;
    }
    const displayio_area_t *first_area = NULL;
    if (self->epaper.core.current_group != NULL) {
        first_area = displayio_group_get_refresh_areas(self->epaper.core.current_group, NULL);
    }
    if (first_area != NULL && self->epaper.bus.row_command == NO_COMMAND) {
        // Do a full refresh if the display doesn't support partial updates.
        self->epaper.core.area.next = NULL;
        return &self->epaper.core.area;
    }
    return first_area;
}

LAG_SHARED void epaperdisplay_epaper_core_start_refresh(epaperdisplay_epaper_core_t *self) {
    if (!displayio_display_bus_is_free(&self->bus)) {
        // Can't acquire display bus; skip updating this display. Try next display.
        return;
    }
    if (!displayio_display_core_start_refresh(&self->core)) {
        // Refresh on this display already in progress.
        return;
    }

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->fast && self->awake) {
        if (!self->needs_start) {
            // The panel is still configured from the previous refresh.
            return;
        }
        epaperdisplay_epaper_core_wait_for_busy(self);
        // SSD168x controllers commit the last RAM byte when the next command arrives, so a reset
        // can't come straight after a RAM write.
        epaperdisplay_epaper_core_send_stop_sequence_guarded(self);
        self->awake = false;
    }
    #endif

    // run start sequence
    self->bus.bus_reset(self->bus.bus);

    epaperdisplay_epaper_core_delay_ms(self, self->start_up_time_ms);

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (!epaperdisplay_epaper_core_send_command_sequence(self, true, self->start_sequence, self->start_sequence_len) ||
        epaperdisplay_epaper_core_interrupted(self)) {
        return;
    }
    self->awake = true;
    self->needs_start = false;
    #else
    epaperdisplay_epaper_core_send_command_sequence(self, true, self->start_sequence, self->start_sequence_len);
    #endif
}

#if CIRCUITPY_EPAPERDISPLAY_FAST
void epaperdisplay_backend_full_area(epaperdisplay_epaper_core_t *self, displayio_area_t *area) {
    *area = self->core.area;
    area->next = NULL;
}

bool epaperdisplay_backend_clip_area(epaperdisplay_epaper_core_t *self, const displayio_area_t *area,
    displayio_area_t *clipped) {
    bool ok = displayio_display_core_clip_area(&self->core, area, clipped);
    clipped->next = NULL;
    return ok;
}

void epaperdisplay_backend_render_area(epaperdisplay_epaper_core_t *self, const displayio_area_t *area,
    uint8_t *frame, uint16_t frame_stride, bool white) {
    uint16_t width = displayio_area_width(area);
    uint16_t bytes_per_row = width / 8;
    if (white) {
        for (int16_t row = area->y1; row < area->y2; row++) {
            memset(frame + row * frame_stride + area->x1 / 8, self->black_bits_inverted ? 0x00 : 0xff,
                bytes_per_row);
        }
        return;
    }
    uint16_t rows_per_buffer = MAX(1, 128 * 32 / width);
    size_t words = (rows_per_buffer * width + 31) / 32;
    uint32_t buffer[words];
    uint32_t mask[words];

    self->core.colorspace.grayscale = true;
    self->core.colorspace.grayscale_bit = 7;
    for (int16_t y = area->y1; y < area->y2; y += rows_per_buffer) {
        displayio_area_t subrectangle = {
            .x1 = area->x1,
            .y1 = y,
            .x2 = area->x2,
            .y2 = MIN(y + rows_per_buffer, area->y2),
        };
        memset(mask, 0, sizeof(mask));
        memset(buffer, 0, sizeof(buffer));
        displayio_display_core_fill_area(&self->core, &subrectangle, mask, buffer);
        if (self->black_bits_inverted) {
            for (size_t k = 0; k < words; k++) {
                buffer[k] = ~buffer[k];
            }
        }
        const uint8_t *in = (const uint8_t *)buffer;
        for (int16_t row = subrectangle.y1; row < subrectangle.y2; row++) {
            memcpy(frame + row * frame_stride + area->x1 / 8, in, bytes_per_row);
            in += bytes_per_row;
        }
        // Run background tasks like a regular refresh does.
        RUN_BACKGROUND_TASKS;
    }
}

void epaperdisplay_backend_finish_refresh(epaperdisplay_epaper_core_t *self) {
    displayio_display_core_finish_refresh(&self->core);
}

void epaperdisplay_backend_set_full_refresh(epaperdisplay_epaper_core_t *self, bool full_refresh) {
    self->core.full_refresh = full_refresh;
}

void epaperdisplay_backend_set_refresh_in_progress(epaperdisplay_epaper_core_t *self, bool in_progress) {
    self->core.refresh_in_progress = in_progress;
}
#endif

#if CIRCUITPY_EPAPERDISPLAY_FAST
static bool epaperdisplay_epaperdisplay_refresh_area(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *area, bool write_previous);

static void epaperdisplay_epaperdisplay_finish_refresh(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas) {
    // Actually refresh the display now that all pixel RAM has been updated.
    epaperdisplay_epaper_core_send_command_sequence(&self->epaper, false, self->epaper.refresh_sequence, self->epaper.refresh_sequence_len);
    const uint64_t refresh_start = supervisor_ticks_ms64();
    bool previous_written = true;
    mp_obj_t raised = MP_OBJ_NULL;

    if (self->epaper.write_previous_ram_command != NO_COMMAND && areas != NULL) {
        // Copy the new image into the previous image RAM so the next partial update compares
        // against what is on screen. The controller ignores RAM writes while it is updating.
        if (self->epaper.busy.base.type == &mp_type_NoneType) {
            common_hal_time_delay_ms(self->epaper.refresh_time);
        } else {
            previous_written = epaperdisplay_epaper_core_wait_for_update_end(&self->epaper);
        }
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            while (areas != NULL && previous_written) {
                previous_written = !mp_hal_is_interrupted() &&
                    epaperdisplay_epaperdisplay_refresh_area(self, areas, true);
                areas = areas->next;
            }
            nlr_pop();
        } else {
            // Ctrl-C raises from a transfer in progress, and that transaction is still open.
            raised = nlr.ret_val;
            previous_written = false;
            if (!displayio_display_bus_is_free(&self->epaper.bus)) {
                displayio_display_bus_end_transaction(&self->epaper.bus);
            }
        }
    }

    supervisor_enable_tick();
    self->epaper.refreshing = true;

    displayio_display_core_finish_refresh(&self->epaper.core);
    // The update began before the wait above, and refresh_time and seconds_per_frame count from it.
    self->epaper.core.last_refresh = refresh_start;
    if (!previous_written) {
        // The previous RAM is stale, so the next update has to write everything.
        self->epaper.core.full_refresh = true;
    }
    if (raised != MP_OBJ_NULL) {
        nlr_jump(raised);
    }
}
#else
static void epaperdisplay_epaperdisplay_finish_refresh(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas) {
    // Actually refresh the display now that all pixel RAM has been updated.
    epaperdisplay_epaper_core_send_command_sequence(&self->epaper, false, self->epaper.refresh_sequence, self->epaper.refresh_sequence_len);

    supervisor_enable_tick();
    self->epaper.refreshing = true;

    displayio_display_core_finish_refresh(&self->epaper.core);
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

void common_hal_epaperdisplay_epaperdisplay_set_rotation(epaperdisplay_epaperdisplay_obj_t *self, int rotation) {
    displayio_display_core_set_rotation_and_update(&self->epaper.core, rotation, &self->base);
}

static bool epaperdisplay_epaperdisplay_refresh_area(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *area, bool write_previous) {
    uint16_t buffer_size = 128; // In uint32_ts

    displayio_area_t clipped;
    // Clip the area to the display by overlapping the areas. If there is no overlap then we're done.
    if (!displayio_display_core_clip_area(&self->epaper.core, area, &clipped)) {
        return true;
    }
    uint16_t subrectangles = 1;
    uint16_t rows_per_buffer = displayio_area_height(&clipped);
    uint8_t pixels_per_word = (sizeof(uint32_t) * 8) / self->epaper.core.colorspace.depth;
    uint16_t pixels_per_buffer = displayio_area_size(&clipped);
    if (displayio_area_size(&clipped) > buffer_size * pixels_per_word) {
        rows_per_buffer = buffer_size * pixels_per_word / displayio_area_width(&clipped);
        if (rows_per_buffer == 0) {
            rows_per_buffer = 1;
        }
        subrectangles = displayio_area_height(&clipped) / rows_per_buffer;
        if (displayio_area_height(&clipped) % rows_per_buffer != 0) {
            subrectangles++;
        }
        pixels_per_buffer = rows_per_buffer * displayio_area_width(&clipped);
        buffer_size = pixels_per_buffer / pixels_per_word;
        if (pixels_per_buffer % pixels_per_word) {
            buffer_size += 1;
        }
    }

    // Allocated and shared as a uint32_t array so the compiler knows the
    // alignment everywhere.
    uint32_t buffer[buffer_size];
    volatile uint32_t mask_length = (pixels_per_buffer / 32) + 1;
    uint32_t mask[mask_length];

    uint8_t passes = 1;
    if (self->epaper.write_color_ram_command != NO_COMMAND && !write_previous) {
        passes = 2;
    }
    for (uint8_t pass = 0; pass < passes; pass++) {
        uint16_t remaining_rows = displayio_area_height(&clipped);

        if (self->epaper.bus.row_command != NO_COMMAND) {
            displayio_display_bus_set_region_to_update(&self->epaper.bus, &self->epaper.core.colorspace, &clipped);
        }

        uint8_t write_command = self->epaper.write_black_ram_command;
        if (pass == 1) {
            write_command = self->epaper.write_color_ram_command;
        }
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        else if (write_previous) {
            write_command = self->epaper.write_previous_ram_command;
        }
        #endif
        if (!displayio_display_bus_begin_transaction(&self->epaper.bus)) {
            // Display bus not available now.
            return false;
        }
        self->epaper.bus.send(self->epaper.bus.bus, DISPLAY_COMMAND, self->epaper.chip_select, &write_command, 1);
        displayio_display_bus_end_transaction(&self->epaper.bus);

        for (uint16_t j = 0; j < subrectangles; j++) {
            displayio_area_t subrectangle = {
                .x1 = clipped.x1,
                .y1 = clipped.y1 + rows_per_buffer * j,
                .x2 = clipped.x2,
                .y2 = clipped.y1 + rows_per_buffer * (j + 1)
            };
            if (remaining_rows < rows_per_buffer) {
                subrectangle.y2 = subrectangle.y1 + remaining_rows;
            }
            remaining_rows -= rows_per_buffer;


            uint16_t subrectangle_size_bytes = displayio_area_size(&subrectangle) / (8 / self->epaper.core.colorspace.depth);

            memset(mask, 0, mask_length * sizeof(mask[0]));
            memset(buffer, 0, buffer_size * sizeof(buffer[0]));

            if (!self->epaper.acep) {
                self->epaper.core.colorspace.grayscale = true;
                self->epaper.core.colorspace.grayscale_bit = 7;
            }
            if (pass == 1) {
                if (self->epaper.grayscale) { // 4-color grayscale
                    self->epaper.core.colorspace.grayscale_bit = 6;
                    displayio_display_core_fill_area(&self->epaper.core, &subrectangle, mask, buffer);
                } else if (self->epaper.core.colorspace.tricolor) {
                    self->epaper.core.colorspace.grayscale = false;
                    displayio_display_core_fill_area(&self->epaper.core, &subrectangle, mask, buffer);
                } else if (self->epaper.core.colorspace.sixcolor || self->epaper.core.colorspace.sevencolor) {
                    displayio_display_core_fill_area(&self->epaper.core, &subrectangle, mask, buffer);
                }
            } else {
                displayio_display_core_fill_area(&self->epaper.core, &subrectangle, mask, buffer);
            }

            // Invert it all.
            if ((pass == 1 && self->epaper.color_bits_inverted) ||
                (pass == 0 && self->epaper.black_bits_inverted)) {
                for (uint16_t k = 0; k < buffer_size; k++) {
                    buffer[k] = ~buffer[k];
                }
            }

            if (!displayio_display_bus_begin_transaction(&self->epaper.bus)) {
                // Can't acquire display bus; skip the rest of the data. Try next display.
                return false;
            }
            self->epaper.bus.send(self->epaper.bus.bus, DISPLAY_DATA, self->epaper.chip_select, (uint8_t *)buffer, subrectangle_size_bytes);
            displayio_display_bus_end_transaction(&self->epaper.bus);

            // Run background tasks so they can run during an explicit refresh.
            // Auto-refresh won't run background tasks here because it is a background task itself.
            RUN_BACKGROUND_TASKS;

            // Run USB background tasks so they can run during an implicit refresh.
            #if CIRCUITPY_TINYUSB
            usb_background();
            #endif
        }
    }

    return true;
}

static bool _clean_area(epaperdisplay_epaperdisplay_obj_t *self) {
    uint16_t width = displayio_display_core_get_width(&self->epaper.core);
    uint16_t height = displayio_display_core_get_height(&self->epaper.core);

    uint8_t buffer[width / 2];
    memset(buffer, 0x77, width / 2);

    uint8_t write_command = self->epaper.write_black_ram_command;
    if (!displayio_display_bus_begin_transaction(&self->epaper.bus)) {
        return false;
    }
    self->epaper.bus.send(self->epaper.bus.bus, DISPLAY_COMMAND, self->epaper.chip_select, &write_command, 1);
    displayio_display_bus_end_transaction(&self->epaper.bus);

    for (uint16_t j = 0; j < height; j++) {
        if (!displayio_display_bus_begin_transaction(&self->epaper.bus)) {
            // Can't acquire display bus; skip the rest of the data. Try next display.
            return false;
        }
        self->epaper.bus.send(self->epaper.bus.bus, DISPLAY_DATA, self->epaper.chip_select, buffer, width / 2);
        displayio_display_bus_end_transaction(&self->epaper.bus);

        // TODO(tannewt): Make refresh displays faster so we don't starve other
        // background tasks.
        #if CIRCUITPY_TINYUSB
        usb_background();
        #endif
    }

    return true;
}

bool common_hal_epaperdisplay_epaperdisplay_refresh(epaperdisplay_epaperdisplay_obj_t *self) {

    if (self->epaper.refreshing && self->epaper.busy.base.type == &digitalio_digitalinout_type) {
        if (common_hal_digitalio_digitalinout_get_value(&self->epaper.busy) != self->epaper.busy_state) {
            supervisor_disable_tick();
            self->epaper.refreshing = false;
            #if CIRCUITPY_EPAPERDISPLAY_FAST
            if (!self->epaper.fast) {
                // Run stop sequence but don't wait for busy after it, because some panels set busy
                // when sleeping.
                epaperdisplay_epaper_core_send_command_sequence(&self->epaper, false, self->epaper.stop_sequence, self->epaper.stop_sequence_len);
            }
            #else
            // Run stop sequence but don't wait for busy after it, because some panels set busy
            // when sleeping.
            epaperdisplay_epaper_core_send_command_sequence(&self->epaper, false, self->epaper.stop_sequence, self->epaper.stop_sequence_len);
            #endif
        } else {
            #if CIRCUITPY_EPAPERDISPLAY_FAST
            epaperdisplay_lag_give_up_if_stuck(&self->epaper);
            #endif
            return false;
        }
    }
    if (self->epaper.core.current_group == NULL) {
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        // Nothing to draw, but the drive already started is still owed.
        if (self->epaper.lag.frames > 0 && self->epaper.awake) {
            return epaperdisplay_lag_refresh(&self->epaper, NULL, 0);
        }
        #endif
        return true;
    }
    // Refresh at seconds per frame rate.
    if (common_hal_epaperdisplay_epaperdisplay_get_time_to_refresh(self) > 0) {
        return false;
    }
    if (!displayio_display_bus_is_free(&self->epaper.bus)) {
        // Can't acquire display bus; skip updating this display. Try next display.
        return false;
    }
    const displayio_area_t *current_area = epaperdisplay_epaperdisplay_get_refresh_areas(self);
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->epaper.lag.frames > 0) {
        return epaperdisplay_lag_refresh(&self->epaper, current_area, 0);
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
    if (current_area == NULL) {
        return true;
    }
    if (self->epaper.acep) {
        epaperdisplay_epaper_core_start_refresh(&self->epaper);
        _clean_area(self);
        epaperdisplay_epaperdisplay_finish_refresh(self, NULL);
        while (self->epaper.refreshing && !mp_hal_is_interrupted()) {
            RUN_BACKGROUND_TASKS;
        }
    }
    if (mp_hal_is_interrupted()) {
        return false;
    }

    epaperdisplay_epaper_core_start_refresh(&self->epaper);
    const displayio_area_t *first_area = current_area;
    while (current_area != NULL) {
        epaperdisplay_epaperdisplay_refresh_area(self, current_area, false);
        current_area = current_area->next;
    }
    epaperdisplay_epaperdisplay_finish_refresh(self, first_area);
    return true;
}

void release_epaperdisplay(epaperdisplay_epaperdisplay_obj_t *self) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    epaperdisplay_lag_shutdown(&self->epaper);
    epaperdisplay_lag_free(&self->epaper);
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->epaper.refreshing) {
        // Mark the refresh as done before waiting for busy. Otherwise the background task could
        // end the refresh during the wait by running the stop sequence, and on panels that set
        // busy when sleeping, such as the SSD1680, the wait would never end.
        supervisor_disable_tick();
        self->epaper.refreshing = false;
        epaperdisplay_epaper_core_wait_for_busy(&self->epaper);

        // Run stop sequence but don't wait for busy after it, because some panels set busy when
        // sleeping.
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        if (self->epaper.fast) {
            epaperdisplay_epaper_core_send_stop_sequence_guarded(&self->epaper);
        } else {
            epaperdisplay_epaper_core_send_command_sequence(&self->epaper, false, self->epaper.stop_sequence, self->epaper.stop_sequence_len);
        }
        #else
        epaperdisplay_epaper_core_send_command_sequence(&self->epaper, false, self->epaper.stop_sequence, self->epaper.stop_sequence_len);
        #endif
    }
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    else if (self->epaper.fast && self->epaper.awake) {
        epaperdisplay_epaper_core_send_stop_sequence_guarded(&self->epaper);
    }
    self->epaper.awake = false;
    #endif
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    epaperdisplay_lag_release_interrupts(&self->epaper);
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST

    release_display_core(&self->epaper.core);
    if (self->epaper.busy.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->epaper.busy);
    }
}

void epaperdisplay_epaperdisplay_reset(epaperdisplay_epaperdisplay_obj_t *self) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->epaper.lag.frames > 0) {
        // Only a primary display gets here; reset_displays() has released the others. The heap is
        // still here but the sequences won't be, and the display would otherwise persist to draw
        // the splash with them. Finish now and stay asleep. Its bus still holds the pins until
        // the next VM ends or release_displays().
        epaperdisplay_epaper_core_end_held_bus(&self->epaper);
        release_epaperdisplay(self);
        self->base.type = &mp_type_NoneType;
        return;
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
    displayio_display_core_set_root_group(&self->epaper.core, &circuitpython_splash);
    self->epaper.core.full_refresh = true;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    // Let the panel sleep after the next refresh when code isn't running.
    if (self->epaper.fast && self->epaper.awake && !self->epaper.refreshing) {
        // Nothing can catch a raise from a send. A reload may be pending.
        epaperdisplay_epaper_core_end_held_bus(&self->epaper);
        epaperdisplay_epaper_core_send_stop_sequence_guarded(&self->epaper);
        self->epaper.awake = false;
    }
    self->epaper.fast = false;
    #endif
}

uint16_t common_hal_epaperdisplay_epaperdisplay_get_width(epaperdisplay_epaperdisplay_obj_t *self) {
    return epaperdisplay_epaper_core_get_width(&self->epaper);
}

uint16_t common_hal_epaperdisplay_epaperdisplay_get_height(epaperdisplay_epaperdisplay_obj_t *self) {
    return epaperdisplay_epaper_core_get_height(&self->epaper);
}

uint16_t common_hal_epaperdisplay_epaperdisplay_get_rotation(epaperdisplay_epaperdisplay_obj_t *self) {
    return epaperdisplay_epaper_core_get_rotation(&self->epaper);
}

mp_obj_t common_hal_epaperdisplay_epaperdisplay_get_root_group(epaperdisplay_epaperdisplay_obj_t *self) {
    return epaperdisplay_epaper_core_get_root_group(&self->epaper);
}

mp_obj_t common_hal_epaperdisplay_epaperdisplay_get_bus(epaperdisplay_epaperdisplay_obj_t *self) {
    return epaperdisplay_epaper_core_get_bus(&self->epaper);
}

uint32_t common_hal_epaperdisplay_epaperdisplay_get_time_to_refresh(epaperdisplay_epaperdisplay_obj_t *self) {
    return epaperdisplay_epaper_core_get_time_to_refresh(&self->epaper);
}

bool common_hal_epaperdisplay_epaperdisplay_get_busy(epaperdisplay_epaperdisplay_obj_t *self) {
    return epaperdisplay_epaper_core_get_busy(&self->epaper);
}

void epaperdisplay_epaperdisplay_change_refresh_mode_parameters(epaperdisplay_epaperdisplay_obj_t *self,
    mp_buffer_info_t *start_sequence, mp_buffer_info_t *refresh_sequence, float seconds_per_frame) {
    epaperdisplay_epaper_core_change_refresh_mode_parameters(&self->epaper,
        start_sequence, refresh_sequence, seconds_per_frame);
}

void epaperdisplay_epaperdisplay_background(epaperdisplay_epaperdisplay_obj_t *self) {
    epaperdisplay_epaper_core_background(&self->epaper);
}

void epaperdisplay_epaperdisplay_collect_ptrs(epaperdisplay_epaperdisplay_obj_t *self) {
    epaperdisplay_epaper_core_collect_ptrs(&self->epaper);
}

#if CIRCUITPY_EPAPERDISPLAY_FAST
void epaperdisplay_epaperdisplay_prepare_release(epaperdisplay_epaperdisplay_obj_t *self) {
    epaperdisplay_epaper_core_prepare_release(&self->epaper);
}

void epaperdisplay_epaperdisplay_end_held_bus(epaperdisplay_epaperdisplay_obj_t *self) {
    epaperdisplay_epaper_core_end_held_bus(&self->epaper);
}

bool common_hal_epaperdisplay_epaperdisplay_clean(epaperdisplay_epaperdisplay_obj_t *self, uint8_t frames, bool full) {
    return epaperdisplay_epaper_core_clean(&self->epaper, frames, full);
}
#endif
