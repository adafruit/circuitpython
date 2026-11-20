// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#include "shared-module/busdisplay/BusDisplay.h"

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

#if CIRCUITPY_FULL_BUILD
void busdisplay_backlight_validate(mp_obj_t backlight) {
    if (backlight == NULL || backlight == mp_const_none) {
        return;
    }
    if (mp_obj_is_type(backlight, &digitalio_digitalinout_type)) {
        if (common_hal_digitalio_digitalinout_deinited(MP_OBJ_TO_PTR(backlight))) {
            raise_deinited_error();
        }
        return;
    }
    #if CIRCUITPY_PWMIO
    if (mp_obj_is_type(backlight, &pwmio_pwmout_type)) {
        if (common_hal_pwmio_pwmout_deinited(MP_OBJ_TO_PTR(backlight))) {
            raise_deinited_error();
        }
        return;
    }
    #endif
    mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be a DigitalInOut, PWMOut or None"), MP_QSTR_backlight);
}
#endif

void busdisplay_backlight_construct(busdisplay_backlight_t *self, mp_obj_t backlight,
    uint16_t pwm_frequency) {
    self->object = mp_const_none;
    self->kind = BUSDISPLAY_BACKLIGHT_NONE;
    self->owns = false;
    self->adopted = false;
    #if CIRCUITPY_FULL_BUILD
    self->size = 0;
    #endif
    if (backlight == NULL || backlight == mp_const_none) {
        return;
    }

    if (mp_obj_is_type(backlight, &digitalio_digitalinout_type)) {
        self->object = backlight;
        self->kind = BUSDISPLAY_BACKLIGHT_DIGITAL;
        #if CIRCUITPY_FULL_BUILD
        self->size = sizeof(digitalio_digitalinout_obj_t);
        #endif
        return;
    }
    #if CIRCUITPY_PWMIO
    if (mp_obj_is_type(backlight, &pwmio_pwmout_type)) {
        self->object = backlight;
        self->kind = BUSDISPLAY_BACKLIGHT_PWM;
        #if CIRCUITPY_FULL_BUILD
        self->size = sizeof(pwmio_pwmout_obj_t);
        #endif
        return;
    }
    #endif

    const mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(backlight);
    if (!common_hal_mcu_pin_is_free(pin)) {
        return;
    }
    #if CIRCUITPY_PWMIO
    pwmout_result_t result = common_hal_pwmio_pwmout_construct(&self->native.pwm, pin, 0, pwm_frequency, false);
    if (result == PWMOUT_OK) {
        self->native.pwm.base.type = &pwmio_pwmout_type;
        self->object = MP_OBJ_FROM_PTR(&self->native.pwm);
        self->kind = BUSDISPLAY_BACKLIGHT_PWM;
        self->owns = true;
        return;
    }
    #endif
    self->native.inout.base.type = &digitalio_digitalinout_type;
    if (common_hal_digitalio_digitalinout_construct(&self->native.inout, pin) == DIGITALINOUT_OK) {
        self->object = MP_OBJ_FROM_PTR(&self->native.inout);
        self->kind = BUSDISPLAY_BACKLIGHT_DIGITAL;
        self->owns = true;
    }
}

bool busdisplay_backlight_is_moveable(busdisplay_backlight_t *self) {
    #if CIRCUITPY_FULL_BUILD
    bool has_object = self->object != NULL && self->object != mp_const_none;
    if (!has_object || self->owns || !gc_ptr_on_heap(MP_OBJ_TO_PTR(self->object))) {
        return true;
    }
    switch (self->kind) {
        case BUSDISPLAY_BACKLIGHT_DIGITAL:
            return common_hal_digitalio_digitalinout_is_moveable(MP_OBJ_TO_PTR(self->object));
        #if CIRCUITPY_PWMIO
        case BUSDISPLAY_BACKLIGHT_PWM:
            return common_hal_pwmio_pwmout_is_moveable(MP_OBJ_TO_PTR(self->object));
        #endif
        default:
            break;
    }
    #else
    (void)self;
    #endif
    return true;
}

static void deinit_backlight(busdisplay_backlight_kind_t kind, mp_obj_t object);

