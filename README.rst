CircuitPython
=============

.. contents:: Table of Contents
   :depth: 3
   :local:

.. image:: https://s3.amazonaws.com/adafruit-circuit-python/CircuitPython_Repo_header_logo.png

|Build Status| |Doc Status| |License| |Discord| |Weblate|

Quick Links
-----------
- `Official Website <https://circuitpython.org>`__
- `Downloads <#get-circuitpython>`__
- `Documentation <#documentation>`__ 
- `Contributing Guide <#contributing>`__
- `Community Discord <https://adafru.it/discord>`__

About CircuitPython
-------------------

**CircuitPython** is a *beginner friendly*, open source version of Python for tiny, inexpensive
computers called microcontrollers. Microcontrollers are the brains of many electronics including a
wide variety of development boards used to build hobby projects and prototypes. CircuitPython in
electronics is one of the best ways to learn to code because it connects code to reality. Simply
install CircuitPython on a supported USB board usually via drag and drop and then edit a ``code.py``
file on the CIRCUITPY drive. The code will automatically reload. No software installs are needed
besides a text editor (we recommend `Mu <https://codewith.mu/>`_ for beginners.)

Starting with CircuitPython 7.0.0, some boards may only be connectable over Bluetooth Low Energy
(BLE). Those boards provide serial and file access over BLE instead of USB using open protocols.
(Some boards may use both USB and BLE.) BLE access can be done from a variety of apps including
`code.circuitpython.org <https://code.circuitpython.org>`_.

CircuitPython features unified Python core APIs and a growing list of 300+ device libraries and
drivers that work with it. These libraries also work on single board computers with regular
Python via the `Adafruit Blinka Library <https://github.com/adafruit/Adafruit_Blinka>`_.

CircuitPython is based on `MicroPython <https://micropython.org>`_. See
`below <#differences-from-micropython>`_ for differences. Most, but not all, CircuitPython
development is sponsored by `Adafruit <https://adafruit.com>`_ and is available on their educational
development boards. Please support both MicroPython and Adafruit.

Get CircuitPython
------------------

Official binaries for all supported boards are available through
`circuitpython.org/downloads <https://circuitpython.org/downloads>`_. The site includes stable, unstable and
continuous builds. Full release notes are available through
`GitHub releases <https://github.com/adafruit/circuitpython/releases>`_ as well.

Documentation
-------------

**Official Resources:**
- Guides and videos: `Adafruit Learning System <https://learn.adafruit.com/category/circuitpython>`__
- API Reference: `Read the Docs <http://circuitpython.readthedocs.io/>`__
- `Awesome CircuitPython <https://github.com/adafruit/awesome-circuitpython>`__ resource collection

**Technical Documentation:**
- `Building CircuitPython <BUILDING.md>`__ - Complete build instructions
- `WebUSB Implementation <WEBUSB_README.md>`__ - Details about WebUSB support

**Getting Started Guides:**
- `Welcome to CircuitPython <https://learn.adafruit.com/welcome-to-circuitpython>`__ - First steps tutorial
- `CircuitPython Essentials <https://learn.adafruit.com/circuitpython-essentials>`__ - Core concepts and examples
- `Community Bundle <https://github.com/adafruit/CircuitPython_Community_Bundle>`__ - Community-contributed libraries
- `Adafruit Learning System <https://learn.adafruit.com/category/circuitpython>`__ - Full collection of guides

Key Differences from MicroPython
-------------------------------

Core Features
~~~~~~~~~~~~~
- Native USB/BLE support for file editing without special tools
- Floats (decimals) enabled for all builds
- Translated error messages in 10+ languages
- Simplified concurrency model:
  - No interrupts or threading
  - async/await available on some boards
  - Native modules handle tasks requiring concurrency (e.g., audio playback)

Behavior Differences
~~~~~~~~~~~~~~~~~~~

