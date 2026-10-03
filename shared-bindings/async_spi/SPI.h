// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "supervisor/shared/async_flag.h"
#include "shared-module/async_spi/SPI.h"

extern const mp_obj_type_t async_spi_spi_type;

// The transfers are busio's async ones, with the SPI object as the context. transfer_end() and
// transfer_cancel() act only on the transfer started with done.
mp_obj_t common_hal_async_spi_spi_transfer_end(void *context, circuitpy_async_flag_t *done);
void common_hal_async_spi_spi_transfer_cancel(void *context, circuitpy_async_flag_t *done);

// The names CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW looks up; all three transfers end the same way.
#define common_hal_async_spi_spi_write_end common_hal_async_spi_spi_transfer_end
#define common_hal_async_spi_spi_write_cancel common_hal_async_spi_spi_transfer_cancel
#define common_hal_async_spi_spi_readinto_end common_hal_async_spi_spi_transfer_end
#define common_hal_async_spi_spi_readinto_cancel common_hal_async_spi_spi_transfer_cancel
#define common_hal_async_spi_spi_write_readinto_end common_hal_async_spi_spi_transfer_end
#define common_hal_async_spi_spi_write_readinto_cancel common_hal_async_spi_spi_transfer_cancel
