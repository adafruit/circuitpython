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

#if CIRCUITPY_WIZNET_W6300

extern void common_hal_wiznet_pio_qspi_construct(wiznet_pio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *quad_io0,
    const mcu_pin_obj_t *quad_io1, const mcu_pin_obj_t *quad_io2,
    const mcu_pin_obj_t *quad_io3, bool half_duplex);

#else // W55RP20
// Construct an underlying SPI object.
extern void common_hal_wiznet_pio_spi_construct(wiznet_pio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex);

#endif

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

extern wiznet_pio_spi_obj_t *validate_obj_is_wiznet_pio_spi_bus(mp_obj_t obj_in, qstr arg_name);
