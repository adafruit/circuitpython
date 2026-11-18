# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""A display created when no primary display is set becomes the primary display.

The supervisor keeps only the primary display across the end of a VM. If a
display created by user code (a USB video framebuffer, for example) didn't
default to primary, it was released when code.py finished and the terminal and
REPL had nowhere to draw.
"""

import pytest


PRIMARY_CODE = """\
import board
import displayio
import supervisor
import time
import zephyr_display

print('board primary:', supervisor.runtime.display is board.DISPLAY)

displayio.release_displays()
print('released:', supervisor.runtime.display is None)

display = zephyr_display.Display()
print('new primary:', supervisor.runtime.display is display)
print('done')

while True:
    time.sleep(1)
"""


@pytest.mark.circuitpy_drive({"code.py": PRIMARY_CODE})
@pytest.mark.duration(8)
def test_display_defaults_to_primary(circuitpython):
    """Releasing and re-creating the display leaves the new one primary."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "board primary: True" in output
    assert "released: True" in output
    assert "new primary: True" in output
    assert "done" in output