-  The order that files are run and the state that is shared between
   them. CircuitPython's goal is to clarify the role of each file and
   make each file independent from each other.

   -  ``boot.py`` runs only once on start up before
      workflows are initialized. This lays the ground work for configuring USB at
      startup rather than it being fixed. Since serial is not available,
      output is written to ``boot_out.txt``.
   -  ``code.py`` (or ``main.py``) is run after every reload until it
      finishes or is interrupted. After it is done running, the vm and
      hardware is reinitialized. **This means you cannot read state from**
      ``code.py`` **in the REPL anymore, as the REPL is a fresh vm.** CircuitPython's goal for this
      change includes reducing confusion about pins and memory being used.
   -  After the main code is finished the REPL can be entered by pressing any key.
      - If the file ``repl.py`` exists, it is executed before the REPL Prompt is shown
      - In safe mode this functionality is disabled, to ensure the REPL Prompt can always be reached
   -  Autoreload state will be maintained across reload.

-  Adds a safe mode that does not run user code after a hard crash or brown out. This makes it
   possible to fix code that causes nasty crashes by making it available through mass storage after
   the crash. A reset (the button) is needed after it's fixed to get back into normal mode.
-  A 1 second delay is added to the boot process during which time the status LED will flash, and
   resetting the device or pressing the boot button will force the device into safe mode. This delay
   can be removed by a compile time option (``CIRCUITPY_SKIP_SAFE_MODE_WAIT``).
-  Safe mode may be handled programmatically by providing a ``safemode.py``.
   ``safemode.py`` is run if the board has reset due to entering safe mode, unless the safe mode
   initiated by the user by pressing button(s).
   USB is not available so nothing can be printed.
   ``safemode.py`` can determine why the safe mode occurred
   using ``supervisor.runtime.safe_mode_reason``, and take appropriate action. For instance,
   if a hard crash occurred, ``safemode.py`` may do a ``microcontroller.reset()``
   to automatically restart despite the crash.
   If the battery is low, but is being charged, ``safemode.py`` may put the board in deep sleep
   for a while. Or it may simply reset, and have ``code.py`` check the voltage and do the sleep.
-  RGB status LED indicating CircuitPython state.
   - One green flash - code completed without error.
   - Two red flashes - code ended due to an exception.
   - Three yellow flashes - safe mode. May be due to CircuitPython internal error.
-  Re-runs ``code.py`` or other main file after file system writes by a workflow. (Disable with
   ``supervisor.disable_autoreload()``)
-  Autoreload is disabled while the REPL is active.
-  ``code.py`` may also be named ``code.txt``, ``main.py``, or ``main.txt``.
-  ``boot.py`` may also be named ``boot.txt``.
-  ``safemode.py`` may also be named ``safemode.txt``.

API
~~~

-  Unified hardware APIs. Documented on
   `ReadTheDocs <https://circuitpython.readthedocs.io/en/latest/shared-bindings/index.html>`_.
-  API docs are Python stubs within the C files in ``shared-bindings``.
-  No ``machine`` API.

Modules
~~~~~~~

-  No module aliasing. (``uos`` and ``utime`` are not available as
   ``os`` and ``time`` respectively.) Instead ``os``, ``time``, and
   ``random`` are CPython compatible.
-  New ``storage`` module which manages file system mounts.
   (Functionality from ``uos`` in MicroPython.)
-  Modules with a CPython counterpart, such as ``time``, ``os`` and
   ``random``, are strict
   `subsets <https://circuitpython.readthedocs.io/en/latest/shared-bindings/time/__init__.html>`__
   of their `CPython
   version <https://docs.python.org/3.4/library/time.html?highlight=time#module-time>`__.
   Therefore, code from CircuitPython is runnable on CPython but not
   necessarily the reverse.
-  tick count is available as
   `time.monotonic() <https://circuitpython.readthedocs.io/en/latest/shared-bindings/time/__init__.html#time.monotonic>`__

--------------

Project Structure
-----------------

Here is an overview of the top-level source code directories.

Core
~~~~

The core code of
`MicroPython <https://github.com/micropython/micropython>`__ is shared
amongst ports including CircuitPython:

-  ``docs`` High level user documentation in Sphinx reStructuredText
   format.
-  ``drivers`` External device drivers written in Python.
-  ``examples`` A few example Python scripts.
-  ``extmod`` Shared C code used in multiple ports' modules.
-  ``lib`` Shared core C code including externally developed libraries
   such as FATFS.
-  ``logo`` The CircuitPython logo.
-  ``mpy-cross`` A cross compiler that converts Python files to byte
   code prior to being run in MicroPython. Useful for reducing library
   size.
