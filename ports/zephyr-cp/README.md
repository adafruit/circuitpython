# Zephyr

This is an initial port of CircuitPython onto Zephyr. We intend on migrating all
existing boards onto Zephyr. To start, we'll only support new boards. Existing
boards will be switched as the Zephyr port reaches parity with the existing
implementation.

## Getting Started

First, install Zephyr tools (see [Zephyr's Getting Started Guide](https://docs.zephyrproject.org/4.0.0/develop/getting_started/index.html)). (These are `fish` commands because that's what Scott uses.)


```sh
pip install west
west init -l zephyr-config
west update
west zephyr-export
pip install -r zephyr/scripts/requirements.txt
west sdk install
```

Now to build from `ports/zephyr-cp`:

```sh
make BOARD=nordic_nrf7002dk
```

This uses Zephyr's cmake to generate Makefiles that then delegate to
`tools/cpbuild/build_circuitpython.py` to build the CircuitPython bits in parallel.

## Native simulator build container

Building the native sim requires `libsdl2-dev:i386` and other 32bit dependencies that
can cause conflicts on 64bit systems resulting in the removal of 64bit versions of critical
software such as the display manager and network manager. A Containerfile and a few scripts
are provided to set up a container to make the native sim build inside without affecting the
host system.

The container automatically mounts this instance of the circuitpython repo inside at
`/home/dev/circuitpython`. Changes made in the repo inside the container and on the host PC
will sync automatically between host and container.

To use the container file:

1. Build the container with `podman build -t zephyr-cp-dev -f native_sim_build_Containerfile .`
2. Run/Start the container by running `./native_sim_build_run_container.sh` on the host PC.
   The script will automatically run or start based on whether the container has been run before.
3. Init requirements inside the container with `./native_sim_build_init_container.sh`

To delete the container and cleanup associated files:
```sh
podman ps -a --filter ancestor=zephyr-cp-dev -q | xargs -r podman rm -f
podman rmi zephyr-cp-dev
podman image prune -f
podman rm -f zcp
```

## Running the native simulator

From `ports/zephyr-cp`, run:

```sh
make run-sim
```

`run-sim` starts the native simulator in realtime.
It prints the PTY path to connect to the simulator REPL.
If a local `./CIRCUITPY/` folder exists, its files are used as the simulator's CIRCUITPY drive.

Edit files in `./CIRCUITPY` (for example `code.py`) and rerun `make run-sim` to test changes.

## Shields

Board defaults can be set in `boards/<vendor>/<board>/circuitpython.toml`:

```toml
SHIELDS = ["shield1", "shield2"]
```

For example, `boards/renesas/ek_ra8d1/circuitpython.toml` enables:

```toml
SHIELDS = ["rtkmipilcdb00000be"]
```

You can override shield selection from the command line:

```sh
# Single shield
make BOARD=renesas_ek_ra8d1 SHIELD=rtkmipilcdb00000be

# Multiple shields (comma, semicolon, or space separated)
make BOARD=my_vendor_my_board SHIELDS="shield1,shield2"
```

Behavior and precedence:

- If `SHIELD` or `SHIELDS` is explicitly provided, it overrides board defaults.
- If neither is provided, defaults from `circuitpython.toml` are used.
- Use `SHIELD=` (empty) to disable a board default shield for one build.

## Pin names

Human readable pin names (the `board` module) come from the devicetree by
default: `gpio-leds` and `gpio-keys` labels, node aliases, and connector
`gpio-map`s. Boards can add names without any devicetree involvement by
listing them in `boards/<vendor>/<board>/circuitpython.toml` under `[pins]`.
Each entry maps a board module name to the pin number exposed by the board's
hardware: the SoC package pin (or, for ball grid array packages, the
datasheet's ball id, e.g. `"B2"`), or the castellated module pin when the
board uses a module like the Raytac MDBT50Q:

```toml
[pins]
LED = 17      # QFN package pin number
SDA = "B2"   # ball id for BGA/CSP packages
D13 = 8      # MDBT50Q-1MV2 module pin number
```

The build resolves each package pin to a SoC pad using the iobroker package
pin map selected by `CONFIG_IOBROKER_PACKAGE_<PACKAGE>`
(`modules/iobroker/packages/<package>.toml`) and exposes the name on the
matching pad in the `board` module. A name that already maps to the same pin
(from the devicetree or an earlier entry) is deduplicated; a name that maps
to two different pins is a build error. The package map must not be `CUSTOM`
or missing, and the pad must be on an enabled GPIO controller; otherwise the
build fails with an error naming the offending entry.

## Connector names

Devicetree connector nodes (`gpio-map`) get their names from a generic
per-compatible list in `cptools/zephyr2cp.py`. A board whose silkscreen
differs can override them per position in `circuitpython.toml` under
`[connectors.<node label>]`, keying the gpio-map position (the header pin
number, as a string) to a name:

```toml
[connectors.nordic_expansion_header]
0 = "EXP_00"   # header GPIO 00
21 = "EXP_21"  # header GPIO 21 (QSPI CS)
```

Positions left out of the table get no name.

## Testing other boards

[Any Zephyr board](https://docs.zephyrproject.org/latest/boards/index.html#) can
be used with CircuitPython. To test a different board, use `west` directly to
build the board. The build will do its best to support as much as possible. By
default the Zephyr console will be used for output. USB support is limited by
initialization support in `supervisor/usb.c`. Only flash regions not used by
Zephyr are used for CIRCUITPY. A manual `circuitpython` partition can be
specified instead.

For example, to test the `nrf52840dk` board:

```sh
west build -b nrf52840dk/nrf52840
```

This is already supported in `ports/nordic` as `pca10056`.

The manifest in `zephyr-config/west.yml` skips the HALs of vendors that no board
here uses, so a board from one of them needs its entry removed from the
`name-blocklist` and a `west update` to fetch it. For the
`esp32s3_devkitc` board, that is `hal_espressif`:

```sh
west update
west build -b esp32s3_devkitc/esp32s3/procpu
```
