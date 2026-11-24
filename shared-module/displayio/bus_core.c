// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-module/displayio/bus_core.h"

#include "py/gc.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#if CIRCUITPY_FOURWIRE
#include "shared-bindings/fourwire/FourWire.h"
#endif
#if CIRCUITPY_I2CDISPLAYBUS
#include "shared-bindings/i2cdisplaybus/I2CDisplayBus.h"
#endif
#if CIRCUITPY_PARALLELDISPLAYBUS
#include "shared-bindings/paralleldisplaybus/ParallelBus.h"
#endif
#if CIRCUITPY_QSPIBUS
#include "shared-bindings/qspibus/QSPIBus.h"
#endif
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/display_core.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/tick.h"

#include <stdint.h>
#include <string.h>

#define DELAY 0x80

#define DISPLAYIO_CORE_DEBUG(...) (void)0
// #define DISPLAYIO_CORE_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)

void displayio_display_bus_validate(mp_obj_t bus) {
    if (
        #if CIRCUITPY_PARALLELDISPLAYBUS
        mp_obj_is_type(bus, &paralleldisplaybus_parallelbus_type) ||
        #endif
        #if CIRCUITPY_FOURWIRE
        mp_obj_is_type(bus, &fourwire_fourwire_type) ||
        #endif
        #if CIRCUITPY_I2CDISPLAYBUS
        mp_obj_is_type(bus, &i2cdisplaybus_i2cdisplaybus_type) ||
        #endif
        #if CIRCUITPY_QSPIBUS
        mp_obj_is_type(bus, &qspibus_qspibus_type) ||
        #endif
        false) {
        return;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("Unsupported display bus type"));
}

void displayio_display_bus_construct(displayio_display_bus_t *self,
    mp_obj_t bus, uint16_t ram_width, uint16_t ram_height, int16_t colstart, int16_t rowstart,
    uint16_t column_command, uint16_t row_command, uint16_t set_current_column_command, uint16_t set_current_row_command,
    bool data_as_commands, bool always_toggle_chip_select, bool SH1107_addressing, bool address_little_endian) {

    self->ram_width = ram_width;
    self->ram_height = ram_height;
    self->colstart = colstart;
    self->rowstart = rowstart;

    self->column_command = column_command;
    self->row_command = row_command;
    self->set_current_column_command = set_current_column_command;
    self->set_current_row_command = set_current_row_command;
    self->data_as_commands = data_as_commands;
    self->always_toggle_chip_select = always_toggle_chip_select;
    self->SH1107_addressing = SH1107_addressing;
    self->address_little_endian = address_little_endian;

    self->flush = NULL;
    #if CIRCUITPY_BUSIO_SPI_ASYNC
    self->send_async = NULL;
    #endif

    #if CIRCUITPY_PARALLELDISPLAYBUS
    if (mp_obj_is_type(bus, &paralleldisplaybus_parallelbus_type)) {
        self->bus_reset = common_hal_paralleldisplaybus_parallelbus_reset;
        self->bus_free = common_hal_paralleldisplaybus_parallelbus_bus_free;
        self->begin_transaction = common_hal_paralleldisplaybus_parallelbus_begin_transaction;
        self->send = common_hal_paralleldisplaybus_parallelbus_send;
        self->end_transaction = common_hal_paralleldisplaybus_parallelbus_end_transaction;
        self->collect_ptrs = common_hal_paralleldisplaybus_parallelbus_collect_ptrs;
    } else
    #endif
    #if CIRCUITPY_FOURWIRE
    if (mp_obj_is_type(bus, &fourwire_fourwire_type)) {
        self->bus_reset = common_hal_fourwire_fourwire_reset;
        self->bus_free = common_hal_fourwire_fourwire_bus_free;
        self->begin_transaction = common_hal_fourwire_fourwire_begin_transaction;
        self->send = common_hal_fourwire_fourwire_send;
        self->end_transaction = common_hal_fourwire_fourwire_end_transaction;
        self->collect_ptrs = common_hal_fourwire_fourwire_collect_ptrs;
        #if CIRCUITPY_BUSIO_SPI_ASYNC
        self->send_async = common_hal_fourwire_fourwire_send_async;
        self->flush = common_hal_fourwire_fourwire_flush;
        #endif
    } else
    #endif
    #if CIRCUITPY_I2CDISPLAYBUS
    if (mp_obj_is_type(bus, &i2cdisplaybus_i2cdisplaybus_type)) {
        self->bus_reset = common_hal_i2cdisplaybus_i2cdisplaybus_reset;
        self->bus_free = common_hal_i2cdisplaybus_i2cdisplaybus_bus_free;
        self->begin_transaction = common_hal_i2cdisplaybus_i2cdisplaybus_begin_transaction;
        self->send = common_hal_i2cdisplaybus_i2cdisplaybus_send;
        self->end_transaction = common_hal_i2cdisplaybus_i2cdisplaybus_end_transaction;
        self->collect_ptrs = common_hal_i2cdisplaybus_i2cdisplaybus_collect_ptrs;
    } else
    #endif
    #if CIRCUITPY_QSPIBUS
    if (mp_obj_is_type(bus, &qspibus_qspibus_type)) {
        self->bus_reset = common_hal_qspibus_qspibus_reset;
        self->bus_free = common_hal_qspibus_qspibus_bus_free;
        self->begin_transaction = common_hal_qspibus_qspibus_begin_transaction;
        self->send = common_hal_qspibus_qspibus_send;
        self->end_transaction = common_hal_qspibus_qspibus_end_transaction;
        self->flush = common_hal_qspibus_qspibus_flush;
        self->collect_ptrs = common_hal_qspibus_qspibus_collect_ptrs;
    } else
    #endif
    {
        mp_raise_ValueError(MP_ERROR_TEXT("Unsupported display bus type"));
    }
    self->bus = bus;
}

