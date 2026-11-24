# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Fractional Group scaling in the LVGL compositor."""

import pytest
from PIL import Image


FRACTIONAL_SCENE = """\
import board
import displayio
import time

minimum = displayio.Group(scale=1 / 256)
assert minimum.scale == 1 / 256
minimum.scale = 1
minimum.scale = 1 / 256
assert minimum.scale == 1 / 256
requested = displayio.Group(scale=1.1)
assert requested.scale == 1.1
# Readback follows sub-Q8 changes too, so animations cannot stick at one step.
next_scale = 1.1 + 1 / 4096
requested.scale = next_scale
assert requested.scale == next_scale
try:
    displayio.Group(scale=1 / 512)
except ValueError:
    pass
else:
    raise AssertionError('constructor accepted scale below Q8 minimum')
try:
    minimum.scale = 1 / 512
except ValueError:
    pass
else:
    raise AssertionError('setter accepted scale below Q8 minimum')

palette = displayio.Palette(4)
palette[0] = 0xFF0000
palette[1] = 0x00FF00
palette[2] = 0x00FF00
palette[3] = 0xFFFFFF
checker = displayio.Bitmap(4, 4, 4)
for y in range(4):
    for x in range(4):
        checker[x, y] = 1 if (x + y) % 2 else 2

outer = displayio.Group(scale=1.25, x=11, y=17)
inner = displayio.Group(scale=1.2, x=2, y=1)
inner.append(displayio.TileGrid(checker, pixel_shader=palette))
outer.append(inner)
assert outer.scale == 1.25
assert inner.scale == 1.2

rgb565 = displayio.Bitmap(4, 4, 65536)
for y in range(4):
    for x in range(4):
        rgb565[x, y] = 0xF800 if x < 2 else 0x07E0
converter = displayio.ColorConverter(input_colorspace=displayio.Colorspace.RGB565)
direct = displayio.TileGrid(rgb565, pixel_shader=converter, x=30, y=0)
outer.append(direct)

root = displayio.Group()
root.append(outer)
display = board.DISPLAY
display.auto_refresh = False
display.root_group = root
display.refresh()
print('first')
time.sleep(5)

# Exercise a live scale change, movement, palette update and direct-pixel update.
inner.scale = 1.25
assert inner.scale == 1.25
inner.x = 3
palette[1] = 0xFF00FF
palette[2] = 0xFF00FF
rgb565[0, 0] = 0x001F
display.refresh()
print('second')
while True:
    time.sleep(1)
"""

RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
MAGENTA = (255, 0, 255)
BLACK = (0, 0, 0)


def _expected(width, height, changed):
    frame = Image.new("RGB", (width, height), BLACK)
    inner_scale = 400 if changed else 384  # Q8: 1.25 * 1.25 or 1.25 * 1.2

    def draw(x0, y0, scale, color_at):
        # CircuitPython duplicates the bottom/right source pixels into one
        # pixel of padding, so LVGL's center-based extent includes the edges.
        w = (4 * scale + 128) // 256
        h = w
        for y in range(h):
            for x in range(w):
                sx = x * 256 // scale
                sy = y * 256 // scale
                frame.putpixel((x0 + x, y0 + y), color_at(sx, sy))

    inner_x = 11 + ((3 if changed else 2) * 320 + 128) // 256
    inner_y = 17 + (320 + 128) // 256
    draw(inner_x, inner_y, inner_scale, lambda x, y: MAGENTA if changed else GREEN)

    direct_x = 11 + (30 * 320 + 128) // 256
    draw(
        direct_x,
        17,
        320,
        lambda x, y: BLUE if changed and (x, y) == (0, 0) else RED if x < 2 else GREEN,
    )
    return frame


