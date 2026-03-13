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
