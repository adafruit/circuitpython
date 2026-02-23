# Command-Line Control of Emulated I2C Devices in native_sim

This document describes an approach for enabling/disabling emulated I2C devices
at runtime via command-line options in Zephyr's native_sim environment.

## Background

Zephyr's I2C emulation framework (`zephyr,i2c-emul-controller`) provides:

1. **Bus emulation** - Fake I2C controller that routes transfers to emulated devices
2. **Device emulators** - Software implementations of I2C peripherals (sensors, etc.)
3. **Backend APIs** - Test interfaces for manipulating emulator state

However, there's no built-in mechanism to enable/disable emulated devices from the
command line. This capability would be useful for:

- Testing device hot-plug scenarios
- Simulating hardware failures
- Testing error handling paths in CircuitPython

## Relevant Zephyr APIs

### I2C Emulator Structure

From `include/zephyr/drivers/i2c_emul.h`:

```c
struct i2c_emul {
    sys_snode_t node;
    const struct emul *target;
    const struct i2c_emul_api *api;
    struct i2c_emul_api *mock_api;  // If non-NULL, takes precedence
    uint16_t addr;
};

struct i2c_emul_api {
    i2c_emul_transfer_t transfer;
};
```

Key insight: The `mock_api` field allows overriding the normal transfer function.
If `mock_api->transfer()` returns `-ENOSYS`, it falls back to the real API.

### Command-Line Registration

From `boards/native/native_sim/cmdline.h`:

```c
void native_add_command_line_opts(struct args_struct_t *args);
```

### Emulator Lookup

From `include/zephyr/drivers/emul.h`:

```c
const struct emul *emul_get_binding(const char *name);
```

## Implementation Approach

### 1. Create a Disabled Device Registry

Track which devices are "disabled" (should NACK all transactions):

```c
// i2c_emul_cmdline.c

#include <zephyr/drivers/emul.h>
#include <zephyr/drivers/i2c_emul.h>
#include <zephyr/sys/slist.h>
#include "nsi_cmdline.h"

#define MAX_DISABLED_DEVICES 16

static struct {
    const char *name;
    const struct emul *emul;
    struct i2c_emul_api mock_api;
    struct i2c_emul_api *original_mock_api;
    bool disabled;
} disabled_devices[MAX_DISABLED_DEVICES];

static int num_disabled_devices = 0;
```

### 2. Mock Transfer Function

Return `-EIO` (simulates NACK) when device is disabled:

```c
static int disabled_device_transfer(const struct emul *target,
                                    struct i2c_msg *msgs,
                                    int num_msgs,
                                    int addr)
{
    // Find this device in our registry
    for (int i = 0; i < num_disabled_devices; i++) {
        if (disabled_devices[i].emul == target) {
            if (disabled_devices[i].disabled) {
                // Device is disabled - simulate NACK
                return -EIO;
            }
            break;
        }
    }
    // Fall back to normal emulator behavior
    return -ENOSYS;
}
```

### 3. Enable/Disable Functions

```c
int i2c_emul_cmdline_disable_device(const char *name)
{
    const struct emul *emul = emul_get_binding(name);
    if (!emul || emul->bus_type != EMUL_BUS_TYPE_I2C) {
        return -ENODEV;
    }

    // Find or create registry entry
    int idx = -1;
    for (int i = 0; i < num_disabled_devices; i++) {
        if (disabled_devices[i].emul == emul) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        if (num_disabled_devices >= MAX_DISABLED_DEVICES) {
            return -ENOMEM;
        }
        idx = num_disabled_devices++;
        disabled_devices[idx].name = name;
        disabled_devices[idx].emul = emul;
        disabled_devices[idx].mock_api.transfer = disabled_device_transfer;

        // Save and replace mock_api
        disabled_devices[idx].original_mock_api = emul->bus.i2c->mock_api;
        emul->bus.i2c->mock_api = &disabled_devices[idx].mock_api;
    }

    disabled_devices[idx].disabled = true;
    return 0;
}

int i2c_emul_cmdline_enable_device(const char *name)
{
    for (int i = 0; i < num_disabled_devices; i++) {
        if (strcmp(disabled_devices[i].name, name) == 0) {
            disabled_devices[i].disabled = false;
            return 0;
        }
    }
    return -ENODEV;
}
```

