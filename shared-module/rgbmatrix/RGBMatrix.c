// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/gc.h"
#include "py/obj.h"
#include "py/objarray.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "common-hal/rgbmatrix/RGBMatrix.h"
#include "shared-module/rgbmatrix/allocator.h"
#include "shared-bindings/rgbmatrix/RGBMatrix.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/util.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"

extern Protomatter_core *_PM_protoPtr;

static void common_hal_rgbmatrix_rgbmatrix_construct1(rgbmatrix_rgbmatrix_obj_t *self, mp_obj_t framebuffer);

static void preflight_pins_or_throw(const mcu_pin_obj_t *clock_pin_obj, const mcu_pin_obj_t **rgb_pin_objs, uint8_t rgb_pin_count, bool allow_inefficient) {
    if (rgb_pin_count <= 0 || rgb_pin_count % 6 != 0 || rgb_pin_count > 30) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("The length of rgb_pins must be 6, 12, 18, 24, or 30"));
    }

// Most ports have a strict requirement for how the rgbmatrix pins are laid
// out; these two micros don't. Special-case it here.
    #if !defined(CONFIG_IDF_TARGET_ESP32S3) && !defined(CONFIG_IDF_TARGET_ESP32S2)
    uint8_t clock_pin = common_hal_mcu_pin_number(clock_pin_obj);
    uint32_t port = clock_pin / 32;
    uint32_t bit_mask = 1 << (clock_pin % 32);

    for (uint8_t i = 0; i < rgb_pin_count; i++) {
        uint8_t pin_number = common_hal_mcu_pin_number(rgb_pin_objs[i]);
        uint32_t pin_port = pin_number / 32;

        if (pin_port != port) {
            mp_raise_ValueError_varg(
                MP_ERROR_TEXT("rgb_pins[%d] is not on the same port as clock"), i);
        }

        uint32_t pin_mask = 1 << (pin_number % 32);
        if (pin_mask & bit_mask) {
            mp_raise_ValueError_varg(
                MP_ERROR_TEXT("rgb_pins[%d] duplicates another pin assignment"), i);
        }

        bit_mask |= pin_mask;
    }

    if (allow_inefficient) {
        return;
    }

    uint8_t byte_mask = 0;
    if (bit_mask & 0x000000FF) {
        byte_mask |= 0b0001;
    }
    if (bit_mask & 0x0000FF00) {
        byte_mask |= 0b0010;
    }
    if (bit_mask & 0x00FF0000) {
        byte_mask |= 0b0100;
    }
    if (bit_mask & 0xFF000000) {
        byte_mask |= 0b1000;
    }

    uint8_t bytes_per_element = 0xff;
    uint8_t ideal_bytes_per_element = (rgb_pin_count + 7) / 8;

    switch (byte_mask) {
        case 0b0001:
        case 0b0010:
        case 0b0100:
        case 0b1000:
            bytes_per_element = 1;
            break;

        case 0b0011:
        case 0b1100:
            bytes_per_element = 2;
            break;

        default:
            bytes_per_element = 4;
            break;
    }

    if (bytes_per_element != ideal_bytes_per_element) {
        mp_raise_ValueError_varg(
            MP_ERROR_TEXT("Pinout uses %d bytes per element, which consumes more than the ideal %d bytes.  If this cannot be avoided, pass allow_inefficient=True to the constructor"),
            bytes_per_element, ideal_bytes_per_element);
    }
    #endif
}

// Claim each pin by wrapping it in a DigitalInOut object owned by the matrix.
// The DigitalInOut lives on the port heap (not the VM heap) because the matrix
// itself lives in the static display bus storage and survives VM resets.
static void store_pin_digitalinout(rgbmatrix_rgbmatrix_obj_t *self, const mcu_pin_obj_t *pin) {
    digitalio_digitalinout_obj_t *digitalinout = mp_obj_port_malloc(digitalio_digitalinout_obj_t, &digitalio_digitalinout_type);
    if (digitalinout == NULL) {
        m_malloc_fail(sizeof(digitalio_digitalinout_obj_t));
    }
    common_hal_digitalio_digitalinout_construct(digitalinout, pin);
    #if CIRCUITPY_BULK_RESET
    common_hal_digitalio_digitalinout_never_reset(digitalinout);
    #endif
    self->pin_digitalinouts[self->pin_digitalinout_count++] = digitalinout;
}

