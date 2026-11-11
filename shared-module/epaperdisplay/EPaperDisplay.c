// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/epaperdisplay/EPaperDisplay.h"

#include "py/gc.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#if CIRCUITPY_EPAPERDISPLAY_FAST && CIRCUITPY_WATCHDOG
#include "shared-bindings/watchdog/WatchDogTimer.h"
#endif
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
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

#define DELAY 0x80

// lag_drive.c calls these only when it is built. Otherwise they stay static, as before it.
#if CIRCUITPY_EPAPERDISPLAY_FAST
#define LAG_SHARED
#else
#define LAG_SHARED static
#endif

void common_hal_epaperdisplay_epaperdisplay_construct(epaperdisplay_epaperdisplay_obj_t *self,
    const epaperdisplay_construct_args_t *args) {
    uint16_t color_depth = 1;
    bool core_grayscale = true;
    if (args->highlight_color != 0x000000) {
        self->core.colorspace.tricolor = true;
        self->core.colorspace.tricolor_hue = displayio_colorconverter_compute_hue(args->highlight_color);
    } else {
        self->core.colorspace.tricolor = false;
    }
    if (args->highlight_color != 0x000000 && args->highlight_color2 != 0x000000) {
        self->core.colorspace.tricolor = false;
        self->core.colorspace.fourcolor = true;
        self->core.colorspace.fourcolor_hue = displayio_colorconverter_compute_hue(args->highlight_color2);
    } else {
        self->core.colorspace.fourcolor = false;
    }
    self->acep = args->acep || args->spectra6;
    self->core.colorspace.sixcolor = args->spectra6;
    self->core.colorspace.sevencolor = args->acep;
    bool grayscale = args->grayscale;
    if (self->acep) {
        color_depth = 4; // bits. 7 colors + clean
        grayscale = false;
        core_grayscale = false;
    }
    if ((args->highlight_color != 0x000000 || args->highlight_color2 != 0x000000) && args->write_color_ram_command == NO_COMMAND) {
        color_depth = 2;
        core_grayscale = false;
        grayscale = false;
    }

    displayio_display_core_construct(&self->core, args->width, args->height, args->rotation, color_depth, core_grayscale, true, 1, true, true);
    displayio_display_bus_construct(&self->bus, args->bus, args->ram_width, args->ram_height,
        args->colstart, args->rowstart,
        args->set_column_window_command, args->set_row_window_command, args->set_current_column_command, args->set_current_row_command,
        false /* data_as_commands */, args->always_toggle_chip_select,
        false /* SH1107_addressing */, args->address_little_endian);

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    self->lag.slots = NULL;
    self->lag.frames = 0;
    self->lag.busy_deadline = 0;
    self->lag.bus_held = false;
    self->lag.state_lost = false;
    self->lag.slot_overwriting = false;
    self->lag.commit = LAG_COMMIT_NONE;
    self->lag.holding_interrupts = false;
    self->lag.held_exception = MP_OBJ_NULL;
    self->clean_sequence = args->clean_sequence;
    self->clean_sequence_len = args->clean_sequence_len;
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST

    self->write_black_ram_command = args->write_black_ram_command;
    self->black_bits_inverted = args->black_bits_inverted;
    self->write_color_ram_command = args->write_color_ram_command;
    self->color_bits_inverted = args->color_bits_inverted;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    self->write_previous_ram_command = args->write_previous_ram_command;
    #endif
    self->refresh_time = args->refresh_time * 1000;
    self->busy_state = args->busy_state;
    self->refreshing = false;
    self->milliseconds_per_frame = args->seconds_per_frame * 1000;
    self->chip_select = args->always_toggle_chip_select ? CHIP_SELECT_TOGGLE_EVERY_BYTE : CHIP_SELECT_UNTOUCHED;
    self->grayscale = grayscale;

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (args->lag_frames > 0) {
        epaperdisplay_lag_construct(self, args->lag_frames);
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST

    self->start_sequence = args->start_sequence;
    self->start_sequence_len = args->start_sequence_len;
    self->start_up_time_ms = args->start_up_time * 1000;
    self->stop_sequence = args->stop_sequence;
    self->stop_sequence_len = args->stop_sequence_len;
    self->refresh_sequence = args->refresh_sequence;
    self->refresh_sequence_len = args->refresh_sequence_len;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    self->fast = args->fast;
    self->awake = false;
    self->needs_start = false;
    #endif

    self->busy.base.type = &mp_type_NoneType;
    self->two_byte_sequence_length = args->two_byte_sequence_length;
    if (args->busy_pin != NULL) {
        self->busy.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->busy, args->busy_pin);
        #if CIRCUITPY_BULK_RESET
        common_hal_never_reset_pin(args->busy_pin);
        #endif
    }

    // Clear the color memory if it isn't in use.
    if (args->highlight_color == 0x00 && args->highlight_color2 == 0x00 && args->write_color_ram_command != NO_COMMAND) {
        // TODO: Clear
    }

    // Set the group after initialization otherwise we may send pixels while we delay in
    // initialization.
    common_hal_epaperdisplay_epaperdisplay_set_root_group(self, &circuitpython_splash);
}

