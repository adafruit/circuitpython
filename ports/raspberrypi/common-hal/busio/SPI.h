// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "peripherals/spi.h"

typedef struct {
    mp_obj_base_t base;
    rp2_spi_t spi;
    bool has_lock;
} busio_spi_obj_t;
