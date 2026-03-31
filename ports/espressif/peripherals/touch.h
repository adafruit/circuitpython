// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "driver/touch_sens.h"

extern void peripherals_touch_init(const int channel_id);
extern uint16_t peripherals_touch_read(int channel_id);
extern void peripherals_touch_reset(void);
extern void peripherals_touch_never_reset(const bool enable);
extern touch_sensor_handle_t peripherals_touch_get_controller(void);
extern touch_channel_handle_t peripherals_touch_get_handle(int channel_id);