bool common_hal_epaperdisplay_epaperdisplay_set_root_group(epaperdisplay_epaperdisplay_obj_t *self, displayio_group_t *root_group) {
    return displayio_display_core_set_root_group(&self->core, root_group);
}

static const displayio_area_t *epaperdisplay_epaperdisplay_get_refresh_areas(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->core.full_refresh) {
        self->core.area.next = NULL;
        return &self->core.area;
    }
    const displayio_area_t *first_area = NULL;
    if (self->core.current_group != NULL) {
        first_area = displayio_group_get_refresh_areas(self->core.current_group, NULL);
    }
    if (first_area != NULL && self->bus.row_command == NO_COMMAND) {
        // Do a full refresh if the display doesn't support partial updates.
        self->core.area.next = NULL;
        return &self->core.area;
    }
    return first_area;
}

uint16_t common_hal_epaperdisplay_epaperdisplay_get_width(epaperdisplay_epaperdisplay_obj_t *self) {
    return displayio_display_core_get_width(&self->core);
}

uint16_t common_hal_epaperdisplay_epaperdisplay_get_height(epaperdisplay_epaperdisplay_obj_t *self) {
    return displayio_display_core_get_height(&self->core);
}

#if CIRCUITPY_EPAPERDISPLAY_FAST
// BUSY can take a moment to rise after an update command, and reading it low before then would
// look like a finished update. If it never rises, nothing has started or it was too quick to see,
// so this returns false after 2 ms. Short enough that an interrupt doesn't have to end it.
bool epaperdisplay_epaperdisplay_wait_for_update_start(epaperdisplay_epaperdisplay_obj_t *self) {
    for (uint8_t wait = 0; wait < 200; wait++) {
        if (common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state) {
            return true;
        }
        common_hal_mcu_delay_us(10);
    }
    return false;
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

#if CIRCUITPY_EPAPERDISPLAY_FAST
// When a wait for BUSY or the bus gives up, or 0 for never. Lag drive always has an end, so a
// stuck panel can't hold up a caller that holds interrupts or has no way to be interrupted.
static uint64_t display_wait_deadline(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag.busy_deadline != 0 || self->lag.frames == 0) {
        return self->lag.busy_deadline;
    }
    return supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
}

// Between hold and release a waveform or a panel reset is under way, and cutting it short leaves
// the panel dark. Whatever interrupts (Ctrl-C, a reload, a watchdog) waits for the release.
bool epaperdisplay_epaperdisplay_interrupted(epaperdisplay_epaperdisplay_obj_t *self) {
    if (!self->lag.holding_interrupts) {
        return mp_hal_is_interrupted();
    }
    if (mp_hal_is_interrupted()) {
        if (self->lag.held_exception == MP_OBJ_NULL) {
            self->lag.held_exception = MP_STATE_THREAD(mp_pending_exception);
        }
        MP_STATE_THREAD(mp_pending_exception) = MP_OBJ_NULL;
    }
    #if CIRCUITPY_WATCHDOG
    // A stored waveform takes seconds, and a reset mid-waveform leaves the panel dark. These waits
    // are bounded, so a hang still ends.
    if (common_hal_watchdog_get_mode(&common_hal_mcu_watchdogtimer_obj) != WATCHDOGMODE_NONE) {
        common_hal_watchdog_feed(&common_hal_mcu_watchdogtimer_obj);
    }
    #endif
    return false;
}

