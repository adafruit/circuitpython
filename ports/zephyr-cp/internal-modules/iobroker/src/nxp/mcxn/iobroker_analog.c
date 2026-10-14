// NXP MCX N LPADC/LPDAC analog pad allocation for the iobroker module.
//
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// The LPADC inputs and LPDAC outputs are bonded to pads: each pad exposes at
// most one analog function per unit (a few pads feed both LPADC units and the
// DAC output) and there is no runtime analog routing. Allocate resolves the
// pad's analog function from the SoC's pin functions table and claims the pad.
//
// ADC: allocate additionally hands out a free LPADC command-channel slot (the
// Zephyr channel_id indexes the CMD registers; the number is
// CONFIG_LPADC_CHANNEL_COUNT). The hardware input comes from input_positive,
// which Zephyr encodes as the channel number in bits 0..4 and the B-side mux
// in bit 5 (the driver writes ADCH = input & 0x1f and picks the sample mode's
// side from bit 5). The same channel slot is handed back at release; the
// LPADC driver has no persistent per-channel state (commands are set up per
// adc_read()), so a release only frees the slot.
//
// DAC: the LPDAC has a single output channel, so channel 0 is always the
// slot; values are 12-bit (scaled down by analogout before the write).
//
// Neither needs pin activity: the analog functions are bonded at mux 0 like
// every other analog function on MCX N (see
// modules/hal/nxp/dts/nxp/mcx/MCXN947VDF-pinctrl.h, e.g. ADC0_A2_PIO4_23 and
// DAC0_OUT_PIO4_2), so the quiesced pad exposes the analog signal once the
// driver has configured the peripheral.

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "iobroker_internal.h"

LOG_MODULE_DECLARE(iobroker, CONFIG_LOG_DEFAULT_LEVEL);

#if IOBROKER_DAC && !DT_HAS_COMPAT_STATUS_OKAY(nxp_lpdac)
#error "iobroker: DAC support built but no nxp,lpdac node is okay"
#endif

#if IOBROKER_ADC

// Zephyr's io-channel encoding of one LPADC input for the lpadc driver:
// ADCH channel number in the low 5 bits, bit 5 set for a B-side input (the
// driver reads it as input_positive and maps it onto the command's channel
// number and sample-side mode).
#define LPADC_INPUT_A(ch) (ch)
#define LPADC_INPUT_B(ch) (0x20U | (ch))

#if defined(CONFIG_SOC_MCXN947)

// The package pads each LPADC input is bonded to, transcribed from the
// MCXN947 VFBGA184 pin functions (modules/hal/nxp/dts/nxp/mcx/
// MCXN947VDF-pinctrl.h, entries ADCn_Xy_PIOx_y: unit n, side X (A/B),
// channel y), keyed to the package pad ids the package numbers its pins
// by (mcxn947_vdf.toml: row-major ball order with power balls leaving
// gaps, per the package Pinout workbook; the analog-only ANA_x pads are
// carried by name; e.g. ANA_0 = ball P3 = pad 139, P4_23 = ball U12 =
// pad 181). Entries are ordered by pad; a pad can bond to more than one
// input (across the two units and the mux sides), and the first entry
// wins with the single-ended A-side input preferred.
//
// Add the mapping from the SoC's pin function table for other MCX SoCs; a
// SoC whose table is empty reports -ENOSYS.
struct analog_input {
    uint16_t pad;
    uint8_t unit;
    uint8_t input;
};

