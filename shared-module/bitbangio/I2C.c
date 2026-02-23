// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George, Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/bitbangio/I2C.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/DigitalInOutProtocol.h"
#include "shared-bindings/util.h"
#include "supervisor/port.h"

static void delay(bitbangio_i2c_obj_t *self) {
    // We need to use an accurate delay to get acceptable I2C
    // speeds (eg 1us should be not much more than 1us).
    common_hal_mcu_delay_us(self->us_delay);
}

static bool scl_low(bitbangio_i2c_obj_t *self) {
    return digitalinout_protocol_set_value(self->scl, false) == 0;
}

static bool scl_release(bitbangio_i2c_obj_t *self) {
    if (digitalinout_protocol_set_value(self->scl, true) != 0) {
        return false;
    }
    uint32_t count = self->us_timeout;
    delay(self);
    // For clock stretching, wait for the SCL pin to be released, with timeout.
    digitalinout_protocol_switch_to_input(self->scl, PULL_UP);
    bool value;
    for (; count; --count) {
        if (digitalinout_protocol_get_value(self->scl, &value) != 0) {
            return false;
        }
        if (value) {
            break;
        }
        common_hal_mcu_delay_us(1);
    }
    digitalinout_protocol_switch_to_output(self->scl, true, DRIVE_MODE_OPEN_DRAIN);
    // raise exception on timeout
    if (count == 0) {
        mp_raise_msg_varg(&mp_type_TimeoutError, MP_ERROR_TEXT("%q too long"), MP_QSTR_timeout);
    }
    return true;
}

static bool sda_low(bitbangio_i2c_obj_t *self) {
    return digitalinout_protocol_set_value(self->sda, false) == 0;
}

static bool sda_release(bitbangio_i2c_obj_t *self) {
    return digitalinout_protocol_set_value(self->sda, true) == 0;
}

static bool sda_read(bitbangio_i2c_obj_t *self, bool *value) {
    digitalinout_protocol_switch_to_input(self->sda, PULL_UP);
    if (digitalinout_protocol_get_value(self->sda, value) != 0) {
        return false;
    }
    digitalinout_protocol_switch_to_output(self->sda, true, DRIVE_MODE_OPEN_DRAIN);
    return true;
}

static bool start(bitbangio_i2c_obj_t *self) {
    if (!sda_release(self)) {
        return false;
    }
    delay(self);
    scl_release(self);
    sda_low(self);
    delay(self);
    return true;
}

static bool stop(bitbangio_i2c_obj_t *self) {
    delay(self);
    if (!sda_low(self)) {
        return false;
    }
    delay(self);
    scl_release(self);
    sda_release(self);
    delay(self);
    return true;
}

static int write_byte(bitbangio_i2c_obj_t *self, uint8_t val) {
    delay(self);
    if (!scl_low(self)) {
        return -1;
    }

    for (int i = 7; i >= 0; i--) {
        if ((val >> i) & 1) {
            sda_release(self);
        } else {
            sda_low(self);
        }
        delay(self);
        scl_release(self);
        scl_low(self);
    }

    sda_release(self);
    delay(self);
    scl_release(self);

    bool ret;
    if (!sda_read(self, &ret)) {
        return -1;
    }
    delay(self);
    scl_low(self);

    return !ret;
}

static bool read_byte(bitbangio_i2c_obj_t *self, uint8_t *val, bool ack) {
    delay(self);
    if (!scl_low(self)) {
        return false;
    }
    delay(self);

    uint8_t data = 0;
    for (int i = 7; i >= 0; i--) {
        scl_release(self);
        bool bit;
        sda_read(self, &bit);
        data = (data << 1) | bit;
        scl_low(self);
        delay(self);
    }
    *val = data;

    // send ack/nack bit
    if (ack) {
        sda_low(self);
    }
    delay(self);
    scl_release(self);
    scl_low(self);
    sda_release(self);

    return true;
}