void busdisplay_backlight_adopt(busdisplay_backlight_t *self) {
    #if CIRCUITPY_FULL_BUILD
    bool has_object = self->object != NULL && self->object != mp_const_none;
    bool on_heap = has_object && gc_ptr_on_heap(MP_OBJ_TO_PTR(self->object));

    // Static and native objects already live outside the VM. The primary
    // display adopts a supplied heap object before its VM is torn down.
    if (!has_object || self->owns || !on_heap) {
        return;
    }

    void *original = MP_OBJ_TO_PTR(self->object);
    bool moved = false;
    switch (self->kind) {
        case BUSDISPLAY_BACKLIGHT_DIGITAL:
            moved = common_hal_digitalio_digitalinout_move(original, &self->native.inout);
            break;
        #if CIRCUITPY_PWMIO
        case BUSDISPLAY_BACKLIGHT_PWM:
            moved = common_hal_pwmio_pwmout_move(original, &self->native.pwm);
            break;
        #endif
        default:
            return;
    }
    if (!moved) {
        deinit_backlight(self->kind, self->object);
        self->object = mp_const_none;
        self->kind = BUSDISPLAY_BACKLIGHT_NONE;
        self->size = 0;
        return;
    }
    self->object = MP_OBJ_FROM_PTR(&self->native);
    self->owns = true;
    self->adopted = true;
    #else
    (void)self;
    #endif
}

mp_obj_t busdisplay_backlight_start_vm(busdisplay_backlight_t *self) {
    #if CIRCUITPY_FULL_BUILD
    if (self->adopted) {
        void *native = MP_OBJ_TO_PTR(self->object);
        void *copy = m_malloc_with_finaliser(self->size);
        bool moved = false;
        switch (self->kind) {
            case BUSDISPLAY_BACKLIGHT_DIGITAL:
                moved = common_hal_digitalio_digitalinout_move(native, copy);
                break;
            #if CIRCUITPY_PWMIO
            case BUSDISPLAY_BACKLIGHT_PWM:
                moved = common_hal_pwmio_pwmout_move(native, copy);
                break;
            #endif
            default:
                break;
        }
        if (!moved) {
            deinit_backlight(self->kind, self->object);
            m_free(copy);
            self->object = mp_const_none;
            self->kind = BUSDISPLAY_BACKLIGHT_NONE;
            self->size = 0;
        } else {
            self->object = MP_OBJ_FROM_PTR(copy);
        }
        self->owns = false;
        self->adopted = false;
    }
    #endif
    return self->object;
}

bool busdisplay_backlight_set(busdisplay_backlight_t *self, mp_float_t brightness) {
    mp_obj_t object = self->object;
    if (object == MP_OBJ_NULL || object == mp_const_none) {
        return false;
    }
    #if CIRCUITPY_PWMIO
    if (self->kind == BUSDISPLAY_BACKLIGHT_PWM && !common_hal_pwmio_pwmout_deinited(MP_OBJ_TO_PTR(object))) {
        common_hal_pwmio_pwmout_set_duty_cycle(MP_OBJ_TO_PTR(object), (uint16_t)(0xffff * brightness));
        return true;
    }
    #endif
    if (self->kind == BUSDISPLAY_BACKLIGHT_DIGITAL && !common_hal_digitalio_digitalinout_deinited(MP_OBJ_TO_PTR(object))) {
        common_hal_digitalio_digitalinout_set_value(MP_OBJ_TO_PTR(object), brightness > 0.99);
        return true;
    }
    return false;
}

static void deinit_backlight(busdisplay_backlight_kind_t kind, mp_obj_t object) {
    #if CIRCUITPY_PWMIO
    if (kind == BUSDISPLAY_BACKLIGHT_PWM) {
        common_hal_pwmio_pwmout_deinit(MP_OBJ_TO_PTR(object));
        return;
    }
    #endif
    if (kind == BUSDISPLAY_BACKLIGHT_DIGITAL) {
        common_hal_digitalio_digitalinout_deinit(MP_OBJ_TO_PTR(object));
    }
}

void busdisplay_backlight_deinit(busdisplay_backlight_t *self) {
    if (self->owns) {
        deinit_backlight(self->kind, self->object);
    }
    self->object = mp_const_none;
    self->kind = BUSDISPLAY_BACKLIGHT_NONE;
    self->owns = false;
    self->adopted = false;
    #if CIRCUITPY_FULL_BUILD
    self->size = 0;
    #endif
}
