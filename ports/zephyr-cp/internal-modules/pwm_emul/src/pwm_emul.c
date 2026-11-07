// pwm_emul: emulated PWM controller for native_sim tests.
//
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

// The emulated controller (adafruit,pwm-emul) implements Zephyr's PWM API
// but produces no output. Its channels are independent, each with its own
// period, and channel N stands for GPIO N. set_cycles() records the period
// and pulse and, with Perfetto tracing, emits them on the counter tracks
// "pwm.NN.period" and "pwm.NN.pulse" (matching the "gpio_emul.NN" tracks).

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/sys/util.h>

#if defined(CONFIG_TRACING_PERFETTO)
#include "perfetto_encoder.h"
#endif

#define DT_DRV_COMPAT adafruit_pwm_emul

// Channel numbers are GPIO numbers, so one controller covers every pin and
// the track names need no controller prefix.
BUILD_ASSERT(DT_NUM_INST_STATUS_OKAY(DT_DRV_COMPAT) <= 1,
    "only one adafruit,pwm-emul controller is supported");

struct pwm_emul_channel {
    uint32_t period_cycles;
    uint32_t pulse_cycles;
    // Whether this channel's tracks have been described.
    bool described;
};

struct pwm_emul_config {
    uint32_t clock_frequency;
    struct pwm_emul_channel *channels;
    uint32_t channel_count;
};

#if defined(CONFIG_TRACING_PERFETTO)

// Track UUIDs, clear of the ranges the Zephyr encoder and the GPIO tracks
// use: a "PWM" group track, then two counter tracks per channel (period,
// then pulse).
#define PWM_EMUL_GROUP_TRACK_UUID 0x50000000ULL
#define PWM_EMUL_TRACK_UUID(channel, pulse) \
    (PWM_EMUL_GROUP_TRACK_UUID + 1 + (uint64_t)(channel) * 2 + ((pulse) ? 1 : 0))

static bool group_described;

static void trace_channel(uint32_t channel, struct pwm_emul_channel *ch) {
    if (!perfetto_start()) {
        return;
    }
    if (!group_described) {
        perfetto_emit_track_descriptor(PWM_EMUL_GROUP_TRACK_UUID,
            perfetto_get_trace_uuid(), "PWM");
        group_described = true;
    }
    if (!ch->described) {
        char name[24];
        snprintf(name, sizeof(name), "pwm.%02u.period", (unsigned)channel);
        perfetto_emit_counter_track_descriptor(PWM_EMUL_TRACK_UUID(channel, false),
            PWM_EMUL_GROUP_TRACK_UUID, name, PERFETTO_COUNTER_UNIT_COUNT);
        snprintf(name, sizeof(name), "pwm.%02u.pulse", (unsigned)channel);
        perfetto_emit_counter_track_descriptor(PWM_EMUL_TRACK_UUID(channel, true),
            PWM_EMUL_GROUP_TRACK_UUID, name, PERFETTO_COUNTER_UNIT_COUNT);
        ch->described = true;
    }
    perfetto_emit_counter(PWM_EMUL_TRACK_UUID(channel, false), ch->period_cycles);
    perfetto_emit_counter(PWM_EMUL_TRACK_UUID(channel, true), ch->pulse_cycles);
}

#else

static void trace_channel(uint32_t channel, struct pwm_emul_channel *ch) {
    (void)channel;
    (void)ch;
}

#endif // CONFIG_TRACING_PERFETTO

static int pwm_emul_set_cycles(const struct device *dev, uint32_t channel,
    uint32_t period_cycles, uint32_t pulse_cycles, pwm_flags_t flags) {
    (void)flags;
    const struct pwm_emul_config *config = dev->config;
    if (channel >= config->channel_count) {
        return -EINVAL;
    }
    struct pwm_emul_channel *ch = &config->channels[channel];
    ch->period_cycles = period_cycles;
    ch->pulse_cycles = pulse_cycles;
    trace_channel(channel, ch);
    return 0;
}

static int pwm_emul_get_cycles_per_sec(const struct device *dev, uint32_t channel,
    uint64_t *cycles) {
    (void)channel;
    const struct pwm_emul_config *config = dev->config;
    *cycles = config->clock_frequency;
    return 0;
}

static DEVICE_API(pwm, pwm_emul_api) = {
    .set_cycles = pwm_emul_set_cycles,
    .get_cycles_per_sec = pwm_emul_get_cycles_per_sec,
};

#define PWM_EMUL_DEFINE(n)                                                          \
    static struct pwm_emul_channel pwm_emul_channels_##n[DT_INST_PROP(n, channels)]; \
    static const struct pwm_emul_config pwm_emul_config_##n = {                    \
        .clock_frequency = DT_INST_PROP(n, clock_frequency),                       \
        .channels = pwm_emul_channels_##n,                                         \
        .channel_count = DT_INST_PROP(n, channels),                                \
    };                                                                             \
    DEVICE_DT_INST_DEFINE(n, NULL, NULL, NULL, &pwm_emul_config_##n, POST_KERNEL,  \
    CONFIG_PWM_INIT_PRIORITY, &pwm_emul_api);

DT_INST_FOREACH_STATUS_OKAY(PWM_EMUL_DEFINE)