void common_hal_rgbmatrix_rgbmatrix_construct(rgbmatrix_rgbmatrix_obj_t *self, int width, int bit_depth, uint8_t rgb_count, const mcu_pin_obj_t **rgb_pins, uint8_t addr_count, const mcu_pin_obj_t **addr_pins, const mcu_pin_obj_t *clock_pin, const mcu_pin_obj_t *latch_pin, const mcu_pin_obj_t *oe_pin, bool doublebuffer, mp_obj_t framebuffer, int8_t tile, bool serpentine, void *timer) {
    self->width = width;
    self->bit_depth = bit_depth;
    self->rgb_count = rgb_count;
    preflight_pins_or_throw(clock_pin, rgb_pins, rgb_count, true);
    // Claim and configure each pin by wrapping it in a DigitalInOut owned by the
    // matrix. This runs before Protomatter configures the pins so its setup wins.
    for (uint8_t i = 0; i < rgb_count; i++) {
        store_pin_digitalinout(self, rgb_pins[i]);
        self->rgb_pins[i] = common_hal_mcu_pin_number(rgb_pins[i]);
    }
    self->addr_count = addr_count;
    for (uint8_t i = 0; i < addr_count; i++) {
        store_pin_digitalinout(self, addr_pins[i]);
        self->addr_pins[i] = common_hal_mcu_pin_number(addr_pins[i]);
    }
    store_pin_digitalinout(self, clock_pin);
    self->clock_pin = common_hal_mcu_pin_number(clock_pin);
    store_pin_digitalinout(self, oe_pin);
    self->oe_pin = common_hal_mcu_pin_number(oe_pin);
    store_pin_digitalinout(self, latch_pin);
    self->latch_pin = common_hal_mcu_pin_number(latch_pin);
    self->doublebuffer = doublebuffer;
    self->tile = tile;
    self->serpentine = serpentine;

    self->timer = timer ? timer : common_hal_rgbmatrix_timer_allocate(self);
    if (self->timer == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("No timer available"));
    }

    self->width = width;
    self->bufsize = 2 * width * common_hal_rgbmatrix_rgbmatrix_get_height(self);

    common_hal_rgbmatrix_rgbmatrix_construct1(self, framebuffer);
}

static void common_hal_rgbmatrix_rgbmatrix_construct1(rgbmatrix_rgbmatrix_obj_t *self, mp_obj_t framebuffer) {
    if (framebuffer != mp_const_none) {
        mp_get_buffer_raise(self->framebuffer, &self->bufinfo, MP_BUFFER_READ);
        if (mp_get_buffer(self->framebuffer, &self->bufinfo, MP_BUFFER_RW)) {
            self->bufinfo.typecode = 'H' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW;
        } else {
            self->bufinfo.typecode = 'H';
        }
        // verify that the matrix is big enough
        mp_get_index(mp_obj_get_type(self->framebuffer), self->bufinfo.len, MP_OBJ_NEW_SMALL_INT(self->bufsize - 1), false);
    } else {
        self->bufinfo.buf = port_malloc(self->bufsize, false);
        if (self->bufinfo.buf == NULL) {
            m_malloc_fail(self->bufsize);
        }
        self->bufinfo.len = self->bufsize;
        self->bufinfo.typecode = 'H' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW;
    }
    self->framebuffer = framebuffer;

    memset(&self->protomatter, 0, sizeof(self->protomatter));
    ProtomatterStatus stat = _PM_init(&self->protomatter,
        self->width, self->bit_depth,
        self->rgb_count / 6, self->rgb_pins,
        self->addr_count, self->addr_pins,
        self->clock_pin, self->latch_pin, self->oe_pin,
        self->doublebuffer, self->serpentine ? -self->tile : self->tile,
        self->timer);

    if (stat == PROTOMATTER_OK) {
        _PM_protoPtr = &self->protomatter;
        common_hal_rgbmatrix_timer_enable(self->timer);
        stat = _PM_begin(&self->protomatter);

        if (stat == PROTOMATTER_OK) {
            _PM_convert_565(&self->protomatter, self->bufinfo.buf, self->width);
            _PM_swapbuffer_maybe(&self->protomatter);
        }
    }

    if (stat != PROTOMATTER_OK) {
        common_hal_rgbmatrix_rgbmatrix_deinit(self);
        if (!gc_alloc_possible()) {
            return;
        }
        switch (stat) {
            case PROTOMATTER_ERR_PINS:
                raise_ValueError_invalid_pin();
                break;
            case PROTOMATTER_ERR_ARG:
                mp_arg_error_invalid(MP_QSTR_args);
                break;
            case PROTOMATTER_ERR_MALLOC:
                mp_raise_msg_varg(&mp_type_MemoryError, MP_ERROR_TEXT("Failed to allocate %q buffer"), MP_QSTR_RGBMatrix);
                break;
            default:
                mp_raise_msg_varg(&mp_type_RuntimeError,
                    MP_ERROR_TEXT("Internal error #%d"), (int)stat);
                break;
        }
    }

    self->paused = 0;
}

