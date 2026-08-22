// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "common-hal/analogio/AnalogIn.h"
#include "shared-bindings/analogio/AnalogIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"

#include "nrf_saadc.h"
#include "nrf_gpio.h"

#define CHANNEL_NO 0

void analogin_init(void) {
    // Calibrate the ADC once, on startup.
    nrf_saadc_enable(NRF_SAADC);
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_CALIBRATEDONE);
    nrf_saadc_task_trigger(NRF_SAADC, NRF_SAADC_TASK_CALIBRATEOFFSET);
    while (nrf_saadc_event_check(NRF_SAADC, NRF_SAADC_EVENT_CALIBRATEDONE) == 0) {
    }
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_CALIBRATEDONE);
    // Calibration also raises END, DONE and RESULTDONE. Leaving them set makes the
    // first conversion in common_hal_analogio_analogin_get_value() fall straight
    // through its wait on END and return 0; see the comment there.
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_END);
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_DONE);
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_RESULTDONE);
    nrf_saadc_disable(NRF_SAADC);
}

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self, const mcu_pin_obj_t *pin) {
    if (pin->adc_channel == 0) {
        raise_ValueError_invalid_pin();
    }

    nrf_gpio_cfg_default(pin->number);

    claim_pin(pin);
    self->pin = pin;
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        return;
    }

    nrf_gpio_cfg_default(self->pin->number);

    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    // Something else might have used the ADC in a different way,
    // so we completely re-initialize it.

    // The SAADC fills this in over EasyDMA, so the compiler must not be allowed
    // to keep it in a register or assume it is unchanged across the conversion.
    // `static volatile` forces a real memory location that is re-read after the
    // conversion, and the explicit alignment keeps the EasyDMA pointer
    // word-aligned. Not reentrant, but neither is the single, shared ADC channel
    // that this function reconfigures on every call.
    //
    // This replaces an older `asm volatile ("" : : : "memory")` fence, added for
    // a suspected gcc13 miscompile that made `value` always zero. That diagnosis
    // looks mistaken: on gcc 14.2.1 the generated code was verified correct with
    // and without the fence, and the real cause of the always-zero reading was
    // the stale EVENTS_END handled below.
    static volatile nrf_saadc_value_t value __attribute__((aligned(4)));
    value = 0;

    const nrf_saadc_channel_config_t config = {
        .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
        .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
        .gain = NRF_SAADC_GAIN1_4,
        .reference = NRF_SAADC_REFERENCE_VDD4,
        .acq_time = NRF_SAADC_ACQTIME_3US,
        .mode = NRF_SAADC_MODE_SINGLE_ENDED,
        .burst = NRF_SAADC_BURST_DISABLED
    };

    nrf_saadc_resolution_set(NRF_SAADC, NRF_SAADC_RESOLUTION_14BIT);
    nrf_saadc_oversample_set(NRF_SAADC, NRF_SAADC_OVERSAMPLE_DISABLED);
    nrf_saadc_enable(NRF_SAADC);

    for (uint32_t i = 0; i < SAADC_CH_NUM; i++) {
        nrf_saadc_channel_input_set(NRF_SAADC, i, NRF_SAADC_INPUT_DISABLED, NRF_SAADC_INPUT_DISABLED);
    }

    nrf_saadc_channel_init(NRF_SAADC, CHANNEL_NO, &config);
    nrf_saadc_channel_input_set(NRF_SAADC, CHANNEL_NO, self->pin->adc_channel, self->pin->adc_channel);
    nrf_saadc_buffer_init(NRF_SAADC, (nrf_saadc_value_t *)&value, 1);

    // Clear stale events before triggering anything. EVENTS_END in particular is
    // left set by the *previous* call's TASKS_STOP, and by the offset calibration
    // in analogin_init(). If it is still set when we get to the wait below, that
    // wait falls through immediately, TASKS_STOP then aborts the conversion that
    // has only just started, and EasyDMA never writes the buffer: RESULT.AMOUNT
    // stays 0 and `value` keeps whatever it was. That is the "AnalogIn.value is
    // always zero" failure, and it is self-perpetuating once the first stale
    // EVENTS_END appears.
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_STARTED);
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_END);
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_DONE);
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_RESULTDONE);
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_STOPPED);

    nrf_saadc_task_trigger(NRF_SAADC, NRF_SAADC_TASK_START);
    while (nrf_saadc_event_check(NRF_SAADC, NRF_SAADC_EVENT_STARTED) == 0) {
        ;
    }
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_STARTED);

    nrf_saadc_task_trigger(NRF_SAADC, NRF_SAADC_TASK_SAMPLE);
    while (nrf_saadc_event_check(NRF_SAADC, NRF_SAADC_EVENT_END) == 0) {
        ;
    }
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_END);

    nrf_saadc_task_trigger(NRF_SAADC, NRF_SAADC_TASK_STOP);
    while (nrf_saadc_event_check(NRF_SAADC, NRF_SAADC_EVENT_STOPPED) == 0) {
        ;
    }
    nrf_saadc_event_clear(NRF_SAADC, NRF_SAADC_EVENT_STOPPED);

    nrf_saadc_disable(NRF_SAADC);

    // Belt and braces alongside the `volatile` above; costs nothing to keep.
    asm volatile ("" : : : "memory");

    // Disconnect ADC from pin.
    nrf_saadc_channel_input_set(NRF_SAADC, CHANNEL_NO, NRF_SAADC_INPUT_DISABLED, NRF_SAADC_INPUT_DISABLED);

    // Read the DMA'd result exactly once.
    int32_t result = value;

    // value is signed and might be (slightly) < 0, even on single-ended conversions, so force to 0.
    if (result < 0) {
        result = 0;
    }

    // Stretch 14-bit ADC reading to 16-bit range
    return (result << 2) | (result >> 12);
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    // The nominal VCC voltage
    return 3.3f;
}
