// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/i2cioexpander/IOExpander.h"
#include "shared-bindings/i2cioexpander/IOPin.h"

#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/busio/I2C.h"
#include "supervisor/port.h"

void common_hal_i2cioexpander_ioexpander_construct(
    i2cioexpander_ioexpander_obj_t *self,
    mp_obj_t i2c,
    uint8_t address,
    uint8_t num_pins,
    uint16_t set_value_reg,
    uint16_t get_value_reg,
    uint16_t set_direction_reg) {

    // Store the I2C bus
    self->i2c = (busio_i2c_obj_t *)i2c;
    self->address = address;
    self->num_pins = num_pins;
    self->output_value = 0;
    self->output_mask = 0;

    // Parse optional register addresses
    self->has_set_value = (set_value_reg != NO_REGISTER);
    if (self->has_set_value) {
        self->set_value_reg = set_value_reg;
    }

    self->has_get_value = (get_value_reg != NO_REGISTER);
    if (self->has_get_value) {
        self->get_value_reg = get_value_reg;
    }

    self->has_set_direction = (set_direction_reg != NO_REGISTER);
    if (self->has_set_direction) {
        self->set_direction_reg = set_direction_reg;
    }

    bool allocate_in_port_heap = !gc_alloc_possible() || !gc_ptr_on_heap(self);

    // Allocate tuple with space for pin objects in items[]
    size_t tuple_size = offsetof(mp_obj_tuple_t, items) + sizeof(mp_obj_t) * num_pins;
    mp_obj_tuple_t *pins_tuple = allocate_in_port_heap ? port_malloc(tuple_size, false) : m_malloc(tuple_size);
    pins_tuple->base.type = &mp_type_tuple;
    pins_tuple->len = num_pins;

    // Create IOPin objects for each pin
    size_t pin_size = sizeof(i2cioexpander_iopin_obj_t);
    for (uint8_t i = 0; i < num_pins; i++) {
        i2cioexpander_iopin_obj_t *pin = allocate_in_port_heap ? port_malloc(pin_size, false) : m_malloc(pin_size);
        pin->base.type = &i2cioexpander_iopin_type;
        i2cioexpander_iopin_construct(pin, self, i);
        pins_tuple->items[i] = MP_OBJ_FROM_PTR(pin);
    }

    self->pins = pins_tuple;
}

void common_hal_i2cioexpander_ioexpander_deinit(i2cioexpander_ioexpander_obj_t *self) {
    if (gc_alloc_possible() && !gc_ptr_on_heap(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot deinitialize board IOExpander"));
    }
    for (uint8_t i = 0; i < self->num_pins; i++) {
        circuitpy_free_obj(self->pins->items[i]);
    }
    circuitpy_free_obj(self->pins);
    self->i2c = NULL;
}

bool common_hal_i2cioexpander_ioexpander_deinited(i2cioexpander_ioexpander_obj_t *self) {
    return self->i2c == NULL;
}

mp_negative_errno_t common_hal_i2cioexpander_ioexpander_get_input_value(i2cioexpander_ioexpander_obj_t *self, size_t *value) {
    uint8_t buffer[2];
    uint8_t num_bytes = (self->num_pins > 8) ? 2 : 1;

    while (!common_hal_busio_i2c_try_lock(self->i2c)) {
        RUN_BACKGROUND_TASKS;
    }

    mp_negative_errno_t result;
    if (self->has_get_value) {
        // Send register address then read
        result = common_hal_busio_i2c_write_read(
            self->i2c, self->address, &self->get_value_reg, 1, buffer, num_bytes);
    } else {
        // Read directly without register address
        result = common_hal_busio_i2c_read(self->i2c, self->address, buffer, num_bytes);
    }
    common_hal_busio_i2c_unlock(self->i2c);

    if (result != 0) {
        return result;
    }

    if (num_bytes == 2) {
        *value = buffer[0] | (buffer[1] << 8);
    } else {
        *value = buffer[0];
    }
    return 0;
}

void common_hal_i2cioexpander_ioexpander_get_output_value(i2cioexpander_ioexpander_obj_t *self, size_t *value) {
    *value = self->output_value;
}

mp_negative_errno_t common_hal_i2cioexpander_ioexpander_set_output_value(i2cioexpander_ioexpander_obj_t *self, size_t value) {
    uint8_t buffer[5];
    uint8_t num_bytes = 0;

    // Add register address if provided
    if (self->has_set_value) {
        buffer[num_bytes++] = self->set_value_reg;
    }

    size_t current_value = self->output_value;
    if (current_value == value) {
        return 0;
    }
    size_t diff = current_value ^ value;

    // Add value byte(s) but only if a high bit is changed
    buffer[num_bytes++] = value & 0xFF;
    if (self->num_pins > 8 && (diff >> 8) != 0) {
        buffer[num_bytes++] = (value >> 8) & 0xFF;
    }
    if (self->num_pins > 16 && (diff >> 16) != 0) {
        buffer[num_bytes++] = (value >> 16) & 0xFF;
    }
    if (self->num_pins > 24 && (diff >> 24) != 0) {
        buffer[num_bytes++] = (value >> 24) & 0xFF;
    }

    if (!common_hal_busio_i2c_try_lock(self->i2c)) {
        return -MP_EBUSY;
    }

    mp_negative_errno_t result = common_hal_busio_i2c_write(self->i2c, self->address, buffer, num_bytes);
    common_hal_busio_i2c_unlock(self->i2c);
    if (result == 0) {
        self->output_value = value;
    }
    return result;
}

void common_hal_i2cioexpander_ioexpander_get_output_mask(i2cioexpander_ioexpander_obj_t *self, size_t *mask) {
    *mask = self->output_mask;
}

mp_negative_errno_t common_hal_i2cioexpander_ioexpander_set_output_mask(i2cioexpander_ioexpander_obj_t *self, size_t mask) {
    self->output_mask = mask;

    // Only write to device if direction register is provided
    if (!self->has_set_direction) {
        return 0;
    }

    uint8_t buffer[3];
    uint8_t num_bytes = 0;

    // Add register address
    buffer[num_bytes++] = self->set_direction_reg;

    // Invert the mask so 0 indicates output. We taken 1 for output to match output enable.
    size_t inverted_mask = ~mask;

    // Add mask byte(s)
    buffer[num_bytes++] = inverted_mask & 0xFF;
    if (self->num_pins > 8) {
        buffer[num_bytes++] = (inverted_mask >> 8) & 0xFF;
    }

    if (!common_hal_busio_i2c_try_lock(self->i2c)) {
        return -MP_EBUSY;
    }

    mp_negative_errno_t result = common_hal_busio_i2c_write(self->i2c, self->address, buffer, num_bytes);
    common_hal_busio_i2c_unlock(self->i2c);
    return result;
}

mp_obj_t common_hal_i2cioexpander_ioexpander_get_pins(i2cioexpander_ioexpander_obj_t *self) {
    return MP_OBJ_FROM_PTR(self->pins);
}
