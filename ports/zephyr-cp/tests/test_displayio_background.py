# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""The undrawn display background is black, including after a display is re-created.

LVGL fills its bottom layer with white wherever nothing else covers an area, so
the LVGL compositor makes each display's screen opaque black. That setup used
to run once, for whichever screen existed at the time. Display classes
(BusDisplay, FramebufferDisplay) delete and re-create the LVGL display when
they are constructed, so the screen of the next one came up white.

This draws a red square on a black screen, releases the display, creates a new
one and draws the same scene again, then checks a captured frame from each
display. A compositor that doesn't style the re-created screen fails on the
second capture.
"""

import pytest
from PIL import Image


BACKGROUND_CODE = """\
import board
import displayio
import time
import zephyr_display


def draw(display):
    # A 16-bit bitmap needs no palette, so the square shows the same color before
    # and after the display is re-created.
    bitmap = displayio.Bitmap(8, 8, 65536)
    for y in range(8):
        for x in range(8):
            bitmap[x, y] = 0xF800
    group = displayio.Group()
    group.append(displayio.TileGrid(bitmap,
        pixel_shader=displayio.ColorConverter(input_colorspace=displayio.Colorspace.RGB565),
        x=16, y=16))
    display.auto_refresh = False
    display.root_group = group
    display.refresh()


display = board.DISPLAY
draw(display)
print('first')
time.sleep(10)

displayio.release_displays()
display = zephyr_display.Display(auto_refresh=False)
draw(display)
print('second')

while True:
    time.sleep(1)
"""


def _assert_black_background(path, index):
    with Image.open(path) as img:
        rgb = img.convert("RGB")
    background = rgb.getpixel((0, 0))
    assert background == (0, 0, 0), (
        f"capture {index}: undrawn background is {background}, expected black"
    )
    square = rgb.getpixel((16, 16))
    assert square == (255, 0, 0), (
        f"capture {index}: the TileGrid didn't draw at (16, 16): got {square}"
    )


@pytest.mark.circuitpy_drive({"code.py": BACKGROUND_CODE})
@pytest.mark.display(capture_times_ns=[6_000_000_000, 16_000_000_000])
@pytest.mark.duration(20)
def test_background_is_black_after_display_recreate(circuitpython):
    """A re-created display still has a black background."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "first" in output
    assert "second" in output

    captures = circuitpython.display_capture_paths()
    if len(captures) < 2 or not captures[1].exists():
        pytest.skip("display capture was not produced")

    _assert_black_background(captures[0], 0)
    _assert_black_background(captures[1], 1)
