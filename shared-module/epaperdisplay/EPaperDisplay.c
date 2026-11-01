// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/epaperdisplay/EPaperDisplay.h"

#include "py/gc.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#if CIRCUITPY_EPAPERDISPLAY_LAG && CIRCUITPY_WATCHDOG
#include "shared-bindings/watchdog/WatchDogTimer.h"
#endif
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/display.h"
#if CIRCUITPY_EPAPERDISPLAY_LAG
#include "supervisor/port_heap.h"
#endif
#include "supervisor/shared/tick.h"

#if CIRCUITPY_TINYUSB
#include "supervisor/usb.h"
#endif

#include <stdint.h>
#include <string.h>

#define DELAY 0x80

// One update or stored-waveform refresh takes less. This bounds each wait for an update on a lag
// display, and the wait before writing the previous RAM. A stock display's waits have no bound.
#define EPAPER_WAIT_TIMEOUT_MS 30000

#if CIRCUITPY_EPAPERDISPLAY_LAG
// SSD168x only: NOP. These controllers keep the last RAM byte until the next command and lose it to
// a reset, so a RAM write ends with this. Lag drive is only for displays where it is harmless.
#define LAG_RAM_COMMIT_COMMAND 0x7f
#endif

#if CIRCUITPY_EPAPERDISPLAY_LAG
enum { LAG_COMMIT_NONE, LAG_COMMIT_PLANE, LAG_COMMIT_ACTIVATE };
#endif

#if CIRCUITPY_EPAPERDISPLAY_LAG
static void lag_construct(epaperdisplay_epaperdisplay_obj_t *self, uint8_t lag_frames);
static void lag_flush(epaperdisplay_epaperdisplay_obj_t *self);
static void lag_give_up_if_stuck(epaperdisplay_epaperdisplay_obj_t *self);
static void lag_free(epaperdisplay_epaperdisplay_obj_t *self);
static void lag_shutdown(epaperdisplay_epaperdisplay_obj_t *self);
static void lag_after_raise(epaperdisplay_epaperdisplay_obj_t *self);
static MP_NORETURN void lag_raise(epaperdisplay_epaperdisplay_obj_t *self, mp_obj_t raised);
static bool lag_refresh(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas);
static bool lag_clean(epaperdisplay_epaperdisplay_obj_t *self, uint8_t frames, bool full);
#endif // CIRCUITPY_EPAPERDISPLAY_LAG

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

    #if CIRCUITPY_EPAPERDISPLAY_LAG
    self->lag_slots = NULL;
    self->lag_frames = 0;
    self->lag_busy_deadline = 0;
    self->lag_bus_held = false;
    self->lag_state_lost = false;
    self->lag_slot_overwriting = false;
    self->lag_commit = LAG_COMMIT_NONE;
    self->lag_holding_interrupts = false;
    self->lag_held_exception = MP_OBJ_NULL;
    self->clean_sequence = args->clean_sequence;
    self->clean_sequence_len = args->clean_sequence_len;
    #endif // CIRCUITPY_EPAPERDISPLAY_LAG

    self->write_black_ram_command = args->write_black_ram_command;
    self->black_bits_inverted = args->black_bits_inverted;
    self->write_color_ram_command = args->write_color_ram_command;
    self->color_bits_inverted = args->color_bits_inverted;
    self->write_previous_ram_command = args->write_previous_ram_command;
    self->refresh_time = args->refresh_time * 1000;
    self->busy_state = args->busy_state;
    self->refreshing = false;
    self->milliseconds_per_frame = args->seconds_per_frame * 1000;
    self->chip_select = args->always_toggle_chip_select ? CHIP_SELECT_TOGGLE_EVERY_BYTE : CHIP_SELECT_UNTOUCHED;
    self->grayscale = grayscale;

    #if CIRCUITPY_EPAPERDISPLAY_LAG
    if (args->lag_frames > 0) {
        lag_construct(self, args->lag_frames);
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_LAG

    self->start_sequence = args->start_sequence;
    self->start_sequence_len = args->start_sequence_len;
    self->start_up_time_ms = args->start_up_time * 1000;
    self->stop_sequence = args->stop_sequence;
    self->stop_sequence_len = args->stop_sequence_len;
    self->refresh_sequence = args->refresh_sequence;
    self->refresh_sequence_len = args->refresh_sequence_len;
    self->keep_awake = args->keep_awake;
    self->awake = false;
    self->needs_start = false;

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

// BUSY can take a moment to rise after an update command, and reading it low before then would
// look like a finished update. If it never rises, nothing has started or it was too quick to see,
// so this returns false after 2 ms. Short enough that an interrupt doesn't have to end it.
static bool wait_for_update_start(epaperdisplay_epaperdisplay_obj_t *self) {
    for (uint8_t wait = 0; wait < 200; wait++) {
        if (common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state) {
            return true;
        }
        common_hal_mcu_delay_us(10);
    }
    return false;
}

#if CIRCUITPY_EPAPERDISPLAY_LAG
static bool lag_deadline_passed(epaperdisplay_epaperdisplay_obj_t *self) {
    return self->lag_busy_deadline != 0 && supervisor_ticks_ms64() > self->lag_busy_deadline;
}

// When a wait for BUSY or the bus gives up, or 0 for never. Lag drive always has an end, so a
// stuck panel can't hold up a caller that holds interrupts or has no way to be interrupted.
static uint64_t display_wait_deadline(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag_busy_deadline != 0 || self->lag_frames == 0) {
        return self->lag_busy_deadline;
    }
    return supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
}

