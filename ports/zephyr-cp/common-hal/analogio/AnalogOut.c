// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/analogio/AnalogOut.h"
#include "shared-bindings/microcontroller/Pin.h"

#include <iobroker/iobroker.h>
#include <zephyr/device.h>
#include <zephyr/drivers/dac.h>

#include "bindings/zephyr_kernel/__init__.h"
#include "py/runtime.h"

void common_hal_analogio_analogout_construct(analogio_analogout_obj_t *self, const mcu_pin_obj_t *pin) {
    // Allocate the pad's analog output through the iobroker module: the call
    // claims the pad so that bus and GPIO allocations refuse it and hands out
    // a free channel slot on the DAC device.
    const struct device *dac;
    uint8_t channel;
    uint8_t input;
    int res = iobroker_dac_allocate(pin->package_pin, &dac, &channel, &input);
    if (res == -ENOSYS) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_AnalogOut);
    }
    if (res == -EINVAL) {
        // The pad exists but isn't bonded to a DAC output. Match the other
        // ports, which raise ValueError("Invalid pin") for such pins.
        raise_ValueError_invalid_pin();
    }
    if (res < 0) {
        raise_zephyr_error(res);
    }

    struct dac_channel_cfg channel_cfg = {
        .channel_id = channel,
        .buffered = true,
        .internal = false,
    };

    // Ask the driver for the highest resolution it accepts, instead of picking
    // one per driver Kconfig: drivers reject an unsupported resolution from
    // dac_channel_setup() with -EINVAL/-ENOTSUP before touching hardware. The
    // emulated test DAC answers 16; the RA DAC12 and NXP LPDAC only accept
    // their 12-bit hardware resolution; writes are scaled down in
    // common_hal_analogio_analogout_set_value().
    static const uint8_t dac_resolutions[] = {16, 12};
    for (uint8_t i = 0; i < ARRAY_SIZE(dac_resolutions); i++) {
        channel_cfg.resolution = dac_resolutions[i];
        res = dac_channel_setup(dac, &channel_cfg);
        if (res == 0 || (res != -EINVAL && res != -ENOTSUP)) {
            // Not a resolution rejection: surface driver errors like -EBUSY.
            break;
        }
    }
    if (res != 0) {
        iobroker_dac_release(dac, channel);
        raise_zephyr_error(res);
    }

    self->pin = pin;
    self->dac = dac;
    self->channel_id = channel;
    self->resolution = channel_cfg.resolution;
}

bool common_hal_analogio_analogout_deinited(analogio_analogout_obj_t *self) {
    return self->dac == NULL;
}

void common_hal_analogio_analogout_deinit(analogio_analogout_obj_t *self) {
    if (common_hal_analogio_analogout_deinited(self)) {
        return;
    }

    (void)iobroker_dac_release(self->dac, self->channel_id);
    self->pin = NULL;
    self->dac = NULL;
}

void common_hal_analogio_analogout_set_value(analogio_analogout_obj_t *self, uint16_t value) {
    if (common_hal_analogio_analogout_deinited(self)) {
        return;
    }

    if (self->resolution != 16) {
        // CircuitPython's 16-bit code scales down to the DAC's resolution
        // (probed at construct) with converter rounding: (value * out_max +
        // in_max / 2) / in_max, the same rounding the emulated DAC applies in
        // reverse.
        const uint32_t in_max = 65535u;
        const uint32_t out_max = (1u << self->resolution) - 1u;
        uint32_t scaled = ((uint32_t)value * out_max + in_max / 2u) / in_max;
        if (scaled > out_max) {
            scaled = out_max;
        }
        value = (uint16_t)scaled;
    }

    int err = dac_write_value(self->dac, self->channel_id, value);
    if (err != 0) {
        raise_zephyr_error(err);
    }
}
