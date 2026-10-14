# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test the watchdog module against native_sim's counter-based watchdog.

The native_sim devicetree aliases watchdog0 to a zephyr,counter-watchdog
device built on the simulator's counter, which supports the expiry callback,
so the RAISE behavior can be exercised: an expired watchdog must raise
watchdog.WatchDogTimeout instead of resetting the simulator. RESET mode resets
the simulator, which boots straight back into CircuitPython.
"""

import pytest


RAISE_MODE_CODE = """\
import microcontroller
import watchdog
import time

wdt = microcontroller.watchdog
wdt.timeout = 1.0
wdt.mode = watchdog.WatchDogMode.RAISE
print("EXPIRING")
try:
    while True:
        time.sleep(10)
except watchdog.WatchDogTimeout:
    print("CAUGHT:", wdt.timeout)
except Exception as e:
    print("OTHER:", type(e))
"""


@pytest.mark.circuitpy_drive({"code.py": RAISE_MODE_CODE})
@pytest.mark.duration(30)
def test_watchdog_raise_mode(circuitpython):
    """An expired watchdog in RAISE mode raises WatchDogTimeout."""
    circuitpython.serial.wait_for("EXPIRING", timeout=30)
    circuitpython.serial.wait_for("CAUGHT: 1.0", timeout=30)


RESET_MODE_CODE = """\
import microcontroller
import watchdog
import time

wdt = microcontroller.watchdog
wdt.timeout = 1.0
wdt.mode = watchdog.WatchDogMode.RESET
print("ARMED")
try:
    while True:
        time.sleep(10)
except watchdog.WatchDogTimeout:
    print("UNREACHABLE: caught in RESET mode")
"""


@pytest.mark.circuitpy_drive({"code.py": RESET_MODE_CODE})
@pytest.mark.duration(30)
@pytest.mark.port_resets(3)
def test_watchdog_reset_mode(circuitpython):
    """An expired watchdog in RESET mode reboots the simulator into CircuitPython."""
    circuitpython.serial.wait_for("ARMED", timeout=30)
    assert circuitpython.reconnect_serial(timeout=30), "simulator did not reboot"

    circuitpython.serial.wait_for("ARMED", timeout=30)

    all_output = circuitpython.serial.all_output
    assert "UNREACHABLE" not in all_output
