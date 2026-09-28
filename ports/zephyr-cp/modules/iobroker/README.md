# iobroker

A Zephyr module for **dynamic peripheral allocation and runtime pin routing**:
pick a free bus instance (I2C, SPI, UART) enabled in the devicetree, re-route
it to requested pins at runtime and hand the Zephyr device to the caller.

Pins are specified using `package_pin_t` and represent a single pin on a package
or module containing a system-on-a-chip (SoC). This is the most common boundary
between an SoC and printed circuit board (PCB). Packages and modules may choose
to map more than one SoC pin to a package pin and this way IOBroker ensures that
each package pin is only used for one thing at a time. GPIO port and pin numbers
are often used as names for these pins but using package pins allow us to
accommodate pins without GPIO and those with multiple GPIO.

IOBroker takes in a number of package pins and a device type. Device types are
usually `drivers/<device type>` in the zephyr source tree. For example,
`int iobroker_i2c_allocate(package_pin_t sda, package_pin_t scl, const struct device **dev_out);`
will find a Zephyr I2C device that pins sda and scl can be connected to, connect
them using pinctrl, claim these resources and return it. It will return
`-ENODEV` if no such device can be found. The board DTS must enable these
devices with `status = "okay";`, mark them as `zephyr,deferred-init;` and
provide default pinctrl settings that will be overridden.

## Status

Currently, runtime routing is implemented for nRF SoCs, whose pin control
encoding can be computed at runtime and whose peripherals can be routed to
(almost) any pin via PSEL. On other SoCs the module compiles but the allocate
functions always return `-ENOSYS`.

## Source layout

The source is organized by vendor and SoC family under `src/`:

```
src/
  iobroker.c                       # SoC-agnostic core: package pin map
                                      # lookup, GPIO controller split, claim
                                      # registry, -ENOSYS stubs when routing
                                      # is unavailable
  iobroker_internal.h              # helpers shared between core and
                                      # vendor implementations (private)
  nordic/
    nrf/                              # every nRF SoC shares one pinctrl
      iobroker_route.c             # encoding, so the family is one dir
```

The core compiles on every SoC. Each vendor adds a `src/<vendor>/<soc>/`
directory with an implementation of the bus allocate/release functions and a
corresponding `zephyr_library_sources_ifdef()` line in `CMakeLists.txt`;
without one the core's `#if !IOBROKER_ROUTING` stubs report `-ENOSYS`. When
an implementation covers a whole SoC family (as the nRF one does, keyed on
`CONFIG_PINCTRL_NRF`), the directory is named for the family.

## Enabling

The module is registered by the CircuitPython Zephyr application
(`ports/zephyr-cp/CMakeLists.txt`) via `ZEPHYR_EXTRA_MODULES`, so it works
without west manifest changes. Its core is always compiled and its callers
include `<iobroker/iobroker.h>` directly; runtime routing additionally
requires `CONFIG_PINCTRL_DYNAMIC` and `CONFIG_DEVICE_DEINIT_SUPPORT`, which
the port enables by default on nRF SoCs.

## Board tables

The core does not know which instances are allocatable; the application must
provide per-board tables (the generated `board.c` always emits them):

```c
const iobroker_instance_t iobroker_i2c_buses[];   // + _states[] and _bus_count
const iobroker_instance_t iobroker_spi_buses[];   // ...
const iobroker_instance_t iobroker_uart_buses[];  // ...
const struct device * const iobroker_gpio_port_devices[];  // + _indexes[] and _count
const iobroker_package_pin_t iobroker_package_pins[];   // + _pin_count
const uint16_t iobroker_reserved_pads[];   // + _pin_count
```

