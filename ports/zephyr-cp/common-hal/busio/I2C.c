// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include <zephyr/drivers/i2c.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

// Helper function for Zephyr-specific initialization from device tree
mp_obj_t common_hal_busio_i2c_construct_from_device(busio_i2c_obj_t *self, const struct device *i2c_device) {
    self->base.type = &busio_i2c_type;
    self->i2c_device = i2c_device;
    k_mutex_init(&self->mutex);
    self->has_lock = false;
    return MP_OBJ_FROM_PTR(self);
}

// Standard busio construct - not used in Zephyr port (devices come from device tree)
void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda,
    uint32_t frequency, uint32_t timeout_ms) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Use device tree to define %q devices"), MP_QSTR_I2C);
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    // Always leave it active (managed by Zephyr)
    return false;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }
    // Always leave it active (managed by Zephyr)
}

void common_hal_busio_i2c_mark_deinit(busio_i2c_obj_t *self) {
    // Not needed for Zephyr port
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    if (common_hal_busio_i2c_deinited(self)) {
        return false;
    }

    // Try a zero-length write to probe for device
    uint8_t dummy;
    int ret = i2c_write(self->i2c_device, &dummy, 0, addr);
    return ret == 0;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return false;
    }

    self->has_lock = k_mutex_lock(&self->mutex, K_NO_WAIT) == 0;
    return self->has_lock;
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
    k_mutex_unlock(&self->mutex);
}

mp_negative_errno_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len) {

    if (common_hal_busio_i2c_deinited(self)) {
        return -MP_EIO;
    }

    int ret = i2c_write(self->i2c_device, data, len, addr);
    if (ret != 0) {
        // Map Zephyr error codes to errno
        if (ret == -ENOTSUP) {
            return -MP_EOPNOTSUPP;
        } else if (ret == -EIO || ret == -ENXIO) {
            return -MP_EIO;
        } else if (ret == -EBUSY) {
            return -MP_EBUSY;
        }
        return -MP_EIO;
    }

    return 0;
}

mp_negative_errno_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *data, size_t len) {

    if (common_hal_busio_i2c_deinited(self)) {
        return -MP_EIO;
    }

    if (len == 0) {
        return 0;
    }

    int ret = i2c_read(self->i2c_device, data, len, addr);
    if (ret != 0) {
        // Map Zephyr error codes to errno
        if (ret == -ENOTSUP) {
            return -MP_EOPNOTSUPP;
        } else if (ret == -EIO || ret == -ENXIO) {
            return -MP_EIO;
        } else if (ret == -EBUSY) {
            return -MP_EBUSY;
        }
        return -MP_EIO;
    }

    return 0;
}

mp_negative_errno_t common_hal_busio_i2c_write_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *out_data, size_t out_len, uint8_t *in_data, size_t in_len) {

    if (common_hal_busio_i2c_deinited(self)) {
        return -MP_EIO;
    }

    // Use i2c_write_read for combined transaction with repeated start
    int ret = i2c_write_read(self->i2c_device, addr, out_data, out_len, in_data, in_len);
    if (ret != 0) {
        // Map Zephyr error codes to errno
        if (ret == -ENOTSUP) {
            return -MP_EOPNOTSUPP;
        } else if (ret == -EIO || ret == -ENXIO) {
            return -MP_EIO;
        } else if (ret == -EBUSY) {
            return -MP_EBUSY;
        }
        return -MP_EIO;
    }

    return 0;
}

void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    // Not needed for Zephyr port (devices are managed by Zephyr)
}
