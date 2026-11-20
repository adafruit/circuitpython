# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""LVGL allocation lifetimes, including recovery from allocation/validation failures."""

import pytest
from PIL import Image


@pytest.fixture(params=["native_native_sim_lvgl", "native_native_sim_lvgl_asan"])
def board(request):
    return request.param


def _pixel(path, xy):
    assert path.exists(), f"display capture missing: {path}"
    with Image.open(path) as image:
        return image.convert("RGB").getpixel(xy)


POLYGON_OOM_CODE = """\
import board
import displayio
import time
import vectorio

display = board.DISPLAY
display.auto_refresh = False
palette = displayio.Palette(1)
palette[0] = 0xffffff
polygon = vectorio.Polygon(pixel_shader=palette,
    points=[(0, 0), (4, 0), (0, 4)], x=20, y=20)
group = displayio.Group()
group.append(polygon)
display.root_group = group
display.refresh()
# A 6001x6001 A8 mask cannot fit in the native LVGL board's 16 MiB heap.
# The old mask must no longer be the image source if its replacement fails.
polygon.points = [(0, 0), (6000, 0), (0, 6000)]
display.refresh()
print('oom refresh survived')
time.sleep(10)
polygon.points = [(0, 0), (4, 0), (0, 4)]
display.refresh()
print('mask recovered')
while True:
    time.sleep(1)
"""


@pytest.mark.circuitpy_drive({"code.py": POLYGON_OOM_CODE})
@pytest.mark.display(capture_times_ns=[6_000_000_000, 16_000_000_000])
@pytest.mark.duration(20)
def test_polygon_mask_allocation_failure(circuitpython):
    circuitpython.wait_until_done()
    assert "oom refresh survived" in circuitpython.serial.all_output
    assert "mask recovered" in circuitpython.serial.all_output
    before, after = circuitpython.display_capture_paths()
    assert _pixel(before, (21, 21)) == (0, 0, 0)
    assert _pixel(after, (21, 21)) == (255, 255, 255)


BITMAP_DEINIT_CODE = """\
import board
import displayio
import time

display = board.DISPLAY
display.auto_refresh = False
group = displayio.Group()
bitmaps = []
# Exercise direct sources, converted/cached sources and composed indexed sources.
for i, colorspace in enumerate((displayio.Colorspace.RGB565,
                                displayio.Colorspace.RGB565_SWAPPED, None)):
    bitmap = displayio.Bitmap(64, 64, 2 if colorspace is None else 65536)
    if colorspace is None:
        shader = displayio.Palette(2)
        shader[1] = 0xff0000
        bitmap.fill(1)
    else:
        shader = displayio.ColorConverter(input_colorspace=colorspace)
        bitmap.fill(0xf800 if i == 0 else 0x00f8)
    bitmaps.append(bitmap)
    group.append(displayio.TileGrid(bitmap, pixel_shader=shader, x=i * 80))
display.root_group = group
display.refresh()
time.sleep(10)
for bitmap in bitmaps:
    bitmap.deinit()
    bitmap.deinit()  # teardown is idempotent
# Reuse freed GC storage: a stale direct source would now show someone else's pixels.
reused = [displayio.Bitmap(64, 64, 65536) for _ in range(4)]
for bitmap in reused:
    bitmap.fill(0x07e0)
display.refresh()
print('deinit refreshed')
while True:
    time.sleep(1)
"""


@pytest.mark.circuitpy_drive({"code.py": BITMAP_DEINIT_CODE})
@pytest.mark.display(capture_times_ns=[6_000_000_000, 16_000_000_000])
@pytest.mark.duration(20)
def test_deinitialized_bitmap_drops_borrowed_pixels(circuitpython):
    circuitpython.wait_until_done()
    assert "deinit refreshed" in circuitpython.serial.all_output
    before, after = circuitpython.display_capture_paths()
    for x in (1, 81, 161):
        assert _pixel(before, (x, 1)) == (255, 0, 0)
        assert _pixel(after, (x, 1)) == (0, 0, 0)


