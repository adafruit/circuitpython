# SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""OnDiskBitmap rendering on both displayio compositors.

The BMPs are generated here and copied onto the simulated drive. The expected
frame is computed from the same pixel descriptions, so the legacy scanline
compositor and the LVGL compositor's OnDiskBitmap image decoder are checked
against an independent model. Only fully saturated colors are used so the
result doesn't depend on the display's pixel format.
"""

import struct

import pytest
from PIL import Image

_DISPLAY_HELPER = """\
import board
display = board.DISPLAY
"""

_CODE = (
    _DISPLAY_HELPER
    + """\
import displayio
import time

group = displayio.Group()

# 8 bit indexed, bottom-up rows.
quad = displayio.OnDiskBitmap('/quad.bmp')
group.append(displayio.TileGrid(quad, pixel_shader=quad.pixel_shader, x=10, y=10))

# A sprite sheet with two 4x4 tiles.
sheet = displayio.OnDiskBitmap('/sheet.bmp')
sheet_grid = displayio.TileGrid(sheet, pixel_shader=sheet.pixel_shader,
    width=2, height=1, tile_width=4, tile_height=4, x=10, y=40)
sheet_grid[1] = 1
group.append(sheet_grid)

# 16 bit 565 with explicit bitfields.
rgb565 = displayio.OnDiskBitmap('/rgb565.bmp')
group.append(displayio.TileGrid(rgb565, pixel_shader=rgb565.pixel_shader, x=40, y=10))

# 16 bit 555 (the BI_RGB default).
rgb555 = displayio.OnDiskBitmap('/rgb555.bmp')
group.append(displayio.TileGrid(rgb555, pixel_shader=rgb555.pixel_shader, x=70, y=10))

# 24 bit BGR.
rgb888 = displayio.OnDiskBitmap('/rgb888.bmp')
group.append(displayio.TileGrid(rgb888, pixel_shader=rgb888.pixel_shader, x=100, y=10))

# 8 bit indexed, top-down rows (negative height).
topdown = displayio.OnDiskBitmap('/topdown.bmp')
group.append(displayio.TileGrid(topdown, pixel_shader=topdown.pixel_shader, x=130, y=10))

display.auto_refresh = False
display.root_group = group
display.refresh()
print('first')
time.sleep(10)

# Palette changes have to reach the image decoder.
quad.pixel_shader[0] = 0xFF00FF
display.refresh()
print('second')
while True:
    time.sleep(1)
"""
)

RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
WHITE = (255, 255, 255)
MAGENTA = (255, 0, 255)
BLACK = (0, 0, 0)


def _bmp(width, height, bpp, rows, palette=None, top_down=False, bitfields=None):
    """Build a BMP. rows are top to bottom; pixels are palette indices, 16 bit
    values, or (r, g, b) tuples for 24/32 bit."""
    row_bytes = ((width * bpp + 31) // 32) * 4
    pixels = bytearray()
    for row in rows if top_down else reversed(rows):
        out = bytearray(row_bytes)
        if bpp == 8:
            out[:width] = bytes(row)
        elif bpp == 16:
            for x, value in enumerate(row):
                out[x * 2 : x * 2 + 2] = struct.pack("<H", value)
        elif bpp == 24:
            for x, (r, g, b) in enumerate(row):
                out[x * 3 : x * 3 + 3] = bytes((b, g, r))
        elif bpp == 32:
            for x, (r, g, b) in enumerate(row):
                out[x * 4 : x * 4 + 4] = bytes((b, g, r, 0))
        pixels += out

    palette = palette or []
    palette_bytes = b"".join(bytes((b, g, r, 0)) for (r, g, b) in palette)
    if bitfields is None:
        header_size = 40
        header = b""
        compression = 0
    else:
        # A V4 header carries the channel masks OnDiskBitmap reads for 565.
        header_size = 108
        compression = 3
        header = b"".join(struct.pack("<I", mask) for mask in bitfields) + bytes(56)

    offset = 14 + header_size + len(palette_bytes)
    height_field = -height if top_down else height
    file_header = b"BM" + struct.pack("<IHHI", offset + len(pixels), 0, 0, offset)
    info = struct.pack(
        "<IiiHHIIiiII",
        header_size,
        width,
        height_field,
        1,
        bpp,
        compression,
        len(pixels),
        2835,
        2835,
        len(palette),
        0,
    )
    return file_header + info + header + palette_bytes + bytes(pixels)


_QUAD_PALETTE = [RED, GREEN, BLUE, WHITE]


def _quad_rows(palette):
    """A 4x4 indexed image: one palette entry per quadrant."""
    return [
        [0, 0, 1, 1],
        [0, 0, 1, 1],
        [2, 2, 3, 3],
        [2, 2, 3, 3],
    ]


_FILES = {
    "quad.bmp": _bmp(4, 4, 8, _quad_rows(_QUAD_PALETTE), palette=_QUAD_PALETTE),
    "sheet.bmp": _bmp(8, 4, 8, [[0] * 4 + [1] * 4] * 4, palette=_QUAD_PALETTE),
    "rgb565.bmp": _bmp(
        4,
        4,
        16,
        [[0xF800] * 2 + [0x07E0] * 2] * 4,
        top_down=True,
        bitfields=(0xF800, 0x07E0, 0x001F),
    ),
    "rgb555.bmp": _bmp(4, 4, 16, [[0x7C00] * 2 + [0x001F] * 2] * 4),
    "rgb888.bmp": _bmp(4, 4, 24, [[BLUE] * 2 + [WHITE] * 2] * 4),
    "topdown.bmp": _bmp(4, 4, 8, _quad_rows(_QUAD_PALETTE), palette=_QUAD_PALETTE, top_down=True),
}


def _expected(width, height, changed):
    frame = [[BLACK] * width for _ in range(height)]

    def draw(x0, y0, w, h, color_at):
        for y in range(h):
            for x in range(w):
                frame[y0 + y][x0 + x] = color_at(x, y)

    quad_colors = list(_QUAD_PALETTE)
    if changed:
        quad_colors[0] = MAGENTA

    def quad(x, y):
        return quad_colors[(2 if y >= 2 else 0) + (1 if x >= 2 else 0)]

    draw(10, 10, 4, 4, quad)
    draw(10, 40, 8, 4, lambda x, y: RED if x < 4 else GREEN)

    draw(40, 10, 4, 4, lambda x, y: RED if x < 2 else GREEN)
    draw(70, 10, 4, 4, lambda x, y: RED if x < 2 else BLUE)
    draw(100, 10, 4, 4, lambda x, y: BLUE if x < 2 else WHITE)

    def unchanged_quad(x, y):
        return _QUAD_PALETTE[(2 if y >= 2 else 0) + (1 if x >= 2 else 0)]

    draw(130, 10, 4, 4, unchanged_quad)

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


_CODE_FILE = {"code.py": _CODE, **_FILES}


@pytest.mark.parametrize(
    "board", ["native_native_sim_lvgl", "native_native_sim_lvgl_asan"], indirect=True
)
@pytest.mark.circuitpy_drive(_CODE_FILE)
@pytest.mark.display(capture_times_ns=[8_000_000_000, 20_000_000_000])
@pytest.mark.duration(24)
def test_ondiskbitmap_render(circuitpython):
    """Indexed, 16 bit and 24 bit BMPs render, including top-down rows and tiles."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "first" in output
    assert "second" in output

    captures = circuitpython.display_capture_paths()
    if len(captures) < 2 or not captures[1].exists():
        pytest.skip("display capture was not produced")

    _compare(captures[0], changed=False)
    _compare(captures[1], changed=True)
