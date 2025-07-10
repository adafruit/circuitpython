// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/busio/WIZNET_PIO_SPI.h"

#include "shared/runtime/interrupt_char.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "supervisor/board.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

#include "wiznet_pio_spi/wizchip_pio_spi.h"

#define NO_INSTANCE 0xff

#ifndef PIO_SPI_PREFERRED_PIO
#define PIO_SPI_PREFERRED_PIO 1
#endif

// All wiznet spi operations must start with writing a 3 byte header

wiznet_pio_spi_config_t wiznet_pio_spi_config;
wiznet_pio_spi_handle_t wiznet_pio_spi_handle = NULL;

void common_hal_wiznet_pio_spi_construct(wiznet_pio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex) {

    if (half_duplex) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_half_duplex);
    }

    wiznet_pio_spi_config.data_in_pin = miso->number;
    wiznet_pio_spi_config.data_out_pin = mosi->number;
    wiznet_pio_spi_config.clock_pin = clock->number;

    if (wiznet_pio_spi_handle != NULL) {
        wiznet_pio_spi_close(wiznet_pio_spi_handle);
    }

    wiznet_pio_spi_handle = wiznet_pio_spi_open(&wiznet_pio_spi_config);
    (*wiznet_pio_spi_handle)->set_active(wiznet_pio_spi_handle);

}

bool common_hal_wiznet_pio_spi_deinited(wiznet_pio_spi_obj_t *self) {
    return wiznet_pio_spi_config.clock_pin == 0;
}

void common_hal_wiznet_pio_spi_deinit(wiznet_pio_spi_obj_t *self) {
    if (common_hal_wiznet_pio_spi_deinited(self)) {
        return;
    }

    common_hal_reset_pin(self->clock);
    common_hal_reset_pin(self->MOSI);
    common_hal_reset_pin(self->MISO);

    wiznet_pio_spi_config.clock_pin = 0;
}

bool common_hal_wiznet_pio_spi_configure(wiznet_pio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {

    uint32_t clock = clock_get_hz(clk_sys);
    uint32_t clock_div = clock / baudrate;

    if (clock_div > clock / 4) {
        clock_div = clock / 4;
    }

    wiznet_pio_spi_config.clock_div_major = clock_div;
    wiznet_pio_spi_config.clock_div_minor = 0;

    return true;
}

bool common_hal_wiznet_pio_spi_try_lock(wiznet_pio_spi_obj_t *self) {
    if (common_hal_wiznet_pio_spi_deinited(self)) {
        return false;
    }

    bool grabbed_lock = false;
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    return grabbed_lock;
}

bool common_hal_wiznet_pio_spi_has_lock(wiznet_pio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_wiznet_pio_spi_unlock(wiznet_pio_spi_obj_t *self) {
    self->has_lock = false;
}

bool common_hal_wiznet_pio_spi_write(wiznet_pio_spi_obj_t *self,
    const uint8_t *data, size_t len) {
    wiznet_pio_spi_write_buffer(data, len);
    return true;
}

bool common_hal_wiznet_pio_spi_read(wiznet_pio_spi_obj_t *self,
    uint8_t *data, size_t len, uint8_t write_value) {
    wiznet_pio_spi_read_buffer(data, len);
    return true;
}

bool common_hal_wiznet_pio_spi_transfer(wiznet_pio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len) {
    return wiznet_pio_spi_transfer(data_out, len, data_in, len);
}