static const struct analog_input analog_inputs[] = {
    // Ball-id pads from the VFBGA184 ball map (mcxn947_vdf.toml): the
    // grid's row-major numbering with power balls leaving gaps. The
    // analog-only pads carry ANA_x names (ANA_0 = ball P3 = pad 139).
    {1, 1, LPADC_INPUT_A(8)},
    {2, 0, LPADC_INPUT_A(23)},
    {3, 0, LPADC_INPUT_A(20)},
    {4, 0, LPADC_INPUT_B(17)},
    {5, 0, LPADC_INPUT_A(13)},
    {6, 0, LPADC_INPUT_A(9)},
    {7, 0, LPADC_INPUT_B(9)},
    {11, 1, LPADC_INPUT_A(9)},
    {12, 0, LPADC_INPUT_A(22)},
    {13, 0, LPADC_INPUT_A(21)},
    {14, 0, LPADC_INPUT_A(19)},
    {15, 0, LPADC_INPUT_B(16)},
    {16, 0, LPADC_INPUT_A(15)},
    {17, 0, LPADC_INPUT_A(14)},
    {18, 0, LPADC_INPUT_A(8)},
    {19, 0, LPADC_INPUT_B(11)},
    {20, 0, LPADC_INPUT_B(10)},
    {25, 1, LPADC_INPUT_A(10)},
    {26, 0, LPADC_INPUT_A(18)},
    {27, 0, LPADC_INPUT_A(17)},
    {28, 0, LPADC_INPUT_A(16)},
    {29, 0, LPADC_INPUT_A(12)},
    {30, 0, LPADC_INPUT_A(11)},
    {31, 0, LPADC_INPUT_A(10)},
    {32, 0, LPADC_INPUT_B(8)},
    {36, 1, LPADC_INPUT_A(13)},
    {37, 1, LPADC_INPUT_A(12)},
    {38, 1, LPADC_INPUT_A(11)},
    {39, 1, LPADC_INPUT_A(14)},
    {41, 0, LPADC_INPUT_B(23)},
    {43, 0, LPADC_INPUT_B(12)},
    {49, 1, LPADC_INPUT_A(15)},
    {51, 0, LPADC_INPUT_B(22)},
    {52, 0, LPADC_INPUT_B(20)},
    {53, 0, LPADC_INPUT_B(19)},
    {54, 0, LPADC_INPUT_B(14)},
    {60, 1, LPADC_INPUT_A(17)},
    {61, 1, LPADC_INPUT_A(16)},
    {62, 0, LPADC_INPUT_B(21)},
    {63, 0, LPADC_INPUT_B(18)},
    {64, 0, LPADC_INPUT_B(13)},
    {70, 1, LPADC_INPUT_A(18)},
    {71, 1, LPADC_INPUT_A(19)},
    {74, 0, LPADC_INPUT_B(15)},
    {99, 1, LPADC_INPUT_A(20)},
    {104, 1, LPADC_INPUT_B(13)},
    {105, 1, LPADC_INPUT_B(14)},
    {110, 1, LPADC_INPUT_A(22)},
    {111, 1, LPADC_INPUT_A(21)},
    {114, 1, LPADC_INPUT_B(15)},
    {115, 1, LPADC_INPUT_B(16)},
    {120, 1, LPADC_INPUT_A(23)},
    {123, 1, LPADC_INPUT_B(10)},
    {124, 1, LPADC_INPUT_B(12)},
    {125, 1, LPADC_INPUT_B(17)},
    {134, 1, LPADC_INPUT_B(11)},
    {139, 0, LPADC_INPUT_A(0)},
    {150, 0, LPADC_INPUT_B(0)},
    {154, 0, LPADC_INPUT_A(6)},
    {155, 0, LPADC_INPUT_B(6)},
    {156, 0, LPADC_INPUT_B(1)},
    {161, 0, LPADC_INPUT_A(4)},
    {161, 1, LPADC_INPUT_A(4)},
    {162, 1, LPADC_INPUT_A(0)},
    {163, 1, LPADC_INPUT_B(0)},
    {165, 0, LPADC_INPUT_A(5)},
    {165, 1, LPADC_INPUT_A(5)},
    {166, 0, LPADC_INPUT_B(5)},
    {166, 1, LPADC_INPUT_B(5)},
    {167, 0, LPADC_INPUT_A(1)},
    {168, 1, LPADC_INPUT_A(6)},
    {169, 1, LPADC_INPUT_B(6)},
    {175, 0, LPADC_INPUT_B(4)},
    {175, 1, LPADC_INPUT_B(4)},
    {181, 0, LPADC_INPUT_A(2)},
    {181, 1, LPADC_INPUT_B(3)},
    {183, 1, LPADC_INPUT_B(8)},
    {184, 1, LPADC_INPUT_B(9)},
};
#define ANALOG_INPUT_COUNT ARRAY_SIZE(analog_inputs)

