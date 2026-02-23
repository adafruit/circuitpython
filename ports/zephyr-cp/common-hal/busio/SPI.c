// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/busio/SPI.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "shared/runtime/interrupt_char.h"
#include "supervisor/port.h"

#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

// Helper function for Zephyr-specific initialization from device tree
mp_obj_t common_hal_busio_spi_construct_from_device(busio_spi_obj_t *self, const struct device *spi_device) {
    self->base.type = &busio_spi_type;
    self->spi_device = spi_device;
    k_mutex_init(&self->mutex);
    self->has_lock = false;
    self->active_config = 0;

    k_poll_signal_init(&self->signal);

    // Default configuration for both config slots
    self->config[0].frequency = 100000;
    self->config[0].operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_LINES_SINGLE;
    self->config[1].frequency = 100000;
    self->config[1].operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_LINES_SINGLE;

    return MP_OBJ_FROM_PTR(self);
}

// Standard busio construct - not used in Zephyr port (devices come from device tree)
void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Use device tree to define %q devices"), MP_QSTR_SPI);
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    // Always leave it active
    return false;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }
    // Always leave it active
}

void common_hal_busio_spi_mark_deinit(busio_spi_obj_t *self) {
    // Not needed for Zephyr port
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return false;
    }

    self->has_lock = k_mutex_lock(&self->mutex, K_NO_WAIT) == 0;
    return self->has_lock;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->has_lock = false;
    k_mutex_unlock(&self->mutex);
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self, uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    if (common_hal_busio_spi_deinited(self)) {
        return false;
    }

    // Set operation mode based on polarity and phase
    uint16_t operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(bits) | SPI_LINES_SINGLE;

    if (polarity) {
        operation |= SPI_MODE_CPOL;
    }
    if (phase) {
        operation |= SPI_MODE_CPHA;
    }

    // Check if settings have changed. We must switch to the other config slot if they have because
    // Zephyr drivers are allowed to use the pointer value to know if it has changed.
    struct spi_config *current_config = &self->config[self->active_config];
    if (current_config->frequency != baudrate || current_config->operation != operation) {
        // Settings changed, switch to the other config slot
        self->active_config = 1 - self->active_config;

        // Update the new active configuration
        self->config[self->active_config].frequency = baudrate;
        self->config[self->active_config].operation = operation;
    }

    return true;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self, const uint8_t *data, size_t len) {
    if (common_hal_busio_spi_deinited(self)) {
        return false;
    }

    if (len == 0) {
        return true;
    }

    const struct spi_buf tx_buf = {
        .buf = (void *)data,
        .len = len
    };
    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1
    };

    // Initialize the signal for async operation
    k_poll_signal_reset(&self->signal);

    int ret = spi_transceive_signal(self->spi_device, &self->config[self->active_config], &tx, NULL, &self->signal);
    if (ret != 0) {
        return false;
    }

    // Wait for the transfer to complete while running background tasks
    int signaled = 0;
    int result = 0;
    while (!signaled && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        k_poll_signal_check(&self->signal, &signaled, &result);
    }

    return signaled && result == 0;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self, uint8_t *data, size_t len, uint8_t write_value) {
    if (common_hal_busio_spi_deinited(self)) {
        return false;
    }

    if (len == 0) {
        return true;
    }

    // For read, we need to write dummy bytes
    // We'll allocate a temporary buffer if write_value is not 0
    uint8_t *tx_data = NULL;
    bool need_free = false;
    bool used_port_malloc = false;

    if (write_value != 0) {
        // Use port_malloc if GC isn't active, otherwise use m_malloc
        if (gc_alloc_possible()) {
            tx_data = m_malloc(len);
        } else {
            tx_data = port_malloc(len, false);
            used_port_malloc = true;
        }
        if (tx_data == NULL) {
            return false;
        }
        memset(tx_data, write_value, len);
        need_free = true;
    }

    const struct spi_buf tx_buf = {
        .buf = tx_data,
        .len = tx_data ? len : 0
    };
    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = tx_data ? 1 : 0
    };

    const struct spi_buf rx_buf = {
        .buf = data,
        .len = len
    };
    const struct spi_buf_set rx = {
        .buffers = &rx_buf,
        .count = 1
    };

    // Initialize the signal for async operation
    k_poll_signal_reset(&self->signal);

    int ret = spi_transceive_signal(self->spi_device, &self->config[self->active_config], &tx, &rx, &self->signal);

    if (need_free) {
        if (used_port_malloc) {
            port_free(tx_data);
        } else {
            m_free(tx_data);
        }
    }

    if (ret != 0) {
        return false;
    }

    // Wait for the transfer to complete while running background tasks
    int signaled = 0;
    int result = 0;
    while (!signaled && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        k_poll_signal_check(&self->signal, &signaled, &result);
    }

    return signaled && result == 0;
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len) {
    if (common_hal_busio_spi_deinited(self)) {
        return false;
    }

    if (len == 0) {
        return true;
    }

    const struct spi_buf tx_buf = {
        .buf = (void *)data_out,
        .len = len
    };
    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1
    };

    const struct spi_buf rx_buf = {
        .buf = data_in,
        .len = len
    };
    const struct spi_buf_set rx = {
        .buffers = &rx_buf,
        .count = 1
    };

    // Initialize the signal for async operation
    k_poll_signal_reset(&self->signal);

    int ret = spi_transceive_signal(self->spi_device, &self->config[self->active_config], &tx, &rx, &self->signal);
    if (ret != 0) {
        return false;
    }

    // Wait for the transfer to complete while running background tasks
    int signaled = 0;
    int result = 0;
    while (!signaled && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        k_poll_signal_check(&self->signal, &signaled, &result);
    }

    return signaled && result == 0;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->config[self->active_config].frequency;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return (self->config[self->active_config].operation & SPI_MODE_CPHA) ? 1 : 0;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return (self->config[self->active_config].operation & SPI_MODE_CPOL) ? 1 : 0;
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    // Not needed for Zephyr port (devices are managed by Zephyr)
}
