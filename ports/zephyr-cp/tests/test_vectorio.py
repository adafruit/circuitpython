# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""vectorio tests that run against both displayio compositors.

native_sim is built twice: once with the legacy scanline compositor
(``native_native_sim``) and once with the LVGL compositor
(``native_native_sim_lvgl``). vectorio has an implementation for each one, so
these tests check that the LVGL implementation behaves like the legacy one: the
same API, the same shapes in the same place and the same pixels on the screen.

The golden image was captured from the legacy compositor and is compared by
both. The only pixels that are not compared are the ones along the outline of
the circle: LVGL draws a circle as a rounded rectangle and its rasterizer
supersamples the curve, so the outline is a blend of the shape and the
background there while the legacy compositor fills whole pixels. The band
around the outline is skipped, everything else has to match exactly.

Two differences between the two implementations are known and are not tested
here:

- LVGL antialiases the outline of a circle, the legacy compositor does not.
- The legacy compositor does not scale vectorio shapes with the scale of the
  group they are in, while the LVGL one does. A shape in a scaled group is
  therefore bigger on an LVGL display.
"""

import math
import shutil
from pathlib import Path

import pytest
from PIL import Image

_DISPLAY_HELPER = """\
import board
display = board.DISPLAY
"""

# The scene both compositors render. The shapes are placed so that none of them
# overlap and so that the circle is far away from the other two.
SCENE_CODE = (
    _DISPLAY_HELPER
    + """\
import displayio
import time
import vectorio

palette = displayio.Palette(4)
palette[0] = 0x000000
palette[1] = 0xFF0000
palette[2] = 0x00FF00
palette[3] = 0x0000FF

group = displayio.Group()

circle = vectorio.Circle(pixel_shader=palette, radius=20, x=40, y=40, color_index=1)
group.append(circle)

rectangle = vectorio.Rectangle(
    pixel_shader=palette, width=40, height=20, x=100, y=40, color_index=2
)
group.append(rectangle)

polygon = vectorio.Polygon(
    pixel_shader=palette,
    points=[(0, 0), (40, 10), (20, 40)],
    x=180,
    y=40,
    color_index=3,
)
group.append(polygon)

# Replacing the root group hides the console so the frame only has the shapes.
display.auto_refresh = False
display.root_group = group
display.refresh()
print('rendered')
while True:
    time.sleep(1)
"""
)

# The scene with the rectangle moved and the circle hidden. It is captured twice:
# once before the move and once after it.
MOVE_CODE = (
    _DISPLAY_HELPER
    + """\
import displayio
import time
import vectorio

palette = displayio.Palette(4)
palette[0] = 0x000000
palette[1] = 0xFF0000
palette[2] = 0x00FF00
palette[3] = 0x0000FF

group = displayio.Group()

circle = vectorio.Circle(pixel_shader=palette, radius=20, x=40, y=40, color_index=1)
circle.hidden = True
group.append(circle)

rectangle = vectorio.Rectangle(
    pixel_shader=palette, width=40, height=20, x=100, y=40, color_index=2
)
group.append(rectangle)

display.auto_refresh = False
display.root_group = group
display.refresh()
print('first')
time.sleep(2)
rectangle.x = 160
display.refresh()
print('second')
while True:
    time.sleep(1)
"""
)

API_CODE = """\
import displayio
import vectorio

palette = displayio.Palette(4)
palette[0] = 0x000000
palette[1] = 0xFF0000
palette[2] = 0x00FF00
palette[3] = 0x0000FF

circle = vectorio.Circle(pixel_shader=palette, radius=10, x=20, y=20)
rectangle = vectorio.Rectangle(pixel_shader=palette, width=30, height=20, x=5, y=5)
polygon = vectorio.Polygon(pixel_shader=palette, points=[(0, 0), (10, 0), (0, 10)])

print('circle', circle.x, circle.y, circle.radius, circle.color_index)
print('rectangle', rectangle.x, rectangle.y, rectangle.width, rectangle.height)
print('polygon', polygon.points, polygon.color_index)

# The circle is centered on its position and covers the pixels whose center is
# within the radius.
print('circle contains', circle.contains(20, 20), circle.contains(30, 20),
    circle.contains(31, 20), circle.contains(20, 31))

# The rectangle's position is its top left corner.
print('rectangle contains', rectangle.contains(5, 5), rectangle.contains(34, 24),
    rectangle.contains(35, 24), rectangle.contains(4, 5))

# The polygon's points are relative to its position.
print('polygon contains', polygon.contains(0, 0), polygon.contains(5, 1),
    polygon.contains(9, 9))

