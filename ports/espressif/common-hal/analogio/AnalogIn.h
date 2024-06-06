// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "components/hal/include/hal/adc_types.h"
#include "FreeRTOS.h"
#include "freertos/semphr.h"
#include "py/obj.h"
#include "adc_cali_schemes.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "driver/gpio.h"
#include "hal/adc_types.h"


typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    uint8_t sample_size;
    adc_oneshot_unit_handle_t adc_handle;
    adc_channel_t channel;
    #if defined(ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED) && ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t config;
    #endif
    #if defined(ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED) && ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_line_fitting_config_t config;
    #endif
    adc_cali_scheme_ver_t calibration_scheme;
    adc_cali_handle_t calibration;
    uint8_t samples;

} analogio_analogin_obj_t;