// The two LPADC units, in devicetree lpadc0/lpadc1 label order; NULL when
// the unit is not enabled or the SoC has no such node.
static const struct device *const analog_adc_devices[2] = {
    #if DT_NODE_EXISTS(DT_NODELABEL(lpadc0)) && \
    DT_NODE_HAS_STATUS(DT_NODELABEL(lpadc0), okay)
    DEVICE_DT_GET(DT_NODELABEL(lpadc0)),
    #else
    NULL,
    #endif
    #if DT_NODE_EXISTS(DT_NODELABEL(lpadc1)) && \
    DT_NODE_HAS_STATUS(DT_NODELABEL(lpadc1), okay)
    DEVICE_DT_GET(DT_NODELABEL(lpadc1)),
    #else
    NULL,
    #endif
};

static uint8_t analog_adc_channel_count(const struct device *dev) {
    (void)dev;
    // One command-channel slot per LPADC CMD register.
    return CONFIG_LPADC_CHANNEL_COUNT;
}

#else
// TODO: add the LPADC input -> pad mapping from this SoC's pin function
// table (see CONFIG_SOC_MCXN947 above).
#define ANALOG_INPUT_COUNT 0

static const struct device *const analog_adc_devices[2] = {NULL, NULL};

static uint8_t analog_adc_channel_count(const struct device *dev) {
    (void)dev;
    return 0;
}
#endif // CONFIG_SOC_MCXN947

int iobroker_adc_allocate(package_pin_t pin, const struct device **dev_out,
    uint8_t *channel_out, uint8_t *input_out) {
    if (ANALOG_INPUT_COUNT == 0) {
        LOG_DBG("adc allocate: no LPADC input table for this SoC");
        return -ENOSYS;
    }

    uint16_t soc_pad;
    int ret = iobroker_package_pin_soc_pad(pin, &soc_pad);
    if (ret < 0) {
        LOG_WRN("adc allocate: package pin %u not in package map",
            (unsigned)pin);
        return ret;
    }

    // Find the analog input the pad is bonded to.
    const struct analog_input *found = NULL;
    for (size_t i = 0; i < ANALOG_INPUT_COUNT; i++) {
        if (analog_inputs[i].pad == soc_pad) {
            found = &analog_inputs[i];
            break;
        }
    }
    if (found == NULL) {
        LOG_WRN("adc allocate: pad %u has no analog input", (unsigned)soc_pad);
        return -EINVAL;
    }
    const struct device *dev = analog_adc_devices[found->unit];
    if (dev == NULL || !device_is_ready(dev)) {
        LOG_WRN("adc allocate: no ready LPADC unit %u", (unsigned)found->unit);
        return -ENODEV;
    }

    // Hand out a free command-channel slot.
    uint8_t count = analog_adc_channel_count(dev);
    uint8_t channel = count;
    for (uint8_t i = 0; i < count; i++) {
        if (!iobroker_analog_channel_in_use(dev, i)) {
            channel = i;
            break;
        }
    }
    if (channel == count) {
        LOG_WRN("adc allocate: %s has no free channel", dev->name);
        return -ENOMEM;
    }

    ret = iobroker_analog_claim_add(pin, dev, channel);
    if (ret < 0) {
        return ret;
    }

    // The pad's digital side is quiesced; the analog input is a bond, not a
    // mux setting beyond mux 0, so nothing else to apply until the caller
    // configures the channel.
    iobroker_gpio_pad_quiesce(soc_pad);

    *dev_out = dev;
    *channel_out = channel;
    *input_out = found->input;
    LOG_INF("adc allocate: package pin %u (pad %u) -> %s channel %u, LPADC%u "
        "input 0x%02x",
        (unsigned)pin, (unsigned)soc_pad, dev->name, (unsigned)channel,
        (unsigned)found->unit, (unsigned)found->input);
    return 0;
}

bool iobroker_adc_release(const struct device *dev, uint8_t channel) {
    uint16_t soc_pad;
    if (!iobroker_analog_claim_remove(dev, channel, &soc_pad)) {
        LOG_DBG("adc release: %s channel %u not allocated",
            dev == NULL ? "(null)" : dev->name, (unsigned)channel);
        return false;
    }

    // The LPADC driver keeps no persistent per-channel configuration: the
    // command registers are rewritten by every adc_read() call, so freeing
    // the slot needs no device activity.
    iobroker_gpio_pad_quiesce(soc_pad);
    LOG_INF("adc release: %s channel %u released", dev->name,
        (unsigned)channel);
    return true;
}