### 4. Command-Line Option Registration

```c
static char *disabled_device_args[MAX_DISABLED_DEVICES];
static int num_disabled_device_args = 0;

static void cmd_disable_i2c_device(char *argv, int offset)
{
    ARG_UNUSED(offset);
    if (num_disabled_device_args < MAX_DISABLED_DEVICES) {
        disabled_device_args[num_disabled_device_args++] = argv;
    }
}

static struct args_struct_t i2c_emul_args[] = {
    {
        .option = "disable-i2c",
        .name = "device_name",
        .type = 's',
        .call_when_found = cmd_disable_i2c_device,
        .descript = "Disable an emulated I2C device (can be repeated)"
    },
    ARG_TABLE_ENDMARKER
};

static void register_cmdline_opts(void)
{
    native_add_command_line_opts(i2c_emul_args);
}

// Hook into native_sim initialization
NATIVE_TASK(register_cmdline_opts, PRE_BOOT_1, 0);

static void apply_disabled_devices(void)
{
    for (int i = 0; i < num_disabled_device_args; i++) {
        int rc = i2c_emul_cmdline_disable_device(disabled_device_args[i]);
        if (rc != 0) {
            printk("Warning: Failed to disable I2C device '%s': %d\n",
                   disabled_device_args[i], rc);
        }
    }
}

// Apply after emulators are initialized
NATIVE_TASK(apply_disabled_devices, PRE_BOOT_3, 0);
```

### 5. Usage

After building with this code:

```bash
# Disable a sensor at startup
./build/zephyr/zephyr.exe --disable-i2c=bmi160@68

# Disable multiple devices
./build/zephyr/zephyr.exe --disable-i2c=bmi160@68 --disable-i2c=sht4x@44
```

## Runtime Control Extension

For runtime enable/disable (not just at startup), you could:

### Option A: Use a Named Pipe / FIFO

```c
// Create a FIFO that accepts commands
// "disable bmi160@68" or "enable bmi160@68"
static void *cmdline_control_thread(void *arg)
{
    int fd = open("/tmp/i2c_emul_control", O_RDONLY);
    char buf[256];
    while (read(fd, buf, sizeof(buf)) > 0) {
        if (strncmp(buf, "disable ", 8) == 0) {
            i2c_emul_cmdline_disable_device(buf + 8);
        } else if (strncmp(buf, "enable ", 7) == 0) {
            i2c_emul_cmdline_enable_device(buf + 7);
        }
    }
    return NULL;
}
```

### Option B: Signal Handler

Use `SIGUSR1`/`SIGUSR2` with a config file that specifies which device to toggle.

### Option C: Shared Memory

Map a shared memory region that external tools can write to for device state.

## Integration with CircuitPython Tests

For pytest-based tests, you could:

```python
import subprocess
import os

class NativeSimProcess:
    def __init__(self, exe_path):
        self.exe_path = exe_path
        self.control_fifo = "/tmp/i2c_emul_control"

    def start(self, disabled_devices=None):
        args = [self.exe_path]
        if disabled_devices:
            for dev in disabled_devices:
                args.extend(["--disable-i2c", dev])
        self.proc = subprocess.Popen(args, ...)

    def disable_device(self, name):
        """Runtime disable via FIFO"""
        with open(self.control_fifo, 'w') as f:
            f.write(f"disable {name}\n")

    def enable_device(self, name):
        """Runtime enable via FIFO"""
        with open(self.control_fifo, 'w') as f:
            f.write(f"enable {name}\n")
```

## Alternative: Device Tree Approach

For compile-time configuration, use device tree overlays:

```dts
// boards/native_sim_no_bmi160.overlay
&bmi160 {
    status = "disabled";
};
```

Build separate variants:
```bash
west build -b native_sim -- -DDTC_OVERLAY_FILE=boards/native_sim_no_bmi160.overlay
```

## References

- Zephyr I2C Emulation: `zephyr/drivers/i2c/i2c_emul.c`
- Emulator Framework: `zephyr/doc/hardware/emulator/bus_emulators.rst`
- Native Sim Docs: `zephyr/boards/native/native_sim/doc/index.rst`
- Command-line handling: `zephyr/boards/native/native_sim/cmdline.c`
- Example mock API usage: `zephyr/tests/drivers/sensor/bmi160/src/i2c.c`