static bool display_send_command(epaperdisplay_epaperdisplay_obj_t *self, const uint8_t *command,
    const uint8_t *data, uint32_t length) {
    for (uint8_t attempt = 0; attempt < 3; attempt++) {
        if (epaperdisplay_lag_send(self, DISPLAY_COMMAND, command, 1) && epaperdisplay_lag_send(self, DISPLAY_DATA, data, length)) {
            return true;
        }
        // The send stopped, and it may have arrived. If it was an update, sending it again would
        // drive every pixel a second time. BUSY high says it landed. A command with no data could
        // be an update that BUSY hasn't shown yet, so don't risk it: the drive is now unknown.
        if (epaperdisplay_epaperdisplay_wait_for_update_start(self)) {
            return true;
        }
        if (length == 0) {
            self->lag.state_lost = true;
            return false;
        }
    }
    return false;
}

// A delay ends early when interrupted, which a held interrupt must not do.
void epaperdisplay_epaperdisplay_delay_ms(epaperdisplay_epaperdisplay_obj_t *self, uint32_t delay_ms) {
    uint64_t start = supervisor_ticks_ms64();
    uint32_t elapsed = 0;
    while (true) {
        common_hal_time_delay_ms(delay_ms - elapsed);
        elapsed = supervisor_ticks_ms64() - start;
        if (!self->lag.holding_interrupts || elapsed >= delay_ms) {
            return;
        }
        epaperdisplay_epaperdisplay_interrupted(self);
    }
}
#else // CIRCUITPY_EPAPERDISPLAY_FAST
#define display_wait_deadline(self) ((uint64_t)0)
#define epaperdisplay_epaperdisplay_interrupted(self) mp_hal_is_interrupted()
#define display_send_command(self, command, data, length) ( \
    (self)->bus.send((self)->bus.bus, DISPLAY_COMMAND, (self)->chip_select, command, 1), \
    (self)->bus.send((self)->bus.bus, DISPLAY_DATA, (self)->chip_select, data, length), \
    true)
#define epaperdisplay_epaperdisplay_delay_ms(self, delay_ms) common_hal_time_delay_ms(delay_ms)
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

LAG_SHARED void epaperdisplay_epaperdisplay_wait_for_busy(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->busy.base.type == &mp_type_NoneType) {
        return;
    }
    const uint64_t deadline = display_wait_deadline(self);
    while (common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state &&
           !epaperdisplay_epaperdisplay_interrupted(self)) {
        if (deadline != 0 && supervisor_ticks_ms64() > deadline) {
            return;
        }
        RUN_BACKGROUND_TASKS;
    }
}

