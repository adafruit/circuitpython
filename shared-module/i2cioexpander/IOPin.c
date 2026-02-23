// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/i2cioexpander/IOPin.h"
#include "shared-bindings/i2cioexpander/IOExpander.h"

#include "py/runtime.h"

mp_negative_errno_t i2cioexpander_iopin_construct(
    i2cioexpander_iopin_obj_t *self,
    i2cioexpander_ioexpander_obj_t *expander,
    uint8_t pin_number) {

    if (pin_number >= expander->num_pins) {
        return MP_EINVAL;  // Reusing this for "invalid pin"
    }

    self->expander = expander;
    self->pin_number = pin_number;
    self->direction = DIRECTION_INPUT;

    return 0;
}

void common_hal_i2cioexpander_iopin_deinit(i2cioexpander_iopin_obj_t *self) {
    // Switch to input on deinit.
    common_hal_i2cioexpander_iopin_switch_to_input(self, PULL_NONE);
}

bool common_hal_i2cioexpander_iopin_deinited(i2cioexpander_iopin_obj_t *self) {
    return self->expander == NULL || common_hal_i2cioexpander_ioexpander_deinited(self->expander);
}

digitalinout_result_t common_hal_i2cioexpander_iopin_switch_to_input(
    i2cioexpander_iopin_obj_t *self,
    digitalio_pull_t pull) {

    if (pull != PULL_NONE) {
        // IO expanders typically don't support pull resistors
        #if CIRCUITPY_DIGITALIO_HAVE_INVALID_PULL
        return DIGITALINOUT_INVALID_PULL;
        #endif
    }

    self->direction = DIRECTION_INPUT;

    // Clear the output mask bit for this pin
    size_t new_mask = self->expander->output_mask & ~(1 << self->pin_number);
    common_hal_i2cioexpander_ioexpander_set_output_mask(self->expander, new_mask);

    return DIGITALINOUT_OK;
}

digitalinout_result_t common_hal_i2cioexpander_iopin_switch_to_output(
    i2cioexpander_iopin_obj_t *self,
    bool value,
    digitalio_drive_mode_t drive_mode) {

    if (drive_mode != DRIVE_MODE_PUSH_PULL) {
        // IO expanders typically only support push-pull
        #if CIRCUITPY_DIGITALIO_HAVE_INVALID_DRIVE_MODE
        return DIGITALINOUT_INVALID_DRIVE_MODE;
        #endif
    }

    self->direction = DIRECTION_OUTPUT;

    // Set the value first
    size_t new_value = self->expander->output_value;
    if (value) {
        new_value |= (1 << self->pin_number);
    } else {
        new_value &= ~(1 << self->pin_number);
    }
    common_hal_i2cioexpander_ioexpander_set_output_value(self->expander, new_value);

    // Set the output mask bit for this pin
    size_t new_mask = self->expander->output_mask | (1 << self->pin_number);
    common_hal_i2cioexpander_ioexpander_set_output_mask(self->expander, new_mask);

    return DIGITALINOUT_OK;
}

digitalio_direction_t common_hal_i2cioexpander_iopin_get_direction(i2cioexpander_iopin_obj_t *self) {
    return self->direction;
}

mp_negative_errno_t common_hal_i2cioexpander_iopin_set_value(i2cioexpander_iopin_obj_t *self, bool value) {
    size_t current_value;
    common_hal_i2cioexpander_ioexpander_get_output_value(self->expander, &current_value);
    size_t new_value;
    if (value) {
        new_value = current_value | (1 << self->pin_number);
    } else {
        new_value = current_value & ~(1 << self->pin_number);
    }
    if (new_value != current_value) {
        return common_hal_i2cioexpander_ioexpander_set_output_value(self->expander, new_value);
    }
    return 0;
}

mp_negative_errno_t common_hal_i2cioexpander_iopin_get_value(i2cioexpander_iopin_obj_t *self, bool *value) {
    size_t full_value;
    mp_negative_errno_t result = common_hal_i2cioexpander_ioexpander_get_input_value(self->expander, &full_value);
    if (result != 0) {
        return result;
    }
    *value = (full_value & (1 << self->pin_number)) != 0;
    return 0;
}

digitalinout_result_t common_hal_i2cioexpander_iopin_set_drive_mode(
    i2cioexpander_iopin_obj_t *self,
    digitalio_drive_mode_t drive_mode) {

    if (drive_mode != DRIVE_MODE_PUSH_PULL) {
        #if CIRCUITPY_DIGITALIO_HAVE_INVALID_DRIVE_MODE
        return DIGITALINOUT_INVALID_DRIVE_MODE;
        #endif
    }

    return DIGITALINOUT_OK;
}

digitalio_drive_mode_t common_hal_i2cioexpander_iopin_get_drive_mode(i2cioexpander_iopin_obj_t *self) {
    return DRIVE_MODE_PUSH_PULL;
}

digitalinout_result_t common_hal_i2cioexpander_iopin_set_pull(
    i2cioexpander_iopin_obj_t *self,
    digitalio_pull_t pull) {

    if (pull != PULL_NONE) {
        #if CIRCUITPY_DIGITALIO_HAVE_INVALID_PULL
        return DIGITALINOUT_INVALID_PULL;
        #endif
    }

    return DIGITALINOUT_OK;
}

digitalio_pull_t common_hal_i2cioexpander_iopin_get_pull(i2cioexpander_iopin_obj_t *self) {
    return PULL_NONE;
}
