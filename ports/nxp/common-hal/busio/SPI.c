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

#include "boards/spi_config.h"
#include "shared-bindings/busio/SPI.h"

#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "supervisor/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"


#define CRITICAL_SECTION_ENTER()    {}
#define CRITICAL_SECTION_LEAVE()    {}

STATIC uint32_t __spi_events[SPI_INSTANCES_NUM];

/* Conforming SD card, start with 250 kHz */
#define SPI_DEFAULT_FREQ    250000U

typedef void (*cb_func)(uint32_t event);

STATIC void __SPI_cb(size_t n, uint32_t event) {
    if (n < sizeof(__spi_events) / sizeof(__spi_events[0U])) {
        __spi_events[n] = event;
    }

    return;
}

#define CALLBACK(n) \
    STATIC void __SPI_cb##n(uint32_t event) \
    { \
        __SPI_cb(n, event); \
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
    __SPI_cb##n,

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


#if (SPI_INSTANCES_NUM == 1U)
MAKE_CALLBACKS(1);
#elif (SPI_INSTANCES_NUM == 2U)
MAKE_CALLBACKS(2);
#elif (SPI_INSTANCES_NUM == 3U)
MAKE_CALLBACKS(3);
#elif (SPI_INSTANCES_NUM == 4U)
MAKE_CALLBACKS(4);
#elif (SPI_INSTANCES_NUM == 5U)
MAKE_CALLBACKS(5);
#endif

const STATIC cb_func __cb[SPI_INSTANCES_NUM] =
{
    #if (SPI_INSTANCES_NUM == 1U)
    INIT_CALLBACKS(1)
    #elif (SPI_INSTANCES_NUM == 2U)
    INIT_CALLBACKS(2)
    #elif (SPI_INSTANCES_NUM == 3U)
    INIT_CALLBACKS(3)
    #elif (SPI_INSTANCES_NUM == 4U)
    INIT_CALLBACKS(4)
    #elif (SPI_INSTANCES_NUM == 5U)
    INIT_CALLBACKS(4)
    #endif
};

STATIC bool __match_spi_instance(const spi_pin_set_t *pin_set, const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso) {
    bool is_matched = true;

    uint8_t clock_pin = NXP_PORT_GPIO_PIN(clock->port, clock->number);
    uint8_t mosi_pin = NXP_PORT_GPIO_PIN(mosi->port, mosi->number);
    uint8_t miso_pin = NXP_PORT_GPIO_PIN(miso->port, miso->number);

    is_matched = is_matched && (pin_set->clock == clock_pin);
    is_matched = is_matched && (pin_set->mosi == mosi_pin);
    is_matched = is_matched && (pin_set->miso == miso_pin);

    return is_matched;
}

STATIC bool __validate_pins(const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso) {
    bool is_free = true;

    is_free = is_free && pin_number_is_free(clock->port, clock->number);
    is_free = is_free && pin_number_is_free(mosi->port, mosi->number);
    is_free = is_free && pin_number_is_free(miso->port, miso->number);

    return is_free;
}

size_t __lookup_matching_free_spi_instance(const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso) {
    bool valid_pin_set = false;

    const size_t N = MP_ARRAY_SIZE(spi_instances);
    size_t n;
    for (n = 0U; n < N; ++n) {
        /* ... loop over all SPI pin set for given SPI instance */
        spi_inst_t *instance = &spi_instances[n];
        const spi_pin_set_t *pin_set = instance->pin_map;
        const size_t M = instance->pin_map_len;
        for (size_t m = 0U; m < M; ++m) {
            valid_pin_set = __match_spi_instance(pin_set, clock, mosi, miso);
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

STATIC void __spi_init(busio_spi_obj_t *self, ARM_DRIVER_SPI *spi_drv, cb_func cb) {
    int32_t drv_err = spi_drv->Initialize(cb);
    if (ARM_DRIVER_OK == drv_err) {
        spi_drv->PowerControl(ARM_POWER_FULL);
        uint32_t ctrl = ARM_SPI_MODE_MASTER;
        spi_drv->Control(ctrl, SPI_DEFAULT_FREQ);
        self->target_baudrate = SPI_DEFAULT_FREQ;
        self->real_baudrate = spi_drv->Control(ARM_SPI_GET_BUS_SPEED, 0UL);

        ctrl = ARM_SPI_MODE_MASTER;
        ctrl |= ARM_SPI_SS_MASTER_SW | ARM_SPI_CPOL0_CPHA0;
        ctrl |= ARM_SPI_DATA_BITS(8UL);
        spi_drv->Control(ctrl, SPI_DEFAULT_FREQ);
        self->bits = 8u;

        spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
    } else {
        spi_drv->Uninitialize();
        mp_raise_RuntimeError(translate("SPI Init Error"));
    }

    return;
}

void reset_spi(void) {
    // TODO: Implement reset_spi
    return;
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso) {

    if (NULL == self->clock) {
        self->spi_instance = (spi_inst_t *)NULL;
        self->clock = (const mcu_pin_obj_t *)NULL;
        self->miso = (const mcu_pin_obj_t *)NULL;
        self->mosi = (const mcu_pin_obj_t *)NULL;

        bool valid_pin_set = __validate_pins(clock, mosi, miso);
        size_t instance_idx = __lookup_matching_free_spi_instance(clock, mosi, miso);
        spi_inst_t *spi_instance = &spi_instances[instance_idx];

        if (valid_pin_set && (SIZE_MAX > instance_idx) && !(spi_instance->is_used)) {
            reset_pin_number(clock->port, clock->number);
            reset_pin_number(miso->port, mosi->number);
            reset_pin_number(miso->port, miso->number);

            spi_instance->is_used = true;
            ARM_DRIVER_SPI *spi_drv = spi_instance->driver;

            self->spi_instance = spi_instance;

            spi_enable(spi_instance);
            __spi_init(self, spi_drv, __cb[instance_idx]);

            self->clock = clock;
            common_hal_mcu_pin_claim(self->clock);

            self->mosi = mosi;
            if (NULL != self->mosi) {
                common_hal_mcu_pin_claim(self->mosi);
            }

            self->miso = miso;
            if (NULL != self->miso) {
                common_hal_mcu_pin_claim(self->miso);
            }
        } else {
            mp_raise_ValueError(translate("Invalid pins"));
        }
    } else {
        mp_raise_ValueError(translate("SPI peripheral in use"));
    }

    return;
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    #if (0)
    never_reset_spi[spi_get_index(self->peripheral)] = true;

    common_hal_never_reset_pin(self->clock);
    common_hal_never_reset_pin(self->MOSI);
    common_hal_never_reset_pin(self->MISO);
    #endif
    return;
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return NULL == self->clock;
}


void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    bool is_deinited = common_hal_busio_spi_deinited(self);

    if (!is_deinited) {
        #if (0)
        never_reset_spi[spi_get_index(self->peripheral)] = false;
        spi_deinit(self->peripheral);
        #endif

        common_hal_reset_pin(self->clock);
        common_hal_reset_pin(self->mosi);
        common_hal_reset_pin(self->miso);
        self->clock = NULL;
        self->mosi = NULL;
        self->miso = NULL;

        spi_inst_t *spi_instance = self->spi_instance;
        if (spi_instance) {
            self->spi_instance = NULL;
            spi_instance->is_used = false;
            ARM_DRIVER_SPI *spi_drv = spi_instance->driver;

            if (spi_drv) {
                spi_drv->PowerControl(ARM_POWER_OFF);
                spi_drv->Uninitialize();
            }
        }
    }
    return;
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {

    bool is_baudrate_changed = (baudrate != self->target_baudrate);
    bool is_polarity_changed = (polarity != self->polarity);
    bool is_phase_changed = (phase != self->phase);
    bool is_bits_changed = (bits != self->bits);
    bool is_configured = !(is_baudrate_changed || is_polarity_changed || is_phase_changed || is_bits_changed);

    if (!is_configured) {
        ARM_DRIVER_SPI *spi_drv = self->spi_instance->driver;
        int32_t drv_err = ARM_DRIVER_OK;
        if (is_baudrate_changed) {
            drv_err = spi_drv->Control(ARM_SPI_SET_BUS_SPEED, baudrate);
        }

        if ((ARM_DRIVER_OK == drv_err) && (is_polarity_changed || is_phase_changed || is_bits_changed)) {
            uint32_t ctrl = ARM_SPI_MODE_MASTER | ARM_SPI_SS_MASTER_SW;

            if (!polarity && !phase) {
                ctrl |= ARM_SPI_CPOL0_CPHA0;
            } else if (!polarity && phase) {
                ctrl |= ARM_SPI_CPOL0_CPHA1;
            } else if (polarity && !phase) {
                ctrl |= ARM_SPI_CPOL1_CPHA0;
            } else if (polarity && phase) {
                ctrl |= ARM_SPI_CPOL1_CPHA1;
            }

            ctrl |= ARM_SPI_DATA_BITS(bits);
            drv_err = spi_drv->Control(ctrl, baudrate);
        }

        if (ARM_DRIVER_OK == drv_err) {
            self->target_baudrate = baudrate;
            self->real_baudrate = spi_drv->Control(ARM_SPI_GET_BUS_SPEED, 0UL);
            self->polarity = polarity;
            self->phase = phase;
            self->bits = bits;
            is_configured = true;
        } else {
            mp_raise_ValueError(translate("SPI Re-initialization error"));
        }
    }

    return is_configured;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
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

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
    const uint8_t *data, size_t len) {
    int32_t drv_err = ARM_DRIVER_ERROR;

    if (NULL != self->mosi) {
        spi_inst_t *spi_instance = self->spi_instance;
        ARM_DRIVER_SPI *spi_drv = spi_instance->driver;
        __spi_events[spi_instance->id] = 0U;

        drv_err = spi_drv->Send(data, len);

        if (!drv_err) {
            ARM_SPI_STATUS status;
            do {
                RUN_BACKGROUND_TASKS;
                status = spi_drv->GetStatus();
            } while (status.busy);
        }
    } else {
        mp_raise_ValueError(translate("No MOSI Pin"));
    }

    return ARM_DRIVER_OK == drv_err;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len, uint8_t write_value) {
    int32_t drv_err = ARM_DRIVER_ERROR;

    if (NULL != self->miso) {
        ARM_DRIVER_SPI *spi_drv = self->spi_instance->driver;
        drv_err = spi_drv->Receive(data, len);

        if (!drv_err) {
            ARM_SPI_STATUS status;
            do {
                RUN_BACKGROUND_TASKS;
                status = spi_drv->GetStatus();
            } while (status.busy);
        }
    } else {
        mp_raise_ValueError(translate("No MISO Pin"));
    }

    return ARM_DRIVER_OK == drv_err;
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self,
    const uint8_t *data_out, uint8_t *data_in, size_t len) {
    int32_t drv_err = ARM_DRIVER_ERROR;

    if ((NULL != self->mosi) && (NULL != self->miso)) {
        ARM_DRIVER_SPI *spi_drv = self->spi_instance->driver;
        drv_err = spi_drv->Transfer(data_out, data_in, len);

        if (!drv_err) {
            ARM_SPI_STATUS status;
            do {
                RUN_BACKGROUND_TASKS;
                status = spi_drv->GetStatus();
            } while (status.busy);
        }
    } else {
        mp_raise_ValueError(translate("Missing MISO or MOSI Pin"));
    }

    return ARM_DRIVER_OK == drv_err;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->real_baudrate;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->phase;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->polarity;
}