// Between hold and release a waveform or a panel reset is under way, and cutting it short leaves
// the panel dark. Whatever interrupts (Ctrl-C, a reload, a watchdog) waits for the release.
static bool display_interrupted(epaperdisplay_epaperdisplay_obj_t *self) {
    if (!self->lag_holding_interrupts) {
        return mp_hal_is_interrupted();
    }
    if (mp_hal_is_interrupted()) {
        if (self->lag_held_exception == MP_OBJ_NULL) {
            self->lag_held_exception = MP_STATE_THREAD(mp_pending_exception);
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

static void lag_hold_interrupts(epaperdisplay_epaperdisplay_obj_t *self) {
    self->lag_holding_interrupts = true;
    if (self->lag_busy_deadline == 0) {
        self->lag_busy_deadline = supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
    }
    display_interrupted(self);
}

static void lag_release_interrupts(epaperdisplay_epaperdisplay_obj_t *self) {
    if (!self->lag_holding_interrupts) {
        return;
    }
    self->lag_holding_interrupts = false;
    self->lag_busy_deadline = 0;
    if (self->lag_held_exception != MP_OBJ_NULL) {
        mp_sched_exception(self->lag_held_exception);
        self->lag_held_exception = MP_OBJ_NULL;
    }
}

// A transfer in progress raises when something interrupts. On a port with async SPI that can be
// any transfer, however short. While holding, that returns false: the transfer stopped partway,
// so the caller goes back to where the RAM or command can start again.
static bool lag_send(epaperdisplay_epaperdisplay_obj_t *self, display_byte_type_t type,
    const uint8_t *data, uint32_t length) {
    if (!self->lag_holding_interrupts) {
        self->bus.send(self->bus.bus, type, self->chip_select, data, length);
        return true;
    }
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        self->bus.send(self->bus.bus, type, self->chip_select, data, length);
        nlr_pop();
        return true;
    }
    MP_STATE_THREAD(mp_pending_exception) = nlr.ret_val;
    display_interrupted(self);
    return false;
}

static bool display_send_command(epaperdisplay_epaperdisplay_obj_t *self, const uint8_t *command,
    const uint8_t *data, uint32_t length) {
    for (uint8_t attempt = 0; attempt < 3; attempt++) {
        if (lag_send(self, DISPLAY_COMMAND, command, 1) && lag_send(self, DISPLAY_DATA, data, length)) {
            return true;
        }
        // The send stopped, and it may have arrived. If it was an update, sending it again would
        // drive every pixel a second time. BUSY high says it landed. A command with no data could
        // be an update that BUSY hasn't shown yet, so don't risk it: the drive is now unknown.
        if (wait_for_update_start(self)) {
            return true;
        }
        if (length == 0) {
            self->lag_state_lost = true;
            return false;
        }
    }
    return false;
}

// set_region_to_update() has its own transaction, so a raise from it leaves the bus locked.
static bool lag_set_region(epaperdisplay_epaperdisplay_obj_t *self, displayio_area_t *box) {
    for (volatile uint8_t attempt = 0; attempt < 3; attempt++) {
        // It skips its commands if it can't get the bus, which would leave the old window.
        if (!displayio_display_bus_is_free(&self->bus)) {
            return false;
        }
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            self->lag_bus_held = true;
            displayio_display_bus_set_region_to_update(&self->bus, &self->core, box);
            self->lag_bus_held = false;
            nlr_pop();
            return true;
        }
        if (self->lag_bus_held) {
            if (!displayio_display_bus_is_free(&self->bus)) {
                displayio_display_bus_end_transaction(&self->bus);
            }
            self->lag_bus_held = false;
        }
        if (!self->lag_holding_interrupts) {
            nlr_jump(nlr.ret_val);
        }
        MP_STATE_THREAD(mp_pending_exception) = nlr.ret_val;
        display_interrupted(self);
    }
    return false;
}

// A delay ends early when interrupted, which a held interrupt must not do.
static void display_delay_ms(epaperdisplay_epaperdisplay_obj_t *self, uint32_t delay_ms) {
    uint64_t start = supervisor_ticks_ms64();
    uint32_t elapsed = 0;
    while (true) {
        common_hal_time_delay_ms(delay_ms - elapsed);
        elapsed = supervisor_ticks_ms64() - start;
        if (!self->lag_holding_interrupts || elapsed >= delay_ms) {
            return;
        }
        display_interrupted(self);
    }
}
#else // CIRCUITPY_EPAPERDISPLAY_LAG
#define display_wait_deadline(self) ((uint64_t)0)
#define display_interrupted(self) mp_hal_is_interrupted()
#define display_send_command(self, command, data, length) ( \
    (self)->bus.send((self)->bus.bus, DISPLAY_COMMAND, (self)->chip_select, command, 1), \
    (self)->bus.send((self)->bus.bus, DISPLAY_DATA, (self)->chip_select, data, length), \
    true)
#define display_delay_ms(self, delay_ms) common_hal_time_delay_ms(delay_ms)
#endif // CIRCUITPY_EPAPERDISPLAY_LAG

static void wait_for_busy(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->busy.base.type == &mp_type_NoneType) {
        return;
    }
    const uint64_t deadline = display_wait_deadline(self);
    while (common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state &&
           !display_interrupted(self)) {
        if (deadline != 0 && supervisor_ticks_ms64() > deadline) {
            return;
        }
        RUN_BACKGROUND_TASKS;
    }
}

