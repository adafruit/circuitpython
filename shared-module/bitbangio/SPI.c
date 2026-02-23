// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/bitbangio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/DigitalInOutProtocol.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "supervisor/port.h"

#define MAX_BAUDRATE (common_hal_mcu_get_clock_frequency() / 48)

void shared_module_bitbangio_spi_construct(bitbangio_spi_obj_t *self,
    mp_obj_t clock, mp_obj_t mosi, mp_obj_t miso) {

    // Allocate the pins in the same place as self.
    bool use_port_allocation = !gc_alloc_possible() || !gc_ptr_on_heap(self);

    // Convert clock from Pin to DigitalInOutProtocol
    self->clock = digitalinout_protocol_from_pin(clock, MP_QSTR_clock, false, use_port_allocation, &self->own_clock);
    digitalinout_protocol_switch_to_output(self->clock, self->polarity == 1, DRIVE_MODE_PUSH_PULL);

    // Convert mosi from Pin to DigitalInOutProtocol (optional)
    self->mosi = digitalinout_protocol_from_pin(mosi, MP_QSTR_mosi, true, use_port_allocation, &self->own_mosi);
    self->has_mosi = (self->mosi != mp_const_none);
    if (self->has_mosi) {
        digitalinout_protocol_switch_to_output(self->mosi, false, DRIVE_MODE_PUSH_PULL);
    }

    // Convert miso from Pin to DigitalInOutProtocol (optional)
    self->miso = digitalinout_protocol_from_pin(miso, MP_QSTR_miso, true, use_port_allocation, &self->own_miso);
    self->has_miso = (self->miso != mp_const_none);
    // MISO starts out as input by default, no need to change

    self->delay_half = 5;
    self->polarity = 0;
    self->phase = 0;
}

bool shared_module_bitbangio_spi_deinited(bitbangio_spi_obj_t *self) {
    return digitalinout_protocol_deinited(self->clock);
}

void shared_module_bitbangio_spi_deinit(bitbangio_spi_obj_t *self) {
    if (shared_module_bitbangio_spi_deinited(self)) {
        return;
    }
    // Only deinit and free the pins if we own them
    if (self->own_clock) {
        digitalinout_protocol_deinit(self->clock);
        circuitpy_free_obj(self->clock);
    }
    if (self->has_mosi && self->own_mosi) {
        digitalinout_protocol_deinit(self->mosi);
        circuitpy_free_obj(self->mosi);
    }
    if (self->has_miso && self->own_miso) {
        digitalinout_protocol_deinit(self->miso);
        circuitpy_free_obj(self->miso);
    }
}

void shared_module_bitbangio_spi_configure(bitbangio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    self->delay_half = 500000 / baudrate;
    // round delay_half up so that: actual_baudrate <= requested_baudrate
    if (500000 % baudrate != 0) {
        self->delay_half += 1;
    }

    if (polarity != self->polarity) {
        // If the polarity has changed, make sure we re-initialize the idle state
        // of the clock as well.
        self->polarity = polarity;
        digitalinout_protocol_switch_to_output(self->clock, polarity == 1, DRIVE_MODE_PUSH_PULL);
    }
    self->phase = phase;
}

bool shared_module_bitbangio_spi_try_lock(bitbangio_spi_obj_t *self) {
    bool success = false;
    common_hal_mcu_disable_interrupts();
    if (!self->locked) {
        self->locked = true;
        success = true;
    }
    common_hal_mcu_enable_interrupts();
    return success;
}

bool shared_module_bitbangio_spi_has_lock(bitbangio_spi_obj_t *self) {
    return self->locked;
}

void shared_module_bitbangio_spi_unlock(bitbangio_spi_obj_t *self) {
    self->locked = false;
}

