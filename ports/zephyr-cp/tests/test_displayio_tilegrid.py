# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""displayio TileGrid rendering on both displayio compositors.

The scene exercises indexed bitmaps with palettes, which the LVGL compositor
hands to LVGL as indexed images whose palette is kept separately and shared
between the TileGrids that use it, and the cases that go through its custom
decoder (scaled groups and OnDiskBitmaps). The expected frame is computed here
from the same description, so both compositors are checked against an
independent model instead of a golden image. Only fully saturated colors are used so the result doesn't depend on the
display's pixel format.
"""

import re

import pytest
from PIL import Image

_DISPLAY_HELPER = """\
import board
display = board.DISPLAY
"""

_SCENE_TEMPLATE = (
    _DISPLAY_HELPER
    + """\
import displayio
import time

palette = displayio.Palette(4)
palette[0] = 0xFF0000
palette[1] = 0x00FF00
palette[2] = 0x0000FF
palette[3] = 0xFFFFFF

group = displayio.Group()

# Four vertical color bands in a 2 bit bitmap.
bands = displayio.Bitmap(40, 20, 4)
for y in range(bands.height):
    for x in range(bands.width):
        bands[x, y] = (x * 4) // bands.width
group.append(displayio.TileGrid(bands, pixel_shader=palette, x=10, y=10))

# A sprite sheet with two 8x8 tiles: tile 0 green, tile 1 blue with a white dot.
sheet = displayio.Bitmap(16, 8, 4)
for y in range(8):
    for x in range(16):
        sheet[x, y] = 1 if x < 8 else 2
sheet[9, 1] = 3
sheet_grid = displayio.TileGrid(sheet, pixel_shader=palette, width=3, height=1,
    tile_width=8, tile_height=8, x=100, y=10)
sheet_grid[0] = 1
sheet_grid[1] = 0
sheet_grid[2] = 1
group.append(sheet_grid)

# Transparency: a white square partially covered by a sprite whose index 0 is
# transparent.
white = displayio.Bitmap(30, 30, 2)
white_palette = displayio.Palette(2)
white_palette[0] = 0xFFFFFF
white_palette[1] = 0xFFFFFF
group.append(displayio.TileGrid(white, pixel_shader=white_palette, x=200, y=10))
overlay_palette = displayio.Palette(2)
overlay_palette[0] = 0x000000
overlay_palette[1] = 0xFF0000
overlay_palette.make_transparent(0)
overlay = displayio.Bitmap(10, 10, 2)
for y in range(10):
    for x in range(5, 10):
        overlay[x, y] = 1
group.append(displayio.TileGrid(overlay, pixel_shader=overlay_palette, x=210, y=20))

# An 8 bit bitmap with a large palette.
big_palette = displayio.Palette(256)
for i in range(256):
    big_palette[i] = 0x000000
big_palette[200] = 0x00FFFF
big_palette[201] = 0xFFFF00
eight = displayio.Bitmap(20, 10, 256)
for y in range(10):
    for x in range(20):
        eight[x, y] = 200 if x < 10 else 201
group.append(displayio.TileGrid(eight, pixel_shader=big_palette, x=10, y=100))

{scaled}# One bitmap shown with two different palettes.
other_palette = displayio.Palette(4)
other_palette[0] = 0xFF00FF
other_palette[1] = 0x00FFFF
other_palette[2] = 0xFFFF00
other_palette[3] = 0x000000
group.append(displayio.TileGrid(bands, pixel_shader=other_palette, x=10, y=150))
# A second TileGrid with the same alternate palette shares its LVGL palette.
group.append(displayio.TileGrid(bands, pixel_shader=other_palette, x=10, y=180))

# A 16 bit bitmap with a pass-through ColorConverter.
rgb565 = displayio.Bitmap(10, 10, 65536)
for y in range(10):
    for x in range(10):
        rgb565[x, y] = 0xF800 if x < 5 else 0x07E0
group.append(displayio.TileGrid(rgb565,
    pixel_shader=displayio.ColorConverter(input_colorspace=displayio.Colorspace.RGB565),
    x=250, y=100))

# Nested groups with a TileGrid and a vectorio shape; the inner group moves later.
import vectorio
checker = displayio.Bitmap(4, 4, 4)
for y in range(4):
    for x in range(4):
        checker[x, y] = 1 if (x + y) % 2 else 2
outer = displayio.Group(x=10, y=0)
inner = displayio.Group(x=60, y=200)
inner.append(displayio.TileGrid(checker, pixel_shader=palette, x=5, y=5))
inner.append(vectorio.Rectangle(pixel_shader=palette, width=6, height=4, x=20, y=0,
    color_index=2))
