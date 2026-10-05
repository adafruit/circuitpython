// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/runtime.h"
#include "common-hal/async_spi/SPI.h"
#include "common-hal/microcontroller/Pin.h"
#include "supervisor/shared/async_flag.h"

extern const mp_obj_type_t async_spi_spi_type;

// Argument tables, shared with busio.SPI, which takes the same arguments.
extern const mp_arg_t async_spi_spi_make_new_args[4];      // clock, MOSI, MISO, half_duplex
extern const mp_arg_t async_spi_spi_configure_args[4];
extern const mp_arg_t async_spi_spi_write_args[3];
extern const mp_arg_t async_spi_spi_readinto_args[4];
extern const mp_arg_t async_spi_spi_write_readinto_args[6];

void common_hal_async_spi_spi_construct(async_spi_spi_obj_t *self, const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso);
bool common_hal_async_spi_spi_deinited(async_spi_spi_obj_t *self);
// Stops a running transfer.
void common_hal_async_spi_spi_deinit(async_spi_spi_obj_t *self);
uint32_t common_hal_async_spi_spi_get_frequency(async_spi_spi_obj_t *self);

// configure(): done is set once the running transfer, if any, has finished and the settings
// are in place. The context is the SPI object.
void common_hal_async_spi_spi_configure_start(async_spi_spi_obj_t *self, uint32_t baudrate,
    uint8_t polarity, uint8_t phase, uint8_t bits, circuitpy_async_flag_t *done);
mp_obj_t common_hal_async_spi_spi_configure_end(void *context, circuitpy_async_flag_t *done);
void common_hal_async_spi_spi_configure_cancel(void *context, circuitpy_async_flag_t *done);

// Each transfer start sets done once the transfer has finished, possibly from an interrupt. The
// buffers must stay valid until then. transfer_end() and transfer_cancel() take the SPI object
// as the context and act only on the transfer started with done.
void common_hal_async_spi_spi_write_start(async_spi_spi_obj_t *self, const uint8_t *data,
    size_t len, circuitpy_async_flag_t *done);
void common_hal_async_spi_spi_readinto_start(async_spi_spi_obj_t *self, uint8_t *data,
    size_t len, uint8_t write_value, circuitpy_async_flag_t *done);
void common_hal_async_spi_spi_write_readinto_start(async_spi_spi_obj_t *self,
    const uint8_t *data_out, uint8_t *data_in, size_t len, circuitpy_async_flag_t *done);
mp_obj_t common_hal_async_spi_spi_transfer_end(void *context, circuitpy_async_flag_t *done);
void common_hal_async_spi_spi_transfer_cancel(void *context, circuitpy_async_flag_t *done);

// The names CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW looks up; all three transfers end the same way.
#define common_hal_async_spi_spi_write_end common_hal_async_spi_spi_transfer_end
#define common_hal_async_spi_spi_write_cancel common_hal_async_spi_spi_transfer_cancel
#define common_hal_async_spi_spi_readinto_end common_hal_async_spi_spi_transfer_end
#define common_hal_async_spi_spi_readinto_cancel common_hal_async_spi_spi_transfer_cancel
#define common_hal_async_spi_spi_write_readinto_end common_hal_async_spi_spi_transfer_end
#define common_hal_async_spi_spi_write_readinto_cancel common_hal_async_spi_spi_transfer_cancel
