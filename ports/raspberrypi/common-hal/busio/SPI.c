// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/busio/SPI.h"

#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex) {
    if (half_duplex) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_half_duplex);
    }
    rp2_spi_error_t error = rp2_spi_construct(&self->spi, clock, mosi, miso);
    if (error == RP2_SPI_INVALID_PINS) {
        raise_ValueError_invalid_pins();
    }
    if (error == RP2_SPI_IN_USE) {
        mp_raise_ValueError(MP_ERROR_TEXT("SPI peripheral in use"));
    }
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    rp2_spi_never_reset(&self->spi);
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return rp2_spi_deinited(&self->spi);
}

void common_hal_busio_spi_mark_deinit(busio_spi_obj_t *self) {
    self->spi.clock = NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    rp2_spi_deinit(&self->spi);
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    rp2_spi_configure(&self->spi, baudrate, polarity, phase, bits);
    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return false;
    }
    bool grabbed_lock = false;
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
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
    uint32_t data_in;
    rp2_spi_transfer(&self->spi, data, len, (uint8_t *)&data_in, MIN(len, 4));
    return true;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len, uint8_t write_value) {
    uint32_t data_out = write_value << 24 | write_value << 16 | write_value << 8 | write_value;
    rp2_spi_transfer(&self->spi, (const uint8_t *)&data_out, MIN(4, len), data, len);
    return true;
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len) {
    rp2_spi_transfer(&self->spi, data_out, len, data_in, len);
    return true;
}

void common_hal_busio_spi_write_start(busio_spi_obj_t *self, const uint8_t *data, size_t len,
    circuitpy_async_flag_t *done) {
    rp2_spi_write_start(&self->spi, data, len, done);
}

void common_hal_busio_spi_end(busio_spi_obj_t *self) {
    rp2_spi_end(&self->spi);
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->spi.real_frequency;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->spi.phase;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->spi.polarity;
}