// Writes out the given data.
bool shared_module_bitbangio_spi_write(bitbangio_spi_obj_t *self, const uint8_t *data, size_t len) {
    if (len > 0 && !self->has_mosi) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_mosi);
    }
    uint32_t delay_half = self->delay_half;

    // only MSB transfer is implemented

    // If a port defines MICROPY_PY_MACHINE_SPI_MIN_DELAY, and the configured
    // delay_half is equal to this value, then the software SPI implementation
    // will run as fast as possible, limited only by CPU speed and GPIO time.
    #ifdef MICROPY_PY_MACHINE_SPI_MIN_DELAY
    if (delay_half <= MICROPY_PY_MACHINE_SPI_MIN_DELAY) {
        for (size_t i = 0; i < len; ++i) {
            uint8_t data_out = data[i];
            for (int j = 0; j < 8; ++j, data_out <<= 1) {
                if (i == 0 && j == 0) {
                    if (digitalinout_protocol_set_value(self->mosi, (data_out >> 7) & 1) != 0) {
                        return false;
                    }
                } else {
                    digitalinout_protocol_set_value(self->mosi, (data_out >> 7) & 1);
                }
                digitalinout_protocol_set_value(self->clock, 1 - self->polarity);
                digitalinout_protocol_set_value(self->clock, self->polarity);
            }
            if (dest != NULL) {
                dest[i] = data_in;
            }
        }
        return true;
    }
    #endif

    for (size_t i = 0; i < len; ++i) {
        uint8_t data_out = data[i];
        for (int j = 0; j < 8; ++j, data_out <<= 1) {
            if (i == 0 && j == 0) {
                if (!digitalinout_protocol_set_value(self->mosi, (data_out >> 7) & 1)) {
                    return false;
                }
            } else {
                digitalinout_protocol_set_value(self->mosi, (data_out >> 7) & 1);
            }
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                digitalinout_protocol_set_value(self->clock, 1 - self->polarity);
                common_hal_mcu_delay_us(delay_half);
                digitalinout_protocol_set_value(self->clock, self->polarity);
            } else {
                digitalinout_protocol_set_value(self->clock, 1 - self->polarity);
                common_hal_mcu_delay_us(delay_half);
                digitalinout_protocol_set_value(self->clock, self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
        }

        // Some ports need a regular callback, but probably we don't need
        // to do this every byte, or even at all.
        #ifdef MICROPY_EVENT_POLL_HOOK
        MICROPY_EVENT_POLL_HOOK;
        #endif
    }
    return true;
}