// True if every command went out. Lag drive counts an update only if its last command did.
LAG_SHARED bool epaperdisplay_epaperdisplay_send_command_sequence(epaperdisplay_epaperdisplay_obj_t *self,
    bool should_wait_for_busy, const uint8_t *sequence, uint32_t sequence_len) {
    uint32_t i = 0;
    while (i < sequence_len) {
        const uint8_t *cmd = sequence + i;
        uint8_t data_size = *(cmd + 1);
        bool delay = (data_size & DELAY) != 0;
        const uint8_t *data = cmd + 2;
        data_size &= ~DELAY;
        if (self->two_byte_sequence_length) {
            data_size = ((data_size & ~DELAY) << 8) + *(cmd + 2);
            data = cmd + 3;
        }
        bool have_bus;
        const uint64_t deadline = display_wait_deadline(self);
        while (!(have_bus = displayio_display_bus_begin_transaction(&self->bus)) &&
               !epaperdisplay_epaperdisplay_interrupted(self) && (deadline == 0 || supervisor_ticks_ms64() <= deadline)) {
            RUN_BACKGROUND_TASKS;
        }
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        if (!have_bus && self->lag.frames > 0) {
            return false;
        }
        self->lag.bus_held = have_bus;
        #endif // CIRCUITPY_EPAPERDISPLAY_FAST
        const bool sent = display_send_command(self, cmd, data, data_size);
        displayio_display_bus_end_transaction(&self->bus);
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        self->lag.bus_held = false;
        #endif // CIRCUITPY_EPAPERDISPLAY_FAST
        if (!sent) {
            return false;
        }
        uint16_t delay_length_ms = 0;
        if (delay) {
            data_size++;
            delay_length_ms = *(cmd + 1 + data_size + self->two_byte_sequence_length);
            if (delay_length_ms == 255) {
                delay_length_ms = 500;
            }
        }
        epaperdisplay_epaperdisplay_delay_ms(self, delay_length_ms);
        if (should_wait_for_busy) {
            #if CIRCUITPY_EPAPERDISPLAY_FAST
            if (self->lag.frames > 0) {
                epaperdisplay_epaperdisplay_wait_for_update_start(self);
            }
            #endif
            epaperdisplay_epaperdisplay_wait_for_busy(self);
        }
        #if !CIRCUITPY_EPAPERDISPLAY_FAST
        if (mp_hal_is_interrupted()) {
            return true;
        }
        #endif
        i += 2 + data_size;
        if (self->two_byte_sequence_length) {
            i++;
        }
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        if (epaperdisplay_epaperdisplay_interrupted(self)) {
            return i >= sequence_len;
        }
        #endif
    }
    return true;
}

void epaperdisplay_epaperdisplay_change_refresh_mode_parameters(epaperdisplay_epaperdisplay_obj_t *self,
    mp_buffer_info_t *start_sequence, mp_buffer_info_t *refresh_sequence, float seconds_per_frame) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->lag.frames > 0) {
        // Finish the drive with the waveform that started it, and rewrite both planes after.
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            epaperdisplay_lag_flush(self);
            epaperdisplay_epaperdisplay_wait_for_busy(self);
            nlr_pop();
        } else {
            epaperdisplay_lag_raise(self, nlr.ret_val);
        }
        if (self->lag.settle > 0 || self->lag.commit != LAG_COMMIT_NONE) {
            // Still owed: keep the mode that started it.
            if (!mp_hal_is_interrupted()) {
                mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_display_bus);
            }
            return;
        }
        self->lag.force_full = true;
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
    self->start_sequence = (uint8_t *)start_sequence->buf;
    self->start_sequence_len = start_sequence->len;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    // NULL keeps the current refresh sequence.
    if (refresh_sequence != NULL) {
        self->refresh_sequence = (uint8_t *)refresh_sequence->buf;
        self->refresh_sequence_len = refresh_sequence->len;
    }
    #endif
    self->milliseconds_per_frame = seconds_per_frame * 1000;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    self->needs_start = true;
    #endif
}

#if CIRCUITPY_EPAPERDISPLAY_FAST
// For a panel kept awake. Ctrl-C raises from a send in progress, and that transaction is still
// open. The caller still has things to do first, so the interrupt raises later.
static void send_stop_sequence_guarded(epaperdisplay_epaperdisplay_obj_t *self) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
        nlr_pop();
    } else {
        if (!displayio_display_bus_is_free(&self->bus)) {
            displayio_display_bus_end_transaction(&self->bus);
        }
        mp_sched_exception(nlr.ret_val);
    }
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

LAG_SHARED void epaperdisplay_epaperdisplay_start_refresh(epaperdisplay_epaperdisplay_obj_t *self) {
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
        epaperdisplay_epaperdisplay_wait_for_busy(self);
        // SSD168x controllers commit the last RAM byte when the next command arrives, so a reset
        // can't come straight after a RAM write.
        send_stop_sequence_guarded(self);
        self->awake = false;
    }
    #endif

    // run start sequence
    self->bus.bus_reset(self->bus.bus);

    epaperdisplay_epaperdisplay_delay_ms(self, self->start_up_time_ms);

    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (!epaperdisplay_epaperdisplay_send_command_sequence(self, true, self->start_sequence, self->start_sequence_len) ||
        epaperdisplay_epaperdisplay_interrupted(self)) {
        return;
    }
    self->awake = true;
    self->needs_start = false;
    #else
    epaperdisplay_epaperdisplay_send_command_sequence(self, true, self->start_sequence, self->start_sequence_len);
    #endif
}