void shared_module_bitbangio_i2c_construct(bitbangio_i2c_obj_t *self,
    mp_obj_t scl,
    mp_obj_t sda,
    uint32_t frequency,
    uint32_t us_timeout) {

    self->us_timeout = us_timeout;
    self->us_delay = 500000 / frequency;
    if (self->us_delay == 0) {
        self->us_delay = 1;
    }

    // Allocate the pins in the same place as self.
    bool use_port_allocation = !gc_alloc_possible() || !gc_ptr_on_heap(self);

    // Convert scl from Pin to DigitalInOutProtocol
    self->scl = digitalinout_protocol_from_pin(scl, MP_QSTR_scl, false, use_port_allocation, &self->own_scl);

    // Convert sda from Pin to DigitalInOutProtocol
    self->sda = digitalinout_protocol_from_pin(sda, MP_QSTR_sda, false, use_port_allocation, &self->own_sda);

    digitalinout_protocol_switch_to_output(self->scl, true, DRIVE_MODE_OPEN_DRAIN);
    digitalinout_protocol_switch_to_output(self->sda, true, DRIVE_MODE_OPEN_DRAIN);

    if (!stop(self)) {
        mp_raise_OSError(MP_EIO);
    }
}

bool shared_module_bitbangio_i2c_deinited(bitbangio_i2c_obj_t *self) {
    // If one is deinited, both will be.
    return digitalinout_protocol_deinited(self->scl);
}

void shared_module_bitbangio_i2c_deinit(bitbangio_i2c_obj_t *self) {
    if (shared_module_bitbangio_i2c_deinited(self)) {
        return;
    }
    // Only deinit and free the pins if we own them
    if (self->own_scl) {
        digitalinout_protocol_deinit(self->scl);
        circuitpy_free_obj(self->scl);
    }
    if (self->own_sda) {
        digitalinout_protocol_deinit(self->sda);
        circuitpy_free_obj(self->sda);
    }
}

bool shared_module_bitbangio_i2c_try_lock(bitbangio_i2c_obj_t *self) {
    bool success = false;
    common_hal_mcu_disable_interrupts();
    if (!self->locked) {
        self->locked = true;
        success = true;
    }
    common_hal_mcu_enable_interrupts();
    return success;
}

bool shared_module_bitbangio_i2c_has_lock(bitbangio_i2c_obj_t *self) {
    return self->locked;
}

void shared_module_bitbangio_i2c_unlock(bitbangio_i2c_obj_t *self) {
    self->locked = false;
}

bool shared_module_bitbangio_i2c_probe(bitbangio_i2c_obj_t *self, uint8_t addr) {
    if (!start(self)) {
        mp_raise_OSError(MP_EIO);
    }
    int result = write_byte(self, addr << 1);
    stop(self);
    if (result < 0) {
        mp_raise_OSError(MP_EIO);
    }
    return result;
}

uint8_t shared_module_bitbangio_i2c_write(bitbangio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len, bool transmit_stop_bit) {
    // start the I2C transaction
    if (!start(self)) {
        return MP_EIO;
    }
    uint8_t status = 0;
    int result = write_byte(self, addr << 1);
    if (result < 0) {
        status = MP_EIO;
    } else if (!result) {
        status = MP_ENODEV;
    }

    if (status == 0) {
        for (uint32_t i = 0; i < len; i++) {
            if (!write_byte(self, data[i])) {
                status = MP_EIO;
                break;
            }
        }
    }

    if (transmit_stop_bit) {
        stop(self);
    }
    return status;
}

uint8_t shared_module_bitbangio_i2c_read(bitbangio_i2c_obj_t *self, uint16_t addr,
    uint8_t *data, size_t len) {
    // start the I2C transaction
    if (!start(self)) {
        return MP_EIO;
    }
    uint8_t status = 0;
    if (!write_byte(self, (addr << 1) | 1)) {
        status = MP_ENODEV;
    }

    if (status == 0) {
        for (uint32_t i = 0; i < len; i++) {
            if (!read_byte(self, data + i, i < len - 1)) {
                status = MP_EIO;
                break;
            }
        }
    }

    stop(self);
    return status;
}
