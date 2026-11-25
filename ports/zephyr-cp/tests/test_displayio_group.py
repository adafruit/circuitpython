# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Integer Group scaling in the LVGL compositor."""

import pytest
from PIL import Image


GROUP_SCALE_SCENE = """\
import board
import displayio
import time

palette = displayio.Palette(4)
palette[0] = 0x000000
palette[1] = 0xFF0000
palette[2] = 0x00FF00
palette[3] = 0x0000FF


def solid(width, height, color_index):
    bitmap = displayio.Bitmap(width, height, 4)
    for y in range(height):
        for x in range(width):
            bitmap[x, y] = color_index
    return bitmap


root = displayio.Group()

# A default Group must retain 1x scale. Q8 conversion must not be applied twice
# to a Group that has already been through the Python constructor.
default = displayio.Group(x=5, y=7)
default.append(displayio.TileGrid(solid(4, 3, 1), pixel_shader=palette))
root.append(default)

# Integer scaling must produce whole pixels without resampling.
scaled = displayio.Group(scale=2, x=20, y=5)
scaled.append(displayio.TileGrid(solid(2, 2, 2), pixel_shader=palette))
root.append(scaled)

display = board.DISPLAY
display.auto_refresh = False
display.root_group = root
display.refresh()

# Append after the Group is attached to exercise the adoption path separately
# from the root-attachment path used above.
scaled.append(displayio.TileGrid(solid(2, 1, 3), pixel_shader=palette, x=4, y=0))
display.refresh()
print('ready')

while True:
    time.sleep(1)
"""


@pytest.mark.parametrize(
    "board", ["native_native_sim_lvgl", "native_native_sim_lvgl_asan"], indirect=True
)
@pytest.mark.circuitpy_drive({"code.py": GROUP_SCALE_SCENE})
@pytest.mark.display(capture_times_ns=[4_000_000_000])
@pytest.mark.duration(7)
def test_displayio_integer_group_scale(circuitpython):
    """Default, integer and post-attachment Group scales retain all pixels."""
    circuitpython.wait_until_done()
    assert "ready" in circuitpython.serial.all_output

    captures = circuitpython.display_capture_paths()
    if not captures or not captures[0].exists():
        pytest.skip("display capture was not produced")

    with Image.open(captures[0]) as image:
        actual = image.convert("RGB")
    expected = Image.new("RGB", actual.size, (0, 0, 0))

    def draw(x0, y0, width, height, color):
        for y in range(y0, y0 + height):
            for x in range(x0, x0 + width):
                expected.putpixel((x, y), color)

    draw(5, 7, 4, 3, (255, 0, 0))
    draw(20, 5, 4, 4, (0, 255, 0))
    draw(28, 5, 4, 2, (0, 0, 255))

    mismatches = []
    for y in range(actual.height):
        for x in range(actual.width):
            actual_pixel = actual.getpixel((x, y))
            expected_pixel = expected.getpixel((x, y))
            if actual_pixel != expected_pixel:
                mismatches.append((x, y, expected_pixel, actual_pixel))
    assert not mismatches, f"{len(mismatches)} pixels differ, first: {mismatches[:8]}"


SUPERVISOR_SPLASH_SCENE = """\
import time

# Leave the supervisor's static splash Group attached. In particular, don't replace
# it with a Python-created Group, whose constructor already converts scale to Q8.
while True:
    time.sleep(1)
"""


@pytest.mark.parametrize(
    "board", ["native_native_sim_lvgl", "native_native_sim_lvgl_asan"], indirect=True
)
@pytest.mark.circuitpy_drive({"code.py": SUPERVISOR_SPLASH_SCENE})
@pytest.mark.display(capture_times_ns=[4_000_000_000])
@pytest.mark.duration(7)
def test_supervisor_splash_group_scale(circuitpython):
    """The static supervisor splash renders Blinka at native_sim's 1x scale."""
    circuitpython.wait_until_done()
    captures = circuitpython.display_capture_paths()
    if not captures or not captures[0].exists():
        pytest.skip("display capture was not produced")

    with Image.open(captures[0]) as image:
        actual = image.convert("RGB")

    # Blinka is a 12x12 TileGrid in the static splash Group. Native_sim selects
    # scale 1 for its 320-pixel display. A raw 1 was interpreted as Q8 1 / 256
    # and clipped the grid to zero size. Use Blinka's unique chromatic colors so
    # white terminal text in the same region cannot make this test pass.
    logo = []
    for y in range(24):
        for x in range(24):
            pixel = actual.getpixel((x, y))
            if pixel not in ((0, 0, 0), (255, 255, 255)):
                logo.append((x, y))
    assert len(logo) > 50, "the supervisor splash is missing Blinka's colored pixels"
    assert max(x for x, y in logo) == 11, "Blinka has the wrong horizontal extent"
    assert max(y for x, y in logo) == 11, "Blinka has the wrong vertical extent"