uint32_t common_hal_epaperdisplay_epaperdisplay_get_time_to_refresh(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->core.last_refresh == 0) {
        return 0;
    }
    // Refresh at seconds per frame rate.
    uint32_t elapsed_time = supervisor_ticks_ms64() - self->core.last_refresh;
    if (elapsed_time > self->milliseconds_per_frame) {
        return 0;
    }
    return self->milliseconds_per_frame - elapsed_time;
}

#if CIRCUITPY_EPAPERDISPLAY_FAST
static bool epaperdisplay_epaperdisplay_refresh_area(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *area, bool write_previous);

// False if the panel is still updating when the wait is over.
static bool wait_for_update_end(epaperdisplay_epaperdisplay_obj_t *self) {
    epaperdisplay_epaperdisplay_wait_for_update_start(self);
    const uint64_t deadline = supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
    bool busy;
    while ((busy = common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state) &&
           !mp_hal_is_interrupted() && supervisor_ticks_ms64() <= deadline) {
        RUN_BACKGROUND_TASKS;
    }
    return !busy;
}

static void epaperdisplay_epaperdisplay_finish_refresh(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas) {
    // Actually refresh the display now that all pixel RAM has been updated.
    epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->refresh_sequence, self->refresh_sequence_len);
    const uint64_t refresh_start = supervisor_ticks_ms64();
    bool previous_written = true;
    mp_obj_t raised = MP_OBJ_NULL;

    if (self->write_previous_ram_command != NO_COMMAND && areas != NULL) {
        // Copy the new image into the previous image RAM so the next partial update compares
        // against what is on screen. The controller ignores RAM writes while it is updating.
        if (self->busy.base.type == &mp_type_NoneType) {
            common_hal_time_delay_ms(self->refresh_time);
        } else {
            previous_written = wait_for_update_end(self);
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
            if (!displayio_display_bus_is_free(&self->bus)) {
                displayio_display_bus_end_transaction(&self->bus);
            }
        }
    }

    supervisor_enable_tick();
    self->refreshing = true;

    displayio_display_core_finish_refresh(&self->core);
    // The update began before the wait above, and refresh_time and seconds_per_frame count from it.
    self->core.last_refresh = refresh_start;
    if (!previous_written) {
        // The previous RAM is stale, so the next update has to write everything.
        self->core.full_refresh = true;
    }
    if (raised != MP_OBJ_NULL) {
        nlr_jump(raised);
    }
}
#else
static void epaperdisplay_epaperdisplay_finish_refresh(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas) {
    // Actually refresh the display now that all pixel RAM has been updated.
    epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->refresh_sequence, self->refresh_sequence_len);

    supervisor_enable_tick();
    self->refreshing = true;

    displayio_display_core_finish_refresh(&self->core);
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

mp_obj_t common_hal_epaperdisplay_epaperdisplay_get_bus(epaperdisplay_epaperdisplay_obj_t *self) {
    return self->bus.bus;
}

void common_hal_epaperdisplay_epaperdisplay_set_rotation(epaperdisplay_epaperdisplay_obj_t *self, int rotation) {
    bool transposed = (self->core.rotation == 90 || self->core.rotation == 270);
    bool will_transposed = (rotation == 90 || rotation == 270);
    if (transposed != will_transposed) {
        int tmp = self->core.width;
        self->core.width = self->core.height;
        self->core.height = tmp;
    }
    displayio_display_core_set_rotation(&self->core, rotation);
    if (self == &displays[0].epaper_display) {
        supervisor_stop_terminal();
        supervisor_start_terminal(self->core.width, self->core.height);
    }
    if (self->core.current_group != NULL) {
        displayio_group_update_transform(self->core.current_group, &self->core.transform);
    }
}

uint16_t common_hal_epaperdisplay_epaperdisplay_get_rotation(epaperdisplay_epaperdisplay_obj_t *self) {
    return self->core.rotation;
}

