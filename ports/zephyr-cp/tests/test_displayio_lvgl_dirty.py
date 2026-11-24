# SPDX-FileCopyrightText: 2026 Mikey Sklar for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""How much of the screen the LVGL compositor redraws for a bitmap change."""

import re

import pytest


@pytest.fixture
def board():
    return "native_native_sim_lvgl"


DIRTY_CODE = """\
import bitmaptools
import board
import displayio

display = board.DISPLAY
display.auto_refresh = False
print("size", display.width, display.height)

for kind in ("rgb565", "palette"):
    if kind == "rgb565":
        bitmap = displayio.Bitmap(display.width, display.height, 65535)
        shader = displayio.ColorConverter(input_colorspace=displayio.Colorspace.RGB565)
    else:
        bitmap = displayio.Bitmap(display.width, display.height, 256)
        shader = displayio.Palette(256)
    group = displayio.Group()
    group.append(displayio.TileGrid(bitmap, pixel_shader=shader))
    display.root_group = group
    display.refresh()

    bitmap[10, 20] = 1
    display.refresh()

    bitmaptools.fill_region(bitmap, 100, 50, 120, 70, 2)
    display.refresh()

    for i in range(40):
        bitmap[200 + i, 100 + i] = 3
    display.refresh()
    print("done", kind)
"""


def _dirty_areas(output):
    return [
        tuple(int(v) for v in m) for m in re.findall(r"lvgl dirty (\d+) (\d+) (\d+) (\d+)", output)
    ]


@pytest.mark.circuitpy_drive({"code.py": DIRTY_CODE})
@pytest.mark.duration(15)
def test_bitmap_writes_redraw_only_what_changed(circuitpython):
    """One-cell TileGrids redraw only the written pixels, for direct and composed sources."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "done rgb565" in output
    assert "done palette" in output
    width, height = (int(v) for v in re.search(r"size (\d+) (\d+)", output).groups())

    areas = _dirty_areas(circuitpython.debug_serial.all_output)
    # Per source kind: showing the group redraws the whole screen, then one pixel, a 20x20
    # square, and a diagonal of 40 single-pixel writes redrawn as its bounding box.
    screen = (0, 0, width - 1, height - 1)
    expected = [screen, (10, 20, 10, 20), (100, 50, 119, 69), (200, 100, 239, 139)] * 2
    assert any(areas[i : i + len(expected)] == expected for i in range(len(areas))), (
        f"expected {expected} in {areas}"
    )
