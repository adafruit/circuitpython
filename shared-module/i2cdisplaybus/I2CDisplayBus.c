// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/i2cdisplaybus/I2CDisplayBus.h"

#include <stdint.h>
#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/display_core.h"

void common_hal_i2cdisplaybus_i2cdisplaybus_construct(i2cdisplaybus_i2cdisplaybus_obj_t *self,
    busio_i2c_obj_t *i2c, uint16_t device_address, const mcu_pin_obj_t *reset) {

    // Reset the display before probing
    self->reset.base.type = &mp_type_NoneType;
    if (reset != NULL) {
        self->reset.base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(&self->reset, reset);
        common_hal_digitalio_digitalinout_switch_to_output(&self->reset, true, DRIVE_MODE_PUSH_PULL);
        common_hal_i2cdisplaybus_i2cdisplaybus_reset(self);
    }

    // Probe the bus to see if a device acknowledges the given address.
    if (!common_hal_busio_i2c_probe(i2c, device_address)) {
        common_hal_i2cdisplaybus_i2cdisplaybus_deinit(self);
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Unable to find I2C Display at %x"), device_address);
    }

    // Write to the device and return 0 on success or an appropriate error code from mperrno.h
    self->bus = i2c;

    self->address = device_address;

    displayio_register(MP_OBJ_FROM_PTR(self));
}

void common_hal_i2cdisplaybus_i2cdisplaybus_deinit(i2cdisplaybus_i2cdisplaybus_obj_t *self) {
    if (self->bus == &self->inline_bus) {
        common_hal_busio_i2c_deinit(self->bus);
    }

    self->bus = NULL;

    if (self->reset.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->reset);
    }
}

static void i2cdisplaybus_protocol_deinit(void *self) {
    common_hal_i2cdisplaybus_i2cdisplaybus_deinit(self);
}

static void *i2cdisplaybus_protocol_start_vm(void *self) {
    return i2cdisplaybus_i2cdisplaybus_start_vm(self);
}

static bool i2cdisplaybus_protocol_is_moveable(void *self_in) {
    i2cdisplaybus_i2cdisplaybus_obj_t *self = self_in;
    return !gc_ptr_on_heap(self->bus) || common_hal_busio_i2c_is_moveable(self->bus);
}

static bool i2cdisplaybus_protocol_stop_vm(void *self_in) {
    i2cdisplaybus_i2cdisplaybus_obj_t *self = self_in;
    if (!i2cdisplaybus_protocol_is_moveable(self)) {
        if (gc_ptr_on_heap(self->bus)) {
            common_hal_busio_i2c_deinit(self->bus);
        }
        return false;
    }
    if (gc_ptr_on_heap(self->bus)) {
        if (!common_hal_busio_i2c_move(self->bus, &self->inline_bus)) {
            common_hal_busio_i2c_deinit(self->bus);
            return false;
        }
        self->bus = &self->inline_bus;
    }
    return true;
}

const displayio_protocol_t i2cdisplaybus_i2cdisplaybus_protocol = {
    .deinit = i2cdisplaybus_protocol_deinit,
    .is_moveable = i2cdisplaybus_protocol_is_moveable,
    .start_vm = i2cdisplaybus_protocol_start_vm,
    .stop_vm = i2cdisplaybus_protocol_stop_vm,
};

bool common_hal_i2cdisplaybus_i2cdisplaybus_reset(mp_obj_t obj) {
    i2cdisplaybus_i2cdisplaybus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->reset.base.type == &mp_type_NoneType) {
        return false;
    }

    common_hal_digitalio_digitalinout_set_value(&self->reset, false);
    common_hal_mcu_delay_us(4);
    common_hal_digitalio_digitalinout_set_value(&self->reset, true);
    return true;
}

bool common_hal_i2cdisplaybus_i2cdisplaybus_bus_free(mp_obj_t obj) {
    i2cdisplaybus_i2cdisplaybus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!common_hal_busio_i2c_try_lock(self->bus)) {
        return false;
    }
    common_hal_busio_i2c_unlock(self->bus);
    return true;
}

bool common_hal_i2cdisplaybus_i2cdisplaybus_begin_transaction(mp_obj_t obj) {
    i2cdisplaybus_i2cdisplaybus_obj_t *self = MP_OBJ_TO_PTR(obj);
    return common_hal_busio_i2c_try_lock(self->bus);
}

void common_hal_i2cdisplaybus_i2cdisplaybus_send(mp_obj_t obj, display_byte_type_t data_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length) {
    i2cdisplaybus_i2cdisplaybus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (data_type == DISPLAY_COMMAND) {
        uint8_t command_bytes[2 * data_length];
        for (uint32_t i = 0; i < data_length; i++) {
            command_bytes[2 * i] = 0x80;
            command_bytes[2 * i + 1] = data[i];
        }
        common_hal_busio_i2c_write(self->bus, self->address, command_bytes, 2 * data_length);
    } else {
        uint8_t data_bytes[data_length + 1];
        data_bytes[0] = 0x40;
        memcpy(data_bytes + 1, data, data_length);
        common_hal_busio_i2c_write(self->bus, self->address, data_bytes, data_length + 1);
    }
}

void common_hal_i2cdisplaybus_i2cdisplaybus_end_transaction(mp_obj_t obj) {
    i2cdisplaybus_i2cdisplaybus_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_busio_i2c_unlock(self->bus);
}

mp_obj_t i2cdisplaybus_i2cdisplaybus_start_vm(i2cdisplaybus_i2cdisplaybus_obj_t *self) {
    mp_obj_t refs = mp_obj_new_list(1, NULL);
    if (self->bus == &self->inline_bus) {
        busio_i2c_obj_t *heap_bus = mp_obj_malloc_with_finaliser(busio_i2c_obj_t, &busio_i2c_type);
        if (!common_hal_busio_i2c_move(self->bus, heap_bus)) {
            common_hal_busio_i2c_deinit(self->bus);
            m_free(heap_bus);
            mp_raise_ValueError(MP_ERROR_TEXT("Display resources cannot move between VMs"));
        }
        self->bus = heap_bus;
    }
    mp_obj_list_store(refs, MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_FROM_PTR(self->bus));
    return refs;
}
