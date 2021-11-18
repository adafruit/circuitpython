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

#include "boards/i2c_config.h"
#include "shared-bindings/busio/I2C.h"

#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "supervisor/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"


#define CRITICAL_SECTION_ENTER()    {}
#define CRITICAL_SECTION_LEAVE()    {}

STATIC uint32_t __i2c_events[I2C_INSTANCES_NUM];

typedef void (*cb_func)(uint32_t event);

STATIC void __I2C_cb(size_t n, uint32_t event) {
    if (n < sizeof(__i2c_events) / sizeof(__i2c_events[0U])) {
        __i2c_events[n] = event;
    }

    return;
}

#define CALLBACK(n) \
    STATIC void __I2C_cb##n(uint32_t event) \
    { \
        __I2C_cb(n, event); \
    }

#define CALLBACK1 \
    CALLBACK(0)

#define CALLBACK2 \
    CALLBACK1 \
    CALLBACK(1)

#define CALLBACK3 \
    CALLBACK2 \
    CALLBACK(2) \

#define CALLBACK4 \
    CALLBACK3 \
    CALLBACK(3)

#define CALLBACK5 \
    CALLBACK3 \
    CALLBAC(4)


#define MAKE_CALLBACKS(n) \
    CALLBACK##n


#define INIT_CALLBACK(n) \
    __I2C_cb##n,

#define INIT_CALLBACK1 \
    INIT_CALLBACK(0)

#define INIT_CALLBACK2 \
    INIT_CALLBACK1 \
    INIT_CALLBACK(1)

#define INIT_CALLBACK3 \
    INIT_CALLBACK2 \
    INIT_CALLBACK(2)

#define INIT_CALLBACK4 \
    INIT_CALLBACK3 \
    INIT_CALLBACK(3)

#define INIT_CALLBACK5 \
    INIT_CALLBACK4 \
    INIT_CALLBACK(4)

#define INIT_CALLBACKS(n) \
    INIT_CALLBACK##n


#if (I2C_INSTANCES_NUM == 1U)
MAKE_CALLBACKS(1);
#elif (I2C_INSTANCES_NUM == 2U)
MAKE_CALLBACKS(2);
#elif (I2C_INSTANCES_NUM == 3U)
MAKE_CALLBACKS(3);
#elif (I2C_INSTANCES_NUM == 4U)
MAKE_CALLBACKS(4);
#elif (I2C_INSTANCES_NUM == 5U)
MAKE_CALLBACKS(5);
#endif

const STATIC cb_func __cb[I2C_INSTANCES_NUM] =
{
    #if (I2C_INSTANCES_NUM == 1U)
    INIT_CALLBACKS(1)
    #elif (I2C_INSTANCES_NUM == 2U)
    INIT_CALLBACKS(2)
    #elif (I2C_INSTANCES_NUM == 3U)
    INIT_CALLBACKS(3)
    #elif (I2C_INSTANCES_NUM == 4U)
    INIT_CALLBACKS(4)
    #elif (I2C_INSTANCES_NUM == 5U)
    INIT_CALLBACKS(4)
    #endif
};


/* TODO: Move board dependencies to the board functions accordingly */
#if defined(BOARD_LPC55S28_EVK)
#include "fsl_clock.h"

extern ARM_DRIVER_I2C Driver_I2C4;

#define MAX_I2C 1
const STATIC i2c_inst_t *i2c[MAX_I2C] = {&Driver_I2C4};


#elif defined(BOARD_BRKR_ICT2)
extern ARM_DRIVER_I2C Driver_I2C0;

#define MAX_I2C 1
const STATIC i2c_inst_t *i2c[MAX_I2C] = {&Driver_I2C0};

#endif


STATIC bool __match_i2c_instance(const i2c_pin_set_t *pin_set, const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda) {
    bool is_matched = true;

    uint8_t scl_pin = NXP_PORT_GPIO_PIN(scl->port, scl->number);
    uint8_t sda_pin = NXP_PORT_GPIO_PIN(sda->port, sda->number);

    is_matched = is_matched && (pin_set->scl == scl_pin);
    is_matched = is_matched && (pin_set->sda == sda_pin);

    return is_matched;
}

STATIC bool __validate_pins(const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda) {
    bool is_free = true;

    is_free = is_free && pin_number_is_free(scl->port, scl->number);
    is_free = is_free && pin_number_is_free(sda->port, sda->number);

    return is_free;
}