# Moving a shape moves its coverage with it.
circle.x = 50
print('moved circle contains', circle.contains(60, 20), circle.contains(20, 20))

# The color index selects the palette entry.
circle.color_index = 2
print('color index', circle.color_index)

# A shape can only be in one group.
group = displayio.Group()
group.append(circle)
print('in group', len(group))
other = displayio.Group()
try:
    other.append(circle)
    print('second group: no error')
except ValueError as e:
    print('second group: ValueError')
print('done')
"""


def _read_image(path):
    with Image.open(path) as img:
        rgb = img.convert("RGB")
        return rgb.width, rgb.height, rgb.tobytes()


def _pixel_at(image, x, y):
    return tuple(image.getpixel((x, y)))


def _circle_outline_band(width, height, center_x, center_y, radius):
    """Return the pixels LVGL may antialias when it draws a circle.

    LVGL draws the circle as a rounded rectangle and supersamples the curve, so
    the pixels along the outline are a blend of the shape and the background
    while the legacy compositor fills whole pixels. The pixels inside the
    outline are filled completely by both and the pixels outside it are not
    touched by either, so only the outline itself is skipped.
    """
    band = set()
    for y in range(height):
        for x in range(width):
            distance = math.hypot(x - center_x, y - center_y)
            if radius - 1 < distance <= radius + 1.5:
                band.add((x, y))
    return band


@pytest.mark.circuitpy_drive({"code.py": API_CODE})
@pytest.mark.duration(15)
def test_vectorio_api(circuitpython):
    """The vectorio API behaves the same on both compositors."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "circle 20 20 10 0" in output
    assert "rectangle 5 5 30 20" in output
    assert "polygon [(0, 0), (10, 0), (0, 10)] 0" in output
    # The circle covers the pixels whose center is within the radius.
    assert "circle contains True True False False" in output
    # The rectangle's position is its top left corner.
    assert "rectangle contains True True False False" in output
    assert "polygon contains True True False" in output
    # Moving a shape moves its coverage with it.
    assert "moved circle contains True False" in output
    assert "color index 2" in output
    assert "in group 1" in output
    assert "second group: ValueError" in output
    assert "done" in output


@pytest.mark.circuitpy_drive({"code.py": SCENE_CODE})
@pytest.mark.display(capture_times_ns=[4_000_000_000])
@pytest.mark.duration(8)
def test_vectorio_render_golden(request, circuitpython):
    """The shapes are rendered exactly like the legacy compositor renders them."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "rendered" in output

    captures = circuitpython.display_capture_paths()
    if not captures or not captures[0].exists():
        pytest.skip("display capture was not produced")

    golden = Path(__file__).parent / "vectorio" / "golden" / "vectorio_shapes_320x240.png"
    if request.config.getoption("--update-goldens"):
        golden.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(captures[0], golden)
        return

    gw, gh, gpx = _read_image(golden)
    dw, dh, dpx = _read_image(captures[0])
    assert (dw, dh) == (gw, gh)

    band = _circle_outline_band(dw, dh, 40, 40, 20)
    mismatches = []
    for y in range(dh):
        for x in range(dw):
            if (x, y) in band:
                continue
            i = (y * dw + x) * 3
            if gpx[i : i + 3] != dpx[i : i + 3]:
                mismatches.append((x, y, tuple(gpx[i : i + 3]), tuple(dpx[i : i + 3])))
    assert not mismatches, f"{len(mismatches)} pixels differ, first: {mismatches[:5]}"


@pytest.mark.circuitpy_drive({"code.py": MOVE_CODE})
@pytest.mark.display(capture_times_ns=[8_000_000_000, 10_000_000_000, 12_000_000_000])
@pytest.mark.duration(16)
def test_vectorio_move_and_hide(circuitpython):
    """Moving a shape updates the screen and a hidden shape is not drawn."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "first" in output
    assert "second" in output

    captures = circuitpython.display_capture_paths()
    if not captures or not captures[-1].exists():
        pytest.skip("display capture was not produced")

    # The last capture is the one taken after the shape was moved.
    with Image.open(captures[-1]) as img:
        after = img.convert("RGB")

    green = (0, 255, 0)
    black = (0, 0, 0)

    # The hidden circle is not drawn at all.
    assert _pixel_at(after, 40, 40) == black
    # After the move the rectangle is at its new position and the old one is
    # cleared again.
    assert _pixel_at(after, 110, 45) == black
    assert _pixel_at(after, 160, 45) == green
