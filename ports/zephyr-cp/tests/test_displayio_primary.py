# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Assigning supervisor.runtime.display makes a display primary.

Nothing is assigned automatically in either display lifetime mode. User code
opts in by assigning the property. The supervisor keeps only the assigned
primary display across the end of a VM in full lifetime mode.
"""

import pytest


PRIMARY_CODE = """\
import board
import displayio
import supervisor
import time
import zephyr_display

print('board primary:', supervisor.runtime.display is None)

displayio.release_displays()
print('released:', supervisor.runtime.display is None)

display = zephyr_display.Display()
print('unassigned:', supervisor.runtime.display is None)
supervisor.runtime.display = display
print('assigned:', supervisor.runtime.display is display)
print('done')

while True:
    time.sleep(1)
"""


@pytest.mark.circuitpy_drive({"code.py": PRIMARY_CODE})
@pytest.mark.duration(8)
def test_display_primary_is_explicit(circuitpython):
    """Creating a display does not make it primary until it is assigned."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "board primary: True" in output
    assert "released: True" in output
    assert "unassigned: True" in output
    assert "assigned: True" in output
    assert "done" in output
