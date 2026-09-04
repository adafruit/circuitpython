# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test that a plain microcontroller.reset() reboots back into CircuitPython.

This is like the safe-mode test (test_saved_word.py) but without changing the
run mode: no sentinel is armed, so the reboot should be a normal boot that
runs code.py again instead of entering safe mode.
"""

import pytest


RESET_CODE = """\
import microcontroller
reason = microcontroller.cpu.reset_reason
print("reset reason:", reason)
if reason == microcontroller.ResetReason.POWER_ON:
    print("resetting")
    microcontroller.reset()
else:
    print("not resetting")
"""


@pytest.mark.circuitpy_drive({"code.py": RESET_CODE})
@pytest.mark.duration(30)
@pytest.mark.port_resets(4)  # Two startups and two code.py
def test_plain_reset_reboots_into_circuitpython(circuitpython):
    """microcontroller.reset() reboots into code.py and reports a software reset."""
    circuitpython.serial.wait_for("resetting", timeout=30)

    assert circuitpython.reconnect_serial(timeout=30), "simulator did not reboot"

    circuitpython.serial.wait_for("not resetting", timeout=30)

    all_output = circuitpython.serial.all_output
    assert "reset reason: microcontroller.ResetReason.SOFTWARE" in all_output
    assert "Running in safe mode" not in all_output