#endif // IOBROKER_ADC

#if IOBROKER_DAC

// Package pad (gpio port index * 32 + pin within the port) bonded to the
// LPDAC outputs (MCXN947 pin functions: DAC0_OUT on P4_02, DAC1_OUT on
// P4_03), as the package map's ball ids (mcxn947_vdf.toml: P4_02 = ball
// T1 = pad 161, P4_03 = ball U1 = pad 175).
#define DAC0_OUTPUT_PAD 161
#define DAC1_OUTPUT_PAD 175

#if DT_NODE_EXISTS(DT_NODELABEL(dac0)) && \
    DT_NODE_HAS_STATUS(DT_NODELABEL(dac0), okay)
#define ANALOG_DAC_DEVICE DEVICE_DT_GET(DT_NODELABEL(dac0))
#define ANALOG_DAC_OUTPUT_PAD DAC0_OUTPUT_PAD
#elif DT_NODE_EXISTS(DT_NODELABEL(dac1)) && \
    DT_NODE_HAS_STATUS(DT_NODELABEL(dac1), okay)
#define ANALOG_DAC_DEVICE DEVICE_DT_GET(DT_NODELABEL(dac1))
#define ANALOG_DAC_OUTPUT_PAD DAC1_OUTPUT_PAD
#endif

#ifdef ANALOG_DAC_DEVICE
int iobroker_dac_allocate(package_pin_t pin, const struct device **dev_out,
    uint8_t *channel_out, uint8_t *input_out) {
    uint16_t soc_pad;
    int ret = iobroker_package_pin_soc_pad(pin, &soc_pad);
    if (ret < 0) {
        LOG_WRN("dac allocate: package pin %u not in package map",
            (unsigned)pin);
        return ret;
    }

    if (soc_pad != ANALOG_DAC_OUTPUT_PAD) {
        LOG_WRN("dac allocate: pad %u is not a DAC output", (unsigned)soc_pad);
        return -EINVAL;
    }

    const struct device *dev = ANALOG_DAC_DEVICE;
    if (dev == NULL || !device_is_ready(dev)) {
        LOG_WRN("dac allocate: no ready DAC device");
        return -ENODEV;
    }

    // The LPDAC has one output channel.
    if (iobroker_analog_channel_in_use(dev, 0)) {
        LOG_WRN("dac allocate: %s has no free channel", dev->name);
        return -ENOMEM;
    }

    ret = iobroker_analog_claim_add(pin, dev, 0);
    if (ret < 0) {
        return ret;
    }

    iobroker_gpio_pad_quiesce(soc_pad);

    *dev_out = dev;
    *channel_out = 0;
    // The device takes no input selector; the caller ignores this when it
    // configures the channel.
    *input_out = 0;
    LOG_INF("dac allocate: package pin %u (pad %u) -> %s channel 0",
        (unsigned)pin, (unsigned)soc_pad, dev->name);
    return 0;
}
#else
int iobroker_dac_allocate(package_pin_t pin, const struct device **dev_out,
    uint8_t *channel_out, uint8_t *input_out) {
    (void)pin;
    (void)dev_out;
    (void)channel_out;
    (void)input_out;
    LOG_DBG("dac allocate: no enabled nxp,lpdac instance");
    return -ENOSYS;
}
#endif

bool iobroker_dac_release(const struct device *dev, uint8_t channel) {
    uint16_t soc_pad;
    if (!iobroker_analog_claim_remove(dev, channel, &soc_pad)) {
        LOG_DBG("dac release: %s channel %u not allocated",
            dev == NULL ? "(null)" : dev->name, (unsigned)channel);
        return false;
    }

    // Zephyr's DAC API has no channel release: the write value stays applied
    // until the next channel_setup reconfigures the device. The claim is
    // dropped and the pad is quiesced.
    iobroker_gpio_pad_quiesce(soc_pad);
    LOG_INF("dac release: %s channel %u released", dev->name,
        (unsigned)channel);
    return true;
}

#endif // IOBROKER_DAC