GROUP_ERROR_CODE = """\
import board
import displayio
import time

display = board.DISPLAY
palette = displayio.Palette(1)
palette[0] = 0xff0000
bitmap = displayio.Bitmap(32, 32, 1)
grid = displayio.TileGrid(bitmap, pixel_shader=palette)
group = displayio.Group()
group.append(grid)
display.root_group = group
display.refresh()
time.sleep(10)
other = displayio.Group()
member = displayio.Group()
other.append(member)
for invalid in (object(), member):
    try:
        group[0] = invalid
        raise AssertionError('assignment should have failed')
    except ValueError:
        pass
    assert group[0] is grid
# Assigning the existing member to itself is a no-op, not a failed adoption.
group[0] = grid
palette[0] = 0x00ff00
# Do NOT manually refresh: the workqueue must be able to acquire the LVGL mutex.
print('rejected replacements')
while True:
    time.sleep(1)
"""


@pytest.mark.circuitpy_drive({"code.py": GROUP_ERROR_CODE})
@pytest.mark.display(capture_times_ns=[6_000_000_000, 16_000_000_000])
@pytest.mark.duration(20)
def test_failed_group_assignment_preserves_mirror_and_unlocks(circuitpython):
    circuitpython.wait_until_done()
    assert "rejected replacements" in circuitpython.serial.all_output
    before, after = circuitpython.display_capture_paths()
    assert _pixel(before, (1, 1)) == (255, 0, 0)
    assert _pixel(after, (1, 1)) == (0, 255, 0)


DISPLAY_RECREATE_CODE = """\
import board
import displayio
import gc
import zephyr_display

display = board.DISPLAY
for i in range(1200):
    displayio.release_displays()
    display = zephyr_display.Display(auto_refresh=False)
    if i % 50 == 0:
        gc.collect()
# Leaking one 320x24 RGB565 rendering buffer per recreation exhausts 16 MiB.
group = displayio.Group()
bitmap = displayio.Bitmap(4, 4, 65536)
bitmap.fill(0xf800)
group.append(displayio.TileGrid(bitmap, pixel_shader=displayio.ColorConverter(
    input_colorspace=displayio.Colorspace.RGB565)))
display.root_group = group
display.refresh()
print('recreated 1200 displays')
"""


@pytest.mark.circuitpy_drive({"code.py": DISPLAY_RECREATE_CODE})
@pytest.mark.duration(60)
def test_display_recreation_releases_port_allocations(circuitpython):
    circuitpython.wait_until_done()
    assert "recreated 1200 displays" in circuitpython.serial.all_output


TILEGRID_CHURN_CODE = """\
import board
import displayio
import gc

display = board.DISPLAY
display.auto_refresh = False
group = displayio.Group()
display.root_group = group
bitmap = displayio.Bitmap(2, 1, 2)
bitmap[1, 0] = 1
palette = displayio.Palette(2)
palette[1] = 0xff0000
for i in range(550):
    grid = displayio.TileGrid(bitmap, pixel_shader=palette, width=512, height=512,
        tile_width=1, tile_height=1)
    group.append(grid)
    display.refresh()
    group.pop()
    del grid
    gc.collect()
# Each discarded grid used a 32 KiB port-heap dirty-cell bitset. If those leaked,
# the next, larger composed grid cannot allocate its buffer. Same-sized grids
# can still draw with no bitset when that allocation fails, so require more buffer
# space than one iteration did and check pixels, not just that the loop finished.
grid = displayio.TileGrid(bitmap, pixel_shader=palette, width=512, height=1024,
    tile_width=1, tile_height=1)
grid[0] = 1
group.append(grid)
display.refresh()
print('tilegrid churn survived')
import time
while True:
    time.sleep(1)
"""


@pytest.mark.circuitpy_drive({"code.py": TILEGRID_CHURN_CODE})
@pytest.mark.display(capture_times_ns=[80_000_000_000])
@pytest.mark.duration(90)
def test_discarded_tilegrids_release_dirty_cell_bits(circuitpython):
    circuitpython.wait_until_done()
    assert "tilegrid churn survived" in circuitpython.serial.all_output
    (capture,) = circuitpython.display_capture_paths()
    assert _pixel(capture, (0, 0)) == (255, 0, 0)


