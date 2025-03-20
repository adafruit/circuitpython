// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include <zephyr/kernel.h>

typedef struct {
    mp_obj_base_t base;

    const struct device *uart_device;
    struct k_msgq msgq;

    k_timeout_t timeout;

    bool rx_paused;     // set by irq if no space in rbuf
} zephyr_serial_uart_obj_t;
