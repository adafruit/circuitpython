// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-module/displayio/__init__.h"
#include "shared-bindings/displayio/__init__.h"

#include "shared/runtime/interrupt_char.h"
#include "py/runtime.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-module/displayio/area.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/reload.h"

#include "py/mpconfig.h"

#if CIRCUITPY_OS_GETENV && CIRCUITPY_SET_DISPLAY_LIMIT
#include "shared-module/os/__init__.h"
#endif

#if CIRCUITPY_BUSDISPLAY
#include "shared-bindings/busdisplay/BusDisplay.h"
#endif

#if CIRCUITPY_DOTCLOCKFRAMEBUFFER
#include "shared-bindings/dotclockframebuffer/DotClockFramebuffer.h"
#endif

#if CIRCUITPY_SHARPDISPLAY
#include "shared-bindings/sharpdisplay/SharpMemoryFramebuffer.h"
#include "shared-module/sharpdisplay/SharpMemoryFramebuffer.h"
#endif

#if CIRCUITPY_AURORA_EPAPER
#include "shared-bindings/aurora_epaper/aurora_framebuffer.h"
#include "shared-module/aurora_epaper/aurora_framebuffer.h"
#endif

#ifdef BOARD_USE_INTERNAL_SPI
#include "supervisor/spi_flash_api.h"
#endif

// The default indicates no primary display
static int primary_display_number = -1;
static mp_int_t max_num_displays = CIRCUITPY_DISPLAY_LIMIT;

primary_display_bus_t display_buses[CIRCUITPY_DISPLAY_LIMIT];
primary_display_t displays[CIRCUITPY_DISPLAY_LIMIT];
#if CIRCUITPY_OS_GETENV && CIRCUITPY_SET_DISPLAY_LIMIT
primary_display_bus_t *display_buses_dyn = &display_buses[0];
primary_display_t *displays_dyn = &displays[0];
#define DYN_DISPLAY_BUSES(indx) (indx < CIRCUITPY_DISPLAY_LIMIT ? display_buses[indx] : display_buses_dyn[indx - CIRCUITPY_DISPLAY_LIMIT])
#define DYN_DISPLAY_BUSES_ADR(indx, membr) (indx < CIRCUITPY_DISPLAY_LIMIT ? &display_buses[indx].membr : &display_buses_dyn[indx - CIRCUITPY_DISPLAY_LIMIT].membr)
#define DYN_DISPLAY_BUSES_ADR0(indx) (indx < CIRCUITPY_DISPLAY_LIMIT ? &display_buses[indx] : &display_buses_dyn[indx - CIRCUITPY_DISPLAY_LIMIT])
#define DYN_DISPLAYS(indx) (indx < CIRCUITPY_DISPLAY_LIMIT ? displays[indx] : displays_dyn[indx - CIRCUITPY_DISPLAY_LIMIT])
#define DYN_DISPLAYS_ADR(indx, membr) (indx < CIRCUITPY_DISPLAY_LIMIT ? &displays[indx].membr : &displays_dyn[indx - CIRCUITPY_DISPLAY_LIMIT].membr)
#define DYN_DISPLAYS_ADR0(indx) (indx < CIRCUITPY_DISPLAY_LIMIT ? &displays[indx] : &displays_dyn[indx - CIRCUITPY_DISPLAY_LIMIT])
#else
#define DYN_DISPLAY_BUSES(indx) (display_buses[indx])
#define DYN_DISPLAY_BUSES_ADR(indx, membr) (&display_buses[indx].membr)
#define DYN_DISPLAY_BUSES_ADR0(indx) (&display_buses[indx])
#define DYN_DISPLAYS(indx) (displays[indx])
#define DYN_DISPLAYS_ADR(indx, membr) (&displays[indx].membr)
#define DYN_DISPLAYS_ADR0(indx) (&displays[indx])
#endif

displayio_buffer_transform_t null_transform = {
    .x = 0,
    .y = 0,
    .dx = 1,
    .dy = 1,
    .scale = 1,
    .width = 0,
    .height = 0,
    .mirror_x = false,
    .mirror_y = false,
    .transpose_xy = false
};

