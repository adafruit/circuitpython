// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/picodvi/__init__.h"
#include "common-hal/picodvi/Framebuffer.h"
#include "bindings/picodvi/Framebuffer.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/settings.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "supervisor/port_heap.h"

#if CIRCUITPY_PICODVI_AUDIOOUT && defined(DEFAULT_DVI_BUS_CLK_DP)
#include "common-hal/picodvi/AudioOut.h"
#endif

// Every supervisor allocation starts in internal RAM, so without this the heap
// takes the space of a display that a program builds only later.
static uint32_t *_boot_ram;
static size_t _boot_ram_size;   // bytes

uint32_t *picodvi_boot_ram_take(size_t bytes) {
    // NULL when the boot allocation failed, or there is none.
    return bytes <= _boot_ram_size ? _boot_ram : NULL;
}

bool picodvi_is_boot_ram(const void *ptr) {
    return ptr != NULL && ptr == _boot_ram;
}

#if defined(DEFAULT_DVI_BUS_CLK_DP)
// True when a display answers at the EDID address and supports one of our modes.
static bool picodvi_display_detected(mp_int_t *default_width, mp_int_t *default_height) {
    // check if address 0x50 is live on the I2C bus
    busio_i2c_obj_t *i2c = common_hal_board_create_i2c(0);
    if (!i2c) {
        return false;
    }
    if (!common_hal_busio_i2c_try_lock(i2c)) {
        return false;
    }
    bool probed = common_hal_busio_i2c_probe(i2c, 0x50);
    if (probed) {
        uint8_t edid[128];
        uint8_t out[1] = {0};
        common_hal_busio_i2c_write_read(i2c, 0x50, out, 1, edid, sizeof(edid));
        bool edid_ok = true;
        if (edid[0] != 0x00 || edid[1] != 0xFF || edid[2] != 0xFF || edid[3] != 0xFF || edid[4] != 0xFF || edid[5] != 0xFF || edid[6] != 0xFF || edid[7] != 0x00) {
            edid_ok = false;
        }
        uint8_t checksum = 0;
        for (size_t i = 0; i < sizeof(edid); i++) {
            checksum += edid[i];
        }
        if (checksum != 0) {
            edid_ok = false;
        }

        if (edid_ok) {
            uint8_t established_timings = edid[35];
            if ((established_timings & 0xa0) == 0) {
                // Check that 720x400@70Hz or 640x480@60Hz is supported. If not
                // and we read EDID ok, then don't autostart.
                probed = false;
            } else {
                size_t offset = 54;
                uint16_t preferred_pixel_clock = edid[offset] | (edid[offset + 1] << 8);
                if (preferred_pixel_clock != 0) {
                    size_t preferred_width = ((edid[offset + 4] & 0xf0) << 4) | edid[offset + 2];
                    size_t preferred_height = ((edid[offset + 7] & 0xf0) << 4) | edid[offset + 5];
                    // Use 720x400 on 1080p, 4k and 8k displays.
                    if ((established_timings & 0x80) != 0 &&
                        preferred_width % 1920 == 0 &&
                        preferred_height % 1080 == 0) {
                        *default_width = 720;
                        *default_height = 400;
                    } else {
                        *default_width = 640;
                        *default_height = 480;
                    }
                }
            }
        }
    }
    common_hal_busio_i2c_unlock(i2c);
    return probed;
}

