// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    mp_float_t monotonic_time;
} alarm_time_timealarm_obj_t;

mp_obj_t alarm_time_timealarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms);
mp_obj_t alarm_time_timealarm_record_wake_alarm(void);

void alarm_time_timealarm_reset(void);
void alarm_time_timealarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms);
bool alarm_time_timealarm_woke_this_cycle(void);
bool alarm_time_timealarm_is_set(void);
#if PICO_RP2350
// Returns the AON timer time, in milliseconds, of the last alarm given to
// alarm_time_timealarm_set_alarms(). Only meaningful when alarm_time_timealarm_is_set().
uint64_t alarm_time_timealarm_get_wakeup_ms(void);
#endif
