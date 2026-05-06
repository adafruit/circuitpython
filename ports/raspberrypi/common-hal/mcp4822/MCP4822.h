// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tod Kurt
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/rp2pio/StateMachine.h"

#include "audio_dma.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    rp2pio_statemachine_obj_t state_machine;
    audio_dma_t dma;
    bool playing;
} mcp4822_mcp4822_obj_t;

void mcp4822_reset(void);
