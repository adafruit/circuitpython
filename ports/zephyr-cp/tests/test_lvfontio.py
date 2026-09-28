# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""lvfontio OnDiskFont tests that run against every filesystem the port supports.

The font file is preloaded onto the simulated CIRCUITPY drive with the
``circuitpy_drive`` marker, so the same expectations run against FAT
(``native_native_sim``) and littlefs (``native_native_sim_lfs``). This checks
that OnDiskFont's supervisor filesystem API usage works on both.
"""

import shutil
from pathlib import Path

import pytest
from PIL import Image

from .conftest import FILESYSTEM_SIMULATORS

FONT = (Path(__file__).parent / "lvfontio" / "unifont-16.0.02-ja.bin").read_bytes()

# あ in UTF-8: full-width (16px advance) glyph found through the fmt 0 (sparse)
# cmap subtable, unlike ASCII which comes from the fmt 2 (range) subtable.

CODE = """\
import displayio
import lvfontio
import terminalio

font = lvfontio.OnDiskFont("/fonts/unifont-ja.bin", max_glyphs=8)
w, h = font.get_bounding_box()
print("bbox:", w, h)
print("bitmap:", font.bitmap.width, font.bitmap.height)

# Render through terminalio to force glyph caching, which reads the font file
# via the supervisor filesystem API. The kana is full-width and occupies two
# adjacent cache slots.
grid = displayio.TileGrid(
    font.bitmap,
    pixel_shader=displayio.Palette(2),
    width=8,
    height=1,
    tile_width=w,
    tile_height=h,
)
terminal = terminalio.Terminal(grid, font)
terminal.write("AあB")
print("tiles:", [grid[x, 0] for x in range(4)])

lit = 0
for y in range(font.bitmap.height):
    for x in range(font.bitmap.width):
        if font.bitmap[x, y]:
            lit += 1
print("lit pixels:", lit)

try:
    lvfontio.OnDiskFont("/fonts/missing.fnt")
    print("missing: no error")
except ValueError:
    print("missing: ValueError")
print("done")
"""


@pytest.mark.parametrize("board", FILESYSTEM_SIMULATORS, indirect=True)
@pytest.mark.circuitpy_drive({"code.py": CODE, "fonts/unifont-ja.bin": FONT})
@pytest.mark.duration(30)
def test_ondiskfont_render(circuitpython):
    """OnDiskFont loads, caches glyphs and renders text from the filesystem."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "bbox: 8 16" in output
    # The glyph cache is 8 slots of 8px; the font is 16px tall.
    assert "bitmap: 64 16" in output
    # ASCII 'A' cached in one slot; the full-width kana needed two adjacent
    # slots and shifted the next glyph over by two.
    assert "tiles: [1, 2, 3, 4]" in output
    # Glyph bitmaps were actually read from the font file.
    assert "lit pixels: 0" not in output
    assert "missing: ValueError" in output
    assert "done" in output


DISPLAY_CODE = """\
import board
import displayio
import lvfontio
import terminalio
import time

font = lvfontio.OnDiskFont("/fonts/unifont-ja.bin", max_glyphs=8)
w, h = font.get_bounding_box()
palette = displayio.Palette(2)
palette[1] = 0xFFFFFF
root = displayio.Group()
grid = displayio.TileGrid(
    font.bitmap,
    pixel_shader=palette,
    width=8,
    height=1,
    tile_width=w,
    tile_height=h,
    x=8,
    y=24,
)
root.append(grid)
# Replacing the root group hides the console; the frame is then just the
# rendered text, independent of the console boot output.
board.DISPLAY.root_group = root
terminal = terminalio.Terminal(grid, font)
terminal.write("AあB")
print("done")
while True:
    time.sleep(1)
"""


def _read_image(path):
    with Image.open(path) as img:
        rgb = img.convert("RGB")
        return rgb.width, rgb.height, rgb.tobytes()


@pytest.mark.parametrize("board", FILESYSTEM_SIMULATORS, indirect=True)
@pytest.mark.circuitpy_drive({"code.py": DISPLAY_CODE, "fonts/unifont-ja.bin": FONT})
@pytest.mark.display(capture_times_ns=[4_000_000_000])
@pytest.mark.duration(8)
def test_ondiskfont_display_golden(request, circuitpython):
    """Rendered lvfontio text on the display matches the golden image.

    The code.py replaces the console root group, so the captured frame is
    exactly the 'AあB' text rendered from the font file through the
    supervisor filesystem API — independent of console output.
    """
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "done" in output

    captures = circuitpython.display_capture_paths()
    if not captures or not captures[0].exists():
        pytest.skip("display capture was not produced")

    golden_path = Path(__file__).parent / "lvfontio" / "golden" / "lvfontio_terminal_320x240.png"
    if request.config.getoption("--update-goldens"):
        golden_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(captures[0], golden_path)
        return

    gw, gh, gpx = _read_image(golden_path)
    dw, dh, dpx = _read_image(captures[0])
    assert (dw, dh) == (gw, gh)
    assert gpx == dpx, "captured frame does not match the golden image"
