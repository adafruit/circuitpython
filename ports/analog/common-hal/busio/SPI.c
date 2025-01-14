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

// Note that any bugs introduced in this file can cause crashes
// at startupfor chips using external SPI flash.

// Reset SPI when reload
void spi_reset(void) {
    // FIXME: Implement
    return;
}

// Construct SPI protocol, this function init SPI peripheral
void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *sck,
    const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso,
    bool half_duplex) {


    // FIXME: Implement
}

// Never reset SPI when reload
void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    // FIXME: Implement
}

// Check SPI status, deinited or not
bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->sck == NULL;
}

// Deinit SPI obj
void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {

    // FIXME: Implement
}

// Configures the SPI bus. The SPI object must be locked.
bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate,
    uint8_t polarity,
    uint8_t phase,
    uint8_t bits) {

    // FIXME: Implement
    return true;
}

// Lock SPI bus
bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    // FIXME: Implement
    return false;
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

    // FIXME: Implement
    return false;
}

// Read data into buffer
bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len,
    uint8_t write_value) {

    // FIXME: Implement
    return false;
}

// Write out the data in data_out
// while simultaneously reading data into data_in
bool common_hal_busio_spi_transfer(busio_spi_obj_t *self,
    const uint8_t *data_out,
    uint8_t *data_in,
    size_t len) {

    // FIXME: Implement
    return false;
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