// True if every command went out. Lag drive counts an update only if its last command did.
static bool send_command_sequence(epaperdisplay_epaperdisplay_obj_t *self,
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
               !display_interrupted(self) && (deadline == 0 || supervisor_ticks_ms64() <= deadline)) {
            RUN_BACKGROUND_TASKS;
        }
        #if CIRCUITPY_EPAPERDISPLAY_LAG
        if (!have_bus && self->lag_frames > 0) {
            return false;
        }
        self->lag_bus_held = have_bus;
        #endif // CIRCUITPY_EPAPERDISPLAY_LAG
        const bool sent = display_send_command(self, cmd, data, data_size);
        displayio_display_bus_end_transaction(&self->bus);
        #if CIRCUITPY_EPAPERDISPLAY_LAG
        self->lag_bus_held = false;
        #endif // CIRCUITPY_EPAPERDISPLAY_LAG
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
        display_delay_ms(self, delay_length_ms);
        if (should_wait_for_busy) {
            #if CIRCUITPY_EPAPERDISPLAY_LAG
            if (self->lag_frames > 0) {
                wait_for_update_start(self);
            }
            #endif
            wait_for_busy(self);
        }
        i += 2 + data_size;
        if (self->two_byte_sequence_length) {
            i++;
        }
        if (display_interrupted(self)) {
            return i >= sequence_len;
        }
    }
    return true;
}

void epaperdisplay_epaperdisplay_change_refresh_mode_parameters(epaperdisplay_epaperdisplay_obj_t *self,
    mp_buffer_info_t *start_sequence, mp_buffer_info_t *refresh_sequence, float seconds_per_frame) {
    #if CIRCUITPY_EPAPERDISPLAY_LAG
    if (self->lag_frames > 0) {
        // Finish the drive with the waveform that started it, and rewrite both planes after.
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            lag_flush(self);
            wait_for_busy(self);
            nlr_pop();
        } else {
            lag_raise(self, nlr.ret_val);
        }
        if (self->lag_settle > 0 || self->lag_commit != LAG_COMMIT_NONE) {
            // Still owed: keep the mode that started it.
            if (!mp_hal_is_interrupted()) {
                mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_display_bus);
            }
            return;
        }
        self->lag_force_full = true;
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_LAG
    self->start_sequence = (uint8_t *)start_sequence->buf;
    self->start_sequence_len = start_sequence->len;
    // NULL keeps the current refresh sequence.
    if (refresh_sequence != NULL) {
        self->refresh_sequence = (uint8_t *)refresh_sequence->buf;
        self->refresh_sequence_len = refresh_sequence->len;
    }
    self->milliseconds_per_frame = seconds_per_frame * 1000;
    self->needs_start = true;
}

// For a panel kept awake. Ctrl-C raises from a send in progress, and that transaction is still
// open. The caller still has things to do first, so the interrupt raises later.
static void send_stop_sequence_guarded(epaperdisplay_epaperdisplay_obj_t *self) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
        nlr_pop();
    } else {
        if (!displayio_display_bus_is_free(&self->bus)) {
            displayio_display_bus_end_transaction(&self->bus);
        }
        mp_sched_exception(nlr.ret_val);
    }
}

static void epaperdisplay_epaperdisplay_start_refresh(epaperdisplay_epaperdisplay_obj_t *self) {
    if (!displayio_display_bus_is_free(&self->bus)) {
        // Can't acquire display bus; skip updating this display. Try next display.
        return;
    }
    if (!displayio_display_core_start_refresh(&self->core)) {
        // Refresh on this display already in progress.
        return;
    }

    if (self->keep_awake && self->awake) {
        if (!self->needs_start) {
            // The panel is still configured from the previous refresh.
            return;
        }
        wait_for_busy(self);
        // SSD168x controllers commit the last RAM byte when the next command arrives, so a reset
        // can't come straight after a RAM write.
        send_stop_sequence_guarded(self);
        self->awake = false;
    }

    // run start sequence
    self->bus.bus_reset(self->bus.bus);

    display_delay_ms(self, self->start_up_time_ms);

    if (!send_command_sequence(self, true, self->start_sequence, self->start_sequence_len) ||
        display_interrupted(self)) {
        return;
    }
    self->awake = true;
    self->needs_start = false;
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

static bool epaperdisplay_epaperdisplay_refresh_area(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *area, bool write_previous);

// False if the panel is still updating when the wait is over.
static bool wait_for_update_end(epaperdisplay_epaperdisplay_obj_t *self) {
    wait_for_update_start(self);
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
    send_command_sequence(self, false, self->refresh_sequence, self->refresh_sequence_len);
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
        } else if (write_previous) {
            write_command = self->write_previous_ram_command;
        }
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
    if (displayio_display_bus_begin_transaction(&self->bus)) {
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
            if (!self->keep_awake) {
                // Run stop sequence but don't wait for busy because busy is set when sleeping.
                send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            }
        } else {
            #if CIRCUITPY_EPAPERDISPLAY_LAG
            lag_give_up_if_stuck(self);
            #endif
            return false;
        }
    }
    if (self->core.current_group == NULL) {
        #if CIRCUITPY_EPAPERDISPLAY_LAG
        // Nothing to draw, but the drive already started is still owed.
        if (self->lag_frames > 0 && self->awake) {
            return lag_refresh(self, NULL);
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
    #if CIRCUITPY_EPAPERDISPLAY_LAG
    if (self->lag_frames > 0) {
        return lag_refresh(self, current_area);
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_LAG
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
            if (!self->keep_awake) {
                // Run stop sequence but don't wait for busy because busy is set when sleeping.
                send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            }
        }
        #if CIRCUITPY_EPAPERDISPLAY_LAG
        else {
            lag_give_up_if_stuck(self);
        }
        #endif
    }
}

