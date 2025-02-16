// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2017 hathach
// SPDX-FileCopyrightText: Copyright (c) 2016 Sandeep Mistry All right reserved.
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/tick.h"
#include "py/mperrno.h"
#include "py/runtime.h"


void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    // never_reset_pin_number(self->scl_pin_number);
    // never_reset_pin_number(self->sda_pin_number);
}

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self, const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {

}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    // return self->sda_pin_number == NO_PIN;
    return true;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }

    // nrfx_twim_uninit(&self->twim_peripheral->twim);

    // reset_pin_number(self->sda_pin_number);
    // reset_pin_number(self->scl_pin_number);

    // self->twim_peripheral->in_use = false;
    // common_hal_busio_i2c_mark_deinit(self);
}

void common_hal_busio_i2c_mark_deinit(busio_i2c_obj_t *self) {
    // self->sda_pin_number = NO_PIN;
}

// nrfx_twim_tx doesn't support 0-length data so we fall back to the hal API
bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    bool found = true;

    return found;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return false;
    }
    bool grabbed_lock = false;
    return grabbed_lock;
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
}

uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr, const uint8_t *data, size_t len) {
    return 0;
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr, uint8_t *data, size_t len) {
    if (len == 0) {
        return 0;
    }

}

uint8_t common_hal_busio_i2c_write_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *out_data, size_t out_len, uint8_t *in_data, size_t in_len) {
    uint8_t result = _common_hal_busio_i2c_write(self, addr, out_data, out_len, false);
    if (result != 0) {
        return result;
    }

    return common_hal_busio_i2c_read(self, addr, in_data, in_len);
}