#if CIRCUITPY_RGBMATRIX || CIRCUITPY_IS31FL3741 || CIRCUITPY_VIDEOCORE || CIRCUITPY_PICODVI
static bool any_display_uses_this_framebuffer(mp_obj_base_t *obj) {
    for (uint8_t i = 0; i < max_num_displays; i++) {
        if (DYN_DISPLAYS(i).display_base.type == &framebufferio_framebufferdisplay_type) {
            framebufferio_framebufferdisplay_obj_t *display = DYN_DISPLAYS_ADR(i, framebuffer_display);
            if (display->framebuffer == obj) {
                return true;
            }
        }
    }
    return false;
}
#endif


void displayio_background(void) {
    if (mp_hal_is_interrupted()) {
        return;
    }
    if (autoreload_ready()) {
        // Reload is about to happen, so don't redisplay.
        return;
    }

    for (uint8_t i = 0; i < max_num_displays; i++) {
        mp_const_obj_t display_type = DYN_DISPLAYS(i).display_base.type;
        if (display_type == NULL || display_type == &mp_type_NoneType) {
            // Skip null display.
            continue;
        }
        if (false) {
        #if CIRCUITPY_BUSDISPLAY
        } else if (display_type == &busdisplay_busdisplay_type) {
            busdisplay_busdisplay_background(DYN_DISPLAYS_ADR(i, display));
        #endif
        #if CIRCUITPY_FRAMEBUFFERIO
        } else if (display_type == &framebufferio_framebufferdisplay_type) {
            framebufferio_framebufferdisplay_background(DYN_DISPLAYS_ADR(i, framebuffer_display));
        #endif
        #if CIRCUITPY_EPAPERDISPLAY
        } else if (display_type == &epaperdisplay_epaperdisplay_type) {
            epaperdisplay_epaperdisplay_background(DYN_DISPLAYS_ADR(i, epaper_display));
        #endif
        }
    }

}