static void deinit_pin_digitalinouts(rgbmatrix_rgbmatrix_obj_t *self) {
    for (uint8_t i = 0; i < self->pin_digitalinout_count; i++) {
        common_hal_digitalio_digitalinout_deinit(self->pin_digitalinouts[i]);
        port_free(self->pin_digitalinouts[i]);
        self->pin_digitalinouts[i] = NULL;
    }
    self->pin_digitalinout_count = 0;
}

extern int pm_row_count;
static void common_hal_rgbmatrix_rgbmatrix_deinit1(rgbmatrix_rgbmatrix_obj_t *self) {
    common_hal_rgbmatrix_timer_disable(self->timer);

    if (_PM_protoPtr == &self->protomatter) {
        _PM_protoPtr = NULL;
    }

    if (self->protomatter.rgbPins) {
        _PM_deallocate(&self->protomatter);
    }

    memset(&self->protomatter, 0, sizeof(self->protomatter));

    // If it was supervisor-allocated, it is supervisor-freed and the pointer
    // is zeroed, otherwise the pointer is just zeroed
    if (self->framebuffer == mp_const_none) {
        port_free(self->bufinfo.buf);
    }
    self->bufinfo.buf = NULL;

    // If a framebuffer was passed in to the constructor, clear the reference
    // here so that it will become GC'able
    self->framebuffer = mp_const_none;
}

void common_hal_rgbmatrix_rgbmatrix_deinit(rgbmatrix_rgbmatrix_obj_t *self) {
    common_hal_rgbmatrix_rgbmatrix_deinit1(self);
    if (self->timer) {
        common_hal_rgbmatrix_timer_free(self->timer);
        self->timer = 0;
    }

    deinit_pin_digitalinouts(self);

    self->base.type = &mp_type_NoneType;
}

void common_hal_rgbmatrix_rgbmatrix_get_bufinfo(rgbmatrix_rgbmatrix_obj_t *self, mp_buffer_info_t *bufinfo) {
    *bufinfo = self->bufinfo;
}

void common_hal_rgbmatrix_rgbmatrix_reconstruct(rgbmatrix_rgbmatrix_obj_t *self) {
    common_hal_rgbmatrix_rgbmatrix_set_paused(self, true);
    // Stop using any Python provided framebuffer.
    if (self->framebuffer != mp_const_none) {
        memset(&self->bufinfo, 0, sizeof(self->bufinfo));
        self->bufinfo.buf = port_malloc(self->bufsize, false);
        if (self->bufinfo.buf == NULL) {
            common_hal_rgbmatrix_rgbmatrix_deinit(self);
            return;
        }
        self->bufinfo.len = self->bufsize;
        self->bufinfo.typecode = 'H' | MP_OBJ_ARRAY_TYPECODE_FLAG_RW;
    }
    memset(self->bufinfo.buf, 0, self->bufinfo.len);
    common_hal_rgbmatrix_rgbmatrix_set_paused(self, false);
}

void rgbmatrix_rgbmatrix_collect_ptrs(rgbmatrix_rgbmatrix_obj_t *self) {
    gc_collect_ptr(self->framebuffer);
}

void common_hal_rgbmatrix_rgbmatrix_set_paused(rgbmatrix_rgbmatrix_obj_t *self, bool paused) {
    if (paused && !self->paused) {
        _PM_stop(&self->protomatter);
    } else if (!paused && self->paused) {
        _PM_resume(&self->protomatter);
        _PM_convert_565(&self->protomatter, self->bufinfo.buf, self->width);
        _PM_swapbuffer_maybe(&self->protomatter);
    }
    self->paused = paused;
}

bool common_hal_rgbmatrix_rgbmatrix_get_paused(rgbmatrix_rgbmatrix_obj_t *self) {
    return self->paused;
}

void common_hal_rgbmatrix_rgbmatrix_refresh(rgbmatrix_rgbmatrix_obj_t *self) {
    if (!self->paused) {
        _PM_convert_565(&self->protomatter, self->bufinfo.buf, self->width);
        _PM_swapbuffer_maybe(&self->protomatter);
    }
}

int common_hal_rgbmatrix_rgbmatrix_get_width(rgbmatrix_rgbmatrix_obj_t *self) {
    return self->width;
}

int common_hal_rgbmatrix_rgbmatrix_get_height(rgbmatrix_rgbmatrix_obj_t *self) {
    int computed_height = (self->rgb_count / 3) * (1 << (self->addr_count)) * self->tile;
    return computed_height;
}