@pytest.mark.parametrize(
    "board", ["native_native_sim_lvgl", "native_native_sim_lvgl_asan"], indirect=True
)
@pytest.mark.circuitpy_drive({"code.py": FRACTIONAL_SCENE})
@pytest.mark.display(capture_times_ns=[4_000_000_000, 9_000_000_000])
@pytest.mark.duration(11)
def test_displayio_fractional_group(circuitpython):
    """Fractional Q8 scales render and invalidate indexed/direct TileGrids."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "first" in output
    assert "second" in output

    captures = circuitpython.display_capture_paths()
    if len(captures) < 2 or not captures[1].exists():
        pytest.skip("display capture was not produced")

    for path, changed in ((captures[0], False), (captures[1], True)):
        with Image.open(path) as image:
            actual = image.convert("RGB")
        expected = _expected(actual.width, actual.height, changed)
        mismatches = []
        for y in range(actual.height):
            for x in range(actual.width):
                if actual.getpixel((x, y)) != expected.getpixel((x, y)):
                    mismatches.append((x, y, expected.getpixel((x, y)), actual.getpixel((x, y))))
        assert not mismatches, f"{len(mismatches)} pixels differ, first: {mismatches[:8]}"

    with Image.open(captures[0]) as image:
        first_direct_pixel = image.convert("RGB").getpixel((49, 17))
    with Image.open(captures[1]) as image:
        second_direct_pixel = image.convert("RGB").getpixel((49, 17))
    assert first_direct_pixel != BLACK
    assert second_direct_pixel != BLACK
    assert first_direct_pixel != second_direct_pixel


VECTOR_SCENE = """\
import board
import displayio
import time
import vectorio

palette = displayio.Palette(3)
palette[0] = 0x000000
palette[1] = 0x00FF00
palette[2] = 0x0000FF

group = displayio.Group(scale=0.5, x=10, y=20)
rectangle = vectorio.Rectangle(pixel_shader=palette, width=8, height=4, color_index=1)
polygon = vectorio.Polygon(pixel_shader=palette, points=[(0, 0), (7, 0), (0, 3)],
    x=12, color_index=2)
group.append(rectangle)
group.append(polygon)
root = displayio.Group()
root.append(group)

assert rectangle.contains(10, 20)
assert rectangle.contains(13, 21)
assert not rectangle.contains(14, 20)

display = board.DISPLAY
display.auto_refresh = False
display.root_group = root
display.refresh()
print('first')
time.sleep(5)

# Exercise both the style-drawn shape and the polygon mask at a new fractional scale.
group.scale = 1.5
assert rectangle.contains(21, 25)
assert not rectangle.contains(22, 20)
display.refresh()
print('second')
while True:
    time.sleep(1)
"""


def _polygon_covered(x, y):
    points = ((0, 0), (7, 0), (0, 3))
    winding = 0
    x1, y1 = points[0]
    for x2, y2 in points[1:] + points[:1]:
        side = (x - x1) * (y2 - y1) - (y - y1) * (x2 - x1)
        if y1 <= y:
            if y2 > y and side < 0:
                winding += 1
        elif y2 <= y and side > 0:
            winding -= 1
        x1, y1 = x2, y2
    return winding != 0


def _expected_vectors(scale):
    frame = Image.new("RGB", (320, 240), BLACK)
    scale_q8 = int(scale * 256)

    def scaled(value):
        return (value * scale_q8 + 128) // 256

    for y in range(scaled(4)):
        for x in range(scaled(8)):
            frame.putpixel((10 + x, 20 + y), GREEN)

    polygon_x = 10 + scaled(12)
    for y in range(scaled(4)):
        source_y = y * 256 // scale_q8
        for x in range(scaled(8)):
            source_x = x * 256 // scale_q8
            if _polygon_covered(source_x, source_y):
                frame.putpixel((polygon_x + x, 20 + y), BLUE)
    return frame


@pytest.mark.parametrize(
    "board", ["native_native_sim_lvgl", "native_native_sim_lvgl_asan"], indirect=True
)
@pytest.mark.circuitpy_drive({"code.py": VECTOR_SCENE})
@pytest.mark.display(capture_times_ns=[4_000_000_000, 9_000_000_000])
@pytest.mark.duration(11)
def test_displayio_fractional_vector_shapes(circuitpython):
    """Fractional scales work for style-drawn shapes and polygon masks."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "first" in output
    assert "second" in output

    captures = circuitpython.display_capture_paths()
    if len(captures) < 2 or not captures[1].exists():
        pytest.skip("display capture was not produced")

    for path, scale in ((captures[0], 0.5), (captures[1], 1.5)):
        with Image.open(path) as image:
            actual = image.convert("RGB")
        expected = _expected_vectors(scale)
        assert actual.tobytes() == expected.tobytes()
