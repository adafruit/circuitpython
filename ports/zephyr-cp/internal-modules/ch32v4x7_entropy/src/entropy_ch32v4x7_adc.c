// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Entropy source for the WCH CH32V4x7, whose on-chip RNG repeats too often to
// use. Each 32-byte output block is SHA-256 over 512 ADC readings of the
// internal temperature sensor, credited at 0.5 bit each.
//
// This driver owns ADC1. Nothing else on these boards uses it.
//
// The hash is CircuitPython's own SHA-256, not PSA crypto, because PSA seeds
// its random generator from this driver and is not ready at PRE_KERNEL_1.

#define DT_DRV_COMPAT adafruit_ch32v4x7_adc_entropy

#include <errno.h>
#include <string.h>

#include <zephyr/arch/riscv/csr.h>
#include <zephyr/device.h>
#include <zephyr/drivers/entropy.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include "lib/crypto-algorithms/sha256.h"

#define REG(addr) (*(volatile uint32_t *)(addr))

// RCC (reference manual chapter 3).
#define RCC_CFGR0 0x40021004u
#define RCC_CFGR0_ADCPRE_MASK (3u << 14)
#define RCC_CFGR0_ADCPRE_DIV8 (3u << 14)
#define RCC_PB2PCENR 0x40021018u
#define RCC_PB2PCENR_ADC1EN (1u << 9)

// ADC1 (chapter 12).
#define ADC1_BASE 0x40012400u
#define ADC_STATR (ADC1_BASE + 0x00)
#define ADC_STATR_EOC (1u << 1)
#define ADC_CTLR1 (ADC1_BASE + 0x04)
#define ADC_CTLR2 (ADC1_BASE + 0x08)
#define ADC_CTLR2_ADON (1u << 0)
#define ADC_CTLR2_CAL (1u << 2)
#define ADC_CTLR2_RSTCAL (1u << 3)
#define ADC_CTLR2_EXTSEL_SWSTART (7u << 17)
#define ADC_CTLR2_EXTTRIG (1u << 20)
#define ADC_CTLR2_SWSTART (1u << 22)
#define ADC_CTLR2_TSVREFE (1u << 23)
#define ADC_SAMPTR1 (ADC1_BASE + 0x0C)
#define ADC_SAMPTR1_SMP16_MASK (7u << 18)
#define ADC_RSQR1 (ADC1_BASE + 0x2C)
#define ADC_RSQR3 (ADC1_BASE + 0x34)
#define ADC_RDATAR (ADC1_BASE + 0x4C)
#define ADC_CHANNEL_TEMPSENSOR 16u

// Electronic signature (chapter 31): 96-bit unique ID.
#define ESIG_UNIID1 0x1FFFF7E8u

#define SAMPLES_PER_BLOCK 512
#define SAMPLES_PER_UPDATE 64
#define TIMEOUT_LOOPS 100000

// SP 800-90B health test cutoffs for 0.5 bit per reading and a false alarm
// rate of 2^-20.
// Repetition count test (4.4.1): one reading this many times in a row.
#define REPETITION_CUTOFF 41
// Adaptive proportion test (4.4.2): the first reading of a block this many
// times in the 512 readings of that block.
#define PROPORTION_CUTOFF 410

static K_MUTEX_DEFINE(entropy_lock);
static uint32_t block_counter;
// Repetition count state, kept across calls so a run can't hide at a call
// boundary.
static uint16_t repetition_last;
static int repetition_count;

static void wipe(void *p, size_t n) {
    volatile uint8_t *v = p;
    while (n--) {
        *v++ = 0;
    }
}

static bool adc_read(uint16_t *out) {
    REG(ADC_CTLR2) |= ADC_CTLR2_EXTTRIG | ADC_CTLR2_SWSTART;
    for (int i = 0; i < TIMEOUT_LOOPS; i++) {
        if (REG(ADC_STATR) & ADC_STATR_EOC) {
            // Reading RDATAR clears EOC.
            *out = REG(ADC_RDATAR);
            return true;
        }
    }
    return false;
}

static bool wait_ctlr2_clear(uint32_t bit) {
    for (int i = 0; i < TIMEOUT_LOOPS; i++) {
        if (!(REG(ADC_CTLR2) & bit)) {
            return true;
        }
    }
    return false;
}