// This is just a hint, and is not a reliable result, since the bus could be grabbed in between this called
// and the attempt to use the bus. Use displayio_display_bus_begin_transaction(), which is atomic.
bool displayio_display_bus_is_free(displayio_display_bus_t *self) {
    return !self->bus || self->bus_free(self->bus);
}

MP_WARN_UNUSED_RESULT bool displayio_display_bus_begin_transaction(displayio_display_bus_t *self) {
    if (!self->bus) {
        return false;
    }
    mp_obj_base_t *bus_base = MP_OBJ_TO_PTR(self->bus);
    if (bus_base->type == &mp_type_NoneType) {
        return false;
    }
    return self->begin_transaction(self->bus);
}

void displayio_display_bus_end_transaction(displayio_display_bus_t *self) {
    self->end_transaction(self->bus);
}

void displayio_display_bus_send_init_sequence(displayio_display_bus_t *self,
    const uint8_t *init_sequence, uint16_t init_sequence_len) {
    uint32_t i = 0;
    while (i < init_sequence_len) {
        const uint8_t *cmd = init_sequence + i;
        uint8_t data_size = *(cmd + 1);
        bool delay = (data_size & DELAY) != 0;
        data_size &= ~DELAY;
        const uint8_t *data = cmd + 2;
        while (!displayio_display_bus_begin_transaction(self)) {
            RUN_BACKGROUND_TASKS;
            if (mp_hal_is_interrupted()) {
                mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q init failed"), MP_QSTR_display);
            }
        }
        if (self->data_as_commands) {
            uint8_t full_command[data_size + 1];
            full_command[0] = cmd[0];
            memcpy(full_command + 1, data, data_size);
            self->send(self->bus, DISPLAY_COMMAND, CHIP_SELECT_TOGGLE_EVERY_BYTE, full_command, data_size + 1);
        } else {
            self->send(self->bus, DISPLAY_COMMAND, data_size > 0 ? CHIP_SELECT_TOGGLE_EVERY_BYTE : CHIP_SELECT_UNTOUCHED, cmd, 1);
            self->send(self->bus, DISPLAY_DATA, CHIP_SELECT_UNTOUCHED, data, data_size);
        }
        displayio_display_bus_end_transaction(self);
        uint16_t delay_length_ms = 10;
        if (delay) {
            data_size++;
            delay_length_ms = *(cmd + 1 + data_size);
            if (delay_length_ms == 255) {
                delay_length_ms = 500;
            }
        }
        common_hal_time_delay_ms(delay_length_ms);
        i += 2 + data_size;
    }
}