outer.append(inner)
group.append(outer)

# A sprite that is moved many times before the frame is captured.
mover = displayio.TileGrid(bands, pixel_shader=palette, x=0, y=0)
group.append(mover)

display.auto_refresh = False
display.root_group = group
display.refresh()
for i in range(200):
    mover.x = 150 + (i % 20)
    mover.y = 150 + (i % 10)
    display.refresh()
mover.x = 160
mover.y = 200
display.refresh()
print('first')
time.sleep(10)

# Content changes after the first frame.
palette[3] = 0xFF00FF
bands[0, 0] = 3
sheet_grid[1] = 1
# The two alternate-palette TileGrids share one LVGL palette, so the Palette change
# reaches both, and the pixel change reaches every grid drawing the bitmap.
other_palette[3] = 0x00FF00
bands[39, 19] = 2
# A direct-color bitmap has no palette, only its own pixels.
rgb565[0, 0] = 0x001F
inner.x = 80
display.refresh()
print('second')
while True:
    time.sleep(1)
"""
)

_SCALED_GROUP = """\
# A scaled group.
small = displayio.Bitmap(4, 4, 4)
for y in range(4):
    for x in range(4):
        small[x, y] = 1 if (x + y) % 2 else 2
scaled = displayio.Group(scale=2, x=100, y=100)
scaled.append(displayio.TileGrid(small, pixel_shader=palette, x=2, y=1))
group.append(scaled)

"""

SCENE_CODE = _SCENE_TEMPLATE.replace("{scaled}", "")
SCALED_SCENE_CODE = _SCENE_TEMPLATE.replace("{scaled}", _SCALED_GROUP)

RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
MAGENTA = (255, 0, 255)
CYAN = (0, 255, 255)
YELLOW = (255, 255, 0)


def _expected(width, height, changed, scaled):
    frame = [[BLACK] * width for _ in range(height)]

    def draw(x0, y0, w, h, color_at, scale=1):
        for y in range(h * scale):
            for x in range(w * scale):
                color = color_at(x // scale, y // scale)
                if color is not None:
                    frame[y0 + y][x0 + x] = color

    palette = [RED, GREEN, BLUE, MAGENTA if changed else WHITE]
    other = [MAGENTA, CYAN, YELLOW, GREEN if changed else BLACK]

    def bands(colors):
        def color_at(x, y):
            if changed and (x, y) == (0, 0):
                return colors[3]
            if changed and (x, y) == (39, 19):
                return colors[2]
            return colors[(x * 4) // 40]

        return color_at

    draw(10, 10, 40, 20, bands(palette))

    tiles = [1, 1 if changed else 0, 1]

    def sheet(x, y):
        tile = tiles[x // 8]
        sx = (x % 8) + tile * 8
        if (sx, y) == (9, 1):
            return palette[3]
        return palette[1] if sx < 8 else palette[2]

    draw(100, 10, 24, 8, sheet)

    draw(200, 10, 30, 30, lambda x, y: WHITE)
    draw(210, 20, 10, 10, lambda x, y: RED if x >= 5 else None)

    draw(10, 100, 20, 10, lambda x, y: CYAN if x < 10 else YELLOW)

    if scaled:
        draw(
            104,
            102,
            4,
            4,
            lambda x, y: palette[1] if (x + y) % 2 else palette[2],
            scale=2,
        )

    draw(
        250,
        100,
        10,
        10,
        lambda x, y: BLUE if changed and (x, y) == (0, 0) else RED if x < 5 else GREEN,
    )

    inner_x = 10 + (80 if changed else 60)
    draw(inner_x + 5, 205, 4, 4, lambda x, y: palette[1] if (x + y) % 2 else palette[2])
    draw(inner_x + 20, 200, 6, 4, lambda x, y: palette[2])

    draw(10, 150, 40, 20, bands(other))
    draw(10, 180, 40, 20, bands(other))

    draw(160, 200, 40, 20, bands(palette))
    return frame


def _compare(path, changed, scaled=False):
    with Image.open(path) as img:
        rgb = img.convert("RGB")
    expected = _expected(rgb.width, rgb.height, changed, scaled)
    mismatches = []
    for y in range(rgb.height):
        for x in range(rgb.width):
            actual = rgb.getpixel((x, y))
            if actual != expected[y][x]:
                mismatches.append((x, y, expected[y][x], actual))
    assert not mismatches, f"{len(mismatches)} pixels differ, first: {mismatches[:8]}"


@pytest.mark.circuitpy_drive({"code.py": SCENE_CODE})
@pytest.mark.display(capture_times_ns=[8_000_000_000, 20_000_000_000])
@pytest.mark.duration(24)
def test_tilegrid_render(circuitpython):
    """Indexed bitmaps, sprite sheets, transparency and updates render exactly."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "first" in output
    assert "second" in output

    captures = circuitpython.display_capture_paths()
    if len(captures) < 2 or not captures[1].exists():
        pytest.skip("display capture was not produced")

    _compare(captures[0], changed=False)
    _compare(captures[1], changed=True)


