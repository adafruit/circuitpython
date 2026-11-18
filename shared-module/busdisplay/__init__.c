// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#include "shared-module/busdisplay/BusDisplay.h"

#include "shared-bindings/microcontroller/Pin.h"

void busdisplay_backlight_construct(busdisplay_backlight_t *self, const mcu_pin_obj_t *pin,
    uint16_t pwm_frequency) {
    // Always set the backlight type in case we're reusing memory.
    self->inout.base.type = &mp_type_NoneType;
    if (pin == NULL || !common_hal_mcu_pin_is_free(pin)) {
        return;
    }
    #if CIRCUITPY_PWMIO
    pwmout_result_t result = common_hal_pwmio_pwmout_construct(&self->pwm, pin, 0, pwm_frequency, false);
    if (result != PWMOUT_OK) {
        self->inout.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->inout, pin);
    } else {
        self->pwm.base.type = &pwmio_pwmout_type;
    }
    #else
    self->inout.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&self->inout, pin);
    #endif
}

bool busdisplay_backlight_set(busdisplay_backlight_t *self, mp_float_t brightness) {
    #if CIRCUITPY_PWMIO
    if (self->pwm.base.type == &pwmio_pwmout_type) {
        common_hal_pwmio_pwmout_set_duty_cycle(&self->pwm, (uint16_t)(0xffff * brightness));
        return true;
    }
    #endif
    if (self->inout.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_set_value(&self->inout, brightness > 0.99);
        return true;
    }
    return false;
}

void busdisplay_backlight_deinit(busdisplay_backlight_t *self) {
    #if CIRCUITPY_PWMIO
    if (self->pwm.base.type == &pwmio_pwmout_type) {
        common_hal_pwmio_pwmout_deinit(&self->pwm);
        return;
    }
    #endif
    if (self->inout.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->inout);
    }
}