bool common_hal_epaperdisplay_epaperdisplay_get_busy(epaperdisplay_epaperdisplay_obj_t *self) {
    epaperdisplay_epaperdisplay_background(self);
    return self->refreshing;
}

void release_epaperdisplay(epaperdisplay_epaperdisplay_obj_t *self) {
    #if CIRCUITPY_EPAPERDISPLAY_LAG
    lag_shutdown(self);
    lag_free(self);
    #endif // CIRCUITPY_EPAPERDISPLAY_LAG
    if (self->refreshing) {
        wait_for_busy(self);
        supervisor_disable_tick();
        self->refreshing = false;
        // Run stop sequence but don't wait for busy because busy is set when sleeping.
        if (self->keep_awake) {
            send_stop_sequence_guarded(self);
        } else {
            send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
        }
    } else if (self->keep_awake && self->awake) {
        send_stop_sequence_guarded(self);
    }
    self->awake = false;
    #if CIRCUITPY_EPAPERDISPLAY_LAG
    lag_release_interrupts(self);
    #endif // CIRCUITPY_EPAPERDISPLAY_LAG

    release_display_core(&self->core);
    if (self->busy.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->busy);
    }
}

#if CIRCUITPY_EPAPERDISPLAY_LAG
void epaperdisplay_epaperdisplay_prepare_release(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag_frames == 0 || (!self->awake && !self->lag_state_lost)) {
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
#endif // CIRCUITPY_EPAPERDISPLAY_LAG

void epaperdisplay_epaperdisplay_end_held_bus(epaperdisplay_epaperdisplay_obj_t *self) {
    // The VM is over, so whatever holds the bus can never let it go: end it here.
    if (self->keep_awake && !displayio_display_bus_is_free(&self->bus)) {
        displayio_display_bus_end_transaction(&self->bus);
        #if CIRCUITPY_EPAPERDISPLAY_LAG
        self->lag_bus_held = false;
        #endif
    }
}

void epaperdisplay_epaperdisplay_reset(epaperdisplay_epaperdisplay_obj_t *self) {
    #if CIRCUITPY_EPAPERDISPLAY_LAG
    if (self->lag_frames > 0) {
        // Only a primary display gets here; reset_displays() has released the others. The heap is
        // still here but the sequences won't be, and the display would otherwise persist to draw
        // the splash with them. Finish now and stay asleep. Its bus still holds the pins until
        // the next VM ends or release_displays().
        epaperdisplay_epaperdisplay_end_held_bus(self);
        release_epaperdisplay(self);
        self->base.type = &mp_type_NoneType;
        return;
    }
    #endif // CIRCUITPY_EPAPERDISPLAY_LAG
    displayio_display_core_set_root_group(&self->core, &circuitpython_splash);
    self->core.full_refresh = true;
    // Let the panel sleep after the next refresh when code isn't running.
    if (self->keep_awake && self->awake && !self->refreshing) {
        // Nothing can catch a raise from a send. A reload may be pending.
        epaperdisplay_epaperdisplay_end_held_bus(self);
        send_stop_sequence_guarded(self);
        self->awake = false;
    }
    self->keep_awake = false;
}

void epaperdisplay_epaperdisplay_collect_ptrs(epaperdisplay_epaperdisplay_obj_t *self) {
    displayio_display_core_collect_ptrs(&self->core);
    displayio_display_bus_collect_ptrs(&self->bus);
    gc_collect_ptr((void *)self->start_sequence);
    gc_collect_ptr((void *)self->stop_sequence);
    gc_collect_ptr((void *)self->refresh_sequence);
    #if CIRCUITPY_EPAPERDISPLAY_LAG
    gc_collect_ptr((void *)self->clean_sequence);
    gc_collect_ptr(self->lag_held_exception);
    #endif // CIRCUITPY_EPAPERDISPLAY_LAG
}

#if CIRCUITPY_EPAPERDISPLAY_LAG
// Lag drive. Plane 0x26 holds frame N-lag instead of N-1, so a pixel that changes is driven for
// lag steps, then driven the other way by the same number of steps if it changes back. Each
// frame is rendered once into the ring and the planes are sent from there.
//
// The ring has lag slots, N-1 to N-lag, and N replaces N-lag in its slot. The box to send must
// be known before that, so it comes from the areas that were rendered, not from comparing frames.
// Frames N and N-lag can only differ inside the areas of the last lag steps. Sending the areas of
// the last lag + 1 steps also covers what the previous step left unequal, so the planes are
// equal outside the box: a byte there is either outside the last box too, so equal by induction,
// or was last written with frames that have not changed there since.

static void lag_construct(epaperdisplay_epaperdisplay_obj_t *self, uint8_t lag_frames) {
    self->lag_stride = (self->core.area.x2 + 7) / 8;
    self->lag_rows = self->core.area.y2;
    self->lag_slot_size = self->lag_stride * self->lag_rows;
    self->lag_frames = lag_frames;
    self->lag_head = 0;
    self->lag_box_index = 0;
    self->lag_settle = 0;
    // The panel's RAM is unknown, so the first step writes everything.
    self->lag_force_full = true;
    size_t meta_size = lag_frames * sizeof(uint8_t *) + (lag_frames + 1) * sizeof(displayio_area_t);
    self->lag_slots = port_malloc(meta_size, false);
    bool ok = self->lag_slots != NULL;
    if (ok) {
        memset(self->lag_slots, 0, meta_size);
        self->lag_boxes = (displayio_area_t *)(self->lag_slots + lag_frames);
    }
    for (uint8_t i = 0; ok && i < lag_frames; i++) {
        self->lag_slots[i] = port_malloc(self->lag_slot_size, true);
        ok = self->lag_slots[i] != NULL;
    }
    if (!ok) {
        lag_free(self);
        self->base.type = &mp_type_NoneType;
        m_malloc_fail(self->lag_slot_size);
    }
    // History starts white, the state a cleared panel is in.
    for (uint8_t i = 0; i < lag_frames; i++) {
        memset(self->lag_slots[i], self->black_bits_inverted ? 0x00 : 0xff, self->lag_slot_size);
    }
}

static void lag_free(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag_slots != NULL) {
        for (uint8_t i = 0; i < self->lag_frames; i++) {
            if (self->lag_slots[i] != NULL) {
                port_free(self->lag_slots[i]);
            }
        }
        port_free(self->lag_slots);
        self->lag_slots = NULL;
    }
    self->lag_frames = 0;
}

