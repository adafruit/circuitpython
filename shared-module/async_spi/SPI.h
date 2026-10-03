// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/misc.h"
#include "common-hal/busio/SPI.h"

// busio_spi_obj_t first, so that busio.SPI's methods work on these objects too. busio.SPI's
// make_new allocates them, so nothing else may be added.
typedef struct {
    busio_spi_obj_t spi;
} async_spi_spi_obj_t;
MP_STATIC_ASSERT(sizeof(async_spi_spi_obj_t) == sizeof(busio_spi_obj_t));
