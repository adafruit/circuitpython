// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Operations on the state shared by the legacy and LVGL EPaperDisplay implementations.

#include "shared-bindings/epaperdisplay/EPaperDisplay.h"

#include "py/gc.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/displayio/ColorConverter.h"
#if CIRCUITPY_EPAPERDISPLAY_FAST && CIRCUITPY_WATCHDOG
#include "shared-bindings/watchdog/WatchDogTimer.h"
#endif
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/epaperdisplay/lag_drive.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"

#include <stdint.h>

#define DELAY 0x80

void epaperdisplay_epaper_core_construct(epaperdisplay_epaper_core_t *self,
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

    displayio_display_core_construct(&self->core, args->width, args->height, args->rotation % 360,
        color_depth, core_grayscale, true, 1, true, true);
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
    self->write_previous_ram_command = args->write_previous_ram_command;
    #endif // CIRCUITPY_EPAPERDISPLAY_FAST

    self->write_black_ram_command = args->write_black_ram_command;
    self->black_bits_inverted = args->black_bits_inverted;
    self->write_color_ram_command = args->write_color_ram_command;
    self->color_bits_inverted = args->color_bits_inverted;
    self->refresh_time = args->refresh_time * 1000;
    self->busy_state = args->busy_state;
    self->refreshing = false;
    self->milliseconds_per_frame = args->seconds_per_frame * 1000;
    self->chip_select = args->always_toggle_chip_select ? CHIP_SELECT_TOGGLE_EVERY_BYTE : CHIP_SELECT_UNTOUCHED;
    self->grayscale = grayscale;
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
    }

    // Clear the color memory if it isn't in use.
    if (args->highlight_color == 0x00 && args->highlight_color2 == 0x00 && args->write_color_ram_command != NO_COMMAND) {
        // TODO: Clear
    }
}

uint16_t epaperdisplay_epaper_core_get_width(epaperdisplay_epaper_core_t *self) {
    return displayio_display_core_get_width(&self->core);
}

uint16_t epaperdisplay_epaper_core_get_height(epaperdisplay_epaper_core_t *self) {
    return displayio_display_core_get_height(&self->core);
}

#if CIRCUITPY_EPAPERDISPLAY_FAST
// BUSY can take a moment to rise after an update command, and reading it low before then would
// look like a finished update. If it never rises, nothing has started or it was too quick to see,
// so this returns false after 2 ms. Short enough that an interrupt doesn't have to end it.
bool epaperdisplay_epaper_core_wait_for_update_start(epaperdisplay_epaper_core_t *self) {
    for (uint8_t wait = 0; wait < 200; wait++) {
        if (common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state) {
            return true;
        }
        common_hal_mcu_delay_us(10);
        RUN_BACKGROUND_TASKS;
        port_task_yield();
    }
    return false;
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

#if CIRCUITPY_EPAPERDISPLAY_FAST
// When a wait for BUSY or the bus gives up, or 0 for never. Lag drive always has an end, so a
// stuck panel can't hold up a caller that holds interrupts or has no way to be interrupted.
static uint64_t display_wait_deadline(epaperdisplay_epaper_core_t *self) {
    if (self->lag.busy_deadline != 0 || self->lag.frames == 0) {
        return self->lag.busy_deadline;
    }
    return supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
}

// Between hold and release a waveform or a panel reset is under way, and cutting it short leaves
// the panel dark. Whatever interrupts (Ctrl-C, a reload, a watchdog) waits for the release.
bool epaperdisplay_epaper_core_interrupted(epaperdisplay_epaper_core_t *self) {
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

static bool display_send_command(epaperdisplay_epaper_core_t *self, const uint8_t *command,
    const uint8_t *data, uint32_t length) {
    for (uint8_t attempt = 0; attempt < 3; attempt++) {
        if (epaperdisplay_lag_send(self, DISPLAY_COMMAND, command, 1) && epaperdisplay_lag_send(self, DISPLAY_DATA, data, length)) {
            return true;
        }
        // The send stopped, and it may have arrived. If it was an update, sending it again would
        // drive every pixel a second time. BUSY high says it landed. A command with no data could
        // be an update that BUSY hasn't shown yet, so don't risk it: the drive is now unknown.
        if (epaperdisplay_epaper_core_wait_for_update_start(self)) {
            return true;
        }
        if (length == 0) {
            self->lag.state_lost = true;
            return false;
        }
    }
    return false;
}
#else // CIRCUITPY_EPAPERDISPLAY_FAST
#define display_wait_deadline(self) ((uint64_t)0)
#define epaperdisplay_epaper_core_interrupted(self) mp_hal_is_interrupted()
#define display_send_command(self, command, data, length) ( \
    (self)->bus.send((self)->bus.bus, DISPLAY_COMMAND, (self)->chip_select, command, 1), \
    (self)->bus.send((self)->bus.bus, DISPLAY_DATA, (self)->chip_select, data, length), \
    true)
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

// A delay ends early when interrupted, which a held interrupt must not do.
void epaperdisplay_epaper_core_delay_ms(epaperdisplay_epaper_core_t *self, uint32_t delay_ms) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    uint64_t start = supervisor_ticks_ms64();
    uint32_t elapsed = 0;
    while (true) {
        common_hal_time_delay_ms(delay_ms - elapsed);
        elapsed = supervisor_ticks_ms64() - start;
        if (!self->lag.holding_interrupts || elapsed >= delay_ms) {
            return;
        }
        epaperdisplay_epaper_core_interrupted(self);
    }
    #else
    (void)self;
    common_hal_time_delay_ms(delay_ms);
    #endif
}

