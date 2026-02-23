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

#include "max32_port.h"

#define I2C_PRIORITY 1

// Set each bit to indicate an active I2c
static uint8_t i2c_active = 0;
static volatile int i2c_err;

// I2C struct for configuring GPIO pins
extern const mxc_gpio_cfg_t i2c_maps[NUM_I2C];

// Construct I2C protocol, this function init i2c peripheral
void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl,
    const mcu_pin_obj_t *sda,
    uint32_t frequency, uint32_t timeout) {
    // Check for NULL Pointers && valid I2C settings
    assert(self);

    /* NOTE: The validate_obj_is_free_pin() calls in shared-bindings/busio/I2C.c
       will ensure that scl and sda are both pins and cannot be null
       ref: https://github.com/adafruit/circuitpython/pull/10413
    */

    // Assign I2C ID based on pins
    int i2c_id = pinsToI2c(sda, scl);
    if (i2c_id == -1) {
        return;
    } else {
        self->i2c_id = i2c_id;
        self->i2c_regs = MXC_I2C_GET_I2C(i2c_id);
    }

    // Check for valid I2C controller
    assert((self->i2c_id >= 0) && (self->i2c_id < NUM_I2C));
    assert(!(i2c_active & (1 << self->i2c_id)));

    // Attach I2C pins
    self->sda = sda;
    self->scl = scl;
    common_hal_mcu_pin_claim(self->sda);
    common_hal_mcu_pin_claim(self->scl);

    // Clear all flags
    MXC_I2C_ClearFlags(self->i2c_regs, 0xFFFFFF, 0xFFFFFF);

    // Init as master, no slave address
    MXC_I2C_Shutdown(self->i2c_regs);
    MXC_I2C_Init(self->i2c_regs, 1, 0);

    // Set frequency arg (CircuitPython shared-bindings validate)
    MXC_I2C_SetFrequency(self->i2c_regs, frequency);

    // Indicate to this module that the I2C is active
    i2c_active |= (1 << self->i2c_id);

    // Set the timeout to a default value
    if (timeout > 100) {
        self->timeout = 1;
    } else {
        self->timeout = timeout;
    }

    return;
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
    MXC_I2C_Shutdown(self->i2c_regs);

    common_hal_reset_pin(self->sda);
    common_hal_reset_pin(self->scl);

    self->sda = NULL;
    self->scl = NULL;
}

// Probe device in I2C bus
bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    uint32_t int_fl0;
    bool ret = 0;

    // If not in Master mode, error out (can happen in some error conditions)
    if (!(self->i2c_regs->ctrl & MXC_F_I2C_CTRL_MST_MODE)) {
        return false;
    }

    // Clear FIFOs & all interrupt flags
    MXC_I2C_ClearRXFIFO(self->i2c_regs);
    MXC_I2C_ClearTXFIFO(self->i2c_regs);
    MXC_I2C_ClearFlags(self->i2c_regs, 0xFFFFFF, 0xFFFFFF);

    // Pre-load target address into transmit FIFO
    addr = (addr << 1);
    self->i2c_regs->fifo = addr;

    // Set start bit & wait for it to clear
    MXC_I2C_Start(self->i2c_regs);

    // wait for ACK/NACK
    while (!(self->i2c_regs->intfl0 & MXC_F_I2C_INTFL0_ADDR_ACK) &&
           !(self->i2c_regs->intfl0 & MXC_F_I2C_INTFL0_ADDR_NACK_ERR)) {
    }

    // Save interrupt flags for ACK/NACK checking
    int_fl0 = self->i2c_regs->intfl0;

    // Set / Wait for stop
    MXC_I2C_Stop(self->i2c_regs);

    // Wait for controller not busy, then clear flags
    while (self->i2c_regs->status & MXC_F_I2C_STATUS_BUSY) {
        ;
    }
    MXC_I2C_ClearFlags(self->i2c_regs, 0xFFFFFF, 0xFFFFFF);

    if (int_fl0 & MXC_F_I2C_INTFL0_ADDR_ACK) {
        ret = true;
    } else {
        ret = false;
    }
    return ret;
}

// Lock I2C bus
bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {

    if (self->i2c_regs->status & MXC_F_I2C_STATUS_BUSY) {
        return false;
    } else {
        self->has_lock = true;
        return true;
    }
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
mp_negative_errno_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len) {

    int ret;
    mxc_i2c_req_t wr_req = {
        .addr = addr,
        .i2c = self->i2c_regs,
        .tx_buf = (uint8_t *)data,
        .tx_len = len,
        .rx_buf = NULL,
        .rx_len = 0,
        .callback = NULL
    };
    ret = MXC_I2C_MasterTransaction(&wr_req);
    if (ret) {
        return -MP_EIO;
    }

    return 0;
}

// Read into buffer from the device selected by address
mp_negative_errno_t common_hal_busio_i2c_read(busio_i2c_obj_t *self,
    uint16_t addr,
    uint8_t *data, size_t len) {

    int ret;
    mxc_i2c_req_t rd_req = {
        .addr = addr,
        .i2c = self->i2c_regs,
        .tx_buf = NULL,
        .tx_len = 0,
        .rx_buf = data,
        .rx_len = len,
        .callback = NULL
    };
    ret = MXC_I2C_MasterTransaction(&rd_req);
    if (ret) {
        // Return I/O error
        return -MP_EIO;
    }

    return 0;
}

// Write the bytes from out_data to the device selected by address
mp_negative_errno_t common_hal_busio_i2c_write_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *out_data, size_t out_len,
    uint8_t *in_data, size_t in_len) {

    int ret;
    mxc_i2c_req_t wr_rd_req = {
        .addr = addr,
        .i2c = self->i2c_regs,
        .tx_buf = out_data,
        .tx_len = out_len,
        .rx_buf = in_data,
        .rx_len = in_len,
        .callback = NULL
    };
    ret = MXC_I2C_MasterTransaction(&wr_rd_req);
    if (ret) {
        return -MP_EIO;
    }

    return 0;
}
