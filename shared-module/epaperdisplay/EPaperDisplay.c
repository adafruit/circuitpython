// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/epaperdisplay/EPaperDisplay.h"

#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/display.h"

#if CIRCUITPY_EPAPERDISPLAY_FAST
bool common_hal_epaperdisplay_epaperdisplay_has_lag_frames(epaperdisplay_epaperdisplay_obj_t *self) {
    return self->epaper.lag.frames > 0;
}
#endif

size_t maybe_refresh_epaperdisplay(void) {
    mp_obj_t native = displayio_primary_display_native();
    if (native != MP_OBJ_NULL && mp_obj_get_type(native) == &epaperdisplay_epaperdisplay_type) {
        epaperdisplay_epaperdisplay_obj_t *display = MP_OBJ_TO_PTR(native);
        if (display->epaper.core.current_group == &circuitpython_splash) {
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
    }
    // Return 0 if no primary ePaper display is available to pretend it was updated.
    return 0;
}

#if !CIRCUITPY_DISPLAY_LIFETIME_SIMPLE
epaperdisplay_epaperdisplay_vm_t *epaperdisplay_epaperdisplay_start_vm(epaperdisplay_epaperdisplay_obj_t *self) {
    epaperdisplay_epaperdisplay_vm_t *vm = m_new_obj(epaperdisplay_epaperdisplay_vm_t);
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    vm->held_exception = self->epaper.lag.held_exception;
    #endif
    return vm;
}
#endif

static void epaper_protocol_deinit(void *self) {
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    // The VM is ending, so whatever holds the bus can never let it go.
    epaperdisplay_epaperdisplay_end_held_bus(self);
    #endif
    release_epaperdisplay(self);
}

#if !CIRCUITPY_DISPLAY_LIFETIME_SIMPLE
static void *epaper_protocol_start_vm(void *self) {
    return epaperdisplay_epaperdisplay_start_vm(self);
}
#endif

static void epaper_protocol_background(void *self) {
    epaperdisplay_epaperdisplay_background(self);
}

#if !CIRCUITPY_DISPLAY_LIFETIME_SIMPLE
static bool epaper_protocol_stop_vm(void *self_in) {
    epaperdisplay_epaperdisplay_obj_t *self = self_in;
    #if CIRCUITPY_EPAPERDISPLAY_FAST
    epaperdisplay_epaperdisplay_end_held_bus(self);
    if (self->epaper.lag.frames > 0) {
        // Lag mode must finish and release its waveform on reload.
        return false;
    }
    #endif
    epaperdisplay_epaperdisplay_reset(self);
    return true;
}
#endif

const displayio_protocol_t epaperdisplay_epaperdisplay_protocol = {
    .deinit = epaper_protocol_deinit,
    #if !CIRCUITPY_DISPLAY_LIFETIME_SIMPLE
    .start_vm = epaper_protocol_start_vm,
    .stop_vm = epaper_protocol_stop_vm,
    #endif
    .background = epaper_protocol_background,
};
