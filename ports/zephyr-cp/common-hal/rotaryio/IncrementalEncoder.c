// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "common-hal/rotaryio/IncrementalEncoder.h"
#include "shared-bindings/rotaryio/IncrementalEncoder.h"
#include "shared-module/rotaryio/IncrementalEncoder.h"

#include "bindings/zephyr_kernel/__init__.h"
#include "py/runtime.h"

#include <errno.h>
#include <iobroker/iobroker.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>

static void incrementalencoder_gpio_callback(const struct device *port,
    struct gpio_callback *cb, gpio_port_pins_t pins) {
    (void)port;
    (void)pins;
    rotaryio_incrementalencoder_gpio_callback_t *callback =
        CONTAINER_OF(cb, rotaryio_incrementalencoder_gpio_callback_t, callback);
    rotaryio_incrementalencoder_obj_t *self = callback->encoder;
    if (self == NULL || self->pin_a == NULL) {
        return;
    }

    int a = gpio_pin_get(self->port_a, self->number_a);
    int b = gpio_pin_get(self->port_b, self->number_b);
    if (a < 0 || b < 0) {
        return;
    }
    uint8_t new_state = ((uint8_t)a << 1) | (uint8_t)b;
    shared_module_softencoder_state_update(self, new_state);
}

void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t *self,
    const mcu_pin_obj_t *pin_a, const mcu_pin_obj_t *pin_b) {
    // Ensure object starts in its deinit state.
    common_hal_rotaryio_incrementalencoder_mark_deinit(self);

    self->pin_a = pin_a;
    self->pin_b = pin_b;
    self->divisor = 4;

    // Claim both pins in the iobroker module so that bus allocations
    // refuse them while this object holds them. The calls also resolve the
    // GPIO controller devices and pin numbers from the pins' global numbers;
    // they are kept in the object for every later pad operation.
    int ret = iobroker_gpio_allocate(pin_a->package_pin, &self->port_a, &self->number_a);
    if (ret < 0) {
        common_hal_rotaryio_incrementalencoder_deinit(self);
        raise_zephyr_error(ret);
    }

    ret = iobroker_gpio_allocate(pin_b->package_pin, &self->port_b, &self->number_b);
    if (ret < 0) {
        common_hal_rotaryio_incrementalencoder_deinit(self);
        raise_zephyr_error(ret);
    }

    if (!device_is_ready(self->port_a) || !device_is_ready(self->port_b)) {
        common_hal_rotaryio_incrementalencoder_deinit(self);
        raise_zephyr_error(-ENODEV);
    }

    int result = gpio_pin_configure(self->port_a, self->number_a, GPIO_INPUT | GPIO_PULL_UP);
    if (result != 0) {
        common_hal_rotaryio_incrementalencoder_deinit(self);
        raise_zephyr_error(result);
    }

    result = gpio_pin_configure(self->port_b, self->number_b, GPIO_INPUT | GPIO_PULL_UP);
    if (result != 0) {
        common_hal_rotaryio_incrementalencoder_deinit(self);
        raise_zephyr_error(result);
    }

    self->callback_a.encoder = self;
    gpio_init_callback(&self->callback_a.callback, incrementalencoder_gpio_callback,
        BIT(self->number_a));
    result = gpio_add_callback(self->port_a, &self->callback_a.callback);
    if (result != 0) {
        common_hal_rotaryio_incrementalencoder_deinit(self);
        raise_zephyr_error(result);
    }

    self->callback_b.encoder = self;
    gpio_init_callback(&self->callback_b.callback, incrementalencoder_gpio_callback,
        BIT(self->number_b));
    result = gpio_add_callback(self->port_b, &self->callback_b.callback);
    if (result != 0) {
        common_hal_rotaryio_incrementalencoder_deinit(self);
        raise_zephyr_error(result);
    }

    result = gpio_pin_interrupt_configure(self->port_a, self->number_a, GPIO_INT_EDGE_BOTH);
    if (result != 0) {
        common_hal_rotaryio_incrementalencoder_deinit(self);
        raise_zephyr_error(result);
    }

    result = gpio_pin_interrupt_configure(self->port_b, self->number_b, GPIO_INT_EDGE_BOTH);
    if (result != 0) {
        common_hal_rotaryio_incrementalencoder_deinit(self);
        raise_zephyr_error(result);
    }

    int a = gpio_pin_get(self->port_a, self->number_a);
    int b = gpio_pin_get(self->port_b, self->number_b);
    uint8_t quiescent_state = ((uint8_t)(a > 0) << 1) | (uint8_t)(b > 0);
    shared_module_softencoder_state_init(self, quiescent_state);
}

bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t *self) {
    return self->pin_a == NULL;
}

void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t *self) {
    if (common_hal_rotaryio_incrementalencoder_deinited(self)) {
        return;
    }

    // Best-effort cleanup. During failed construct(), some of these may not be
    // initialized yet. Ignore cleanup errors. The pad operations are only run
    // when both claims were taken, so that the devices are valid.
    if (self->port_a != NULL && self->port_b != NULL) {
        gpio_pin_interrupt_configure(self->port_a, self->number_a, GPIO_INT_DISABLE);
        gpio_pin_interrupt_configure(self->port_b, self->number_b, GPIO_INT_DISABLE);
        gpio_remove_callback(self->port_a, &self->callback_a.callback);
        gpio_remove_callback(self->port_b, &self->callback_b.callback);
    }
    if (self->port_a != NULL) {
        (void)iobroker_gpio_release(self->port_a, self->number_a);
        self->port_a = NULL;
    }
    if (self->port_b != NULL) {
        (void)iobroker_gpio_release(self->port_b, self->number_b);
        self->port_b = NULL;
    }

    common_hal_rotaryio_incrementalencoder_mark_deinit(self);
}

void common_hal_rotaryio_incrementalencoder_mark_deinit(rotaryio_incrementalencoder_obj_t *self) {
    self->pin_a = NULL;
    self->pin_b = NULL;
    self->port_a = NULL;
    self->port_b = NULL;
}
