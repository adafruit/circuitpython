/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

static bool in_used = false;

// Construct I2C protocol, this function init i2c peripheral
void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl,
    const mcu_pin_obj_t *sda,
    uint32_t frequency, uint32_t timeout) {


}

// Never reset I2C obj when reload
void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    common_hal_never_reset_pin(self->sda);
    common_hal_never_reset_pin(self->scl);
}

// Check I2C status, deinited or not
bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda == NULL;
}

// Deinit i2c obj, reset I2C pin
void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    // FIXME: Implement
}

void common_hal_busio_i2c_mark_deinit(busio_i2c_obj_t *self) {
    // FIXME: Implement
}

// Probe device in I2C bus
bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    // FIXME: Implement

    return true;
}

// Lock I2C bus
bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {

    // FIXME: Implement

    return false;
}

// Check I2C lock status
bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

// Unlock I2C bus
void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
}

// Write data to the device selected by address
uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len) {

    // FIXME: Implement
    return 0;
}

// Read into buffer from the device selected by address
uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self,
    uint16_t addr,
    uint8_t *data, size_t len) {

    // FIXME: Implement
    return 0;
}

// Write the bytes from out_data to the device selected by address,
uint8_t common_hal_busio_i2c_write_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *out_data, size_t out_len,
    uint8_t *in_data, size_t in_len) {

    // FIXME: Implement

    return 0;
}