static void lag_render_area(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *area, uint8_t *frame, bool white) {
    displayio_area_t clipped;
    if (!displayio_display_core_clip_area(&self->core, area, &clipped)) {
        return;
    }
    if (white) {
        for (int16_t row = clipped.y1; row < clipped.y2; row++) {
            memset(frame + row * self->lag_stride + clipped.x1 / 8, self->black_bits_inverted ? 0x00 : 0xff,
                displayio_area_width(&clipped) / 8);
        }
        return;
    }
    uint16_t width = displayio_area_width(&clipped);
    uint16_t bytes_per_row = width / 8;
    uint16_t rows_per_buffer = MAX(1, 128 * 32 / width);
    size_t words = (rows_per_buffer * width + 31) / 32;
    uint32_t buffer[words];
    uint32_t mask[words];

    self->core.colorspace.grayscale = true;
    self->core.colorspace.grayscale_bit = 7;
    for (int16_t y = clipped.y1; y < clipped.y2; y += rows_per_buffer) {
        displayio_area_t subrectangle = {
            .x1 = clipped.x1,
            .y1 = y,
            .x2 = clipped.x2,
            .y2 = MIN(y + rows_per_buffer, clipped.y2),
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
            memcpy(frame + row * self->lag_stride + clipped.x1 / 8, in, bytes_per_row);
            in += bytes_per_row;
        }
        // Run background tasks like a regular refresh does.
        RUN_BACKGROUND_TASKS;
    }
}

static bool lag_send_ram_commit(epaperdisplay_epaperdisplay_obj_t *self) {
    const uint8_t command = LAG_RAM_COMMIT_COMMAND;
    return lag_send(self, DISPLAY_COMMAND, &command, 1);
}

// Rows are gathered so a narrow box doesn't cost one SPI transfer per row.
static bool lag_send_plane(epaperdisplay_epaperdisplay_obj_t *self, uint8_t command, const uint8_t *frame, const displayio_area_t *box) {
    if (!lag_send(self, DISPLAY_COMMAND, &command, 1)) {
        return false;
    }
    uint16_t bytes_per_row = displayio_area_width(box) / 8;
    const uint8_t *in = frame + box->y1 * self->lag_stride + box->x1 / 8;
    if (bytes_per_row == self->lag_stride) {
        return lag_send(self, DISPLAY_DATA, in, displayio_area_height(box) * bytes_per_row) && lag_send_ram_commit(self);
    }
    uint8_t chunk[512];
    size_t used = 0;
    for (int16_t y = box->y1; y < box->y2; y++) {
        if (used + bytes_per_row > sizeof(chunk)) {
            if (!lag_send(self, DISPLAY_DATA, chunk, used)) {
                return false;
            }
            used = 0;
        }
        memcpy(chunk + used, in, bytes_per_row);
        used += bytes_per_row;
        in += self->lag_stride;
    }
    return lag_send(self, DISPLAY_DATA, chunk, used) && lag_send_ram_commit(self);
}

// A send stopped by a held interrupt leaves the RAM pointer partway, so start over from the region.
static bool lag_write_plane(epaperdisplay_epaperdisplay_obj_t *self, uint8_t command, const uint8_t *frame, displayio_area_t *box) {
    for (uint8_t attempt = 0; attempt < 3; attempt++) {
        if (!lag_set_region(self, box) || !displayio_display_bus_begin_transaction(&self->bus)) {
            return false;
        }
        self->lag_bus_held = true;
        const bool sent = lag_send_plane(self, command, frame, box);
        displayio_display_bus_end_transaction(&self->bus);
        self->lag_bus_held = false;
        if (sent) {
            return true;
        }
    }
    return false;
}

// Finish a step whose frame is already in the ring: frame N to its plane, then the update. Each
// part stays owed until it has gone out, so a pixel is only counted as driven if it was. Interrupts
// wait, since a send they stop can't be told from one that arrived. False if the bus can't be had.
static bool lag_commit_step(epaperdisplay_epaperdisplay_obj_t *self) {
    const bool was_holding = self->lag_holding_interrupts;
    lag_hold_interrupts(self);
    bool ok = true;
    if (self->lag_commit == LAG_COMMIT_PLANE) {
        ok = lag_write_plane(self, self->write_black_ram_command, self->lag_slots[self->lag_head], &self->lag_commit_box);
        if (ok) {
            self->lag_commit = LAG_COMMIT_ACTIVATE;
        }
    }
    if (ok && self->lag_commit == LAG_COMMIT_ACTIVATE) {
        // Nothing can differ when the box is empty, so an update would only take time.
        if (self->lag_commit_drive) {
            ok = send_command_sequence(self, false, self->refresh_sequence, self->refresh_sequence_len);
            if (ok) {
                wait_for_update_start(self);
                supervisor_enable_tick();
                self->refreshing = true;
                self->lag_update_start = supervisor_ticks_ms64();
            }
        }
        if (ok) {
            self->lag_commit = LAG_COMMIT_NONE;
        }
    }
    if (!was_holding) {
        lag_release_interrupts(self);
    }
    return ok;
}

