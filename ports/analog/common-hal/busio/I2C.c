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

typedef enum {
    I2C_FREE = 0,
    I2C_BUSY,
    I2C_NEVER_RESET,
} i2c_status_t;

// Set each bit to indicate an active UART
// will be checked by ISR Handler for which ones to call
static uint8_t i2c_active = 0;
static i2c_status_t i2c_status[NUM_I2C];
static volatile int i2c_err;

// I2C Interrupt Handler
void i2c_isr(void) {
    for (int i = 0; i < NUM_I2C; i++) {
        if (i2c_active & (1 << i)) {
            // NOTE: I2C_GET_TMR actually returns the I2C registers
            MXC_I2C_AsyncHandler(MXC_I2C_GET_I2C(i));
        }
    }
}

// Callback gets called when AsyncRequest is COMPLETE
// (e.g. txLen == txCnt)
// static volatile void i2cCallback(mxc_i2c_req_t *req, int error) {
//     i2c_status[MXC_I2C_GET_IDX(req->i2c)] = I2C_FREE;
//     i2c_err = error;
// }

// Construct I2C protocol, this function init i2c peripheral
void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl,
    const mcu_pin_obj_t *sda,
    uint32_t frequency, uint32_t timeout) {

    int temp, err = 0;

    // Check for NULL Pointers && valid I2C settings
    assert(self);

    // Assign I2C ID based on pins
    temp = pinsToI2c(sda, scl);
    if (temp == -1) {
        // Error will be indicated by pinsToUart(tx, rx) function
        return;
    } else {
        self->i2c_id = temp;
        self->i2c_regs = MXC_I2C_GET_I2C(temp);
    }

    assert((self->i2c_id >= 0) && (self->i2c_id < NUM_I2C));

    // Init I2C as main / controller node (0x00 is ignored)
    if ((scl != NULL) && (sda != NULL)) {
        err = MXC_I2C_Init(self->i2c_regs, 1, 0x00);
        if (err) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("Failed to init I2C.\n"));
        }
        err = MXC_I2C_SetFrequency(self->i2c_regs, frequency);
        if (err < 0) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("Failed to set I2C frequency\n"));
        }
    } else if (scl != NULL) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("I2C needs SDA & SCL"));
    } else if (sda != NULL) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("I2C needs SDA & SCL"));
    } else {
        // Should not get here, as shared-bindings API should not call this way
    }

    // Attach I2C pins
    self->sda = sda;
    self->scl = scl;
    common_hal_mcu_pin_claim(self->sda);
    common_hal_mcu_pin_claim(self->scl);

    // Indicate to this module that the I2C is active
    i2c_active |= (1 << self->i2c_id);

    // Set the timeout to a default value
    if (((timeout < 0.0) || (timeout > 100.0))) {
        self->timeout = 1.0;
    } else {
        self->timeout = timeout;
    }

    /* Setup I2C interrupt */
    NVIC_ClearPendingIRQ(MXC_I2C_GET_IRQ(self->i2c_id));
    NVIC_DisableIRQ(MXC_I2C_GET_IRQ(self->i2c_id));
    NVIC_SetPriority(MXC_I2C_GET_IRQ(self->i2c_id), I2C_PRIORITY);
    NVIC_SetVector(MXC_I2C_GET_IRQ(self->i2c_id), (uint32_t)i2c_isr);

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

void common_hal_busio_i2c_mark_deinit(busio_i2c_obj_t *self) {
    // FIXME: Implement
}

// Probe device in I2C bus
bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    int nack = 0;

    mxc_i2c_req_t addr_req = {
        .addr = addr,
        .i2c = self->i2c_regs,
        .tx_len = 0,
        .tx_buf = NULL,
        .rx_len = 0,
        .rx_buf = NULL,
        .callback = NULL
    };

    // Probe the address
    nack = MXC_I2C_MasterTransaction(&addr_req);
    if (nack) {
        return false;
    } else {
        return true;
    }
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
