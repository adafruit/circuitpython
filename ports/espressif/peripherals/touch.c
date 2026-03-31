// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#include "peripherals/touch.h"

static touch_sensor_handle_t touch_controller = NULL;
static touch_channel_handle_t touch_channels[TOUCH_TOTAL_CHAN_NUM] = {NULL};
static bool touch_never_reset_flag = false;
static bool touch_enabled = false;
static bool touch_scanning = false;

static int chan_index(int channel_id) {
    return channel_id - TOUCH_MIN_CHAN_ID;
}

touch_sensor_handle_t peripherals_touch_get_controller(void) {
    return touch_controller;
}

touch_channel_handle_t peripherals_touch_get_handle(int channel_id) {
    return touch_channels[chan_index(channel_id)];
}

void peripherals_touch_reset(void) {
    if (touch_controller != NULL && !touch_never_reset_flag) {
        if (touch_scanning) {
            touch_sensor_stop_continuous_scanning(touch_controller);
            touch_scanning = false;
        }
        if (touch_enabled) {
            touch_sensor_disable(touch_controller);
            touch_enabled = false;
        }
        for (unsigned int i = 0; i < TOUCH_TOTAL_CHAN_NUM; i++) {
            if (touch_channels[i] != NULL) {
                touch_sensor_del_channel(touch_channels[i]);
                touch_channels[i] = NULL;
            }
        }
        touch_sensor_del_controller(touch_controller);
        touch_controller = NULL;
    }
}

void peripherals_touch_never_reset(const bool enable) {
    touch_never_reset_flag = enable;
}

void peripherals_touch_init(const int channel_id) {
    int idx = chan_index(channel_id);

    // Already initialized this channel
    if (touch_channels[idx] != NULL) {
        return;
    }

    // Stop scanning and disable before modifying channels
    if (touch_scanning) {
        touch_sensor_stop_continuous_scanning(touch_controller);
        touch_scanning = false;
    }
    if (touch_enabled) {
        touch_sensor_disable(touch_controller);
        touch_enabled = false;
    }

    if (touch_controller == NULL) {
        #if SOC_TOUCH_SENSOR_VERSION == 1
        touch_sensor_sample_config_t sample_cfg = TOUCH_SENSOR_V1_DEFAULT_SAMPLE_CONFIG(5.0, TOUCH_VOLT_LIM_L_0V5, TOUCH_VOLT_LIM_H_1V7);
        #elif SOC_TOUCH_SENSOR_VERSION == 2
        touch_sensor_sample_config_t sample_cfg = TOUCH_SENSOR_V2_DEFAULT_SAMPLE_CONFIG(500, TOUCH_VOLT_LIM_L_0V5, TOUCH_VOLT_LIM_H_2V2);
        #elif SOC_TOUCH_SENSOR_VERSION == 3
        touch_sensor_sample_config_t sample_cfg = TOUCH_SENSOR_V3_DEFAULT_SAMPLE_CONFIG2(3, 29, 8, 3);
        #endif
        touch_sensor_config_t sens_cfg = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(1, &sample_cfg);
        touch_sensor_new_controller(&sens_cfg, &touch_controller);
    }

    #if SOC_TOUCH_SENSOR_VERSION == 1
    touch_channel_config_t chan_cfg = {
        .abs_active_thresh = {1000},
        .charge_speed = TOUCH_CHARGE_SPEED_7,
        .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_DEFAULT,
        .group = TOUCH_CHAN_TRIG_GROUP_BOTH,
    };
    #else
    touch_channel_config_t chan_cfg = {
        .active_thresh = {2000},
        .charge_speed = TOUCH_CHARGE_SPEED_7,
        .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_DEFAULT,
    };
    #endif

    touch_sensor_new_channel(touch_controller, channel_id, &chan_cfg, &touch_channels[idx]);

    // Enable and start continuous scanning
    touch_sensor_enable(touch_controller);
    touch_enabled = true;
    touch_sensor_start_continuous_scanning(touch_controller);
    touch_scanning = true;
}

uint16_t peripherals_touch_read(int channel_id) {
    int idx = chan_index(channel_id);
    if (touch_channels[idx] == NULL) {
        return 0;
    }

    uint32_t touch_value = 0;
    touch_channel_read_data(touch_channels[idx], TOUCH_CHAN_DATA_TYPE_RAW, &touch_value);

    #if SOC_TOUCH_SENSOR_VERSION == 1
    // ESP32 touch reads a lower value when touched.
    // Flip the values to be consistent with TouchIn assumptions.
    if (touch_value > UINT16_MAX) {
        return 0;
    }
    return UINT16_MAX - (uint16_t)touch_value;
    #else
    if (touch_value > UINT16_MAX) {
        return UINT16_MAX;
    }
    return (uint16_t)touch_value;
    #endif
}