mp_obj_t common_hal_epaperdisplay_epaperdisplay_get_root_group(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->core.current_group == NULL) {
        return mp_const_none;
    }
    return self->core.current_group;
}

static bool epaperdisplay_epaperdisplay_refresh_area(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *area, bool write_previous) {
    uint16_t buffer_size = 128; // In uint32_ts

    displayio_area_t clipped;
    // Clip the area to the display by overlapping the areas. If there is no overlap then we're done.
    if (!displayio_display_core_clip_area(&self->core, area, &clipped)) {
        return true;
    }
    uint16_t subrectangles = 1;
    uint16_t rows_per_buffer = displayio_area_height(&clipped);
    uint8_t pixels_per_word = (sizeof(uint32_t) * 8) / self->core.colorspace.depth;
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
    if (self->write_color_ram_command != NO_COMMAND && !write_previous) {
        passes = 2;
    }
    for (uint8_t pass = 0; pass < passes; pass++) {
        uint16_t remaining_rows = displayio_area_height(&clipped);

        if (self->bus.row_command != NO_COMMAND) {
            displayio_display_bus_set_region_to_update(&self->bus, &self->core, &clipped);
        }

        uint8_t write_command = self->write_black_ram_command;
        if (pass == 1) {
            write_command = self->write_color_ram_command;
        }
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        else if (write_previous) {
            write_command = self->write_previous_ram_command;
        }
        #endif
        if (!displayio_display_bus_begin_transaction(&self->bus)) {
            // Display bus not available now.
            return false;
        }
        self->bus.send(self->bus.bus, DISPLAY_COMMAND, self->chip_select, &write_command, 1);
        displayio_display_bus_end_transaction(&self->bus);

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


            uint16_t subrectangle_size_bytes = displayio_area_size(&subrectangle) / (8 / self->core.colorspace.depth);

            memset(mask, 0, mask_length * sizeof(mask[0]));
            memset(buffer, 0, buffer_size * sizeof(buffer[0]));

            if (!self->acep) {
                self->core.colorspace.grayscale = true;
                self->core.colorspace.grayscale_bit = 7;
            }
            if (pass == 1) {
                if (self->grayscale) { // 4-color grayscale
                    self->core.colorspace.grayscale_bit = 6;
                    displayio_display_core_fill_area(&self->core, &subrectangle, mask, buffer);
                } else if (self->core.colorspace.tricolor) {
                    self->core.colorspace.grayscale = false;
                    displayio_display_core_fill_area(&self->core, &subrectangle, mask, buffer);
                } else if (self->core.colorspace.sixcolor || self->core.colorspace.sevencolor) {
                    displayio_display_core_fill_area(&self->core, &subrectangle, mask, buffer);
                }
            } else {
                displayio_display_core_fill_area(&self->core, &subrectangle, mask, buffer);
            }

            // Invert it all.
            if ((pass == 1 && self->color_bits_inverted) ||
                (pass == 0 && self->black_bits_inverted)) {
                for (uint16_t k = 0; k < buffer_size; k++) {
                    buffer[k] = ~buffer[k];
                }
            }

            if (!displayio_display_bus_begin_transaction(&self->bus)) {
                // Can't acquire display bus; skip the rest of the data. Try next display.
                return false;
            }
            self->bus.send(self->bus.bus, DISPLAY_DATA, self->chip_select, (uint8_t *)buffer, subrectangle_size_bytes);
            displayio_display_bus_end_transaction(&self->bus);

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
    uint16_t width = displayio_display_core_get_width(&self->core);
    uint16_t height = displayio_display_core_get_height(&self->core);

    uint8_t buffer[width / 2];
    memset(buffer, 0x77, width / 2);

    uint8_t write_command = self->write_black_ram_command;
    if (!displayio_display_bus_begin_transaction(&self->bus)) {
        return false;
    }
    self->bus.send(self->bus.bus, DISPLAY_COMMAND, self->chip_select, &write_command, 1);
    displayio_display_bus_end_transaction(&self->bus);

    for (uint16_t j = 0; j < height; j++) {
        if (!displayio_display_bus_begin_transaction(&self->bus)) {
            // Can't acquire display bus; skip the rest of the data. Try next display.
            return false;
        }
        self->bus.send(self->bus.bus, DISPLAY_DATA, self->chip_select, buffer, width / 2);
        displayio_display_bus_end_transaction(&self->bus);

        // TODO(tannewt): Make refresh displays faster so we don't starve other
        // background tasks.
        #if CIRCUITPY_TINYUSB
        usb_background();
        #endif
    }

    return true;
}

bool common_hal_epaperdisplay_epaperdisplay_refresh(epaperdisplay_epaperdisplay_obj_t *self) {

    if (self->refreshing && self->busy.base.type == &digitalio_digitalinout_type) {
        if (common_hal_digitalio_digitalinout_get_value(&self->busy) != self->busy_state) {
            supervisor_disable_tick();
            self->refreshing = false;
            #if CIRCUITPY_EPAPERDISPLAY_FAST
            if (!self->fast) {
                // Run stop sequence but don't wait for busy because busy is set when sleeping.
                epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            }
            #else
            // Run stop sequence but don't wait for busy because busy is set when sleeping.
            epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            #endif
        } else {
            #if CIRCUITPY_EPAPERDISPLAY_FAST
            epaperdisplay_lag_give_up_if_stuck(self);
            #endif
            return false;
        }
    }
    if (self->core.current_group == NULL) {
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        // Nothing to draw, but the drive already started is still owed.
        if (self->lag.frames > 0 && self->awake) {
            return epaperdisplay_lag_refresh(self, NULL);
        }
        #endif
        return true;
    }
    // Refresh at seconds per frame rate.
    if (common_hal_epaperdisplay_epaperdisplay_get_time_to_refresh(self) > 0) {
        return false;
    }
    if (!displayio_display_bus_is_free(&self->bus)) {
        // Can't acquire display bus; skip updating this display. Try next display.
        return false;
    }
    const displayio_area_t *current_area = epaperdisplay_epaperdisplay_get_refresh_areas(self);
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->lag.frames > 0) {
        return epaperdisplay_lag_refresh(self, current_area);
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
    if (current_area == NULL) {
        return true;
    }
    if (self->acep) {
        epaperdisplay_epaperdisplay_start_refresh(self);
        _clean_area(self);
        epaperdisplay_epaperdisplay_finish_refresh(self, NULL);
        while (self->refreshing && !mp_hal_is_interrupted()) {
            RUN_BACKGROUND_TASKS;
        }
    }
    if (mp_hal_is_interrupted()) {
        return false;
    }

    epaperdisplay_epaperdisplay_start_refresh(self);
    const displayio_area_t *first_area = current_area;
    while (current_area != NULL) {
        epaperdisplay_epaperdisplay_refresh_area(self, current_area, false);
        current_area = current_area->next;
    }
    epaperdisplay_epaperdisplay_finish_refresh(self, first_area);
    return true;
}

void epaperdisplay_epaperdisplay_background(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->refreshing) {
        bool refresh_done = false;
        if (self->busy.base.type == &digitalio_digitalinout_type) {
            bool busy = common_hal_digitalio_digitalinout_get_value(&self->busy);
            refresh_done = busy != self->busy_state;
        } else {
            refresh_done = supervisor_ticks_ms64() - self->core.last_refresh > self->refresh_time;
        }
        if (refresh_done) {
            supervisor_disable_tick();
            self->refreshing = false;
            #if CIRCUITPY_EPAPERDISPLAY_FAST
            if (!self->fast) {
                // Run stop sequence but don't wait for busy because busy is set when sleeping.
                epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            }
            #else
            // Run stop sequence but don't wait for busy because busy is set when sleeping.
            epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            #endif
        }
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        else {
            epaperdisplay_lag_give_up_if_stuck(self);
        }
        #endif
    }
}