void epaperdisplay_epaper_core_wait_for_busy(epaperdisplay_epaper_core_t *self) {
    if (self->busy.base.type == &mp_type_NoneType) {
        return;
    }
    const uint64_t deadline = display_wait_deadline(self);
    while (common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state &&
           !epaperdisplay_epaper_core_interrupted(self)) {
        if (deadline != 0 && supervisor_ticks_ms64() > deadline) {
            return;
        }
        RUN_BACKGROUND_TASKS;
        port_task_yield();
    }
}

// True if every command went out. Lag drive counts an update only if its last command did.
bool epaperdisplay_epaper_core_send_command_sequence(epaperdisplay_epaper_core_t *self,
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
               !epaperdisplay_epaper_core_interrupted(self) && (deadline == 0 || supervisor_ticks_ms64() <= deadline)) {
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
        epaperdisplay_epaper_core_delay_ms(self, delay_length_ms);
        if (should_wait_for_busy) {
            #if CIRCUITPY_EPAPERDISPLAY_FAST
            if (self->lag.frames > 0) {
                epaperdisplay_epaper_core_wait_for_update_start(self);
            }
            #endif
            epaperdisplay_epaper_core_wait_for_busy(self);
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
        if (epaperdisplay_epaper_core_interrupted(self)) {
            return i >= sequence_len;
        }
        #endif
    }
    return true;
}

void epaperdisplay_epaper_core_change_refresh_mode_parameters(epaperdisplay_epaper_core_t *self,
    mp_buffer_info_t *start_sequence, mp_buffer_info_t *refresh_sequence, float seconds_per_frame) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    if (self->lag.frames > 0) {
        // Finish the drive with the waveform that started it, and rewrite both planes after.
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            epaperdisplay_lag_flush(self);
            epaperdisplay_epaper_core_wait_for_busy(self);
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
void epaperdisplay_epaper_core_send_stop_sequence_guarded(epaperdisplay_epaper_core_t *self) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        epaperdisplay_epaper_core_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
        nlr_pop();
    } else {
        if (!displayio_display_bus_is_free(&self->bus)) {
            displayio_display_bus_end_transaction(&self->bus);
        }
        mp_sched_exception(nlr.ret_val);
    }
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

uint32_t epaperdisplay_epaper_core_get_time_to_refresh(epaperdisplay_epaper_core_t *self) {
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
// False if the panel is still updating when the wait is over.
bool epaperdisplay_epaper_core_wait_for_update_end(epaperdisplay_epaper_core_t *self) {
    epaperdisplay_epaper_core_wait_for_update_start(self);
    const uint64_t deadline = supervisor_ticks_ms64() + EPAPER_WAIT_TIMEOUT_MS;
    bool busy;
    while ((busy = common_hal_digitalio_digitalinout_get_value(&self->busy) == self->busy_state) &&
           !mp_hal_is_interrupted() && supervisor_ticks_ms64() <= deadline) {
        RUN_BACKGROUND_TASKS;
        port_task_yield();
    }
    return !busy;
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

mp_obj_t epaperdisplay_epaper_core_get_bus(epaperdisplay_epaper_core_t *self) {
    return self->bus.bus;
}

uint16_t epaperdisplay_epaper_core_get_rotation(epaperdisplay_epaper_core_t *self) {
    return self->core.rotation;
}

mp_obj_t epaperdisplay_epaper_core_get_root_group(epaperdisplay_epaper_core_t *self) {
    if (self->core.current_group == NULL) {
        return mp_const_none;
    }
    return self->core.current_group;
}

void epaperdisplay_epaper_core_background(epaperdisplay_epaper_core_t *self) {
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
                // Run stop sequence but don't wait for busy after it, because some panels set busy
                // when sleeping.
                epaperdisplay_epaper_core_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            }
            #else
            // Run stop sequence but don't wait for busy after it, because some panels set busy
            // when sleeping.
            epaperdisplay_epaper_core_send_command_sequence(self, false, self->stop_sequence, self->stop_sequence_len);
            #endif
        }
        #if CIRCUITPY_EPAPERDISPLAY_FAST
        else {
            epaperdisplay_lag_give_up_if_stuck(self);
        }
        #endif
    }
}

bool epaperdisplay_epaper_core_get_busy(epaperdisplay_epaper_core_t *self) {
    epaperdisplay_epaper_core_background(self);
    return self->refreshing;
}

#if CIRCUITPY_EPAPERDISPLAY_FAST
void epaperdisplay_epaper_core_prepare_release(epaperdisplay_epaper_core_t *self) {
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
void epaperdisplay_epaper_core_end_held_bus(epaperdisplay_epaper_core_t *self) {
    // The VM is over, so whatever holds the bus can never let it go: end it here.
    if (self->fast && !displayio_display_bus_is_free(&self->bus)) {
        displayio_display_bus_end_transaction(&self->bus);
        self->lag.bus_held = false;
    }
}
#endif // CIRCUITPY_EPAPERDISPLAY_FAST

void epaperdisplay_epaper_core_collect_ptrs(epaperdisplay_epaper_core_t *self) {
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
