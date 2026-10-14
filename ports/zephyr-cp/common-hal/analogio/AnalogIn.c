// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/analogio/AnalogIn.h"
#include "shared-bindings/microcontroller/Pin.h"

#include <stdint.h>

#include <iobroker/iobroker.h>
#include <zephyr/drivers/adc.h>

#include "bindings/zephyr_kernel/__init__.h"
#include "py/runtime.h"

// Zephyr has no API to query an ADC's native resolution: adc_sequence.resolution
// is set by the caller and drivers reject values they can't meet (from probe
// time, before any conversion starts). Ask the driver for the highest
// resolution it accepts with a descending ladder of one-read probes. The
// emulated ADC answers 16; the SAADC only accepts 8/10/12/14 and the Renesas
// RA ADC its fixed hardware resolution.
static const uint8_t adc_resolutions[] = {16, 14, 12, 10, 8};

// Every (reference, gain) pair over the VDD-derived references with gain
// cancelling the divider, i.e. full scale = reference / gain = VDD, which is
// what the 16-bit analogio scaling contract assumes. Drivers only accept the
// combinations their hardware supports (the nRF52 family SAADC has no full-VDD
// reference and lands on VDD/4); probe down the ladder at construct time.
static const struct {
    enum adc_reference reference;
    enum adc_gain gain;
} adc_railings[] = {
    {ADC_REF_VDD_1, ADC_GAIN_1},
    {ADC_REF_VDD_1_2, ADC_GAIN_1_2},
    {ADC_REF_VDD_1_3, ADC_GAIN_1_3},
    {ADC_REF_VDD_1_4, ADC_GAIN_1_4},
};

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self, const mcu_pin_obj_t *pin) {
    // Allocate the pad's analog input through the iobroker module: the call
    // claims the pad so that bus and GPIO allocations refuse it, resolves the
    // fixed analog input the pad is bonded to (identity on the emulated ADC)
    // and hands out a free channel slot on the ADC device.
    const struct device *adc;
    uint8_t channel;
    uint8_t input;
    int res = iobroker_adc_allocate(pin->package_pin, &adc, &channel, &input);
    if (res == -ENOSYS) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_AnalogIn);
    }
    if (res == -EINVAL) {
        // The pad exists but isn't bonded to an analog input. Match the other
        // ports, which raise ValueError("Invalid pin") for such pins.
        raise_ValueError_invalid_pin();
    }
    if (res < 0) {
        raise_zephyr_error(res);
    }

    if (!device_is_ready(adc)) {
        iobroker_adc_release(adc, channel);
        raise_zephyr_error(-ENODEV);
    }

    // Sample against the analog supply, like the other ports (SAMD's
    // INTVCC1, RP2040's VREF pin, STM32/Espressif's supply-referenced
    // ranges): full scale is VDD/VDDA, so a full-rail input reads 65535.
    struct adc_channel_cfg channel_cfg = {
        .acquisition_time = ADC_ACQ_TIME_DEFAULT,
        .channel_id = channel,
    };
    #ifdef CONFIG_ADC_CONFIGURABLE_INPUTS
    channel_cfg.input_positive = input;
    #endif

    int err = 0;
    for (uint8_t i = 0; i < ARRAY_SIZE(adc_railings); i++) {
        channel_cfg.reference = adc_railings[i].reference;
        channel_cfg.gain = adc_railings[i].gain;
        err = adc_channel_setup(adc, &channel_cfg);
        if (err != -EINVAL) {
            break;
        }
    }
    if (err != 0) {
        iobroker_adc_release(adc, channel);
        raise_zephyr_error(err);
    }

    // Do dummy reads to determine what resolution the device actually support.
    uint16_t probe = 0;
    uint8_t resolution = 0;
    for (uint8_t i = 0; i < ARRAY_SIZE(adc_resolutions); i++) {
        struct adc_sequence probe_sequence = {
            .channels = BIT(channel),
            .buffer = &probe,
            .buffer_size = sizeof(probe),
            .resolution = adc_resolutions[i],
        };
        err = adc_read(adc, &probe_sequence);
        if (err == 0) {
            resolution = adc_resolutions[i];
            break;
        }
        if (err != -EINVAL && err != -ENOTSUP) {
            // Not a resolution rejection (drivers report those before sampling):
            // surface driver errors like -EBUSY rather than skipping them.
            iobroker_adc_release(adc, channel);
            raise_zephyr_error(err);
        }
    }
    if (resolution == 0) {
        iobroker_adc_release(adc, channel);
        raise_zephyr_error(-ENOTSUP);
    }

    self->pin = pin;
    self->adc = adc;
    self->channel_id = channel;
    self->resolution = resolution;
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return self->adc == NULL;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        return;
    }

    (void)iobroker_adc_release(self->adc, self->channel_id);
    self->pin = NULL;
    self->adc = NULL;
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    // Sample at the ADC's full hardware resolution (probed at construct) and
    // stretch the raw code to CircuitPython's 16-bit contract relative to the
    // reference voltage, which is what the analogio contract is.
    uint16_t raw = 0;
    struct adc_sequence sequence = {
        .channels = BIT(self->channel_id),
        .buffer = &raw,
        .buffer_size = sizeof(raw),
        .resolution = self->resolution,
    };

    int err = adc_read(self->adc, &sequence);
    if (err != 0) {
        raise_zephyr_error(err);
    }

    if (self->resolution >= 16) {
        return raw;
    }

    // Mirror the top 16 - resolution bits into the bottom: for a 14-bit read,
    // (value << 2) | (value >> 12) (atmel-samd does the same for its 12-bit
    // reads). raw << shift fits in 16 bits because raw is at most resolution
    // bits wide. Zero and full scale map to themselves, unlike scaling by
    // 65535 / max_raw which bunches codes near zero.
    const uint8_t shift = 16 - self->resolution;
    return (uint16_t)((raw << shift) | (raw >> (self->resolution - shift)));
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    // Hard-coded until the reference-voltage query API (upstream adc_ref_get())
    // is available; every other port also reports the nominal analog supply.
    return 3.3f;
}
