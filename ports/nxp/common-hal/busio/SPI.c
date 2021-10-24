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

#include "shared-bindings/busio/SPI.h"

#include "lib/utils/interrupt_char.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "supervisor/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"


#define CRITICAL_SECTION_ENTER()    {}
#define CRITICAL_SECTION_LEAVE()    {}


/* TODO: Move board dependencies to the board functions accordingly */
#if defined(BOARD_MCB1700)
extern ARM_DRIVER_SPI Driver_SPI0;

#define MAX_SPI 1U
const STATIC spi_inst_t *spi[MAX_SPI] = {&Driver_SPI0};
STATIC uint32_t events[MAX_SPI] = {0};


#elif defined(BOARD_LPCEXPRESSO55S28)
#include "fsl_clock.h"

extern ARM_DRIVER_SPI Driver_SPI7;

#define MAX_SPI 1U
const STATIC spi_inst_t *spi[MAX_SPI] = {&Driver_SPI7};
STATIC uint32_t events[MAX_SPI] = {0};


#else
#error "Only BOARD_[BRKR_MCB1700|LPCEXPRESSO55S28] are supported"

#endif

/* Conforming SD card, start with 250 kHz */
#define SPI_DEFAULT_FREQ    250000U



STATIC void SPI0_cb(uint32_t event) {
    events[0U] = event;

    return;
}

STATIC void SPI1_cb(uint32_t event) {
    events[0U] = event;

    return;
}

STATIC void SPI2_cb(uint32_t event) {
    events[0U] = event;

    return;
}

void reset_spi(void) {
    return;
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso) {

    self->clock = (const mcu_pin_obj_t *)NULL;
    self->miso = (const mcu_pin_obj_t *)NULL;
    self->mosi = (const mcu_pin_obj_t *)NULL;

    #if (0)
    if (invalid_pin) {
        mp_raise_ValueError_varg(translate("Invalid %q pin selection"), MP_QSTR_SPI);
    }
    #endif

    reset_pin_number(mosi->number);
    reset_pin_number(miso->number);

    const size_t n = 0U;
    ARM_DRIVER_SPI *spi_drv = spi[n];
    self->driver = spi_drv;

    #if defined(BOARD_LPCEXPRESSO55S28)
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM7);
    #endif

    int32_t drv_err = spi_drv->Initialize(SPI0_cb);
    if (ARM_DRIVER_OK != drv_err) {
        mp_raise_RuntimeError(translate("SPI Init Error"));
    }

    spi_drv->PowerControl(ARM_POWER_FULL);
    uint32_t ctrl = ARM_SPI_MODE_MASTER;
    spi_drv->Control(ctrl, SPI_DEFAULT_FREQ);
    self->target_baudrate = SPI_DEFAULT_FREQ;
    self->real_baudrate = spi_drv->Control(ARM_SPI_GET_BUS_SPEED, 0UL);

    ctrl = ARM_SPI_MODE_MASTER;
    ctrl |= ARM_SPI_SS_MASTER_SW | ARM_SPI_CPOL0_CPHA0;
    ctrl |= ARM_SPI_DATA_BITS(8u);
    spi_drv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);

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

    return;
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
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
        self->driver = NULL;
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
        spi_inst_t *spi_drv = self->driver;
        int32_t drv_err = ARM_DRIVER_OK;
        if (is_baudrate_changed) {
            drv_err = spi_drv->Control(ARM_SPI_SET_BUS_SPEED, baudrate);
        }

        if ((ARM_DRIVER_OK == drv_err) && (is_polarity_changed || is_phase_changed)) {
            uint32_t ctrl = ARM_SPI_MODE_MASTER | ARM_SPI_SS_MASTER_SW | ARM_SPI_SET_BUS_SPEED;

            if (!polarity && !phase) {
                ctrl |= ARM_SPI_CPOL0_CPHA0;
            } else if (!polarity && phase) {
                ctrl |= ARM_SPI_CPOL0_CPHA1;
            } else if (polarity && !phase) {
                ctrl |= ARM_SPI_CPOL1_CPHA0;
            } else if (polarity && phase) {
                ctrl |= ARM_SPI_CPOL1_CPHA1;
            }

            drv_err = spi_drv->Control(ctrl, 0u);
        }

        if ((ARM_DRIVER_OK == drv_err) && (is_bits_changed)) {
            uint32_t ctrl = ARM_SPI_MODE_MASTER | ARM_SPI_SS_MASTER_SW | ARM_SPI_SET_BUS_SPEED;

            ctrl |= ARM_SPI_DATA_BITS(bits);
            drv_err = spi_drv->Control(ctrl, 0u);
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
        spi_inst_t *spi_drv = self->driver;
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
        spi_inst_t *spi_drv = self->driver;
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
        spi_inst_t *spi_drv = self->driver;
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