void displayio_display_bus_set_region_to_update(displayio_display_bus_t *self, const _displayio_colorspace_t *colorspace, displayio_area_t *area) {
    uint16_t x1 = area->x1 + self->colstart;
    uint16_t x2 = area->x2 + self->colstart;
    uint16_t y1 = area->y1 + self->rowstart;
    uint16_t y2 = area->y2 + self->rowstart;

    // Collapse down the dimension where multiple pixels are in a byte.
    if (colorspace->depth < 8) {
        uint8_t pixels_per_byte = 8 / colorspace->depth;
        if (colorspace->pixels_in_byte_share_row) {
            x1 /= pixels_per_byte * colorspace->bytes_per_cell;
            x2 /= pixels_per_byte * colorspace->bytes_per_cell;
        } else {
            y1 /= pixels_per_byte * colorspace->bytes_per_cell;
            y2 /= pixels_per_byte * colorspace->bytes_per_cell;
        }
    }

    x2 -= 1;
    y2 -= 1;

    display_chip_select_behavior_t chip_select = CHIP_SELECT_UNTOUCHED;
    if (self->always_toggle_chip_select || self->data_as_commands) {
        chip_select = CHIP_SELECT_TOGGLE_EVERY_BYTE;
    }

    // Set column.
    if (!displayio_display_bus_begin_transaction(self)) {
        return;
    }
    uint8_t data[5];
    data[0] = self->column_command;
    uint8_t data_length = 1;
    display_byte_type_t data_type = DISPLAY_DATA;
    if (!self->data_as_commands) {
        self->send(self->bus, DISPLAY_COMMAND, CHIP_SELECT_UNTOUCHED, data, 1);
        data_length = 0;
    } else {
        data_type = DISPLAY_COMMAND;
    }

    if (self->ram_width < 0x100) {
        data[data_length++] = x1;
        data[data_length++] = x2;
    } else {
        if (self->address_little_endian) {
            x1 = __builtin_bswap16(x1);
            x2 = __builtin_bswap16(x2);
        }
        data[data_length++] = x1 >> 8;
        data[data_length++] = x1 & 0xff;
        data[data_length++] = x2 >> 8;
        data[data_length++] = x2 & 0xff;
    }

    // Quirk for SH1107 "SH1107_addressing"
    //     Column lower command = 0x00, Column upper command = 0x10
    if (self->SH1107_addressing) {
        data[0] = ((x1 >> 4) & 0x0F) | 0x10; // 0x10 to 0x17
        data[1] = x1 & 0x0F; // 0x00 to 0x0F
        data_length = 2;
    }

    self->send(self->bus, data_type, chip_select, data, data_length);
    displayio_display_bus_end_transaction(self);

    if (self->set_current_column_command != NO_COMMAND) {
        uint8_t command = self->set_current_column_command;
        if (!displayio_display_bus_begin_transaction(self)) {
            return;
        }
        self->send(self->bus, DISPLAY_COMMAND, chip_select, &command, 1);
        // Only send the first half of data because it is the first coordinate.
        self->send(self->bus, DISPLAY_DATA, chip_select, data, data_length / 2);
        displayio_display_bus_end_transaction(self);
    }


    // Set row.
    if (!displayio_display_bus_begin_transaction(self)) {
        return;
    }
    data[0] = self->row_command;
    data_length = 1;
    if (!self->data_as_commands) {
        self->send(self->bus, DISPLAY_COMMAND, CHIP_SELECT_UNTOUCHED, data, 1);
        data_length = 0;
    }

    if (self->ram_height < 0x100) {
        data[data_length++] = y1;
        data[data_length++] = y2;
    } else {
        if (self->address_little_endian) {
            y1 = __builtin_bswap16(y1);
            y2 = __builtin_bswap16(y2);
        }
        data[data_length++] = y1 >> 8;
        data[data_length++] = y1 & 0xff;
        data[data_length++] = y2 >> 8;
        data[data_length++] = y2 & 0xff;
    }

    // Quirk for SH1107 "SH1107_addressing"
    //     Page address command = 0xB0
    if (self->SH1107_addressing) {
        // set the page to our y value
        data[0] = 0xB0 | y1;
        data_length = 1;
    }

    self->send(self->bus, data_type, chip_select, data, data_length);
    displayio_display_bus_end_transaction(self);

    if (self->set_current_row_command != NO_COMMAND) {
        uint8_t command = self->set_current_row_command;
        if (!displayio_display_bus_begin_transaction(self)) {
            return;
        }
        self->send(self->bus, DISPLAY_COMMAND, chip_select, &command, 1);
        // Only send the first half of data because it is the first coordinate.
        self->send(self->bus, DISPLAY_DATA, chip_select, data, data_length / 2);
        displayio_display_bus_end_transaction(self);
    }
}

void displayio_display_bus_flush(displayio_display_bus_t *self) {
    if (self->flush != NULL) {
        self->flush(self->bus);
    }
}

void displayio_display_bus_collect_ptrs(displayio_display_bus_t *self) {
    self->collect_ptrs(self->bus);
}
