// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-bindings/wifi/__init__.h"
#include "shared-bindings/wifi/Radio.h"

// static bool wifi_inited;
// static bool wifi_ever_inited;
// static bool wifi_user_initiated;

wifi_radio_obj_t common_hal_wifi_radio_obj;

void common_hal_wifi_init(bool user_initiated) {
    wifi_radio_obj_t *radio = &common_hal_wifi_radio_obj;

    // if (wifi_inited) {
    //     if (user_initiated && !wifi_user_initiated) {
    //         common_hal_wifi_radio_set_enabled(radio, true);
    //     }
    //     return;
    // }
    // wifi_inited = true;
    // wifi_user_initiated = user_initiated;

    radio->base.type = &wifi_radio_type;
    common_hal_wifi_radio_mark_deinit(radio);
}

void wifi_user_reset(void) {
    // if (wifi_user_initiated) {
    //     wifi_reset();
    //     wifi_user_initiated = false;
    // }
}

void wifi_reset(void) {
    // if (!wifi_inited) {
    //     return;
    // }
}

void common_hal_wifi_gc_collect(void) {
    common_hal_wifi_radio_gc_collect(&common_hal_wifi_radio_obj);
}
