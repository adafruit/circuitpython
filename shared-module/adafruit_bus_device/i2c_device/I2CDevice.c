// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Mark Komus
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/adafruit_bus_device/i2c_device/I2CDevice.h"
#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"

void common_hal_adafruit_bus_device_i2cdevice_construct(adafruit_bus_device_i2cdevice_obj_t *self, mp_obj_t *i2c, uint8_t device_address) {
    self->i2c = i2c;
    self->device_address = device_address;
}

void common_hal_adafruit_bus_device_i2cdevice_lock(adafruit_bus_device_i2cdevice_obj_t *self) {
    mp_obj_t dest[2];
    mp_load_method(self->i2c, MP_QSTR_try_lock, dest);

    mp_obj_t success = mp_call_method_n_kw(0, 0, dest);

    while (!mp_obj_is_true(success)) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }

        success = mp_call_method_n_kw(0, 0, dest);
    }
}

void common_hal_adafruit_bus_device_i2cdevice_unlock(adafruit_bus_device_i2cdevice_obj_t *self) {
    mp_obj_t dest[2];
    mp_load_method(self->i2c, MP_QSTR_unlock, dest);
    mp_call_method_n_kw(0, 0, dest);
}

void common_hal_adafruit_bus_device_i2cdevice_probe_for_device(adafruit_bus_device_i2cdevice_obj_t *self) {
    common_hal_adafruit_bus_device_i2cdevice_lock(self);

    mp_obj_t dest[3];
    mp_load_method(self->i2c, MP_QSTR_probe, dest);
    dest[2] = MP_OBJ_NEW_SMALL_INT(self->device_address);
    const bool found = mp_obj_is_true(mp_call_method_n_kw(1, 0, dest));

    common_hal_adafruit_bus_device_i2cdevice_unlock(self);

    if (!found) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No I2C device at address: 0x%x"), self->device_address);
    }
}
