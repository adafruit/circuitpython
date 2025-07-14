// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/wiznet/PIO_SPI.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t wiznet_pio_spi_type;

// Construct an underlying SPI object.
extern void common_hal_wiznet_pio_spi_construct(wiznet_pio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex);

extern void common_hal_wiznet_pio_spi_deinit(wiznet_pio_spi_obj_t *self);
extern bool common_hal_wiznet_pio_spi_deinited(wiznet_pio_spi_obj_t *self);

extern bool common_hal_wiznet_pio_spi_configure(wiznet_pio_spi_obj_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits);

extern bool common_hal_wiznet_pio_spi_try_lock(wiznet_pio_spi_obj_t *self);
extern bool common_hal_wiznet_pio_spi_has_lock(wiznet_pio_spi_obj_t *self);
extern void common_hal_wiznet_pio_spi_unlock(wiznet_pio_spi_obj_t *self);

// Writes out the given data.
extern bool common_hal_wiznet_pio_spi_write(wiznet_pio_spi_obj_t *self, const uint8_t *data, size_t len);

// Reads in len bytes while outputting the byte write_value.
extern bool common_hal_wiznet_pio_spi_read(wiznet_pio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_value);

// Reads and write len bytes simultaneously.
extern bool common_hal_wiznet_pio_spi_transfer(wiznet_pio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len);

extern wiznet_pio_spi_obj_t *validate_obj_is_wiznet_pio_spi_bus(mp_obj_t obj_in, qstr arg_name);