static int ch32v4x7_adc_entropy_get(const struct device *dev, uint8_t *buf, uint16_t len) {
    ARG_UNUSED(dev);
    int ret = 0;
    bool locked = !k_is_pre_kernel();
    if (locked) {
        k_mutex_lock(&entropy_lock, K_FOREVER);
    }

    CRYAL_SHA256_CTX ctx;
    uint32_t header[5];
    uint16_t samples[SAMPLES_PER_UPDATE];
    BYTE digest[SHA256_BLOCK_SIZE];

    while (len > 0) {
        sha256_init(&ctx);
        header[0] = ++block_counter;
        header[1] = csr_read(mcycle);
        header[2] = REG(ESIG_UNIID1);
        header[3] = REG(ESIG_UNIID1 + 4);
        header[4] = REG(ESIG_UNIID1 + 8);
        sha256_update(&ctx, (const BYTE *)header, sizeof(header));

        uint16_t proportion_first = 0;
        int proportion_count = 0;
        for (int i = 0; i < SAMPLES_PER_BLOCK; i += SAMPLES_PER_UPDATE) {
            for (int j = 0; j < SAMPLES_PER_UPDATE; j++) {
                uint16_t sample;
                if (!adc_read(&sample)) {
                    ret = -EIO;
                    goto done;
                }
                samples[j] = sample;

                if (repetition_count > 0 && sample == repetition_last) {
                    if (++repetition_count >= REPETITION_CUTOFF) {
                        ret = -EIO;
                        goto done;
                    }
                } else {
                    repetition_last = sample;
                    repetition_count = 1;
                }

                if (i + j == 0) {
                    proportion_first = sample;
                }
                if (sample == proportion_first && ++proportion_count >= PROPORTION_CUTOFF) {
                    ret = -EIO;
                    goto done;
                }
            }
            sha256_update(&ctx, (const BYTE *)samples, sizeof(samples));
        }

        sha256_final(&ctx, digest);
        size_t n = MIN(len, sizeof(digest));
        memcpy(buf, digest, n);
        buf += n;
        len -= n;
    }

done:
    wipe(&ctx, sizeof(ctx));
    wipe(header, sizeof(header));
    wipe(samples, sizeof(samples));
    wipe(digest, sizeof(digest));
    if (locked) {
        k_mutex_unlock(&entropy_lock);
    }
    return ret;
}

static int ch32v4x7_adc_entropy_init(const struct device *dev) {
    ARG_UNUSED(dev);

    // ADC1 on the internal temperature sensor, software triggered, with the
    // shortest sample time because that gives the most noise.
    REG(RCC_PB2PCENR) |= RCC_PB2PCENR_ADC1EN;
    REG(RCC_CFGR0) = (REG(RCC_CFGR0) & ~RCC_CFGR0_ADCPRE_MASK) | RCC_CFGR0_ADCPRE_DIV8;
    REG(ADC_CTLR1) = 0;
    REG(ADC_CTLR2) = ADC_CTLR2_EXTSEL_SWSTART | ADC_CTLR2_TSVREFE;
    REG(ADC_RSQR1) = 0;
    REG(ADC_RSQR3) = ADC_CHANNEL_TEMPSENSOR;
    REG(ADC_SAMPTR1) &= ~ADC_SAMPTR1_SMP16_MASK;
    REG(ADC_CTLR2) |= ADC_CTLR2_ADON;

    // Let the ADC and sensor start up. The system timer is not running yet in
    // PRE_KERNEL_1, so this is a plain loop, far longer than the start-up
    // times.
    for (volatile int i = 0; i < 10000; i++) {
    }

    REG(ADC_CTLR2) |= ADC_CTLR2_RSTCAL;
    if (!wait_ctlr2_clear(ADC_CTLR2_RSTCAL)) {
        return -EIO;
    }
    REG(ADC_CTLR2) |= ADC_CTLR2_CAL;
    if (!wait_ctlr2_clear(ADC_CTLR2_CAL)) {
        return -EIO;
    }

    return 0;
}

static DEVICE_API(entropy, ch32v4x7_adc_entropy_api) = {
    .get_entropy = ch32v4x7_adc_entropy_get,
};

DEVICE_DT_INST_DEFINE(0, ch32v4x7_adc_entropy_init, NULL, NULL, NULL, PRE_KERNEL_1,
    CONFIG_ENTROPY_INIT_PRIORITY, &ch32v4x7_adc_entropy_api);