The package pin map is selected from the module's reference maps:
`Kconfig.packages` offers one option per transcribed package
(`packages/*.toml`), each visible only for the SoCs it applies to and
preselected for the development kits. SoCs with no reference map fall back to
`IOBROKER_PACKAGE_ONE_TO_ONE`, an identity map where the package pin number
is the global pin number (gpio port index * 32 + pin within the port), so
boards without a transcribed physical package can still resolve their pins.
`IOBROKER_PACKAGE_NONE` is also available and generates an empty map, making
package pin lookups fail with `-EINVAL`. The selected TOML (or the empty
map) is rendered into a build-directory translation unit at build time. The
identity map needs no rendered table: the core applies it directly.
New maps are transcribed from a SoC datasheet with `tools/gen_package.py`
(see the script's docstring; the datasheets live in `datasheets/`).

Each instance entry contains the Zephyr device, its `struct
pinctrl_dev_config` (via `PINCTRL_DT_DEV_CONFIG_DECLARE`/`_GET`) and, when the
devicetree state has fixed pins, the raw `pinctrl_soc_pin_t` values of the
"default" state. Instances whose devicetree "default" state leaves every
signal disconnected (`NRF_PIN_DISCONNECTED`) set `.dt_psels = NULL` and can be
routed to any pin at runtime. Instances with fixed devicetree pins are only
allocatable when a request matches their existing state.

In the CircuitPython tree these tables are generated into the board's
`board.c` by `cptools/zephyr2cp.py` at build time.

`iobroker_reserved_pads` lists the SoC pads that fixed peripherals (console
UART, flash instance, I2S, ...) drive at boot through their devicetree
pinctrl default state. `iobroker_pin_in_use()` reports them as busy so that
allocate() rejects requests for those pads with `-EBUSY` instead of
re-routing pads something else is already driving. The table only contains
connected pins; dynamically routable instances (all signals disconnected in
the devicetree) contribute nothing.

The GPIO controller table pairs each controller device with its hardware
port index. The indexes define the global pin numbering (index * 32 + pin
within the port) shared with the pin objects, and let the module resolve a
global pin number back to the controller device and pin number that
Zephyr's GPIO API takes.

## Public API

See `include/iobroker/iobroker.h`. Signals are requested with
`package_pin_t` values (`PACKAGE_PIN(n)`): the physical package pin numbered
as the SoC datasheet numbers it (QFN pins 1..N; CSP/BGA balls numbered
sequentially in row-major order, with the datasheet's ball label noted in the
map and the generated C). The module resolves package pins to SoC pads
through the package pin map, `iobroker_package_pins[]`, selected via
`CONFIG_IOBROKER_PACKAGE_*` (see above); internally the lookup then
proceeds package pin -> SoC pad -> peripheral routing. `IOBROKER_NO_PIN` leaves a
signal disconnected. Pull resistors are chosen by the module per bus signal
(I2C SDA/SCL and UART RX idle high). Every allocate call must be paired with
`iobroker_release()`, so the application owns the lifecycle. Releasing also
leaves the pins quiescent: `iobroker_release()` de-initializes the device,
which applies its low-power pinctrl state to the routed pins.
`iobroker_pin_in_use()` reports whether a package pin is claimed by a
currently allocated instance, and `allocate()` refuses (-EBUSY) requests that
would double-use a pin. Package pins can also be claimed for plain GPIO use
with `iobroker_gpio_allocate()` / `iobroker_gpio_release()`: the caller
configures the pad while the claim is held and the module returns it to a
quiescent state (disconnected) on release, and GPIO claims conflict with bus
allocations the same way bus allocations conflict with each other.
`iobroker_gpio_allocate()` resolves the package pin through the map
and returns both the GPIO controller device and the pin number within it.
`iobroker_gpio_package_pin()` maps a GPIO controller's hardware port index
and pin number (the two halves of the global pin numbering) back to the
package pin the pad is bonded to.

## Externalizing

This module is application-agnostic: it depends only on Zephyr. To move it
into its own repository:

1. Copy this directory (minus this README) to the new repo root, keeping
   `zephyr/module.yml`, `CMakeLists.txt`, `Kconfig`, `include/` and `src/`.
2. Point `ZEPHYR_EXTRA_MODULES` in `ports/zephyr-cp/CMakeLists.txt` at the new
   checkout, or register it as a project in `zephyr-config/west.yml`.
3. Keep `iobroker.h`'s struct/function names stable — the generated
   board tables are part of the module's de-facto API.
