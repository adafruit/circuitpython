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

#include "shared-bindings/busio/SPI.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/board.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "max32_port.h"

// Note that any bugs introduced in this file can cause crashes
// at startupfor chips using external SPI flash.

#define SPI_PRIORITY 1

typedef enum {
    SPI_FREE = 0,
    SPI_BUSY,
} spi_status_t;

// Set each bit to indicate an active SPI
static uint8_t spi_active = 0;
static spi_status_t spi_status[NUM_SPI];
static volatile int spi_err;

// Construct SPI protocol, this function init SPI peripheral
void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *sck,
    const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso,
    bool half_duplex) {
    int err = 0;

    // Check for NULL Pointer
    assert(self);

    // Ensure the object starts in its deinit state.
    common_hal_busio_spi_mark_deinit(self);

    // Assign SPI ID based on pins
    int spi_id = pinsToSpi(sck, mosi, miso);
    if (spi_id == -1) {
        return;
    } else {
        self->spi_id = spi_id;
        self->spi_regs = MXC_SPI_GET_SPI(spi_id);
    }

    assert((self->spi_id >= 0) && (self->spi_id < NUM_SPI));

    // Init SPI controller
    if ((mosi != NULL) && (miso != NULL) && (sck != NULL)) {
        err = spi_init(self->spi_regs, 1000000);
        MXC_GPIO_SetVSSEL(MXC_GPIO_GET_GPIO(sck->port), MXC_GPIO_VSSEL_VDDIOH, (sck->mask | miso->mask | mosi->mask | MXC_GPIO_PIN_0));
        if (err) {
            // NOTE: Reuse existing messages from locales/circuitpython.pot to save space
            mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q init failed"), MP_QSTR_SPI);
        }
    } else {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("SPI needs MOSI, MISO, and SCK"));
    }

    // Attach SPI pins
    self->mosi = mosi;
    self->miso = miso;
    self->sck = sck;
    common_hal_mcu_pin_claim(self->mosi);
    common_hal_mcu_pin_claim(self->miso);
    common_hal_mcu_pin_claim(self->sck);

    // Indicate to this module that the SPI is active
    spi_active |= (1 << self->spi_id);

    return;
}

// Never reset SPI when reload
void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    common_hal_never_reset_pin(self->mosi);
    common_hal_never_reset_pin(self->miso);
    common_hal_never_reset_pin(self->sck);
    common_hal_never_reset_pin(self->nss);
}

// Check SPI status, deinited or not
bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->sck == NULL;
}

void common_hal_busio_spi_mark_deinit(busio_spi_obj_t *self) {
    self->sck = NULL;
}

// Deinit SPI obj
void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {

    MXC_SPI_Shutdown(self->spi_regs);
    common_hal_reset_pin(self->mosi);
    common_hal_reset_pin(self->miso);
    common_hal_reset_pin(self->sck);
    common_hal_reset_pin(self->nss);

    self->mosi = NULL;
    self->miso = NULL;
    self->nss = NULL;

    common_hal_busio_spi_mark_deinit(self);
}

