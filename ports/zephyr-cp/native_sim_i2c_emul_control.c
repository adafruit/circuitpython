/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Command-line control for enabling/disabling emulated I2C devices
 * on native_sim. This allows testing device hot-plug and error scenarios.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/i2c_emul.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>

#include "nsi_cmdline.h"
#include "posix_native_task.h"

LOG_MODULE_REGISTER(i2c_emul_control, LOG_LEVEL_INF);

#define MAX_DISABLED_DEVICES 16

struct disabled_device {
    const char *name;
    const struct emul *emul;
    struct i2c_emul_api mock_api;
    bool disabled;
};

static struct disabled_device disabled_devices[MAX_DISABLED_DEVICES];
static int num_disabled_devices = 0;

static char *disabled_device_args[MAX_DISABLED_DEVICES];
static int num_disabled_device_args = 0;

/*
 * Mock transfer function that returns -EIO (NACK) when device is disabled,
 * or -ENOSYS to fall back to the real emulator.
 */
static int disabled_device_transfer(const struct emul *target,
    struct i2c_msg *msgs,
    int num_msgs,
    int addr) {
    ARG_UNUSED(msgs);
    ARG_UNUSED(num_msgs);
    ARG_UNUSED(addr);

    for (int i = 0; i < num_disabled_devices; i++) {
        if (disabled_devices[i].emul == target) {
            if (disabled_devices[i].disabled) {
                LOG_DBG("Device %s is disabled, returning -EIO",
                    disabled_devices[i].name);
                return -EIO;
            }
            break;
        }
    }
    /* Fall back to normal emulator behavior */
    return -ENOSYS;
}

int i2c_emul_control_disable_device(const char *name) {
    const struct emul *emul = emul_get_binding(name);
    if (!emul) {
        LOG_ERR("Emulator '%s' not found", name);
        return -ENODEV;
    }

    if (emul->bus_type != EMUL_BUS_TYPE_I2C) {
        LOG_ERR("Emulator '%s' is not an I2C device", name);
        return -EINVAL;
    }

    /* Find existing entry or create new one */
    int idx = -1;
    for (int i = 0; i < num_disabled_devices; i++) {
        if (disabled_devices[i].emul == emul) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        if (num_disabled_devices >= MAX_DISABLED_DEVICES) {
            LOG_ERR("Too many disabled devices");
            return -ENOMEM;
        }
        idx = num_disabled_devices++;
        disabled_devices[idx].name = name;
        disabled_devices[idx].emul = emul;
        disabled_devices[idx].mock_api.transfer = disabled_device_transfer;

        /* Install our mock_api to intercept transfers */
        emul->bus.i2c->mock_api = &disabled_devices[idx].mock_api;
    }

    disabled_devices[idx].disabled = true;
    LOG_INF("Disabled I2C emulator: %s", name);
    return 0;
}

int i2c_emul_control_enable_device(const char *name) {
    for (int i = 0; i < num_disabled_devices; i++) {
        if (strcmp(disabled_devices[i].name, name) == 0) {
            disabled_devices[i].disabled = false;
            LOG_INF("Enabled I2C emulator: %s", name);
            return 0;
        }
    }
    LOG_ERR("Device '%s' not in disabled list", name);
    return -ENODEV;
}

bool i2c_emul_control_is_disabled(const char *name) {
    for (int i = 0; i < num_disabled_devices; i++) {
        if (strcmp(disabled_devices[i].name, name) == 0) {
            return disabled_devices[i].disabled;
        }
    }
    return false;
}

/* Command-line option handler */
static void cmd_disable_i2c_device(char *argv, int offset) {
    /* The value is at argv + offset (after the '=' in --disable-i2c=value) */
    char *value = argv + offset;
    if (num_disabled_device_args < MAX_DISABLED_DEVICES) {
        disabled_device_args[num_disabled_device_args++] = value;
    } else {
        printk("i2c_emul_control: Too many --disable-i2c arguments, ignoring: %s\n", value);
    }
}

static struct args_struct_t i2c_emul_args[] = {
    {
        .option = "disable-i2c",
        .name = "device",
        .type = 's',
        .call_when_found = cmd_disable_i2c_device,
        .descript = "Disable an emulated I2C device by name (can be repeated). "
            "Example: --disable-i2c=bmi160"
    },
    ARG_TABLE_ENDMARKER
};

static void register_cmdline_opts(void) {
    nsi_add_command_line_opts(i2c_emul_args);
}

/* Register command-line options early in boot */
NATIVE_TASK(register_cmdline_opts, PRE_BOOT_1, 0);

static int apply_disabled_devices(void) {
    LOG_DBG("Applying %d disabled device(s)", num_disabled_device_args);
    for (int i = 0; i < num_disabled_device_args; i++) {
        int rc = i2c_emul_control_disable_device(disabled_device_args[i]);
        if (rc != 0) {
            LOG_WRN("Failed to disable I2C device '%s': %d",
                disabled_device_args[i], rc);
        }
    }
    return 0;
}

/*
 * Apply after emulators are initialized.
 * I2C emulators are registered at POST_KERNEL level, so we need to run
 * at APPLICATION level to ensure they exist.
 */
SYS_INIT(apply_disabled_devices, APPLICATION, 99);