static void common_hal_displayio_release_displays_impl(bool keep_primary) {
    // Release displays before busses so that they can send any final commands to turn the display
    // off properly.
    if (!keep_primary) {
        primary_display_number = -1;
    }
    for (uint8_t i = (keep_primary ? CIRCUITPY_DISPLAY_LIMIT: 0); i < max_num_displays; i++) {
        if (i == primary_display_number) {
            continue;
        }
        mp_const_obj_t display_type = DYN_DISPLAYS(i).display_base.type;
        if (display_type == NULL || display_type == &mp_type_NoneType) {
            continue;
        #if CIRCUITPY_BUSDISPLAY
        } else if (display_type == &busdisplay_busdisplay_type) {
            release_busdisplay(DYN_DISPLAYS_ADR(i, display));
        #endif
        #if CIRCUITPY_EPAPERDISPLAY
        } else if (display_type == &epaperdisplay_epaperdisplay_type) {
            release_epaperdisplay(DYN_DISPLAYS_ADR(i, epaper_display));
        #endif
        #if CIRCUITPY_FRAMEBUFFERIO
        } else if (display_type == &framebufferio_framebufferdisplay_type) {
            release_framebufferdisplay(DYN_DISPLAYS_ADR(i, framebuffer_display));
        #endif
        }
        #if CIRCUITPY_OS_GETENV && CIRCUITPY_SET_DISPLAY_LIMIT
        if (i < CIRCUITPY_DISPLAY_LIMIT) {
            displays[i].display_base.type = &mp_type_NoneType;
        } else {
            displays_dyn[i - CIRCUITPY_DISPLAY_LIMIT].display_base.type = &mp_type_NoneType;
        }
        #else
        displays[i].display_base.type = &mp_type_NoneType;
        #endif
    }
    for (uint8_t i = (keep_primary ? CIRCUITPY_DISPLAY_LIMIT: 0); i < max_num_displays; i++) {
        if (i == primary_display_number) {
            continue;
        }
        mp_const_obj_t bus_type = DYN_DISPLAY_BUSES(i).bus_base.type;
        if (bus_type == NULL || bus_type == &mp_type_NoneType) {
            continue;
        #if CIRCUITPY_FOURWIRE
        } else if (bus_type == &fourwire_fourwire_type) {
            common_hal_fourwire_fourwire_deinit(DYN_DISPLAY_BUSES_ADR(i, fourwire_bus));
        #endif
        #if CIRCUITPY_I2CDISPLAYBUS
        } else if (bus_type == &i2cdisplaybus_i2cdisplaybus_type) {
            common_hal_i2cdisplaybus_i2cdisplaybus_deinit(DYN_DISPLAY_BUSES_ADR(i, i2cdisplay_bus));
        #endif
        #if CIRCUITPY_DOTCLOCKFRAMEBUFFER
        } else if (bus_type == &dotclockframebuffer_framebuffer_type) {
            common_hal_dotclockframebuffer_framebuffer_deinit(DYN_DISPLAY_BUSES_ADR(i, dotclock));
        #endif
        #if CIRCUITPY_PARALLELDISPLAYBUS
        } else if (bus_type == &paralleldisplaybus_parallelbus_type) {
            common_hal_paralleldisplaybus_parallelbus_deinit(DYN_DISPLAY_BUSES_ADR(i, parallel_bus));
        #endif
        #if CIRCUITPY_RGBMATRIX
        } else if (bus_type == &rgbmatrix_RGBMatrix_type) {
            common_hal_rgbmatrix_rgbmatrix_deinit(DYN_DISPLAY_BUSES_ADR(i, rgbmatrix));
        #endif
        #if CIRCUITPY_IS31FL3741
        } else if (bus_type == &is31fl3741_framebuffer_type) {
            common_hal_is31fl3741_framebuffer_deinit(DYN_DISPLAY_BUSES_ADR(i, is31fl3741));
        #endif
        #if CIRCUITPY_SHARPDISPLAY
        } else if (bus_type == &sharpdisplay_framebuffer_type) {
            common_hal_sharpdisplay_framebuffer_deinit(DYN_DISPLAY_BUSES_ADR(i, sharpdisplay));
        #endif
        #if CIRCUITPY_VIDEOCORE
        } else if (bus_type == &videocore_framebuffer_type) {
            common_hal_videocore_framebuffer_deinit(DYN_DISPLAY_BUSES_ADR(i, videocore));
        #endif
        #if CIRCUITPY_PICODVI
        } else if (bus_type == &picodvi_framebuffer_type) {
            common_hal_picodvi_framebuffer_deinit(DYN_DISPLAY_BUSES_ADR(i, picodvi));
        #endif
        }
        #if CIRCUITPY_OS_GETENV && CIRCUITPY_SET_DISPLAY_LIMIT
        if (i < CIRCUITPY_DISPLAY_LIMIT) {
            display_buses[i].bus_base.type = &mp_type_NoneType;
        } else {
            display_buses_dyn[i - CIRCUITPY_DISPLAY_LIMIT].bus_base.type = &mp_type_NoneType;
        }
        #else
        display_buses[i].bus_base.type = &mp_type_NoneType;
        #endif
    }

    if (!keep_primary) {
        supervisor_stop_terminal();
    }
}

void common_hal_displayio_release_displays(void) {
    common_hal_displayio_release_displays_impl(false);
}

void malloc_display_memory(void) {
    #if CIRCUITPY_OS_GETENV && CIRCUITPY_SET_DISPLAY_LIMIT
    (void)common_hal_os_getenv_int("CIRCUITPY_DISPLAY_LIMIT", &max_num_displays);
    if (max_num_displays > CIRCUITPY_DISPLAY_LIMIT) {
        display_buses_dyn = (primary_display_bus_t *)port_malloc(sizeof(primary_display_bus_t) * (max_num_displays - CIRCUITPY_DISPLAY_LIMIT), false);
        displays_dyn = (primary_display_t *)port_malloc(sizeof(primary_display_t) * (max_num_displays - CIRCUITPY_DISPLAY_LIMIT), false);

        for (uint8_t i = CIRCUITPY_DISPLAY_LIMIT; i < max_num_displays; i++) {
            memset(&displays_dyn[i - CIRCUITPY_DISPLAY_LIMIT], 0, sizeof(displays_dyn[i - CIRCUITPY_DISPLAY_LIMIT]));
            memset(&display_buses_dyn[i - CIRCUITPY_DISPLAY_LIMIT], 0, sizeof(display_buses_dyn[i - CIRCUITPY_DISPLAY_LIMIT]));
            display_buses_dyn[i - CIRCUITPY_DISPLAY_LIMIT].bus_base.type = &mp_type_NoneType;
            displays_dyn[i - CIRCUITPY_DISPLAY_LIMIT].display_base.type = &mp_type_NoneType;
        }
    }
    #endif
}

