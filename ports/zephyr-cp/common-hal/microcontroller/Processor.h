// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan  Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#if CONFIG_HWINFO_NRF
#define COMMON_HAL_MCU_PROCESSOR_UID_LENGTH 8
#else
// Extra long and the remainder will be left empty. Will print out the actual length.
#define COMMON_HAL_MCU_PROCESSOR_UID_LENGTH 32
#endif

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    // Stores no state currently.
} mcu_processor_obj_t;

extern uint32_t reset_reason_saved;