// Send one step: frame N-lag to the previous plane, then frame N, rendered from areas (may be
// NULL) over its slot, or white in those areas without rendering, to the other. The history
// doesn't advance if the bus can't be had before the slot is touched, so a retry does the same
// step. Once it has, the step is owed and true is returned, whatever happens to the rest.
static bool lag_step(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas, bool white) {
    uint8_t lag = self->lag_frames;
    uint8_t next = (self->lag_head + 1) % lag;
    uint8_t *frame = self->lag_slots[next];
    displayio_area_t *boxes = self->lag_boxes;
    uint8_t index = self->lag_box_index;
    bool changed = areas != NULL;

    for (const displayio_area_t *area = areas; area != NULL; area = area->next) {
        displayio_area_t clipped;
        if (displayio_display_core_clip_area(&self->core, area, &clipped)) {
            displayio_area_union(&boxes[index], &clipped, &boxes[index]);
        }
    }
    // The oldest entry is lag steps back: only needed to make the planes equal again.
    uint8_t oldest = (index + 1) % (lag + 1);
    displayio_area_t drive_box = {0};
    for (uint8_t i = 0; i <= lag; i++) {
        if (i != oldest) {
            displayio_area_union(&drive_box, &boxes[i], &drive_box);
        }
    }
    displayio_area_t write_box;
    if (self->lag_force_full) {
        displayio_display_core_clip_area(&self->core, &self->core.area, &write_box);
    } else {
        displayio_area_union(&drive_box, &boxes[oldest], &write_box);
    }
    bool write = !displayio_area_empty(&write_box);

    if (write && !lag_write_plane(self, self->write_previous_ram_command, frame, &write_box)) {
        return false;
    }
    // N-1 and N-lag only differ inside the box.
    self->lag_slot_overwriting = true;
    if (write && next != self->lag_head) {
        const uint8_t *in = self->lag_slots[self->lag_head] + write_box.y1 * self->lag_stride + write_box.x1 / 8;
        uint8_t *out = frame + write_box.y1 * self->lag_stride + write_box.x1 / 8;
        uint16_t bytes_per_row = displayio_area_width(&write_box) / 8;
        for (int16_t y = write_box.y1; y < write_box.y2; y++) {
            memcpy(out, in, bytes_per_row);
            in += self->lag_stride;
            out += self->lag_stride;
        }
    }
    for (const displayio_area_t *area = areas; area != NULL; area = area->next) {
        lag_render_area(self, area, frame, white);
    }
    // The slot no longer holds N-lag, so from here the step has to finish or that drive is lost.
    self->lag_slot_overwriting = false;
    self->lag_head = next;
    self->lag_box_index = oldest;
    boxes[oldest] = (displayio_area_t) {0};
    if (changed) {
        self->lag_settle = lag;
    } else if (self->lag_settle > 0) {
        self->lag_settle--;
    }
    self->lag_force_full = false;
    self->lag_commit_box = write_box;
    self->lag_commit_drive = !displayio_area_empty(&drive_box);
    self->lag_commit = write ? LAG_COMMIT_PLANE : LAG_COMMIT_ACTIVATE;
    lag_commit_step(self);
    return true;
}

static bool lag_refresh_unguarded(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas) {
    if (!self->awake || self->lag_state_lost) {
        // The panel may still hold anything, including ghosts from a run that lost power.
        if (!lag_clean(self, 0, true) || mp_hal_is_interrupted()) {
            return false;
        }
        areas = &self->core.area;
    }
    if (self->lag_commit != LAG_COMMIT_NONE) {
        // The areas stay marked, so the next refresh draws them.
        return lag_commit_step(self);
    }
    if (areas == NULL && self->lag_settle == 0) {
        return true;
    }
    if (self->needs_start) {
        lag_hold_interrupts(self);
        epaperdisplay_epaperdisplay_start_refresh(self);
        lag_release_interrupts(self);
    } else {
        epaperdisplay_epaperdisplay_start_refresh(self);
    }
    bool ok = !self->needs_start && lag_step(self, areas, false);
    if (ok) {
        displayio_display_core_finish_refresh(&self->core);
    } else {
        self->core.refresh_in_progress = false;
    }
    return ok;
}

// An update takes far less than the limit, so a panel still BUSY is stuck, and only a reset can
// help. That has to wait for the next refresh or the shutdown, since the bus may be in use here.
// Until then a script polling busy can go on.
static void lag_give_up_if_stuck(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag_frames > 0 && !self->lag_holding_interrupts &&
        supervisor_ticks_ms64() - self->lag_update_start > EPAPER_WAIT_TIMEOUT_MS) {
        supervisor_disable_tick();
        self->refreshing = false;
        self->awake = false;
        self->lag_state_lost = true;
    }
}

// False if the update is still running: interrupted, or the panel is stuck. RAM writes and a reset
// would land on that update, so nothing more can be sent.
static bool lag_wait_idle(epaperdisplay_epaperdisplay_obj_t *self) {
    wait_for_busy(self);
    return common_hal_digitalio_digitalinout_get_value(&self->busy) != self->busy_state;
}

// The deadline bounds each piece of work, not the whole drive, which can be 32 updates and a clean.
// Only called after progress, so a bus that is never free still runs out.
static void lag_renew_deadline(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag_busy_deadline != 0) {
        self->lag_busy_deadline = supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
    }
}

// Give every changed pixel the rest of its drive so the panel is left DC balanced.
static void lag_flush(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag_frames == 0 || !self->awake) {
        return;
    }
    while (self->lag_settle > 0 || self->lag_commit != LAG_COMMIT_NONE) {
        if (!lag_wait_idle(self) || display_interrupted(self)) {
            return;
        }
        if (self->refreshing) {
            supervisor_disable_tick();
            self->refreshing = false;
        }
        if (self->lag_commit != LAG_COMMIT_NONE ? !lag_commit_step(self) : !lag_step(self, NULL, false)) {
            return;
        }
        lag_renew_deadline(self);
    }
}

static bool lag_refresh(epaperdisplay_epaperdisplay_obj_t *self, const displayio_area_t *areas) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        bool ok = lag_refresh_unguarded(self, areas);
        nlr_pop();
        return ok;
    }
    lag_raise(self, nlr.ret_val);
}

// Fill a whole RAM plane with one color from a small buffer.
static bool lag_send_solid(epaperdisplay_epaperdisplay_obj_t *self, uint8_t command, bool white) {
    if (!lag_send(self, DISPLAY_COMMAND, &command, 1)) {
        return false;
    }
    uint8_t chunk[512];
    memset(chunk, white != self->black_bits_inverted ? 0xff : 0x00, sizeof(chunk));
    size_t remaining = self->lag_slot_size;
    while (remaining > 0) {
        size_t count = MIN(remaining, sizeof(chunk));
        if (!lag_send(self, DISPLAY_DATA, chunk, count)) {
            return false;
        }
        remaining -= count;
    }
    return lag_send_ram_commit(self);
}

static bool lag_write_solid(epaperdisplay_epaperdisplay_obj_t *self, uint8_t command, bool white) {
    displayio_area_t box;
    displayio_display_core_clip_area(&self->core, &self->core.area, &box);
    for (uint8_t attempt = 0; attempt < 3; attempt++) {
        if (!lag_set_region(self, &box) || !displayio_display_bus_begin_transaction(&self->bus)) {
            return false;
        }
        self->lag_bus_held = true;
        const bool sent = lag_send_solid(self, command, white);
        displayio_display_bus_end_transaction(&self->bus);
        self->lag_bus_held = false;
        if (sent) {
            return true;
        }
    }
    return false;
}

// Updates with the planes as they are. Background tasks run, so a video keeps decoding. Returns how
// many went out.
static uint8_t lag_wipe_phase(epaperdisplay_epaperdisplay_obj_t *self, uint8_t frames) {
    for (uint8_t i = 0; i < frames; i++) {
        lag_renew_deadline(self);
        if (!send_command_sequence(self, false, self->refresh_sequence, self->refresh_sequence_len)) {
            return i;
        }
        wait_for_update_start(self);
        if (!lag_wait_idle(self)) {
            return i + 1; // A stuck panel: writes would be dropped, so nothing more can help.
        }
    }
    return frames;
}

// Both planes must be white. Every pixel gets frames toward black, then the same number toward white,
// and both planes are white again at the end. The caller holds interrupts, so the phases stay
// together. If that can't be done the drive is unknown, and only a stored-waveform refresh is safe.
static void lag_wipe(epaperdisplay_epaperdisplay_obj_t *self, uint8_t frames) {
    bool ok = lag_write_solid(self, self->write_black_ram_command, false);
    if (ok) {
        const uint8_t toward_black = lag_wipe_phase(self, frames);
        lag_renew_deadline(self);
        ok = toward_black == frames && lag_wait_idle(self) &&
            lag_write_solid(self, self->write_previous_ram_command, false) &&
            lag_write_solid(self, self->write_black_ram_command, true) &&
            lag_wipe_phase(self, frames) == frames &&
            lag_write_solid(self, self->write_previous_ram_command, true);
    }
    if (!ok) {
        self->lag_state_lost = true;
    }
}

// A normal step to white, then the owed steps. Everything changed so far gets all of its drive,
// and the ring and both planes end up white with nothing owed. False if interrupted first, with
// the drive still owed.
static bool lag_to_white(epaperdisplay_epaperdisplay_obj_t *self) {
    while (true) {
        if (!lag_wait_idle(self) || display_interrupted(self)) {
            return false;
        }
        if (self->refreshing) {
            supervisor_disable_tick();
            self->refreshing = false;
        }
        if (self->lag_commit == LAG_COMMIT_NONE) {
            break;
        }
        if (!lag_commit_step(self)) {
            return false;
        }
    }
    displayio_area_t whole = self->core.area;
    whole.next = NULL;
    if (!lag_step(self, &whole, true)) {
        return false;
    }
    lag_renew_deadline(self);
    lag_flush(self);
    if (self->lag_settle > 0 || self->lag_commit != LAG_COMMIT_NONE || !lag_wait_idle(self)) {
        return false;
    }
    if (self->refreshing) {
        supervisor_disable_tick();
        self->refreshing = false;
    }
    return true;
}

// An exception came out of a bus send (Ctrl-C during a long transfer raises) or a render. Without
// this the bus stays locked and chip select low, and everything after waits for it. The next
// step writes all of both planes, since this one may have stopped partway through either.
static void lag_abandon_step(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag_bus_held) {
        displayio_display_bus_end_transaction(&self->bus);
        self->lag_bus_held = false;
    }
    self->core.refresh_in_progress = false;
    self->lag_force_full = true;
    if (self->lag_slot_overwriting) {
        // The drive it held can't be sent any more.
        self->lag_slot_overwriting = false;
        self->lag_state_lost = true;
    }
}