void reset_displays(void) {
    // In CircuitPython 10, release secondary displays before doing anything else:
    common_hal_displayio_release_displays_impl(true);

    // The SPI buses used by FourWires may be allocated on the heap so we need to move them inline.
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        mp_const_obj_t display_bus_type = display_buses[i].bus_base.type;
        if (display_bus_type == NULL || display_bus_type == &mp_type_NoneType) {
            continue;
        #if CIRCUITPY_FOURWIRE
        } else if (display_bus_type == &fourwire_fourwire_type) {
            fourwire_fourwire_obj_t *fourwire = &display_buses[i].fourwire_bus;
            if (((size_t)fourwire->bus) < ((size_t)&display_buses) ||
                ((size_t)fourwire->bus) > ((size_t)&display_buses + CIRCUITPY_DISPLAY_LIMIT * sizeof(primary_display_bus_t))) {
                busio_spi_obj_t *original_spi = fourwire->bus;
                #if CIRCUITPY_BOARD_SPI
                // We don't need to move original_spi if it is a board.SPI object because it is
                // statically allocated already. (Doing so would also make it impossible to reference in
                // a subsequent VM run.)
                if (common_hal_board_is_spi(original_spi)) {
                    continue;
                }
                #endif
                #ifdef BOARD_USE_INTERNAL_SPI
                if (original_spi == (mp_obj_t)(&supervisor_flash_spi_bus)) {
                    continue;
                }
                #endif
                memcpy(&fourwire->inline_bus, original_spi, sizeof(busio_spi_obj_t));
                fourwire->bus = &fourwire->inline_bus;
                // Check for other display buses that use the same spi bus and swap them too.
                for (uint8_t j = i + 1; j < CIRCUITPY_DISPLAY_LIMIT; j++) {
                    if (display_buses[j].fourwire_bus.base.type == &fourwire_fourwire_type &&
                        display_buses[j].fourwire_bus.bus == original_spi) {
                        display_buses[j].fourwire_bus.bus = &fourwire->inline_bus;
                    }
                }
            }
        #endif
        #if CIRCUITPY_I2CDISPLAYBUS
        } else if (display_bus_type == &i2cdisplaybus_i2cdisplaybus_type) {
            i2cdisplaybus_i2cdisplaybus_obj_t *i2c = &display_buses[i].i2cdisplay_bus;
            // Check to see if we need to inline the I2C bus.
            if (((size_t)i2c->bus) < ((size_t)&display_buses) ||
                ((size_t)i2c->bus) > ((size_t)&display_buses + CIRCUITPY_DISPLAY_LIMIT * sizeof(primary_display_bus_t))) {
                busio_i2c_obj_t *original_i2c = i2c->bus;
                #if CIRCUITPY_BOARD_I2C
                // We don't need to move original_i2c if it is a board.I2C object because it is
                // statically allocated already. (Doing so would also make it impossible to reference in
                // a subsequent VM run.)
                if (common_hal_board_is_i2c(original_i2c)) {
                    continue;
                }
                #endif
                memcpy(&i2c->inline_bus, original_i2c, sizeof(busio_i2c_obj_t));
                i2c->bus = &i2c->inline_bus;
                // Check for other displays that use the same i2c bus and swap them too.
                for (uint8_t j = i + 1; j < CIRCUITPY_DISPLAY_LIMIT; j++) {
                    if (display_buses[j].i2cdisplay_bus.base.type == &i2cdisplaybus_i2cdisplaybus_type &&
                        display_buses[j].i2cdisplay_bus.bus == original_i2c) {
                        display_buses[j].i2cdisplay_bus.bus = &i2c->inline_bus;
                    }
                }
                // Mark the old i2c object so it is considered deinit.
                common_hal_busio_i2c_mark_deinit(original_i2c);
            }
        #endif
        #if CIRCUITPY_RGBMATRIX
        } else if (display_bus_type == &rgbmatrix_RGBMatrix_type) {
            rgbmatrix_rgbmatrix_obj_t *pm = &display_buses[i].rgbmatrix;
            if (!any_display_uses_this_framebuffer(&pm->base)) {
                common_hal_rgbmatrix_rgbmatrix_deinit(pm);
            } else {
                common_hal_rgbmatrix_rgbmatrix_set_paused(pm, true);
            }
        #endif
        #if CIRCUITPY_IS31FL3741
        } else if (display_bus_type == &is31fl3741_framebuffer_type) {
            is31fl3741_framebuffer_obj_t *is31fb = &display_buses[i].is31fl3741;

            if (((uint32_t)is31fb->is31fl3741->i2c) < ((uint32_t)&display_buses) ||
                ((uint32_t)is31fb->is31fl3741->i2c) > ((uint32_t)&display_buses + CIRCUITPY_DISPLAY_LIMIT)) {
                #if CIRCUITPY_BOARD_I2C
                // We don't need to move original_i2c if it is the board.I2C object because it is
                // statically allocated already. (Doing so would also make it impossible to reference in
                // a subsequent VM run.)
                if (common_hal_board_is_i2c(is31fb->is31fl3741->i2c)) {
                    continue;
                }
                #endif

                is31fl3741_IS31FL3741_obj_t *original_is31 = is31fb->is31fl3741;
                memcpy(&is31fb->inline_is31fl3741, original_is31, sizeof(is31fl3741_IS31FL3741_obj_t));
                is31fb->is31fl3741 = &is31fb->inline_is31fl3741;

                busio_i2c_obj_t *original_i2c = is31fb->is31fl3741->i2c;
                memcpy(&is31fb->is31fl3741->inline_i2c, original_i2c, sizeof(busio_i2c_obj_t));
                is31fb->is31fl3741->i2c = &is31fb->is31fl3741->inline_i2c;
            }

            if (!any_display_uses_this_framebuffer(&is31fb->base)) {
                common_hal_is31fl3741_framebuffer_deinit(is31fb);
            } else {
                common_hal_is31fl3741_framebuffer_set_paused(is31fb, true);
            }
        #endif
        #if CIRCUITPY_SHARPDISPLAY
        } else if (display_bus_type == &sharpdisplay_framebuffer_type) {
            sharpdisplay_framebuffer_obj_t *sharp = &display_buses[i].sharpdisplay;
            common_hal_sharpdisplay_framebuffer_reset(sharp);
        #endif
        #if CIRCUITPY_VIDEOCORE
        } else if (display_bus_type == &videocore_framebuffer_type) {
            videocore_framebuffer_obj_t *vc = &display_buses[i].videocore;
            if (!any_display_uses_this_framebuffer(&vc->base)) {
                common_hal_videocore_framebuffer_deinit(vc);
            }
            // The framebuffer is allocated outside of the heap so it doesn't
            // need to be moved.
        #endif
        #if CIRCUITPY_PICODVI
        } else if (display_bus_type == &picodvi_framebuffer_type) {
            picodvi_framebuffer_obj_t *vc = &display_buses[i].picodvi;
            if (!any_display_uses_this_framebuffer(&vc->base)) {
                common_hal_picodvi_framebuffer_deinit(vc);
            }
        #endif
        #if CIRCUITPY_AURORA_EPAPER
        } else if (display_bus_type == &aurora_framebuffer_type) {
            #if CIRCUITPY_BOARD_SPI
            aurora_epaper_framebuffer_obj_t *aurora = &display_buses[i].aurora_epaper;
            if (common_hal_board_is_spi(aurora->bus)) {
                common_hal_aurora_epaper_framebuffer_set_free_bus(false);
            }
            #endif
            // Set to None, gets deinit'd up by display_base
            display_buses[i].bus_base.type = &mp_type_NoneType;
        #endif
        } else {
            // Not an active display bus.
            continue;
        }
    }

    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        // Reset the displayed group. Only the first will get the terminal but
        // that's ok.
        mp_const_obj_t display_type = displays[i].display_base.type;
        if (display_type == NULL || display_type == &mp_type_NoneType) {
            continue;
        #if CIRCUITPY_BUSDISPLAY
        } else if (display_type == &busdisplay_busdisplay_type) {
            reset_busdisplay(&displays[i].display);
        #endif
        #if CIRCUITPY_EPAPERDISPLAY
        } else if (display_type == &epaperdisplay_epaperdisplay_type) {
            epaperdisplay_epaperdisplay_obj_t *display = &displays[i].epaper_display;
            epaperdisplay_epaperdisplay_reset(display);
        #endif
        #if CIRCUITPY_FRAMEBUFFERIO
        } else if (display_type == &framebufferio_framebufferdisplay_type) {
            framebufferio_framebufferdisplay_reset(&displays[i].framebuffer_display);
        #endif
        }
    }
}