-  ``py`` Core Python implementation, including compiler, runtime, and
   core library.
-  ``shared-bindings`` Shared definition of Python modules, their docs
   and backing C APIs. Ports must implement the C API to support the
   corresponding module.
-  ``shared-module`` Shared implementation of Python modules that may be
   based on ``common-hal``.
-  ``tests`` Test framework and test scripts.
-  ``tools`` Various tools, including the pyboard.py module.

Ports
~~~~~

Ports include the code unique to a microcontroller line.

The following ports are available: ``atmel-samd``, ``cxd56``, ``espressif``, ``litex``, ``mimxrt10xx``, ``nordic``, ``raspberrypi``, ``renode``, ``silabs`` (``efr32``), ``stm``, ``unix``.

However, not all ports are fully functional. Some have limited functionality and known serious bugs.
For details, refer to the **Port status** section in the `latest release <https://github.com/adafruit/circuitpython/releases/latest>`__ notes.

Boards
~~~~~~

-  Each ``port`` has a ``boards`` directory containing boards
   which belong to a specific microcontroller line.
-  A list of native modules supported by a particular board can be found
   `here <https://circuitpython.readthedocs.io/en/latest/shared-bindings/support_matrix.html>`__.

<!--
SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)

SPDX-License-Identifier: MIT
-->

# Building CircuitPython

Detailed guides on how to build CircuitPython can be found in the Adafruit Learn system at
https://learn.adafruit.com/building-circuitpython/

## Setup

Please ensure you set up your build environment appropriately, as per the guide.  You will need:

* Linux: https://learn.adafruit.com/building-circuitpython/linux
* MacOS: https://learn.adafruit.com/building-circuitpython/macos
* Windows Subsystem for Linux (WSL): https://learn.adafruit.com/building-circuitpython/windows-subsystem-for-linux

### Submodules

This project has a bunch of git submodules.  You will need to update them regularly.

In the root folder of the CircuitPython repository, execute the following:

    make fetch-all-submodules

Or, in the ports directory for the particular port you are building, do:

    make fetch-port-submodules

### Required Python Packages

Failing to install these will prevent from properly building.

    pip3 install -r requirements-dev.txt

If you run into an error installing minify_html, you may need to install `rust`.

### mpy-cross

As part of the build process, mpy-cross is needed to compile .py files into .mpy files.
To compile (or recompile) mpy-cross:

    make -C mpy-cross

## Building

There a number of ports of CircuitPython!  To build for your board, change to the appropriate ports directory and build.

Examples:

    cd ports/atmel-samd
    make BOARD=circuitplayground_express

    cd ports/nordic
    make BOARD=circuitplayground_bluefruit

If you aren't sure what boards exist, have a peek in the boards subdirectory of your port.
If you have a fast computer with many cores, consider adding `-j` to your build flags, such as `-j17` on
a 6-core 12-thread machine.

## Testing

If you are working on changes to the core language, you might find it useful to run the test suite.
The test suite in the top level `tests` directory.  It needs the unix port to run.

    cd ports/unix
    make axtls
    make micropython

Then you can run the test suite:

    cd ../../tests
    ./run-tests.py

A successful run will say something like

    676 tests performed (19129 individual testcases)
    676 tests passed
    30 tests skipped: buffered_writer builtin_help builtin_range_binop class_delattr_setattr cmd_parsetree extra_coverage framebuf1 framebuf16 framebuf2 framebuf4 framebuf8 framebuf_subclass mpy_invalid namedtuple_asdict non_compliant resource_stream schedule sys_getsizeof urandom_extra ure_groups ure_span ure_sub ure_sub_unmatched vfs_basic vfs_fat_fileio1 vfs_fat_fileio2 vfs_fat_more vfs_fat_oldproto vfs_fat_ramdisk vfs_userfs

## Debugging

The easiest way to debug CircuitPython on hardware is with a JLink device, JLinkGDBServer, and an appropriate GDB.
Instructions can be found at https://learn.adafruit.com/debugging-the-samd21-with-gdb

If using JLink, you'll need both the `JLinkGDBServer` and `arm-none-eabi-gdb` running.

