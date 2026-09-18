// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "bindings/zephyr_kernel/__init__.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include <iobroker/iobroker.h>

#include <zephyr/drivers/i2c.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

// Helper function for Zephyr-specific initialization from device tree
mp_obj_t common_hal_busio_i2c_construct_from_device(busio_i2c_obj_t *self, const struct device *i2c_device) {
    self->base.type = &busio_i2c_type;
    self->i2c_device = i2c_device;
    k_mutex_init(&self->mutex);
    self->has_lock = false;
    self->dynamic = false;
    self->sda = NULL;
    self->scl = NULL;
    return MP_OBJ_FROM_PTR(self);
}

static void raise_no_i2c_peripheral(int err) {
    if (err == -ENODEV) {
        mp_raise_ValueError(MP_ERROR_TEXT("All I2C peripherals are in use"));
    }
    if (err == -EBUSY) {
        mp_raise_ValueError(MP_ERROR_TEXT("Internal resource(s) in use"));
    }
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Use device tree to define %q devices"), MP_QSTR_I2C);
}

// Standard busio construct: pick a free peripheral instance and route it to
// the requested pins at runtime (supported on nRF SoCs).
void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda,
    uint32_t frequency, uint32_t timeout_ms) {
    const struct device *dev = NULL;
    int ret = iobroker_i2c_allocate(sda->package_pin, scl->package_pin, &dev);
    if (ret < 0) {
        raise_no_i2c_peripheral(ret);
    }

    common_hal_busio_i2c_construct_from_device(self, dev);
    self->dynamic = true;
    self->sda = sda;
    self->scl = scl;

    // Initialize the deferred device now that it is routed to the requested
    // pins. Fixed devicetree instances are already initialized (-EALREADY).
    int init_ret = device_init(dev);
    if (init_ret < 0 && init_ret != -EALREADY) {
        // The failed init may have routed pins and left the device in a
        // partial state; deinit gives up the claim and resets the pins.
        common_hal_busio_i2c_deinit(self);
        raise_zephyr_error(init_ret);
    }

    // Apply the requested bus frequency. Zephyr nRF drivers support 100k,
    // 400k and (on TWIM) 1M.
    uint8_t speed;
    if (frequency <= 100000) {
        speed = I2C_SPEED_STANDARD;
    } else if (frequency <= 400000) {
        speed = I2C_SPEED_FAST;
    } else {
        speed = I2C_SPEED_FAST_PLUS;
    }
    int config_ret = i2c_configure(self->i2c_device, I2C_SPEED_SET(speed));
    if (config_ret < 0) {
        common_hal_busio_i2c_deinit(self);
        raise_zephyr_error(config_ret);
    }
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->i2c_device == NULL;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }
    if (self->dynamic) {
        // The release de-inits the device, which applies its low-power
        // pinctrl state and leaves the routed pins disconnected.
        (void)iobroker_release(self->i2c_device);
        self->sda = NULL;
        self->scl = NULL;
        self->i2c_device = NULL;
    }
}

void common_hal_busio_i2c_mark_deinit(busio_i2c_obj_t *self) {
    if (self->dynamic) {
        self->i2c_device = NULL;
    }
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
