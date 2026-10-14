// Renesas RA DAC pad allocation for the iobroker module.
//
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT
//
// The DAC output pads are hardwired: each pad bonds at most one DA output and
// there is no runtime routing (unlike the digital peripherals' PSEL).
// Allocate resolves the pad's DA channel from the SoC's pin functions table
// (the datasheet's "Register settings for input/output pin function" tables,
// e.g. RA8M1/RA8D1 table 19.6: AN007/DA0 = P0.14, AN105/DA1 = P0.15), applies
// the analog switch and claims the pad alongside the channel slot.
//
// The analog switch is the PmnPFS.ASEL bit, applied through the datasheet's
// "Notes on Using Analog Functions" sequence: PMR and PDR 0 so the pad acts
// as a general input, then ASEL 1. The Zephyr pinctrl shim writes the whole
// PmnPFS bundle in one go (with the R_BSP_PinAccessEnable protection wrap),
// so a config of only the ASEL bit implements the sequence.
//
// ADC inputs on RA SoCs are not implemented here yet: resolving a pad to the
// FSP adc_channel_t id and the right unit needs the per-SoC AN0xx/AN1xx
// channel numbering checked against a second datasheet source, so
// iobroker_adc_allocate() stays on the core's -ENOSYS stub for RA.

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include <soc.h>

#include "iobroker_internal.h"

LOG_MODULE_DECLARE(iobroker, CONFIG_LOG_DEFAULT_LEVEL);

#if IOBROKER_DAC && !DT_HAS_COMPAT_STATUS_OKAY(renesas_ra_dac)
#error "iobroker: DAC support built but no renesas,ra-dac node is okay"
#endif

#if defined(CONFIG_SOC_SERIES_RA8M1) || defined(CONFIG_SOC_SERIES_RA8D1)
// DAC12 channel 0 is bonded to P0.14 (AN007/DA0 in table 19.6); channel 1 to
// P0.15 (AN105/DA1). Zephyr's renesas_ra_dac driver only supports channel 0:
// both the write and the setup reject any other channel id, so only P0.14
// resolves. Add the DAx -> pad mapping from the datasheet for new SoCs.
#define RA_DAC_OUTPUT_PAD 14
#else
// TODO: add the DAx -> pad mapping from the datasheet for this SoC.
#define RA_DAC_OUTPUT_PAD 0xffff
#endif

// The pinctrl shim holds struct ra_pinctrl_soc_pin { port_num: 4, pin_num: 4,
// cfg } (see zephyr/soc/renesas/ra/common/pinctrl_soc.h); cfg sets only ASEL,
// which is what the analog sequence asks for.
static void ra_analog_enable(uint16_t soc_pad) {
    struct ra_pinctrl_soc_pin cfg = {
        .port_num = (soc_pad / 32U),
        .pin_num = (soc_pad % 32U),
        .cfg = (1u << R_PFS_PORT_PIN_PmnPFS_ASEL_Pos),
    };

    pinctrl_configure_pins(&cfg, 1, PINCTRL_REG_NONE);
    LOG_DBG("dac allocate: pad %u (port %u pin %u) analog switch applied",
        (unsigned)soc_pad, (unsigned)cfg.port_num, (unsigned)cfg.pin_num);
}

#if IOBROKER_DAC

int iobroker_dac_allocate(package_pin_t pin, const struct device **dev_out,
    uint8_t *channel_out, uint8_t *input_out) {
    uint16_t soc_pad;
    int ret = iobroker_package_pin_soc_pad(pin, &soc_pad);
    if (ret < 0) {
        LOG_WRN("dac allocate: package pin %u not in package map",
            (unsigned)pin);
        return ret;
    }

    if (soc_pad != RA_DAC_OUTPUT_PAD) {
        LOG_WRN("dac allocate: pad %u is not a DAC output", (unsigned)soc_pad);
        return -EINVAL;
    }

    const struct device *dev = DEVICE_DT_GET_ANY(renesas_ra_dac);
    if (dev == NULL || !device_is_ready(dev)) {
        LOG_WRN("dac allocate: no ready DAC device");
        return -ENODEV;
    }

    ret = iobroker_analog_claim_add(pin, dev, 0);
    if (ret < 0) {
        return ret;
    }

    ra_analog_enable(soc_pad);

    *dev_out = dev;
    *channel_out = 0;
    // The device has no selectable inputs; the caller ignores this when it
    // configures the channel.
    *input_out = 0;
    LOG_INF("dac allocate: package pin %u (pad %u) -> %s channel 0",
        (unsigned)pin, (unsigned)soc_pad, dev->name);
    return 0;
}

bool iobroker_dac_release(const struct device *dev, uint8_t channel) {
    uint16_t soc_pad;
    if (!iobroker_analog_claim_remove(dev, channel, &soc_pad)) {
        LOG_DBG("dac release: %s channel %u not allocated",
            dev == NULL ? "(null)" : dev->name, (unsigned)channel);
        return false;
    }

    // Zephyr's DAC API has no channel release, and the renesas_ra_dac driver
    // only closes the device from the next channel_setup, so there is no
    // channel to unconfigure. The claim is dropped and the pad's analog
    // switch stays applied until a later GPIO allocation rewires the pad.
    LOG_INF("dac release: %s channel %u released", dev->name,
        (unsigned)channel);
    return true;
}

#endif // IOBROKER_DAC