// Configures the SPI bus. The SPI object must be locked.
bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate,
    uint8_t polarity,
    uint8_t phase,
    uint8_t bits) {

    mxc_spi_mode_t spi_mode;
    int ret;

    self->polarity = polarity;
    self->phase = phase;
    self->bits = bits;

    switch ((polarity << 1) | (phase)) {
        case 0b00:
            spi_mode = SPI_MODE_0;
            break;
        case 0b01:
            spi_mode = SPI_MODE_1;
            break;
        case 0b10:
            spi_mode = SPI_MODE_2;
            break;
        case 0b11:
            spi_mode = SPI_MODE_3;
            break;
        default:
            // should not be reachable; validated in shared-bindings/busio/SPI.c
            return false;
    }

    ret = MXC_SPI_SetFrequency(self->spi_regs, baudrate);
    if (ret) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q out of range"), MP_QSTR_baudrate);
        return false;
    }

    // MXC_SPI_SetFrequency() floors the clock divisor, so the actual rate can be
    // HIGHER than requested. Treat baudrate as a ceiling: if the hardware
    // overshoots, bisect downward on the requested target for the highest rate
    // that does not exceed baudrate. MXC_SPI_GetFrequency() reports the actual
    // configured rate, so we don't depend on the divisor internals.
    uint32_t actual = MXC_SPI_GetFrequency(self->spi_regs);
    if (actual > baudrate) {
        uint32_t lo = 1;         // f(lo) <= baudrate
        uint32_t hi = baudrate;  // f(hi) > baudrate (just measured)
        while (hi - lo > 1) {
            uint32_t mid = lo + (hi - lo) / 2;
            MXC_SPI_SetFrequency(self->spi_regs, mid);
            if (MXC_SPI_GetFrequency(self->spi_regs) <= baudrate) {
                lo = mid;
            } else {
                hi = mid;
            }
        }
        MXC_SPI_SetFrequency(self->spi_regs, lo);
        actual = MXC_SPI_GetFrequency(self->spi_regs);
    }
    self->baudrate = actual;
    ret = MXC_SPI_SetDataSize(self->spi_regs, bits);
    if (ret == E_BAD_PARAM) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q out of range"), MP_QSTR_bits);
        return false;
    } else if (ret == E_BAD_STATE) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid state"));
    }
    ret = MXC_SPI_SetMode(self->spi_regs, spi_mode);
    if (ret) {
        mp_raise_ValueError(MP_ERROR_TEXT("Failed to set SPI Clock Mode"));
        return false;
    }
    return true;
}

// Lock SPI bus
bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    if (spi_status[self->spi_id] != SPI_BUSY) {
        self->has_lock = true;
        return true;
    } else {
        return false;
    }
}

// Check SPI lock status
bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

// Unlock SPI bus
void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
}

// Write the data contained in buffer
bool common_hal_busio_spi_write(busio_spi_obj_t *self,
    const uint8_t *data,
    size_t len) {
    int ret = 0;

    MXC_SPI_SetDefaultTXData(self->spi_regs, 0xFF);

    mxc_spi_req_t wr_req = {
        .spi = self->spi_regs,
        .ssIdx = 0,
        .txCnt = 0,
        .rxCnt = 0,
        .txData = (uint8_t *)data,
        .txLen = len,
        .rxData = NULL,
        .rxLen = 0,
        .ssDeassert = 1,
        .completeCB = NULL,
    };
    ret = MXC_SPI_MasterTransaction(&wr_req);
    if (ret) {
        return false;
    } else {
        return true;
    }
}

// Read data into buffer
bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len,
    uint8_t write_value) {

    int ret = 0;

    MXC_SPI_SetDefaultTXData(self->spi_regs, write_value);

    mxc_spi_req_t rd_req = {
        .spi = self->spi_regs,
        .ssIdx = 0,
        .txCnt = 0,
        .rxCnt = 0,
        .txData = NULL,
        .txLen = len,
        .rxData = data,
        .rxLen = len,
        .ssDeassert = 1,
        .completeCB = NULL,
    };
    ret = MXC_SPI_MasterTransaction(&rd_req);
    if (ret) {
        return false;
    } else {
        return true;
    }
}

// Write out the data in data_out
// while simultaneously reading data into data_in
bool common_hal_busio_spi_transfer(busio_spi_obj_t *self,
    const uint8_t *data_out,
    uint8_t *data_in,
    size_t len) {

    int ret = 0;

    MXC_SPI_SetDefaultTXData(self->spi_regs, 0xFF);

    mxc_spi_req_t transfer_req = {
        .spi = self->spi_regs,
        .ssIdx = 0,
        .txCnt = 0,
        .rxCnt = 0,
        .txData = (uint8_t *)data_out,
        .txLen = len,
        .rxData = data_in,
        .rxLen = len,
        .ssDeassert = 1,
        .completeCB = NULL,
    };
    ret = MXC_SPI_MasterTransaction(&transfer_req);
    if (ret) {
        return false;
    } else {
        return true;
    }
}

// Get SPI baudrate
uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->baudrate;
}

// Get SPI phase
uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->phase;
}

// Get SPI polarity
uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->polarity;
}