bool common_hal_epaperdisplay_epaperdisplay_get_busy(epaperdisplay_epaperdisplay_obj_t *self) {
    epaperdisplay_epaperdisplay_background(self);
    return self->refreshing;
}

void release_epaperdisplay(epaperdisplay_epaperdisplay_obj_t *self) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    epaperdisplay_lag_shutdown(self);
    epaperdisplay_lag_free(self);
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->refreshing) {
        epaperdisplay_epaperdisplay_wait_for_busy(self);
        supervisor_disable_tick();
        self->refreshing = false;
        // Run stop sequence but don't wait for busy because busy is set when sleeping.
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        if (self->fast) {
            send_stop_sequence_guarded(self);
        } else {
            epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
        }
        #else
        epaperdisplay_epaperdisplay_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
        #endif
    }
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    else if (self->fast && self->awake) {
        send_stop_sequence_guarded(self);
    }
    self->awake = false;
    #endif
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    epaperdisplay_lag_release_interrupts(self);
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST

    release_display_core(&self->core);
    if (self->busy.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->busy);
    }
}

#if CIRCUITPY_EPAPERDISPLAY_FAST
void epaperdisplay_epaperdisplay_prepare_release(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag.frames == 0 || (!self->awake && !self->lag.state_lost)) {
        return;
    }
    const uint64_t deadline = supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
    while (!displayio_display_bus_is_free(&self->bus)) {
        mp_handle_pending(true);
        if (supervisor_ticks_ms64() > deadline) {
            mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_display_bus);
        }
        RUN_BACKGROUND_TASKS;
    }
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

