// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tod Kurt
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/mcp4822/MCP4822.h"
#include "common-hal/microcontroller/Pin.h"

extern const mp_obj_type_t mcp4822_mcp4822_type;

void common_hal_mcp4822_mcp4822_construct(mcp4822_mcp4822_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *cs, uint8_t gain);

void common_hal_mcp4822_mcp4822_deinit(mcp4822_mcp4822_obj_t *self);
bool common_hal_mcp4822_mcp4822_deinited(mcp4822_mcp4822_obj_t *self);
void common_hal_mcp4822_mcp4822_play(mcp4822_mcp4822_obj_t *self, mp_obj_t sample, bool loop);
void common_hal_mcp4822_mcp4822_stop(mcp4822_mcp4822_obj_t *self);
bool common_hal_mcp4822_mcp4822_get_playing(mcp4822_mcp4822_obj_t *self);
void common_hal_mcp4822_mcp4822_pause(mcp4822_mcp4822_obj_t *self);
void common_hal_mcp4822_mcp4822_resume(mcp4822_mcp4822_obj_t *self);
bool common_hal_mcp4822_mcp4822_get_paused(mcp4822_mcp4822_obj_t *self);