@pytest.mark.circuitpy_drive({"code.py": SCALED_SCENE_CODE})
@pytest.mark.display(capture_times_ns=[8_000_000_000, 20_000_000_000])
@pytest.mark.duration(24)
def test_tilegrid_render_scaled_group(request, board, circuitpython):
    """A TileGrid in a scaled group is scaled by whole pixels from its corner."""
    if board.startswith("native_native_sim_lvgl"):
        # LVGL scales images about pixel centers ((w - 1) * scale wide) and resamples,
        # so it doesn't reproduce displayio's whole pixel scaling yet.
        request.applymarker(pytest.mark.xfail(reason="LVGL image scaling isn't pixel exact"))
    circuitpython.wait_until_done()

    captures = circuitpython.display_capture_paths()
    if len(captures) < 2 or not captures[1].exists():
        pytest.skip("display capture was not produced")

    _compare(captures[0], changed=False, scaled=True)
    _compare(captures[1], changed=True, scaled=True)


# Nonconsecutive atlas tiles are essential here: overflow from tile N looks correct
# when the next displayed tile happens to be N + 1 (as in an alphabet test).
CROPPED_ATLAS_CODE = """\
import board
import displayio
import time

display = board.DISPLAY
palette = displayio.Palette(4)
for i, color in enumerate((0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF)):
    palette[i] = color
sheet = displayio.Bitmap(12, 10, 4)
for y in range(10):
    for x in range(12):
        sheet[x, y] = (x // 3 + 2 * (y // 5)) % 4
root = displayio.Group()
grids = []
for scale, y in ((1, 8), (2, 50), (3, 110)):
    group = displayio.Group(scale=scale, x=8, y=y)
    grid = displayio.TileGrid(sheet, pixel_shader=palette, width=5, height=3,
        tile_width=3, tile_height=5, x=1, y=1)
    for i in range(15):
        grid[i] = (7 - i * 3) % 8
    group.append(grid)
    # Single-cell TileGrids use a separate LVGL path. Leave space around the sprite
    # so drawing outside its rectangle cannot be hidden by neighboring cells.
    sprite = displayio.TileGrid(sheet, pixel_shader=palette,
        tile_width=3, tile_height=5, default_tile=2, x=30, y=1)
    group.append(sprite)
    root.append(group)
    grids.append((grid, sprite))
display.auto_refresh = False
display.root_group = root
display.refresh()
print('first')
time.sleep(10)
for grid, sprite in grids:
    for i in range(15):
        grid[i] = (i * 5 + 1) % 8
    sprite[0] = 5
display.refresh()
print('second')
while True:
    time.sleep(1)
"""