DIRECT_UPDATE_CODE = """\
import board
import displayio
import time

display = board.DISPLAY
display.auto_refresh = False
bitmap = displayio.Bitmap(16, 16, 65536)
bitmap.fill(0x00f8)  # byte-swapped RGB565 red
shader = displayio.ColorConverter(input_colorspace=displayio.Colorspace.RGB565_SWAPPED)
group = displayio.Group()
group.append(displayio.TileGrid(bitmap, pixel_shader=shader))
display.root_group = group
display.refresh()
time.sleep(10)
bitmap.fill(0xe007)  # byte-swapped RGB565 green
display.refresh()
print('direct source updated')
while True:
    time.sleep(1)
"""


@pytest.mark.circuitpy_drive({"code.py": DIRECT_UPDATE_CODE})
@pytest.mark.display(capture_times_ns=[6_000_000_000, 16_000_000_000])
@pytest.mark.duration(20)
def test_direct_source_updates_drop_cached_conversion(circuitpython):
    circuitpython.wait_until_done()
    assert "direct source updated" in circuitpython.serial.all_output
    before, after = circuitpython.display_capture_paths()
    assert _pixel(before, (1, 1)) == (255, 0, 0)
    assert _pixel(after, (1, 1)) == (0, 255, 0)


BUSDISPLAY_CLEANUP_CODE = """\
import board
import busdisplay
import digitalio
import displayio
import fourwire
from microcontroller import pin

displayio.release_displays()
bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11)
# An unsupported format must not leave the static display slot occupied.
try:
    busdisplay.BusDisplay(bus, b'', width=32, height=32, color_depth=1)
    raise AssertionError('packed format should have been rejected')
except NotImplementedError:
    pass
# Buffers for this display exceed the native simulator's 1 MiB port heap.
# This fails after acquiring the backlight and creating an LVGL display.
try:
    busdisplay.BusDisplay(bus, b'', width=60000, height=32, color_depth=32,
        backlight_pin=pin.P_12, auto_refresh=False)
    raise AssertionError('oversized buffers should have been rejected')
except MemoryError:
    pass
backlight = digitalio.DigitalInOut(pin.P_12)
backlight.deinit()
# Both the slot and the bus must still be usable after failed construction.
display = busdisplay.BusDisplay(bus, b'', width=32, height=32,
    backlight_pin=pin.P_12, auto_refresh=False)
displayio.release_displays()
backlight = digitalio.DigitalInOut(pin.P_12)
backlight.deinit()
print('busdisplay cleanup survived')
"""


@pytest.mark.circuitpy_drive({"code.py": BUSDISPLAY_CLEANUP_CODE})
@pytest.mark.duration(20)
def test_busdisplay_failed_construct_and_release_free_resources(circuitpython):
    circuitpython.wait_until_done()
    assert "busdisplay cleanup survived" in circuitpython.serial.all_output


BUSDISPLAY_ROTATION_CODE = """\
import board
import busdisplay
import displayio
import fourwire
from microcontroller import pin

displayio.release_displays()
bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11)
display = busdisplay.BusDisplay(bus, b'', width=64, height=32, rotation=90,
    auto_refresh=False)
print('start', display.width, display.height, display.rotation)

bitmap = displayio.Bitmap(16, 16, 1)
bitmap.fill(1)
palette = displayio.Palette(1)
palette[0] = 0xff0000
group = displayio.Group()
group.append(displayio.TileGrid(bitmap, pixel_shader=palette))
display.root_group = group
display.refresh()

for rotation in (180, 270, 0):
    display.rotation = rotation
    display.refresh()
    print('rotated', display.width, display.height, display.rotation)
print('busdisplay rotation survived')
"""


@pytest.mark.circuitpy_drive({"code.py": BUSDISPLAY_ROTATION_CODE})
@pytest.mark.duration(20)
def test_busdisplay_rotation_dimensions_and_flush(circuitpython):
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "start 32 64 90" in output
    assert "rotated 64 32 180" in output
    assert "rotated 32 64 270" in output
    assert "rotated 64 32 0" in output
    assert "busdisplay rotation survived" in output
