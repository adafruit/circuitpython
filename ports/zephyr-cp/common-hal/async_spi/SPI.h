// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"
#include "supervisor/shared/async_flag.h"

#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/kernel.h>

typedef struct {
    mp_obj_base_t base;
    const struct device *spi_device;    // NULL once deinited
    struct spi_config config[2];        // two slots: a driver sees a change by the pointer
    uint8_t active_config;
    const mcu_pin_obj_t *clock;
    const mcu_pin_obj_t *mosi;
    const mcu_pin_obj_t *miso;
    circuitpy_async_flag_t *done;       // of the running transfer, NULL when none
    struct spi_buf tx_buf;              // the running transfer's buffers
    struct spi_buf rx_buf;
    struct spi_buf_set tx;
    struct spi_buf_set rx;
    uint8_t *fill;                      // the bytes a read sends, when not zero
    circuitpy_async_flag_t *idle_done;  // a configure() waiting for the transfer to finish
    uint32_t pending_frequency;
    uint16_t pending_operation;
} async_spi_spi_obj_t;
