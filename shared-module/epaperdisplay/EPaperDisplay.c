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
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].epaper_display.base.type != &epaperdisplay_epaperdisplay_type ||
            displays[i].epaper_display.epaper.core.current_group != &circuitpython_splash) {
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
