// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "bindings/zephyr_i2c/I2C.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include <zephyr/drivers/i2c.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

mp_obj_t zephyr_i2c_i2c_zephyr_init(zephyr_i2c_i2c_obj_t *self, const struct device *i2c_device) {
    self->base.type = &zephyr_i2c_i2c_type;
    self->i2c_device = i2c_device;
    k_mutex_init(&self->mutex);
    return MP_OBJ_FROM_PTR(self);
}

bool common_hal_zephyr_i2c_i2c_deinited(zephyr_i2c_i2c_obj_t *self) {
    // Always leave it active
    return false;
}

void common_hal_zephyr_i2c_i2c_deinit(zephyr_i2c_i2c_obj_t *self) {
    if (common_hal_zephyr_i2c_i2c_deinited(self)) {
        return;
    }
    // Always leave it active
}

bool common_hal_zephyr_i2c_i2c_probe(zephyr_i2c_i2c_obj_t *self, uint8_t addr) {
    if (common_hal_zephyr_i2c_i2c_deinited(self)) {
        return false;
    }

    // Try a zero-length write to probe for device
    uint8_t dummy;
    int ret = i2c_write(self->i2c_device, &dummy, 0, addr);
    return ret == 0;
}

bool common_hal_zephyr_i2c_i2c_try_lock(zephyr_i2c_i2c_obj_t *self) {
    if (common_hal_zephyr_i2c_i2c_deinited(self)) {
        return false;
    }

    self->has_lock = k_mutex_lock(&self->mutex, K_NO_WAIT) == 0;
    return self->has_lock;
}

bool common_hal_zephyr_i2c_i2c_has_lock(zephyr_i2c_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_zephyr_i2c_i2c_unlock(zephyr_i2c_i2c_obj_t *self) {
    k_mutex_unlock(&self->mutex);
}

uint8_t common_hal_zephyr_i2c_i2c_write(zephyr_i2c_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len) {

    if (common_hal_zephyr_i2c_i2c_deinited(self)) {
        return MP_EIO;
    }

    int ret = i2c_write(self->i2c_device, data, len, addr);
    if (ret != 0) {
        // Map Zephyr error codes to errno
        if (ret == -ENOTSUP) {
            return MP_EOPNOTSUPP;
        } else if (ret == -EIO || ret == -ENXIO) {
            return MP_EIO;
        } else if (ret == -EBUSY) {
            return MP_EBUSY;
        }
        return MP_EIO;
    }

    return 0;
}

uint8_t common_hal_zephyr_i2c_i2c_read(zephyr_i2c_i2c_obj_t *self, uint16_t addr,
    uint8_t *data, size_t len) {

    if (common_hal_zephyr_i2c_i2c_deinited(self)) {
        return MP_EIO;
    }

    if (len == 0) {
        return 0;
    }

    int ret = i2c_read(self->i2c_device, data, len, addr);
    if (ret != 0) {
        // Map Zephyr error codes to errno
        if (ret == -ENOTSUP) {
            return MP_EOPNOTSUPP;
        } else if (ret == -EIO || ret == -ENXIO) {
            return MP_EIO;
        } else if (ret == -EBUSY) {
            return MP_EBUSY;
        }
        return MP_EIO;
    }

    return 0;
}

uint8_t common_hal_zephyr_i2c_i2c_write_read(zephyr_i2c_i2c_obj_t *self, uint16_t addr,
    uint8_t *out_data, size_t out_len, uint8_t *in_data, size_t in_len) {

    if (common_hal_zephyr_i2c_i2c_deinited(self)) {
        return MP_EIO;
    }

    // Use i2c_write_read for combined transaction with repeated start
    int ret = i2c_write_read(self->i2c_device, addr, out_data, out_len, in_data, in_len);
    if (ret != 0) {
        // Map Zephyr error codes to errno
        if (ret == -ENOTSUP) {
            return MP_EOPNOTSUPP;
        } else if (ret == -EIO || ret == -ENXIO) {
            return MP_EIO;
        } else if (ret == -EBUSY) {
            return MP_EBUSY;
        }
        return MP_EIO;
    }

    return 0;
}