// For picodvi_autoconstruct to work, the 8 DVI/HSTX pin names must be defined, AND
// i2c bus 0 must also be connected to DVI with on-board pull ups
void picodvi_autoconstruct(void) {
    if (get_safe_mode() != SAFE_MODE_NONE) {
        return;
    }

    char buf[sizeof("detect")];
    buf[0] = 0;
    // (any failure leaves the content of buf untouched: an empty nul-terminated string
    (void)settings_get_str("CIRCUITPY_PICODVI_ENABLE", buf, sizeof(buf));
    if (!strcasecmp(buf, "never")) {
        return;
    }
    // It's "always", "detect" or else an invalid value which is treated the same as "detect".
    mp_int_t default_width = 640;
    mp_int_t default_height = 480;
    bool enabled = !strcasecmp(buf, "always") ||
        picodvi_display_detected(&default_width, &default_height);

    #if CIRCUITPY_PICODVI_AUDIOOUT
    bool audio = true;
    (void)settings_get_bool("CIRCUITPY_PICODVI_AUDIO", &audio);
    #endif

    mp_int_t width = default_width;
    mp_int_t height = 0;
    mp_int_t color_depth = 8;
    mp_int_t rotation = 0;

    bool configured = settings_get_int("CIRCUITPY_DISPLAY_WIDTH", &width) == SETTINGS_OK;
    (void)settings_get_int("CIRCUITPY_DISPLAY_HEIGHT", &height);
    (void)settings_get_int("CIRCUITPY_DISPLAY_COLOR_DEPTH", &color_depth);
    (void)settings_get_int("CIRCUITPY_DISPLAY_ROTATION", &rotation);

    if (height == 0) {
        switch (width) {
            case 720:
                height = 400;
                break;
            case 640:
                height = 480;
                break;
            case 320:
                height = 240;
                break;
            case 360:
                height = 200;
                break;
        }
    }

    if (rotation != 0 && rotation != 90 && rotation != 180 && rotation != 270) {
        // invalid rotation
        rotation = 0;
    }

    if (!common_hal_picodvi_framebuffer_preflight(width, height, color_depth)) {
        // invalid configuration, set back to default
        width = default_width;
        height = default_height;
        color_depth = 8;
        configured = false;
    }

    #ifdef CIRCUITPY_PSRAM_CHIP_SELECT
    // Taken now, before anything else lands in internal RAM; the heap has PSRAM to
    // grow into. A configured display gets its own size. A display built without
    // one may be switched for another later, so it gets the largest mode, 320x240
    // at 32 bits. With neither, nothing is held and the heap takes the RAM.
    // Once only: board_init() runs again after a fake deep sleep.
    if (_boot_ram == NULL) {
        if (configured) {
            _boot_ram_size = picodvi_framebuffer_internal_ram(width, height, color_depth);
        } else if (enabled) {
            _boot_ram_size = picodvi_framebuffer_internal_ram(320, 240, 32);
        }
        if (_boot_ram_size > 0) {
            _boot_ram = port_malloc(_boot_ram_size, true);
        }
    }
    #endif
    if (!enabled) {
        return;
    }

    // construct framebuffer and display
    picodvi_framebuffer_obj_t *fb = &allocate_display_bus_or_raise()->picodvi;
    fb->base.type = &picodvi_framebuffer_type;
    common_hal_picodvi_framebuffer_construct(fb,
        width, height,
        DEFAULT_DVI_BUS_CLK_DP,
        DEFAULT_DVI_BUS_CLK_DN,
        DEFAULT_DVI_BUS_RED_DP,
        DEFAULT_DVI_BUS_RED_DN,
        DEFAULT_DVI_BUS_GREEN_DP,
        DEFAULT_DVI_BUS_GREEN_DN,
        DEFAULT_DVI_BUS_BLUE_DP,
        DEFAULT_DVI_BUS_BLUE_DN,
        color_depth);

    framebufferio_framebufferdisplay_obj_t *display = &allocate_display()->framebuffer_display;
    display->base.type = &framebufferio_framebufferdisplay_type;
    common_hal_framebufferio_framebufferdisplay_construct(
        display,
        MP_OBJ_FROM_PTR(fb),
        rotation,
        true);

    #if CIRCUITPY_PICODVI_AUDIOOUT
    // Before the VM heap exists, so the audio buffers get internal RAM. On
    // failure supervisor.runtime.audio is None, and the display still works.
    if (audio) {
        (void)picodvi_audioout_construct_board(fb);
    }
    #endif
}
#else
void picodvi_autoconstruct(void) {
}
#endif
