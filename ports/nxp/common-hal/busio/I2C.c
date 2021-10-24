/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared-bindings/busio/I2C.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "common-hal/microcontroller/Pin.h"


#define CRITICAL_SECTION_ENTER()    {}
#define CRITICAL_SECTION_LEAVE()    {}


/* TODO: Move board dependencies to the board functions accordingly */
#if defined(BOARD_MCB1700)
extern ARM_DRIVER_I2C Driver_I2C0;

#define MAX_I2C 1
const STATIC i2c_inst_t *i2c[MAX_I2C] = {&Driver_I2C0};


#elif defined(BOARD_LPCEXPRESSO55S28)
#include "fsl_clock.h"

extern ARM_DRIVER_I2C Driver_I2C4;

#define MAX_I2C 1
const STATIC i2c_inst_t *i2c[MAX_I2C] = {&Driver_I2C4};


#else
#error "Only BOARD_[BRKR_MCB1700|LPCEXPRESSO55S28] are supported"

#endif


STATIC bool reserved_i2c[MAX_I2C];
STATIC bool never_reset_i2c[MAX_I2C];


static uint32_t last_event;


static void I2C0_cb(uint32_t event) {
    last_event = event;

    return;
}


void reset_i2c() {
    return;
}


void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {

    self->driver = (ARM_DRIVER_I2C *)NULL;
    self->scl_pin = NO_PIN;
    self->sda_pin = NO_PIN;

    reset_pin_number(scl->number);
    reset_pin_number(sda->number);

    i2c_inst_t *i2c_drv = i2c[0U];
    self->driver = i2c_drv;

    #if defined(BOARD_LPCEXPRESSO55S28)
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);
    #endif

    int32_t status = i2c_drv->Initialize(I2C0_cb);

    #if (0)
    mp_raise_ValueError(translate("Invalid pins"));
    mp_raise_ValueError(translate("I2C peripheral in use"));
    mp_raise_RuntimeError(translate("No pull up found on SDA or SCL; check your wiring"));
    #endif

    if (ARM_DRIVER_OK != status) {
        mp_raise_RuntimeError(translate("I2C Init Error"));
    }


    i2c_drv->PowerControl(ARM_POWER_FULL);
    if (100000u == frequency) {
        i2c_drv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
    } else if (400000u == frequency) {
        i2c_drv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
    } else if (1000000u == frequency) {
        i2c_drv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST_PLUS);
    } else if (3400000u == frequency) {
        i2c_drv->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_HIGH);
    } else {
        mp_raise_ValueError(translate("Unsupported baudrate"));
    }

    self->scl_pin = scl->number;
    self->sda_pin = sda->number;

    #if CIRCUITPY_REQUIRE_I2C_PULLUPS
    #endif

    common_hal_mcu_pin_claim(sda);
    common_hal_mcu_pin_claim(scl);

    return;
}


bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return false;
}


void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    return;
}


bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    uint8_t dummy = 0u;
    return common_hal_busio_i2c_write(self, addr, &dummy, 1u, true) == 0;
}


bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    /* TODO: Introduce common_hal_busio_try_lock function */
    bool grabbed_lock = false;
    CRITICAL_SECTION_ENTER();
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    CRITICAL_SECTION_LEAVE();
    return grabbed_lock;
}


bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}


void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
}


uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len, bool transmit_stop_bit) {
    last_event = 0;
    i2c_inst_t *i2c_drv = i2c[0U];
    int32_t drv_err = i2c_drv->MasterTransmit(addr, data, len, transmit_stop_bit);

    uint8_t err = 0u;
    if (!drv_err) {
        ARM_I2C_STATUS status;
        do {
            RUN_BACKGROUND_TASKS;
            status = i2c_drv->GetStatus();
        } while (status.busy);

        if (last_event & ARM_I2C_EVENT_ADDRESS_NACK) {
            err = MP_EIO;
        }
    } else {
        if (ARM_DRIVER_ERROR_PARAMETER == drv_err) {
            err = MP_ENODEV;
        } else if (ARM_DRIVER_ERROR == drv_err) {
            err = MP_ENXIO;
        } else if (ARM_DRIVER_ERROR_BUSY == drv_err) {
            err = MP_EBUSY;
        } else if (ARM_DRIVER_OK == drv_err) {
            err = MP_EIO;
        }
    }

    return err;
}


uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *data, size_t len) {
    bool transmit_stop_bit = false;
    i2c_inst_t *i2c_drv = i2c[0U];
    int32_t drv_err = i2c_drv->MasterReceive(addr, data, len, transmit_stop_bit);

    uint8_t err = 0u;
    if (!drv_err) {
        ARM_I2C_STATUS status;
        do {
            RUN_BACKGROUND_TASKS;
            status = i2c_drv->GetStatus();
        } while (status.busy);

        if (last_event & ARM_I2C_EVENT_ADDRESS_NACK) {
            err = MP_EIO;
        }
    } else {
        if (ARM_DRIVER_ERROR_PARAMETER == drv_err) {
            err = MP_ENODEV;
        } else if (ARM_DRIVER_ERROR == drv_err) {
            err = MP_ENXIO;
        } else if (ARM_DRIVER_ERROR_BUSY == drv_err) {
            err = MP_EBUSY;
        } else if (ARM_DRIVER_OK == drv_err) {
            err = MP_EIO;
        }
    }

    return err;
}


void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    #if (0)
    for (size_t i = 0; i < MP_ARRAY_SIZE(mcu_i2c_banks); i++) {
        if (self->handle.Instance == mcu_i2c_banks[i]) {
            never_reset_i2c[i] = true;

            never_reset_pin_number(self->scl_pin->port, self->scl_pin->number);
            never_reset_pin_number(self->sda_pin->port, self->sda_pin->number);
            break;
        }
    }
    #endif
    return;
}