static void lag_after_raise(epaperdisplay_epaperdisplay_obj_t *self) {
    lag_abandon_step(self);
    lag_release_interrupts(self);
}

// Raise once: what was held, if anything, instead of both that and this.
static MP_NORETURN void lag_raise(epaperdisplay_epaperdisplay_obj_t *self, mp_obj_t raised) {
    if (self->lag_held_exception != MP_OBJ_NULL) {
        raised = self->lag_held_exception;
        self->lag_held_exception = MP_OBJ_NULL;
    }
    lag_after_raise(self);
    nlr_jump(raised);
}

static bool lag_wake(epaperdisplay_epaperdisplay_obj_t *self) {
    self->awake = false;
    self->bus.bus_reset(self->bus.bus);
    display_delay_ms(self, self->start_up_time_ms);
    if (!send_command_sequence(self, true, self->start_sequence, self->start_sequence_len)) {
        return false;
    }
    self->awake = true;
    self->needs_start = false;
    return true;
}

// What the panel received is unknown, so paying back could drive pixels the wrong way. Forget the
// drive and let the stored waveform, which is balanced by itself, bring the panel to white.
static bool lag_recover(epaperdisplay_epaperdisplay_obj_t *self) {
    // An update that may have started shouldn't be cut. A stuck panel has already been waited for.
    if (self->awake) {
        wait_for_busy(self);
    }
    if (!lag_wake(self) || !lag_wait_idle(self)) {
        return false;
    }
    lag_renew_deadline(self);
    if (!lag_write_solid(self, self->write_previous_ram_command, true) ||
        !lag_write_solid(self, self->write_black_ram_command, true) ||
        !send_command_sequence(self, true, self->clean_sequence, self->clean_sequence_len) ||
        !lag_wait_idle(self)) {
        return false;
    }
    lag_renew_deadline(self);
    if (!lag_wake(self)) {
        return false;
    }
    for (uint8_t i = 0; i < self->lag_frames; i++) {
        memset(self->lag_slots[i], self->black_bits_inverted ? 0x00 : 0xff, self->lag_slot_size);
    }
    memset(self->lag_boxes, 0, (self->lag_frames + 1) * sizeof(displayio_area_t));
    self->lag_settle = 0;
    self->lag_commit = LAG_COMMIT_NONE;
    self->lag_force_full = false;
    self->lag_state_lost = false;
    return true;
}

// A clean can't stop partway, so an interrupt raises when it ends. False if the bus can't be had.
static bool lag_clean(epaperdisplay_epaperdisplay_obj_t *self, uint8_t frames, bool full) {
    if (!displayio_display_bus_is_free(&self->bus)) {
        return false;
    }
    lag_hold_interrupts(self);
    self->core.full_refresh = true;
    bool ok;
    if (self->lag_state_lost) {
        ok = lag_recover(self);
    } else {
        ok = self->awake || lag_wake(self);
        ok = ok && lag_to_white(self);
        if (ok && full) {
            lag_renew_deadline(self);
            self->lag_force_full = true;
            ok = send_command_sequence(self, true, self->clean_sequence, self->clean_sequence_len) && lag_wait_idle(self);
            if (ok) {
                lag_renew_deadline(self);
                ok = lag_wake(self);
                lag_renew_deadline(self);
            }
        }
    }
    if (ok && frames > 0) {
        lag_wipe(self, frames);
    }
    lag_release_interrupts(self);
    return ok;
}

bool common_hal_epaperdisplay_epaperdisplay_clean(epaperdisplay_epaperdisplay_obj_t *self, uint8_t frames, bool full) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        bool ok = lag_clean(self, frames, full);
        nlr_pop();
        return ok;
    }
    lag_raise(self, nlr.ret_val);
}

// Leave the panel white and balanced when the display goes away: release, or the end of the VM,
// where nothing can catch an exception. Interrupts stay held until release_epaperdisplay() has sent
// the stop sequence. All of it is bounded so a stuck panel can't hang shutdown. Doesn't allocate.
static void lag_shutdown(epaperdisplay_epaperdisplay_obj_t *self) {
    if (self->lag_frames == 0 || (!self->awake && !self->lag_state_lost)) {
        return; // Nothing was drawn.
    }
    lag_after_raise(self); // Anything still held from before is ours to end.
    lag_hold_interrupts(self);
    volatile bool done = false;
    // Each raise abandons the step and the loop starts it again, up to 4 times.
    volatile uint8_t raises = 0;
    while (!done && !lag_deadline_passed(self) && raises < 4) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            if (self->lag_state_lost) {
                done = lag_recover(self);
            } else if (lag_to_white(self)) {
                lag_renew_deadline(self);
                done = send_command_sequence(self, true, self->clean_sequence, self->clean_sequence_len);
            }
            nlr_pop();
            RUN_BACKGROUND_TASKS; // Whatever holds the bus may let it go.
        } else {
            lag_abandon_step(self);
            raises++;
            if (self->lag_held_exception == MP_OBJ_NULL) {
                self->lag_held_exception = nlr.ret_val;
            }
        }
    }
    wait_for_busy(self);
    if (self->refreshing) {
        supervisor_disable_tick();
        self->refreshing = false;
    }
}
#endif // CIRCUITPY_EPAPERDISPLAY_LAG

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