Example:

    JLinkGDBServer -if SWD -device ATSAMD51J19
    arm-none-eabi-gdb build-metro_m4_express/firmware.elf -iex "target extended-remote :2331"

If your port/build includes `arm-none-eabi-gdb-py`, consider using it instead, as it can be used for better register
debugging with https://github.com/bnahill/PyCortexMDebug

## Code Quality Checks

We apply code quality checks using pre-commit.  Install pre-commit once per system with

    python3 -mpip install pre-commit

Activate it once per git clone with

    pre-commit install

Pre-commit also requires some additional programs to be installed through your package manager:

 * Standard Unix tools such as make, find, etc
 * The gettext package, any modern version
 * uncrustify version 0.71 (0.72 is also tested and OK; 0.75 is not OK)

Each time you create a git commit, the pre-commit quality checks will be run.  You can also run them e.g., with `pre-commit run foo.c` or `pre-commit run --all` to run on all files whether modified or not.

Some pre-commit quality checks require your active attention to resolve, others (such as the formatting checks of uncrustify) are made automatically and must simply be incorporated into your code changes by committing them.

<!--
SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)

SPDX-License-Identifier: MIT
-->

# WebUSB Serial Support

To date, this has only been tested on one port (espressif), on one board (espressif_kaluga_1).

## What it does

If you have ever used CircuitPython on a platform with a graphical LCD display, you have probably
already seen multiple "consoles" in use (although the LCD console is "output only").

New compile-time option CIRCUITPY_USB_VENDOR enables an additional "console" that can be used in
parallel with the original (CDC) serial console.

Web pages that support the WebUSB standard can connect to the "vendor" interface and activate
this WebUSB serial console at any time.

You can type into either console, and CircuitPython output is sent to all active consoles.

One example of a web page you can use to test drive this feature can be found at:

https://adafruit.github.io/Adafruit_TinyUSB_Arduino/examples/webusb-serial/index.html

## How to enable

Update your platform's mpconfigboard.mk file to enable and disable specific types of USB interfaces.

CIRCUITPY_USB_HID = xxx
CIRCUITPY_USB_MIDI = xxx
CIRCUITPY_USB_VENDOR = xxx

On at least some of the hardware platforms, the maximum number of USB endpoints is fixed.
For example, on the ESP32S2, you must pick only one of the above 3 interfaces to be enabled.

Original espressif_kaluga_1 mpconfigboard.mk settings:

CIRCUITPY_USB_HID = 1
CIRCUITPY_USB_MIDI = 0
CIRCUITPY_USB_VENDOR = 0

Settings to enable WebUSB instead:

CIRCUITPY_USB_HID = 0
CIRCUITPY_USB_MIDI = 0
CIRCUITPY_USB_VENDOR = 1

Notice that to enable VENDOR on ESP32-S2, we had to give up HID. There may be platforms that can have both, or even all three.

## Implementation Notes

CircuitPython uses the tinyusb library.

The tinyusb library already has support for WebUSB serial.
The tinyusb examples already include a "WebUSB serial" example.

    Sidenote - The use of the term "vendor" instead of "WebUSB" was done to match tinyusb.

Basically, this feature was ported into CircuitPython by pulling code snippets out of the
tinyusb example, and putting them where they best belonged in the CircuitPython codebase.

### TODO: This needs to be reworked for dynamic USB descriptors.

`Back to Top <#circuitpython>`__

.. |Build Status| image:: https://github.com/adafruit/circuitpython/workflows/Build%20CI/badge.svg
   :target: https://github.com/adafruit/circuitpython/actions?query=branch%3Amain
.. |Doc Status| image:: https://readthedocs.org/projects/circuitpython/badge/?version=latest
   :target: http://circuitpython.readthedocs.io/
.. |Discord| image:: https://img.shields.io/discord/327254708534116352.svg
   :target: https://adafru.it/discord
.. |License| image:: https://img.shields.io/badge/License-MIT-brightgreen.svg
   :target: https://choosealicense.com/licenses/mit/
.. |Weblate| image:: https://hosted.weblate.org/widgets/circuitpython/-/svg-badge.svg
   :target: https://hosted.weblate.org/engage/circuitpython/?utm_source=widget
