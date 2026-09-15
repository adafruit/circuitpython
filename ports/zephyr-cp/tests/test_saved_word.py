# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test that the safe-mode saved word survives a hard reboot on native_sim/bsim."""

import pytest


SAFE_MODE_RESET_CODE = """\
import microcontroller
if microcontroller.cpu.reset_reason == microcontroller.ResetReason.POWER_ON:
    microcontroller.on_next_reset(microcontroller.RunMode.SAFE_MODE)
    print("resetting")
    microcontroller.reset()
else:
    print("not resetting")
"""


@pytest.mark.circuitpy_drive({"code.py": SAFE_MODE_RESET_CODE})
@pytest.mark.duration(30)
@pytest.mark.port_resets(4)
def test_saved_word_survives_reboot_into_safe_mode(circuitpython):
    """The saved word persists across a hard reboot and triggers safe mode."""
    circuitpython.serial.wait_for("resetting", timeout=30)

    # microcontroller.reset() re-execs the process; the UART PTY is O_CLOEXEC so
    # a new one is opened after reboot. Reconnect to it.
    assert circuitpython.reconnect_serial(timeout=30), "simulator did not reboot"

    # The next boot restores the saved word (the SAFE_MODE sentinel) and enters
    # safe mode instead of running code.py.
    circuitpython.serial.wait_for("Running in safe mode", timeout=30)

    assert "Running in safe mode" in circuitpython.serial.all_output
