# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""bitmaptools writes to a Bitmap that a TileGrid is showing.

bitmaptools writes pixels directly and then reports the changed rectangle through the
Bitmap dirty API. Both compositors have to pick that rectangle up and redraw the part of
the TileGrid it reaches.
"""

import pytest
from PIL import Image

_SCENE = """\
import board
import bitmaptools
import displayio
import time

display = board.DISPLAY

palette = displayio.Palette(4)
palette[0] = 0x000000
palette[1] = 0xFF0000
palette[2] = 0x00FF00
palette[3] = 0x0000FF

bitmap = displayio.Bitmap(40, 40, 4)
bitmap.fill(0)

group = displayio.Group()
group.append(displayio.TileGrid(bitmap, pixel_shader=palette, x=40, y=40))

display.auto_refresh = False
display.root_group = group
display.refresh()
print('first')
time.sleep(10)

# A filled rectangle, a line and a blit. Each reports its changed area before it
# writes the pixels, as bitmaptools does.
bitmaptools.fill_region(bitmap, 0, 0, 20, 10, 1)
bitmaptools.draw_line(bitmap, 0, 20, 39, 20, 2)
source = displayio.Bitmap(10, 10, 4)
source.fill(3)
bitmaptools.blit(bitmap, source, x=25, y=25)
display.refresh()
print('second')
while True:
    time.sleep(1)
"""

RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
BLACK = (0, 0, 0)


def _expected(width, height, changed):
    frame = [[BLACK] * width for _ in range(height)]
    if not changed:
        return frame
    # fill_region(bitmap, 0, 0, 20, 10, 1)
    for y in range(40, 50):
        for x in range(40, 60):
            frame[y][x] = RED
    # draw_line(bitmap, 0, 20, 39, 20, 2)
    for x in range(40, 80):
        frame[60][x] = GREEN
    # blit(bitmap, source, x=25, y=25)
    for y in range(65, 75):
        for x in range(65, 75):
            frame[y][x] = BLUE
    return frame


def _compare(path, changed):
    with Image.open(path) as img:
        rgb = img.convert("RGB")
    expected = _expected(rgb.width, rgb.height, changed)
    mismatches = []
    for y in range(rgb.height):
        for x in range(rgb.width):
            actual = rgb.getpixel((x, y))
            if actual != expected[y][x]:
                mismatches.append((x, y, expected[y][x], actual))
    assert not mismatches, f"{len(mismatches)} pixels differ, first: {mismatches[:8]}"


@pytest.mark.circuitpy_drive({"code.py": _SCENE})
@pytest.mark.display(capture_times_ns=[8_000_000_000, 20_000_000_000])
@pytest.mark.duration(24)
def test_bitmaptools_render(board, circuitpython):
    """bitmaptools changes on a displayed Bitmap reach both compositors."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "first" in output
    assert "second" in output

    captures = circuitpython.display_capture_paths()
    if len(captures) < 2 or not captures[1].exists():
        pytest.skip("display capture was not produced")

    _compare(captures[0], changed=False)
    _compare(captures[1], changed=True)