// Reads in len bytes while outputting zeroes.
bool shared_module_bitbangio_spi_read(bitbangio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_data) {
    if (len > 0 && !self->has_miso) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_miso);
    }

    uint32_t delay_half = self->delay_half;

    // only MSB transfer is implemented

    // If a port defines MICROPY_PY_MACHINE_SPI_MIN_DELAY, and the configured
    // delay_half is equal to this value, then the software SPI implementation
    // will run as fast as possible, limited only by CPU speed and GPIO time.
    #ifdef MICROPY_PY_MACHINE_SPI_MIN_DELAY
    if (delay_half <= MICROPY_PY_MACHINE_SPI_MIN_DELAY) {
        // Clock out zeroes while we read.
        if (self->has_mosi) {
            if (digitalinout_protocol_set_value(self->mosi, false) != 0) {
                return false;
            }
        }
        for (size_t i = 0; i < len; ++i) {
            uint8_t data_in = 0;
            for (int j = 0; j < 8; ++j, data_out <<= 1) {
                digitalinout_protocol_set_value(self->clock, 1 - self->polarity);
                bool bit;
                digitalinout_protocol_get_value(self->miso, &bit);
                data_in = (data_in << 1) | bit;
                digitalinout_protocol_set_value(self->clock, self->polarity);
            }
            data[i] = data_in;
        }
        return true;
    }
    #endif
    if (self->has_mosi) {
        if (!digitalinout_protocol_set_value(self->mosi, false)) {
            return false;
        }
    }
    for (size_t i = 0; i < len; ++i) {
        uint8_t data_out = write_data;
        uint8_t data_in = 0;
        for (int j = 0; j < 8; ++j, data_out <<= 1) {
            if (self->has_mosi) {
                digitalinout_protocol_set_value(self->mosi, (data_out >> 7) & 1);
            }
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                digitalinout_protocol_set_value(self->clock, 1 - self->polarity);
            } else {
                digitalinout_protocol_set_value(self->clock, 1 - self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
            bool bit;
            digitalinout_protocol_get_value(self->miso, &bit);
            data_in = (data_in << 1) | bit;
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                digitalinout_protocol_set_value(self->clock, self->polarity);
            } else {
                digitalinout_protocol_set_value(self->clock, self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
        }
        data[i] = data_in;

        // Some ports need a regular callback, but probably we don't need
        // to do this every byte, or even at all.
        #ifdef MICROPY_EVENT_POLL_HOOK
        MICROPY_EVENT_POLL_HOOK;
        #endif
    }
    return true;
}

// transfer
bool shared_module_bitbangio_spi_transfer(bitbangio_spi_obj_t *self, const uint8_t *dout, uint8_t *din, size_t len) {
    if (!self->has_mosi && dout != NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_mosi);
    }
    if (!self->has_miso && din != NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_miso);
    }
    uint32_t delay_half = self->delay_half;

    // only MSB transfer is implemented

    // If a port defines MICROPY_PY_MACHINE_SPI_MIN_DELAY, and the configured
    // delay_half is equal to this value, then the software SPI implementation
    // will run as fast as possible, limited only by CPU speed and GPIO time.
    #ifdef MICROPY_PY_MACHINE_SPI_MIN_DELAY
    if (delay_half <= MICROPY_PY_MACHINE_SPI_MIN_DELAY) {
        for (size_t i = 0; i < len; ++i) {
            uint8_t data_out = dout[i];
            uint8_t data_in = 0;
            for (int j = 0; j < 8; ++j, data_out <<= 1) {
                if (i == 0 && j == 0) {
                    if (digitalinout_protocol_set_value(self->mosi, (data_out >> 7) & 1) != 0) {
                        return false;
                    }
                } else {
                    digitalinout_protocol_set_value(self->mosi, (data_out >> 7) & 1);
                }
                digitalinout_protocol_set_value(self->clock, 1 - self->polarity);
                bool bit;
                digitalinout_protocol_get_value(self->miso, &bit);
                data_in = (data_in << 1) | bit;
                digitalinout_protocol_set_value(self->clock, self->polarity);
            }
            din[i] = data_in;

            if (dest != NULL) {
                dest[i] = data_in;
            }
        }
        return true;
    }
    #endif

    for (size_t i = 0; i < len; ++i) {
        uint8_t data_out = dout[i];
        uint8_t data_in = 0;
        for (int j = 0; j < 8; ++j, data_out <<= 1) {
            if (i == 0 && j == 0) {
                if (!digitalinout_protocol_set_value(self->mosi, (data_out >> 7) & 1)) {
                    return false;
                }
            } else {
                digitalinout_protocol_set_value(self->mosi, (data_out >> 7) & 1);
            }
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                digitalinout_protocol_set_value(self->clock, 1 - self->polarity);
            } else {
                digitalinout_protocol_set_value(self->clock, 1 - self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
            bool bit;
            digitalinout_protocol_get_value(self->miso, &bit);
            data_in = (data_in << 1) | bit;
            if (self->phase == 0) {
                common_hal_mcu_delay_us(delay_half);
                digitalinout_protocol_set_value(self->clock, self->polarity);
            } else {
                digitalinout_protocol_set_value(self->clock, self->polarity);
                common_hal_mcu_delay_us(delay_half);
            }
        }
        din[i] = data_in;

        // Some ports need a regular callback, but probably we don't need
        // to do this every byte, or even at all.
        #ifdef MICROPY_EVENT_POLL_HOOK
        MICROPY_EVENT_POLL_HOOK;
        #endif
    }
    return true;
}
