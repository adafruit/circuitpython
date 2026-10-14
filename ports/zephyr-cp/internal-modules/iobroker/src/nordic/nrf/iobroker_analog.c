// nRF analog pad allocation for the iobroker module.
//
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// The SAADC's analog inputs are hardwired to pads: each pad exposes at most
// one AIN input and there is no runtime routing (unlike the digital
// peripherals' PSEL). Allocate therefore resolves the pad's AIN number from
// the SoC's pin functions table, claims the pad and hands out a free SAADC
// channel slot (the software channels are a runtime resource; Zephyr's ADC
// API has no channel release, so release unconfigures the slot through
// nrfx_saadc_channels_deconfig()).
//
// No DAC peripheral driver exists in Zephyr for nRF SoCs, so
// iobroker_dac_allocate() reports -ENOSYS.

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include <nrfx_saadc.h>
#include <soc.h>

#include "iobroker_internal.h"

LOG_MODULE_DECLARE(iobroker, CONFIG_LOG_DEFAULT_LEVEL);

#if !DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf_saadc)
// SAADC not enabled in the devicetree: the symbols must still exist, but
// every allocate reports -ENOSYS.
#define ANALOG_INPUT_PADS NULL
#define ANALOG_INPUT_COUNT 0
static const struct device *analog_input_device(void) {
    return NULL;
}
static uint8_t analog_input_channel_count(void) {
    return 0;
}
#else

// Package pad (the SoC package's ball id number, in row-major ball order)
// that each SAADC analog input is bonded to, in AIN number order. From the
// pin tables of the packages/ transcriptions (the packages' gpio column
// names the GPIO pad each ball id bonds to; e.g. aQFN73 ball id 3 = P0.02
// = AIN0). SoCs without a transcribed table report -ENOSYS; add the
// mapping through the SoC's package pin map.
#if defined(NRF52840_XXAA) || defined(NRF52833_XXAA) || defined(NRF52832_XXAA) || \
    defined(NRF52820_XXAA) || defined(NRF52811_XXAA) || defined(NRF52810_XXAA) || \
    defined(NRF52805_XXAA)
// nRF52 series: AIN0..AIN7 are P0.02..P0.05 and P0.28..P0.31, ball ids from
// nrf52840_aqfn73.toml (ball grid packages remap them per package; the
// module maps share the aQFN73 balls).
static const uint16_t analog_input_pads[] = { 3, 16, 30, 32, 15, 2, 14, 1 };
#define ANALOG_INPUT_PADS analog_input_pads
#define ANALOG_INPUT_COUNT ARRAY_SIZE(analog_input_pads)
#elif defined(NRF5340_XXAA_APPLICATION)
// nRF5340 application core: AIN0..AIN7 are P0.13..P0.20, ball ids from
// nrf5340_qkaa.toml.
static const uint16_t analog_input_pads[] = { 82, 69, 70, 84, 71, 72, 86, 73 };
#define ANALOG_INPUT_PADS analog_input_pads
#define ANALOG_INPUT_COUNT ARRAY_SIZE(analog_input_pads)
#else
// TODO: add the AIN -> ball id mapping from the SoC's package pin map for
// this SoC.
#define ANALOG_INPUT_PADS NULL
#define ANALOG_INPUT_COUNT 0
#endif

static const struct device *analog_input_device(void) {
    return DEVICE_DT_GET_ANY(nordic_nrf_saadc);
}

static uint8_t analog_input_channel_count(void) {
    // One software channel slot per hardware SAADC channel.
    return SAADC_CH_NUM;
}

#endif // DT_HAS_COMPAT_STATUS_OKAY(nordic_nrf_saadc)

int iobroker_adc_allocate(package_pin_t pin, const struct device **dev_out,
    uint8_t *channel_out, uint8_t *input_out) {
    if (ANALOG_INPUT_COUNT == 0) {
        LOG_DBG("analog allocate: no AIN pad table for this SoC");
        return -ENOSYS;
    }

    uint16_t soc_pad;
    int ret = iobroker_package_pin_soc_pad(pin, &soc_pad);
    if (ret < 0) {
        LOG_WRN("analog allocate: package pin %u not in package map",
            (unsigned)pin);
        return ret;
    }

    // Find the analog input the pad is bonded to.
    uint8_t input = 0;
    bool found = false;
    for (uint8_t i = 0; i < ANALOG_INPUT_COUNT; i++) {
        if (ANALOG_INPUT_PADS[i] == soc_pad) {
            input = i;
            found = true;
            break;
        }
    }
    if (!found) {
        LOG_WRN("analog allocate: pad %u has no analog input", (unsigned)soc_pad);
        return -EINVAL;
    }

    const struct device *dev = analog_input_device();
    if (dev == NULL || !device_is_ready(dev)) {
        LOG_WRN("analog allocate: no ready SAADC device");
        return -ENODEV;
    }

    // Hand out a free channel slot.
    uint8_t count = analog_input_channel_count();
    uint8_t channel = count;
    for (uint8_t i = 0; i < count; i++) {
        if (!iobroker_analog_channel_in_use(dev, i)) {
            channel = i;
            break;
        }
    }
    if (channel == count) {
        LOG_WRN("analog allocate: %s has no free channel", dev->name);
        return -ENOMEM;
    }

    ret = iobroker_analog_claim_add(pin, dev, channel);
    if (ret < 0) {
        return ret;
    }

    // The pad's digital side is quiesced: no routing, no input buffer, no
    // pull. The analog input is an internal mux setting, so nothing else to
    // apply until the caller configures the channel.
    iobroker_gpio_pad_quiesce(soc_pad);

    *dev_out = dev;
    *channel_out = channel;
    *input_out = input;
    LOG_INF("adc allocate: package pin %u (pad %u) -> %s channel %u, AIN%u",
        (unsigned)pin, (unsigned)soc_pad, dev->name, (unsigned)channel,
        (unsigned)input);
    return 0;
}

bool iobroker_adc_release(const struct device *dev, uint8_t channel) {
    uint16_t soc_pad;
    if (!iobroker_analog_claim_remove(dev, channel, &soc_pad)) {
        LOG_DBG("adc release: %s channel %u not allocated",
            dev == NULL ? "(null)" : dev->name, (unsigned)channel);
        return false;
    }

    // Unconfigure the channel so the slot becomes usable again; Zephyr's ADC
    // API has no channel release, so this goes through the nrfx HAL the
    // Zephyr driver is built on (0 on success, negative errno on failure).
    int ret = nrfx_saadc_channels_deconfig(BIT(channel));
    if (ret != 0) {
        LOG_WRN("adc release: deconfig of %s channel %u failed: %d",
            dev->name, (unsigned)channel, ret);
    }

    iobroker_gpio_pad_quiesce(soc_pad);
    LOG_INF("adc release: %s channel %u released", dev->name,
        (unsigned)channel);
    return true;
}

// No DAC driver exists in Zephyr for nRF SoCs, so IOBROKER_DAC is always 0
// here and the core's -ENOSYS stub covers iobroker_dac_allocate().
