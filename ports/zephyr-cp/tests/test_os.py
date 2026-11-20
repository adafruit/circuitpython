# SPDX-FileCopyrightText: 2026 Dan Halbert for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test the os core module."""

import pytest


UNAME_CODE = """\
import os

u = os.uname()
print(f"sysname: {u.sysname}")
print(f"nodename: {u.nodename}")
print(f"machine: {u.machine}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": UNAME_CODE})
def test_os_uname(circuitpython):
    """sysname is "CircuitPython". With no Wi-Fi or Ethernet, nodename is the microcontroller name."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "sysname: CircuitPython" in output
    nodename = output.split("nodename: ", 1)[1].split("\n", 1)[0].strip()
    assert nodename
    # machine is "<board name> with <microcontroller name>".
    assert f" with {nodename}" in output
    assert "done" in output
