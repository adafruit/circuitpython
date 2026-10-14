// Emulated analog pad allocation for the iobroker module (native_sim).
//
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// The emulated ADC (zephyr,adc-emul) and the test DAC (vnd,dac) have no
// analog mux: the emulated channel a pad feeds is the pad number itself, and
// the pads are plain GPIO controller pads on the one-to-one package map
// native_sim selects. So allocate resolves the channel from the pad's global
// number and claims the pad; release only drops the claim (the emulated
// devices need no channel teardown).

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/dac.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "iobroker_internal.h"

LOG_MODULE_DECLARE(iobroker, CONFIG_LOG_DEFAULT_LEVEL);

// Emulated devices: one instance each. Additional instances are ignored
// (there are none in practice on native_sim).

#if IOBROKER_ADC
#if defined(CONFIG_ADC_EMUL) && DT_HAS_COMPAT_STATUS_OKAY(zephyr_adc_emul)
#define ANALOG_ADC_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_adc_emul)
static const struct device *analog_adc_device(void) {
    return DEVICE_DT_GET(ANALOG_ADC_NODE);
}

static uint8_t analog_adc_channel_count(void) {
    return DT_PROP(ANALOG_ADC_NODE, nchannels);
}
#else
static const struct device *analog_adc_device(void) {
    return NULL;
}

static uint8_t analog_adc_channel_count(void) {
    return 0;
}
#endif
#endif // IOBROKER_ADC

#if IOBROKER_DAC
#if defined(CONFIG_DAC_TEST) && DT_HAS_COMPAT_STATUS_OKAY(vnd_dac)
#define ANALOG_DAC_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(vnd_dac)
static const struct device *analog_dac_device(void) {
    return DEVICE_DT_GET(ANALOG_DAC_NODE);
}

// The test DAC's binding has no channel count; the emulated hardware takes
// any channel id, so the number of channels is not bounded beyond the claim
// registry itself.
#define ANALOG_DAC_CHANNEL_COUNT 8
#else
static const struct device *analog_dac_device(void) {
    return NULL;
}

#define ANALOG_DAC_CHANNEL_COUNT 0
#endif
#endif // IOBROKER_DAC

#if IOBROKER_ADC || IOBROKER_DAC
static int adc_dac_allocate(package_pin_t pin, const struct device *dev,
    uint8_t count, const struct device **dev_out, uint8_t *channel_out,
    uint8_t *input_out, const char *what) {
    if (dev == NULL || count == 0) {
        return -ENOSYS;
    }

    uint16_t soc_pad;
    int ret = iobroker_package_pin_soc_pad(pin, &soc_pad);
    if (ret < 0) {
        LOG_WRN("%s allocate: package pin %u not in package map",
            what, (unsigned)pin);
        return ret;
    }

    // Emulated devices have no analog mux: the channel a pad feeds is the
    // pad's global number itself, while it indexes into the device's
    // channel array.
    if (soc_pad >= count) {
        LOG_WRN("%s allocate: pad %u has no analog channel on %s",
            what, (unsigned)soc_pad, dev->name);
        return -EINVAL;
    }
    *channel_out = (uint8_t)soc_pad;

    ret = iobroker_analog_claim_add(pin, dev, *channel_out);
    if (ret < 0) {
        return ret;
    }

    *dev_out = dev;
    // The emulated devices have no selectable inputs; the caller ignores
    // this when it configures the channel.
    *input_out = 0;
    LOG_INF("%s allocate: package pin %u (pad %u) -> %s channel %u",
        what, (unsigned)pin, (unsigned)soc_pad, dev->name,
        (unsigned)*channel_out);
    return 0;
}

static bool adc_dac_release(const struct device *dev, uint8_t channel,
    const char *what) {
    uint16_t soc_pad;
    if (!iobroker_analog_claim_remove(dev, channel, &soc_pad)) {
        LOG_DBG("%s release: %s channel %u not allocated",
            what, dev == NULL ? "(null)" : dev->name, (unsigned)channel);
        return false;
    }

    iobroker_gpio_pad_quiesce(soc_pad);
    LOG_INF("%s release: %s channel %u released", what, dev->name,
        (unsigned)channel);
    return true;
}

int iobroker_adc_allocate(package_pin_t pin, const struct device **dev_out,
    uint8_t *channel_out, uint8_t *input_out) {
    return adc_dac_allocate(pin, analog_adc_device(), analog_adc_channel_count(),
        dev_out, channel_out, input_out, "adc");
}

bool iobroker_adc_release(const struct device *dev, uint8_t channel) {
    return adc_dac_release(dev, channel, "adc");
}
#endif // IOBROKER_ADC || IOBROKER_DAC

#if IOBROKER_DAC
int iobroker_dac_allocate(package_pin_t pin, const struct device **dev_out,
    uint8_t *channel_out, uint8_t *input_out) {
    return adc_dac_allocate(pin, analog_dac_device(), ANALOG_DAC_CHANNEL_COUNT,
        dev_out, channel_out, input_out, "dac");
}

bool iobroker_dac_release(const struct device *dev, uint8_t channel) {
    return adc_dac_release(dev, channel, "dac");
}
#endif // IOBROKER_DAC