size_t __lookup_matching_free_i2c_instance(const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda) {
    bool valid_pin_set = false;

    const size_t N = MP_ARRAY_SIZE(i2c_instances);
    size_t n;
    for (n = 0U; n < N; ++n) {
        /* ... loop over all I2C pin set for given I2C instance */
        i2c_inst_t *instance = &i2c_instances[n];
        const i2c_pin_set_t *pin_set = instance->pin_map;
        const size_t M = instance->pin_map_len;
        for (size_t m = 0U; m < M; ++m) {
            valid_pin_set = __match_i2c_instance(pin_set, scl, sda);
            if (valid_pin_set) {
                break;
            }
        }
        if (valid_pin_set && !(instance->is_used)) {
            break;
        }
    }

    return valid_pin_set ? n : SIZE_MAX;
}

STATIC void __i2c_init(busio_i2c_obj_t *self, ARM_DRIVER_I2C *i2c_drv, cb_func cb, const uint32_t frequency) {
    int32_t status = i2c_drv->Initialize(cb);

    if (ARM_DRIVER_OK != status) {
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
            i2c_drv->Uninitialize();
            mp_raise_ValueError(translate("Unsupported baudrate"));
        }
    } else {
        mp_raise_RuntimeError(translate("I2C Init Error"));
    }


    return;
}

void reset_i2c() {
    // TODO: Implement reset_i2c
    return;
}


void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {

    if (NULL == self->scl) {
        self->i2c_instance = (i2c_inst_t *)NULL;
        self->scl = (const mcu_pin_obj_t *)NULL;
        self->sda = (const mcu_pin_obj_t *)NULL;

        bool valid_pin_set = __validate_pins(scl, sda);
        size_t instance_idx = __lookup_matching_free_i2c_instance(scl, sda);
        i2c_inst_t *i2c_instance = &i2c_instances[instance_idx];

        if (valid_pin_set && (SIZE_MAX > instance_idx)) {
            reset_pin_number(scl->port, scl->number);
            reset_pin_number(scl->port, sda->number);

            i2c_instance->is_used = true;
            ARM_DRIVER_I2C *i2c_drv = i2c_instance->driver;

            self->i2c_instance = i2c_instance;

            i2c_enable(i2c_instance);

            #if CIRCUITPY_REQUIRE_I2C_PULLUPS
            #error "CIRCUITPY_REQUIRE_I2C_PULLUPS option is not yet supported"
            #else
            #if (0)
            mp_raise_RuntimeError(translate("No pull up found on SDA or SCL; check your wiring"));
            #endif
            #endif

            __i2c_init(self, i2c_drv, __cb[instance_idx], frequency);

            self->scl = scl;
            common_hal_mcu_pin_claim(self->scl);

            self->sda = sda;
            common_hal_mcu_pin_claim(self->sda);
        } else {
            mp_raise_ValueError(translate("Invalid pins"));
        }
    } else {
        mp_raise_ValueError(translate("I2C peripheral in use"));
    }

    return;
}


bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return NULL == self->scl;
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

    i2c_inst_t *i2c_instance = self->i2c_instance;
    ARM_DRIVER_I2C *i2c_drv = i2c_instance->driver;
    __i2c_events[i2c_instance->id] = 0U;

    int32_t drv_err = i2c_drv->MasterTransmit(addr, data, len, transmit_stop_bit);

    uint8_t err = 0u;
    if (!drv_err) {
        ARM_I2C_STATUS status;
        do {
            RUN_BACKGROUND_TASKS;
            status = i2c_drv->GetStatus();
        } while (status.busy);

        if (__i2c_events[i2c_instance->id] & ARM_I2C_EVENT_ADDRESS_NACK) {
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
    i2c_inst_t *i2c_instance = self->i2c_instance;
    ARM_DRIVER_I2C *i2c_drv = i2c_instance->driver;
    __i2c_events[i2c_instance->id] = 0U;

    int32_t drv_err = i2c_drv->MasterReceive(addr, data, len, transmit_stop_bit);

    uint8_t err = 0u;
    if (!drv_err) {
        ARM_I2C_STATUS status;
        do {
            RUN_BACKGROUND_TASKS;
            status = i2c_drv->GetStatus();
        } while (status.busy);

        if (__i2c_events[i2c_instance->id] & ARM_I2C_EVENT_ADDRESS_NACK) {
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