void displayio_gc_collect(void) {
    for (uint8_t i = 0; i < max_num_displays; i++) {
        mp_const_obj_t display_bus_type = DYN_DISPLAY_BUSES(i).bus_base.type;
        if (display_bus_type == NULL || display_bus_type == &mp_type_NoneType) {
            continue;
        }
        #if CIRCUITPY_RGBMATRIX
        if (display_bus_type == &rgbmatrix_RGBMatrix_type) {
            rgbmatrix_rgbmatrix_collect_ptrs(DYN_DISPLAY_BUSES_ADR(i, rgbmatrix));
        }
        #endif
        #if CIRCUITPY_IS31FL3741
        if (display_bus_type == &is31fl3741_framebuffer_type) {
            is31fl3741_framebuffer_collect_ptrs(DYN_DISPLAY_BUSES_ADR(i, is31fl3741));
        }
        #endif
        #if CIRCUITPY_SHARPDISPLAY
        if (display_bus_type == &sharpdisplay_framebuffer_type) {
            common_hal_sharpdisplay_framebuffer_collect_ptrs(DYN_DISPLAY_BUSES_ADR(i, sharpdisplay));
        }
        #endif
        #if CIRCUITPY_AURORA_EPAPER
        if (display_bus_type == &aurora_framebuffer_type) {
            common_hal_aurora_epaper_framebuffer_collect_ptrs(DYN_DISPLAY_BUSES_ADR(i, aurora_epaper));
        }
        #endif
    }

    for (uint8_t i = 0; i < max_num_displays; i++) {
        mp_const_obj_t display_type = DYN_DISPLAYS(i).display_base.type;
        if (display_type == NULL || display_type == &mp_type_NoneType) {
            continue;

            // Alternatively, we could use gc_collect_root over the whole object,
            // but this is more precise, and is the only field that needs marking.
        #if CIRCUITPY_BUSDISPLAY
        } else if (display_type == &busdisplay_busdisplay_type) {
            busdisplay_busdisplay_collect_ptrs(DYN_DISPLAYS_ADR(i, display));
        #endif
        #if CIRCUITPY_FRAMEBUFFERIO
        } else if (display_type == &framebufferio_framebufferdisplay_type) {
            framebufferio_framebufferdisplay_collect_ptrs(DYN_DISPLAYS_ADR(i, framebuffer_display));
        #endif
        #if CIRCUITPY_EPAPERDISPLAY
        } else if (display_type == &epaperdisplay_epaperdisplay_type) {
            epaperdisplay_epaperdisplay_collect_ptrs(DYN_DISPLAYS_ADR(i, epaper_display));
        #endif
        }
    }
}

