// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <sys/time.h>

#include "py/runtime.h"
#include "shared-bindings/alarm/__init__.h"
#include "shared-bindings/alarm/time/TimeAlarm.h"
#include "shared-bindings/time/__init__.h"

#include "shared/timeutils/timeutils.h"

#include "pico/aon_timer.h"

#ifdef SLEEP_DEBUG
#include <inttypes.h>
#include "py/mpprint.h"
#define DEBUG_PRINT(fmt, ...) ((void)mp_printf(&mp_plat_print, "DBG:%s:%04d: " fmt "\n", __FILE__, __LINE__,##__VA_ARGS__))
#else
#define DEBUG_PRINT(fmt, ...)((void)0)
#endif

static bool woke_up = false;
static bool _timealarm_set = false;

static void timer_callback(void) {
    DEBUG_PRINT("AON Timer woke us up");
    woke_up = true;
}

void common_hal_alarm_time_timealarm_construct(alarm_time_timealarm_obj_t *self, mp_float_t monotonic_time) {
    self->monotonic_time = monotonic_time;
}

mp_float_t common_hal_alarm_time_timealarm_get_monotonic_time(alarm_time_timealarm_obj_t *self) {
    return self->monotonic_time;
}

mp_obj_t alarm_time_timealarm_find_triggered_alarm(size_t n_alarms, const mp_obj_t *alarms) {
    for (size_t i = 0; i < n_alarms; i++) {
        if (mp_obj_is_type(alarms[i], &alarm_time_timealarm_type)) {
            return alarms[i];
        }
    }
    return mp_const_none;
}

mp_obj_t alarm_time_timealarm_record_wake_alarm(void) {
    alarm_time_timealarm_obj_t *const alarm = &alarm_wake_alarm.time_alarm;

    alarm->base.type = &alarm_time_timealarm_type;
    // TODO: Set monotonic_time based on the RTC state.
    alarm->monotonic_time = 0.0f;
    return alarm;
}

bool alarm_time_timealarm_woke_this_cycle(void) {
    return woke_up;
}

void alarm_time_timealarm_reset(void) {
    aon_timer_disable_alarm();
    woke_up = false;
}

void alarm_time_timealarm_set_alarms(bool deep_sleep, size_t n_alarms, const mp_obj_t *alarms) {
    _timealarm_set = false;
    alarm_time_timealarm_obj_t *timealarm = MP_OBJ_NULL;

    for (size_t i = 0; i < n_alarms; i++) {
        if (!mp_obj_is_type(alarms[i], &alarm_time_timealarm_type)) {
            continue;
        }
        if (_timealarm_set) {
            mp_raise_ValueError(MP_ERROR_TEXT("Only one alarm.time alarm can be set."));
        }
        timealarm = MP_OBJ_TO_PTR(alarms[i]);
        _timealarm_set = true;
    }
    if (!_timealarm_set) {
        return;
    }

    // Compute how long to actually sleep, considering the time now.
    mp_float_t mono_seconds_to_date = uint64_to_float(common_hal_time_monotonic_ms()) / 1000.0f;
    mp_float_t wakeup_in_secs = MAX(0.0f, timealarm->monotonic_time - mono_seconds_to_date);

    uint32_t rtc_seconds_to_date;

    // the SDK suggests not using aon_timer_get_time for the RP2040, because
    // this inflates the binary size by pulling in local_time_r (496 bytes
    // according to nm). But since common-hal/rtc/RTC.c also uses
    // aon_timer_get_time, we use it here too. Any optimization will have to
    // change this everywhere.

    struct timespec t;
    aon_timer_get_time(&t);
    rtc_seconds_to_date = t.tv_sec;
    DEBUG_PRINT("rtc_seconds_to_date: %u", rtc_seconds_to_date);

    // The float value is always slightly under, so add 1 to compensate
    uint32_t alarm_seconds = rtc_seconds_to_date + (uint32_t)wakeup_in_secs + 1;

    // reuse t
    // also see note above regarding aon_timer_get_time, also true here
    t.tv_sec = alarm_seconds;
    DEBUG_PRINT("alarm_seconds: %d", t.tv_sec);
    #ifdef PICO_RP2040
    aon_timer_enable_alarm(&t, &timer_callback, deep_sleep);
    #else
    aon_timer_enable_alarm(&t, &timer_callback, true);
    #endif
    woke_up = false;
}

bool alarm_time_timealarm_is_set(void) {
    return _timealarm_set;
}