#if CIRCUITPY_EPAPERDISPLAY_FAST
void epaperdisplay_epaperdisplay_end_held_bus(epaperdisplay_epaperdisplay_obj_t *self) {
    // The VM is over, so whatever holds the bus can never let it go: end it here.
    if (self->fast && !displayio_display_bus_is_free(&self->bus)) {
        displayio_display_bus_end_transaction(&self->bus);
        self->lag.bus_held = false;
    }
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

void epaperdisplay_epaperdisplay_reset(epaperdisplay_epaperdisplay_obj_t *self) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->lag.frames > 0) {
        // Only a primary display gets here; reset_displays() has released the others. The heap is
        // still here but the sequences won't be, and the display would otherwise persist to draw
        // the splash with them. Finish now and stay asleep. Its bus still holds the pins until
        // the next VM ends or release_displays().
        epaperdisplay_epaperdisplay_end_held_bus(self);
        release_epaperdisplay(self);
        self->base.type = &mp_type_NoneType;
        return;
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
    displayio_display_core_set_root_group(&self->core, &circuitpython_splash);
    self->core.full_refresh = true;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    // Let the panel sleep after the next refresh when code isn't running.
    if (self->fast && self->awake && !self->refreshing) {
        // Nothing can catch a raise from a send. A reload may be pending.
        epaperdisplay_epaperdisplay_end_held_bus(self);
        send_stop_sequence_guarded(self);
        self->awake = false;
    }
    self->fast = false;
    #endif
}

void epaperdisplay_epaperdisplay_collect_ptrs(epaperdisplay_epaperdisplay_obj_t *self) {
    displayio_display_core_collect_ptrs(&self->core);
    displayio_display_bus_collect_ptrs(&self->bus);
    gc_collect_ptr((void *)self->start_sequence);
    gc_collect_ptr((void *)self->stop_sequence);
    gc_collect_ptr((void *)self->refresh_sequence);
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    gc_collect_ptr((void *)self->clean_sequence);
    gc_collect_ptr(self->lag.held_exception);
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST
}

size_t maybe_refresh_epaperdisplay(void) {
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].epaper_display.base.type != &epaperdisplay_epaperdisplay_type ||
            displays[i].epaper_display.core.current_group != &circuitpython_splash) {
            // Skip regular displays and those not showing the splash.
            continue;
        }
        epaperdisplay_epaperdisplay_obj_t *display = &displays[i].epaper_display;
        size_t time_to_refresh = common_hal_epaperdisplay_epaperdisplay_get_time_to_refresh(display);
        if (time_to_refresh > 0) {
            return time_to_refresh;
        }
        if (common_hal_epaperdisplay_epaperdisplay_refresh(display)) {
            return 0;
        }
        // If we could refresh but it failed, then we want to retry.
        return 1;
    }
    // Return 0 if no ePaper displays are available to pretend it was updated.
    return 0;
}