static bool is_display_active(mp_obj_base_t *display_maybe) {
    return display_maybe->type != &mp_type_NoneType && display_maybe->type != NULL;
}

primary_display_t *allocate_display(void) {
    for (uint8_t i = 0; i < max_num_displays; i++) {
        if (!is_display_active(DYN_DISPLAYS_ADR(i, display_base))) {
            // Clear this memory so it is in a known state before init.
            memset(DYN_DISPLAYS_ADR0(i), 0, sizeof(displays[0]));
            // Default to None so that it works as board.DISPLAY.
            #if CIRCUITPY_OS_GETENV && CIRCUITPY_SET_DISPLAY_LIMIT
            if (i < CIRCUITPY_DISPLAY_LIMIT) {
                displays[i].display_base.type = &mp_type_NoneType;
            } else {
                displays_dyn[i - CIRCUITPY_DISPLAY_LIMIT].display_base.type = &mp_type_NoneType;
            }
            #else
            displays[i].display_base.type = &mp_type_NoneType;
            #endif
            return DYN_DISPLAYS_ADR0(i);
        }
    }
    return NULL;
}

primary_display_t *allocate_display_or_raise(void) {
    primary_display_t *result = allocate_display();
    if (result) {
        return result;
    }
    mp_raise_RuntimeError(MP_ERROR_TEXT("Too many displays"));
}