def _cropped_atlas_expected(width, height, changed):
    frame = Image.new("RGB", (width, height), BLACK)
    colors = (RED, GREEN, BLUE, WHITE)

    def tile(x0, y0, index, scale):
        for y in range(5 * scale):
            for x in range(3 * scale):
                color = colors[(index % 4 + 2 * (index // 4)) % 4]
                frame.putpixel((x0 + x, y0 + y), color)

    for scale, y0 in ((1, 8), (2, 50), (3, 110)):
        for i in range(15):
            index = (i * 5 + 1) % 8 if changed else (7 - i * 3) % 8
            tile(8 + (1 + i % 5 * 3) * scale, y0 + (1 + i // 5 * 5) * scale, index, scale)
        tile(8 + 30 * scale, y0 + scale, 5 if changed else 2, scale)
    return frame


@pytest.mark.circuitpy_drive({"code.py": CROPPED_ATLAS_CODE})
@pytest.mark.display(capture_times_ns=[8_000_000_000, 20_000_000_000])
@pytest.mark.duration(24)
def test_tilegrid_scaled_atlas_clipping(circuitpython):
    """Scaled atlas windows never draw neighboring tiles outside their cells."""
    circuitpython.wait_until_done()
    captures = circuitpython.display_capture_paths()
    if len(captures) < 2 or not captures[1].exists():
        pytest.skip("display capture was not produced")
    for path, changed in ((captures[0], False), (captures[1], True)):
        with Image.open(path) as image:
            actual = image.convert("RGB")
        expected = _cropped_atlas_expected(actual.width, actual.height, changed)
        # Check the entire exterior and the center of every cell. Zephyr's older
        # LVGL has a separate nearest-neighbor rounding bug at tile edges; this
        # regression isolates clipping rather than depending on that sampler fix.
        checked = {
            (x, y)
            for y in range(actual.height)
            for x in range(actual.width)
            if expected.getpixel((x, y)) == BLACK
        }
        for scale, y0 in ((1, 8), (2, 50), (3, 110)):
            for i in range(15):
                checked.add((8 + (2 + i % 5 * 3) * scale, y0 + (3 + i // 5 * 5) * scale))
            checked.add((8 + 31 * scale, y0 + 3 * scale))
        mismatches = [
            (x, y, expected.getpixel((x, y)), actual.getpixel((x, y)))
            for x, y in sorted(checked)
            if actual.getpixel((x, y)) != expected.getpixel((x, y))
        ]
        assert not mismatches, f"{len(mismatches)} pixels differ, first: {mismatches[:8]}"


UNSUPPORTED_CODE = """\
import displayio

bitmap = displayio.Bitmap(4, 4, 65536)
try:
    displayio.TileGrid(bitmap, pixel_shader=displayio.ColorConverter())
    print('converting shader: ok')
except NotImplementedError:
    print('converting shader: NotImplementedError')
print('done')
"""


@pytest.mark.circuitpy_drive({"code.py": UNSUPPORTED_CODE})
@pytest.mark.parametrize(
    "board", ["native_native_sim_lvgl", "native_native_sim_lvgl_asan"], indirect=True
)
@pytest.mark.duration(10)
def test_tilegrid_unsupported(circuitpython):
    """Combinations LVGL can't draw directly raise NotImplementedError."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "converting shader: NotImplementedError" in output
    assert "done" in output


VIEW_LIFECYCLE_CODE = """\
import board
import displayio
import time

display = board.DISPLAY

def split_bands():
    bitmap = displayio.Bitmap(8, 8, 4)
    for y in range(8):
        for x in range(8):
            bitmap[x, y] = 1 if x < 4 else 2
    return bitmap

first = split_bands()
second = displayio.Bitmap(8, 8, 4)
for y in range(8):
    for x in range(8):
        second[x, y] = 3

canonical = displayio.Palette(4)
canonical[1] = 0x00FF00
canonical[2] = 0x0000FF
canonical[3] = 0xFFFFFF
alternate = displayio.Palette(4)
alternate[1] = 0xFFFF00
alternate[2] = 0x00FFFF
third = displayio.Palette(4)
third[1] = 0xFF00FF
third[2] = 0x00FF00

group = displayio.Group()
canonical_grid = displayio.TileGrid(first, pixel_shader=canonical, x=0, y=0)
copy_a = displayio.TileGrid(first, pixel_shader=alternate, x=20, y=0)
copy_b = displayio.TileGrid(first, pixel_shader=alternate, x=40, y=0)
second_grid = displayio.TileGrid(second, pixel_shader=canonical, x=0, y=20)
for grid in (canonical_grid, copy_a, copy_b, second_grid):
    group.append(grid)

display.auto_refresh = False
display.root_group = group
display.refresh()
print('first')
time.sleep(10)

# Drop one of the two users of the shared LVGL palette.
group.remove(copy_b)
# Point the second grid at the first bitmap; both are 8x8.
second_grid.bitmap = first
# Give the canonical grid another palette.
canonical_grid.pixel_shader = third
# Change the shared bitmap and the remaining copy's palette.
first[0, 0] = 2
alternate[1] = 0x000000
display.refresh()
print('second')
while True:
    time.sleep(1)
"""


def _lifecycle_expected(width, height, changed):
    frame = [[BLACK] * width for _ in range(height)]

    def draw(x0, y0, colors):
        for y in range(8):
            for x in range(8):
                frame[y0 + y][x0 + x] = colors[0] if x < 4 else colors[1]

    if not changed:
        draw(0, 0, (GREEN, BLUE))
        draw(20, 0, (YELLOW, CYAN))
        draw(40, 0, (YELLOW, CYAN))
        draw(0, 20, (WHITE, WHITE))
        return frame

    # The canonical grid switched to the third palette, and pixel (0, 0) now holds index 2.
    draw(0, 0, (MAGENTA, GREEN))
    frame[0][0] = GREEN
    # The removed grid leaves a hole; the other copy uses its changed palette.
    draw(20, 0, (BLACK, CYAN))
    frame[0][20] = CYAN
    # The second grid now shows the first bitmap with the canonical palette, including the
    # changed pixel.
    draw(0, 20, (GREEN, BLUE))
    frame[20][0] = BLUE
    return frame


@pytest.mark.circuitpy_drive({"code.py": VIEW_LIFECYCLE_CODE})
@pytest.mark.display(capture_times_ns=[8_000_000_000, 20_000_000_000])
@pytest.mark.duration(24)
def test_tilegrid_view_lifecycle(circuitpython):
    """Shared images survive detaching one user, and bitmap/palette swaps rebind cleanly."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "first" in output
    assert "second" in output

    captures = circuitpython.display_capture_paths()
    if len(captures) < 2 or not captures[1].exists():
        pytest.skip("display capture was not produced")

    for path, changed in ((captures[0], False), (captures[1], True)):
        with Image.open(path) as img:
            rgb = img.convert("RGB")
        expected = _lifecycle_expected(rgb.width, rgb.height, changed)
        mismatches = []
        for y in range(rgb.height):
            for x in range(rgb.width):
                actual = rgb.getpixel((x, y))
                if actual != expected[y][x]:
                    mismatches.append((x, y, expected[y][x], actual))
        assert not mismatches, f"{len(mismatches)} pixels differ, first: {mismatches[:8]}"


# Scrolling is not a Python-visible TileGrid property, but the terminal scrolls its TileGrid
# by moving the wrapped window over the tile matrix instead of moving tiles. So writing a
# fifth line into a four line terminal shifts the visible content up by exactly one cell row:
# the first three rows of the scrolled frame must equal the last three rows of the frame
# before it, and the bottom row must hold the new line. The wrapped window is two row runs
# once the terminal has scrolled.
SCROLL_CODE = """\
import board
import displayio
import terminalio
import time

display = board.DISPLAY

font = terminalio.FONT
cell_width, cell_height = font.get_bounding_box()
palette = displayio.Palette(2)
palette[0] = 0x000000
palette[1] = 0xFFFFFF

group = displayio.Group()
grid = displayio.TileGrid(font.bitmap, pixel_shader=palette, width=4, height=4,
    tile_width=cell_width, tile_height=cell_height, x=4, y=4)
group.append(grid)
terminal = terminalio.Terminal(grid, font)

display.auto_refresh = False
display.root_group = group
# Fill the terminal without going past the last row, so this frame doesn't scroll yet.
terminal.write("aa\\r\\nbb\\r\\ncc\\r\\ndd")
display.refresh()
print('filled', cell_width, cell_height)
time.sleep(2)

# Moving to the row below the last one makes the terminal scroll by one line.
terminal.write("\\r\\nee")
display.refresh()
print('scrolled')
time.sleep(2)
while True:
    time.sleep(1)
"""


@pytest.mark.circuitpy_drive({"code.py": SCROLL_CODE})
@pytest.mark.display(capture_times_ns=[4_000_000_000, 8_000_000_000])
@pytest.mark.duration(14)
def test_tilegrid_scroll(circuitpython):
    """A terminal scroll shifts the visible tile rows and wraps the window."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "scrolled" in output
    match = re.search(r"filled (\d+) (\d+)", output)
    assert match, "the scene didn't report the font's cell size"
    cell_w = int(match.group(1))
    cell_h = int(match.group(2))

    captures = circuitpython.display_capture_paths()
    if len(captures) < 2 or not captures[1].exists():
        pytest.skip("display capture was not produced")

    with Image.open(captures[0]) as img:
        filled = img.convert("RGB")
    with Image.open(captures[1]) as img:
        scrolled = img.convert("RGB")

    origin_x = 4
    origin_y = 4
    width = 4 * cell_w
    # Row j of the filled frame is row j - 1 of the scrolled one, for every row but the last.
    for row in range(1, 4):
        for y in range(cell_h):
            for x in range(width):
                before = filled.getpixel((origin_x + x, origin_y + row * cell_h + y))
                after = scrolled.getpixel((origin_x + x, origin_y + (row - 1) * cell_h + y))
                assert before == after, f"row {row - 1} did not shift up (x={x}, y={y})"

    # The new line landed on the bottom row.
    bottom = origin_y + 3 * cell_h
    changed = any(
        scrolled.getpixel((origin_x + x, bottom + y))
        != filled.getpixel((origin_x + x, bottom + y))
        for y in range(cell_h)
        for x in range(width)
    )
    assert changed, "the new line did not appear at the bottom of the scrolled frame"
