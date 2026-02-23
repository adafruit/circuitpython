// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/fourwire/FourWire.h"

#include <stdint.h>

#include "py/gc.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/DigitalInOutProtocol.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"
#include "supervisor/port.h"

void common_hal_fourwire_fourwire_construct(fourwire_fourwire_obj_t *self,
    busio_spi_obj_t *spi, mp_obj_t command,
    mp_obj_t chip_select, mp_obj_t reset, uint32_t baudrate,
    uint8_t polarity, uint8_t phase) {

    self->bus = spi;
    common_hal_busio_spi_never_reset(self->bus);

    self->frequency = baudrate;
    self->polarity = polarity;
    self->phase = phase;

    // Allocate the pins in the same place as self.
    bool use_port_allocation = !gc_alloc_possible() || !gc_ptr_on_heap(self);

    self->command = digitalinout_protocol_from_pin(command, MP_QSTR_command, true, use_port_allocation, &self->own_command);
    if (self->command != mp_const_none) {
        digitalinout_protocol_switch_to_output(self->command, true, DRIVE_MODE_PUSH_PULL);
        common_hal_never_reset_pin(command);
    }

    self->reset = digitalinout_protocol_from_pin(reset, MP_QSTR_reset, true, use_port_allocation, &self->own_reset);
    if (self->reset != mp_const_none) {
        digitalinout_protocol_switch_to_output(self->reset, true, DRIVE_MODE_PUSH_PULL);
        common_hal_never_reset_pin(reset);
        common_hal_fourwire_fourwire_reset(self);
    }

    self->chip_select = digitalinout_protocol_from_pin(chip_select, MP_QSTR_chip_select, true, use_port_allocation, &self->own_chip_select);
    if (self->chip_select != mp_const_none) {
        digitalinout_protocol_switch_to_output(self->chip_select, true, DRIVE_MODE_PUSH_PULL);
        common_hal_never_reset_pin(chip_select);
    }
}

void common_hal_fourwire_fourwire_deinit(fourwire_fourwire_obj_t *self) {
    if (self->bus == &self->inline_bus) {
        common_hal_busio_spi_deinit(self->bus);
    }

    // Only deinit and free the pins if we own them
    if (self->command != mp_const_none && self->own_command) {
        digitalinout_protocol_deinit(self->command);
        circuitpy_free_obj(self->command);
    }
    if (self->chip_select != mp_const_none && self->own_chip_select) {
        digitalinout_protocol_deinit(self->chip_select);
        circuitpy_free_obj(self->chip_select);
    }
    if (self->reset != mp_const_none && self->own_reset) {
        digitalinout_protocol_deinit(self->reset);
        circuitpy_free_obj(self->reset);
    }
}

bool common_hal_fourwire_fourwire_reset(mp_obj_t obj) {
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->reset == mp_const_none) {
        return false;
    }
    if (digitalinout_protocol_set_value(self->reset, false) != 0) {
        return false;
    }
    common_hal_mcu_delay_us(1000);
    if (digitalinout_protocol_set_value(self->reset, true) != 0) {
        return false;
    }
    common_hal_mcu_delay_us(1000);
    return true;
}

bool common_hal_fourwire_fourwire_bus_free(mp_obj_t obj) {
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!common_hal_busio_spi_try_lock(self->bus)) {
        return false;
    }
    common_hal_busio_spi_unlock(self->bus);
    return true;
}

bool common_hal_fourwire_fourwire_begin_transaction(mp_obj_t obj) {
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (!common_hal_busio_spi_try_lock(self->bus)) {
        return false;
    }
    common_hal_busio_spi_configure(self->bus, self->frequency, self->polarity,
        self->phase, 8);
    if (self->chip_select != mp_const_none) {
        // IO Expander CS can fail due to an I2C lock.
        if (digitalinout_protocol_set_value(self->chip_select, false) != 0) {
            common_hal_busio_spi_unlock(self->bus);
            return false;
        }
    }
    return true;
}

void common_hal_fourwire_fourwire_send(mp_obj_t obj, display_byte_type_t data_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length) {
    if (data_length == 0) {
        return;
    }
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->command == mp_const_none) {
        // When the data/command pin is not specified, we simulate a 9-bit SPI mode, by
        // adding a data/command bit to every byte, and then splitting the resulting data back
        // into 8-bit chunks for transmission. If the length of the data being transmitted
        // is not a multiple of 8, there will be additional bits at the end of the
        // transmission. We toggle the CS pin to make the receiver discard them.
        uint8_t buffer = 0;
        uint8_t bits = 0;
        uint8_t dc = (data_type == DISPLAY_DATA);

        for (size_t i = 0; i < data_length; i++) {
            bits = (bits + 1) % 8;

            if (bits == 0) {
                // send the previous byte and the dc bit
                // we will send the current byte later
                buffer = (buffer << 1) | dc;
                common_hal_busio_spi_write(self->bus, &buffer, 1);
                // send the current byte, because previous byte already filled all bits
                common_hal_busio_spi_write(self->bus, &data[i], 1);
            } else {
                // send remaining bits from previous byte, dc and beginning of current byte
                buffer = (buffer << (9 - bits)) | (dc << (8 - bits)) | (data[i] >> bits);
                common_hal_busio_spi_write(self->bus, &buffer, 1);
            }
            // save the current byte
            buffer = data[i];
        }
        // send any remaining bits
        if (bits > 0) {
            buffer = buffer << (8 - bits);
            common_hal_busio_spi_write(self->bus, &buffer, 1);
            if (self->chip_select != mp_const_none) {
                // toggle CS to discard superfluous bits
                digitalinout_protocol_set_value(self->chip_select, true);
                common_hal_mcu_delay_us(1);
                digitalinout_protocol_set_value(self->chip_select, false);
            }
        }
    } else {
        digitalinout_protocol_set_value(self->command, data_type == DISPLAY_DATA);
        if (chip_select == CHIP_SELECT_TOGGLE_EVERY_BYTE) {
            // Toggle chip select after each command byte in case the display driver
            // IC latches commands based on it.
            for (size_t i = 0; i < data_length; i++) {
                common_hal_busio_spi_write(self->bus, &data[i], 1);
                if (self->chip_select != mp_const_none) {
                    digitalinout_protocol_set_value(self->chip_select, true);
                    common_hal_mcu_delay_us(1);
                    digitalinout_protocol_set_value(self->chip_select, false);
                }
            }
        } else {
            common_hal_busio_spi_write(self->bus, data, data_length);
        }
    }
}

void common_hal_fourwire_fourwire_end_transaction(mp_obj_t obj) {
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->chip_select != mp_const_none) {
        digitalinout_protocol_set_value(self->chip_select, true);
    }
    common_hal_busio_spi_unlock(self->bus);
}

void common_hal_fourwire_fourwire_collect_ptrs(mp_obj_t obj) {
    fourwire_fourwire_obj_t *self = MP_OBJ_TO_PTR(obj);
    gc_collect_ptr((void *)self->bus);
}
