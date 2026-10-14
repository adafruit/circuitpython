// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

// SPI loopback for native_sim: every byte sent is read back, and zeros when nothing is sent.

#define DT_DRV_COMPAT circuitpython_spi_loopback

#include <zephyr/device.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/spi_emul.h>

static int loopback_io(const struct emul *target, const struct spi_config *config,
    const struct spi_buf_set *tx_bufs, const struct spi_buf_set *rx_bufs) {
    ARG_UNUSED(target);
    ARG_UNUSED(config);
    if (rx_bufs == NULL) {
        return 0;
    }
    // A buffer without data is that many idle or dropped bytes, as in Zephyr's drivers.
    size_t tx_i = 0;
    size_t tx_pos = 0;
    for (size_t i = 0; i < rx_bufs->count; i++) {
        uint8_t *rx = rx_bufs->buffers[i].buf;
        for (size_t j = 0; j < rx_bufs->buffers[i].len; j++) {
            uint8_t value = 0;
            while (tx_bufs != NULL && tx_i < tx_bufs->count && tx_pos >= tx_bufs->buffers[tx_i].len) {
                tx_i++;
                tx_pos = 0;
            }
            if (tx_bufs != NULL && tx_i < tx_bufs->count) {
                const uint8_t *tx = tx_bufs->buffers[tx_i].buf;
                if (tx != NULL) {
                    value = tx[tx_pos];
                }
                tx_pos++;
            }
            if (rx != NULL) {
                rx[j] = value;
            }
        }
    }
    return 0;
}

static int loopback_init(const struct emul *target, const struct device *parent) {
    ARG_UNUSED(target);
    ARG_UNUSED(parent);
    return 0;
}

static const struct spi_emul_api loopback_api = {
    .io = loopback_io,
};

#define LOOPBACK_DEFINE(n) \
    DEVICE_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL, CONFIG_SPI_INIT_PRIORITY, NULL); \
    EMUL_DT_INST_DEFINE(n, loopback_init, NULL, NULL, &loopback_api, NULL);

DT_INST_FOREACH_STATUS_OKAY(LOOPBACK_DEFINE)
