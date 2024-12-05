# Analog Devices "MAX32" MCUs

This port brings CircuitPython to ADI's "MAX32" series of microcontrollers. These devices are mostly ARM Cortex-M4-based and focus on delivering performance at low-power levels. Currently this port only supports MAX32690.

## Structure of this port

- **`boards/:`** Board-specific definitions including pins, board initialization, etc.
- **`common-hal/:`** Port-specific implementations of CircuitPython common-hal APIs. When a new module is enabled, this is often where the implementation is found. Expected functions for modules in `common-hal` are usually found in `shared-bindings/` or `shared-module/` in the CircuitPy root directory.
- **`linking/:`** Linkerfiles customized for CircuitPython. These are distinct from the linkerfiles used in MSDK as they adopt the structure required by CircuitPython. They may also omit unused features and memory sections, e.g. Mailboxes, RISC-V Flash, & Hyperbus RAM for MAX32690.
- **`msdk:/`** SDK for MAX32 devices. More info on our GitHub: [Analog Devices MSDK GitHub](https://github.com/analogdevicesinc/msdk)
- **`peripherals:/`** Helper files for peripherals such as clocks, gpio, etc. These files tend to be specific to vendor SDKs and provide some useful functions for the common-hal interfaces.
- **`supervisor/:`** Implementation files for the CircuitPython supervisor. This includes port setup, usb, and a filesystem on a storage medium such as SD Card/eMMC, QSPI Flash, or internal flash memory. Currently the internal flash is used. This folder is the most important part of a port's core functionality for CircuitPython.
- **`supervisor/port.c:`** Port-specific startup code including clock initialization, console startup, etc.

- `. :` Build system and high-level interface to the CircuitPython core for the ADI port.

## Building for MAX32 devices

Ensure CircuitPython dependencies are up-to-date by following the CircuitPython introduction on Adafruit's Website: [Building CircuitPython - Introduction](https://learn.adafruit.com/building-circuitpython/introduction). You will require the [ARM GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads), with ARM GCC >=13.x. It is also necessary to fetch all submodules and build the `mpy-cross` compiler, per the "Building CircuitPython" guide.

Ensure the ARM toolchain is contained on your PATH. This can be done in MinGW or WSL by exporting a prefix to the PATH variable. The author's path is included below as an example:

    $ export ARM_GNU_PATH=C:/x-tools/arm-win/arm-none-eabi-w64-i686-13.3rel1/bin
    $ export PATH=$ARM_GNU_PATH:$PATH

This needs to be done each time you open a command environment to build CircuitPython. It can be useful to set up a simple shell script for this.

Once you have built `mpy-cross` and set up your build system for CircuitPython, you can build for MAX32 devices using the following commands:

    $ cd ports/analog
    $ make BOARD=<board from boards/ directory>

Be aware the build may take a long time without parallelizing via the `-jN` flag, where N is the # of cores on your machine.

## Flashing the board

Universal instructions on flashing MAX32 devices this project can be found in the **[MSDK User Guide](https://analogdevicesinc.github.io/msdk/USERGUIDE/)**.

In addition, a user may flash the device by calling `make` with the `flash-msdk` target from within the `ports/analog` directory, as below:

```
$ make BOARD=<target board> flash-msdk
```

This requires the following:
- A MAX32625PICO is connected to the PC via USB
- The PICO board shows up as a "DAPLINK" drive which implements the CMSIS-DAP interface.
- The PICO board is connected to the target board via a 10-pin SWD ribbon cable.
  - If SWD connectors are not keyed, the P1 indicator (red line) on the SWD ribbon cable should match the P1 indicator on the board silkscreen near the 10-pin SWD connector.

## Using the REPL

Once the device is plugged in, it will enumerate via USB as both a USB Serial Device (CDC) and a Mass Storage Device (MSC). You can connect to the Python REPL with your favorite Serial Monitor program e.g. TeraTerm, VS Code, Putty, etc. Use any buadrate with 8-bit, No Parity, 1 Stop Bit (8N1) settings. From this point forward, you can run Python code on the MCU! If you want help with learning CircuitPython-specific code or learning Python in general, a good place to start is Adafruit's ["Welcome to CircuitPython"](https://learn.adafruit.com/welcome-to-circuitpython/) guide.

## Editing code.py

Python code may be executed from `code.py` the `CIRCUITPY:` drive. When editing this file, please be aware that some text editors will work better than others. A list of suggested text editors can be found at Adafruit's guide here: https://learn.adafruit.com/welcome-to-circuitpython/recommended-editors

Once you save `code.py`, it gets written back to the device you are running Circuitpython on, and will automatically run and output it's result to the REPL. You can also automatically reload and run code.py any time from the REPL by pressing CTRL+D.