primary_display_bus_t *allocate_display_bus(void) {
    for (uint8_t i = 0; i < max_num_displays; i++) {
        mp_const_obj_t display_bus_type = DYN_DISPLAY_BUSES(i).bus_base.type;
        if (display_bus_type == NULL || display_bus_type == &mp_type_NoneType) {
            // Clear this memory so it is in a known state before init.
            memset(DYN_DISPLAY_BUSES_ADR0(i), 0, sizeof(display_buses[0]));
            #if CIRCUITPY_OS_GETENV && CIRCUITPY_SET_DISPLAY_LIMIT
            if (i < CIRCUITPY_DISPLAY_LIMIT) {
                display_buses[i].bus_base.type = &mp_type_NoneType;
            } else {
                display_buses_dyn[i - CIRCUITPY_DISPLAY_LIMIT].bus_base.type = &mp_type_NoneType;
            }
            #else
            display_buses[i].bus_base.type = &mp_type_NoneType;
            #endif
            return DYN_DISPLAY_BUSES_ADR0(i);
        }
    }
    return NULL;
}

primary_display_bus_t *allocate_display_bus_or_raise(void) {
    primary_display_bus_t *result = allocate_display_bus();
    if (result) {
        return result;
    }
    mp_raise_RuntimeError(MP_ERROR_TEXT("Too many display busses; forgot displayio.release_displays() ?"));
}

mp_obj_t common_hal_displayio_get_primary_display(void) {
    if (primary_display_number == -1 || primary_display_number >= max_num_displays) {
        return mp_const_none;
    }
    mp_obj_base_t *primary_display = DYN_DISPLAYS_ADR(primary_display_number, display_base);
    if (is_display_active(primary_display)) {
        return MP_OBJ_FROM_PTR(primary_display);
    }
    return mp_const_none;
}

void common_hal_displayio_set_primary_display(mp_obj_t new_primary_display) {
    if (new_primary_display == mp_const_none) {
        primary_display_number = -1;
        return;
    }
    for (uint8_t i = 0; i < max_num_displays; i++) {
        mp_obj_t display = MP_OBJ_FROM_PTR(DYN_DISPLAYS_ADR0(i));
        if (new_primary_display == display && is_display_active(display)) {
            primary_display_number = i;
            return;
        }
    }
    // object was not a display after all...
    mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), MP_QSTR_Display, MP_QSTR_AnyDisplay, MP_QSTR_None, mp_obj_get_type(new_primary_display)->name);
}

void common_hal_displayio_auto_primary_display(void) {
    if (primary_display_number != -1) {
        return;
    }
    for (uint8_t i = 0; i < max_num_displays; i++) {
        if (is_display_active(DYN_DISPLAYS_ADR(i, display_base))) {
            primary_display_number = i;
            return;
        }
    }
}
