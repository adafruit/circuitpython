// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/digitalio/DigitalInOut.h"

#include <iobroker/iobroker.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

void common_hal_digitalio_digitalinout_never_reset(
    digitalio_digitalinout_obj_t *self) {
}

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
    digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {
    // Claim the pin in the iobroker module so that bus allocations refuse
    // it while this object holds it. The call also resolves the GPIO
    // controller device and pin number from the pin's global number; they are
    // kept in the object for every later pad operation.
    int ret = iobroker_gpio_allocate(pin->package_pin, &self->port, &self->number);
    if (ret < 0) {
        return DIGITALINOUT_PIN_BUSY;
    }

    self->pin = pin;

    if (!device_is_ready(self->port)) {
        printk("Port device not ready\n");
        common_hal_digitalio_digitalinout_deinit(self);
        return DIGITALINOUT_PIN_BUSY;
    }

    if (gpio_pin_configure(self->port, self->number, GPIO_INPUT) != 0) {
        common_hal_digitalio_digitalinout_deinit(self);
        return DIGITALINOUT_PIN_BUSY;
    }
    self->direction = DIRECTION_INPUT;

    return DIGITALINOUT_OK;
}

bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }

    (void)iobroker_gpio_release(self->port, self->number);
    self->pin = NULL;
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_input(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    self->direction = DIRECTION_INPUT;
    common_hal_digitalio_digitalinout_set_pull(self, pull);
    return DIGITALINOUT_OK;
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(
    digitalio_digitalinout_obj_t *self, bool value,
    digitalio_drive_mode_t drive_mode) {

    self->direction = DIRECTION_OUTPUT;
    common_hal_digitalio_digitalinout_set_drive_mode(self, drive_mode);
    common_hal_digitalio_digitalinout_set_value(self, value);
    return DIGITALINOUT_OK;
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
    digitalio_digitalinout_obj_t *self) {
    return self->direction;
}

void common_hal_digitalio_digitalinout_set_value(
    digitalio_digitalinout_obj_t *self, bool value) {
    int res = gpio_pin_set(self->port, self->number, value);
    if (res != 0) {
        printk("Failed to set value %d\n", res);
    }
    // Not all MCUs can read back the output value, so store it.
    self->value = value;
}

bool common_hal_digitalio_digitalinout_get_value(
    digitalio_digitalinout_obj_t *self) {
    if (self->direction == DIRECTION_OUTPUT) {
        return self->value;
    }
    return gpio_pin_get(self->port, self->number) == 1;
}

digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(
    digitalio_digitalinout_obj_t *self,
    digitalio_drive_mode_t drive_mode) {
    // Also INPUT so we can read the value back.
    gpio_flags_t flags = GPIO_OUTPUT;
    if (drive_mode == DRIVE_MODE_OPEN_DRAIN) {
        flags |= GPIO_OPEN_DRAIN;
    }
    int res = gpio_pin_configure(self->port, self->number, flags);
    if (res != 0) {
        // TODO: Fake open drain.
        printk("Failed to set drive mode %d\n", res);
        return DIGITALINOUT_INVALID_DRIVE_MODE;
    }
    self->drive_mode = drive_mode;
    return DIGITALINOUT_OK;
}

digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
    digitalio_digitalinout_obj_t *self) {
    return self->drive_mode;
}

digitalinout_result_t common_hal_digitalio_digitalinout_set_pull(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    gpio_flags_t pull_flags = 0;
    if (pull == PULL_UP) {
        pull_flags = GPIO_PULL_UP;
    } else if (pull == PULL_DOWN) {
        pull_flags = GPIO_PULL_DOWN;
    }
    if (gpio_pin_configure(self->port, self->number, GPIO_INPUT | pull_flags) != 0) {
        return DIGITALINOUT_INVALID_PULL;
    }
    self->pull = pull;

    return DIGITALINOUT_OK;
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
    digitalio_digitalinout_obj_t *self) {
    return self->pull;
}
