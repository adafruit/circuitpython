// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"
#include "shared-module/audiocore/__init__.h"

#include <zephyr/device.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/kernel.h>

#if CIRCUITPY_AUDIOBUSIO_I2SOUT

typedef struct {
    mp_obj_base_t base;
    const struct device *i2s_dev;
    const mcu_pin_obj_t *bit_clock;
    const mcu_pin_obj_t *word_select;
    const mcu_pin_obj_t *data;
    const mcu_pin_obj_t *main_clock;
    mp_obj_t sample;
    struct k_mem_slab mem_slab;
    char *slab_buffer;
    struct k_thread thread;
    k_thread_stack_t *thread_stack;
    k_tid_t thread_id;
    size_t block_size;
    bool left_justified;
    bool playing;
    bool paused;
    bool loop;
    bool stopping;
    bool single_buffer;
    uint8_t bytes_per_sample;
    uint8_t channel_count;
} audiobusio_i2sout_obj_t;

mp_obj_t common_hal_audiobusio_i2sout_construct_from_device(audiobusio_i2sout_obj_t *self, const struct device *i2s_device);

void i2sout_reset(void);

#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT
