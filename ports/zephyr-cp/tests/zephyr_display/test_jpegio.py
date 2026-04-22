# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

import shutil
from pathlib import Path

import pytest
from PIL import Image


_TEST_JPG_PATH = Path(__file__).parent / "test.jpg"
_TEST_JPG_BYTES = _TEST_JPG_PATH.read_bytes()


def _read_image(path: Path) -> tuple[int, int, bytes]:
    with Image.open(path) as img:
        rgb = img.convert("RGB")
        return rgb.width, rgb.height, rgb.tobytes()


def _golden_compare_or_update(request, captures, golden_path):
    if not captures or not captures[0].exists():
        pytest.skip("display capture was not produced")

    if request.config.getoption("--update-goldens"):
        golden_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(captures[0], golden_path)
        return

    gw, gh, gpx = _read_image(golden_path)
    dw, dh, dpx = _read_image(captures[0])
    assert (dw, dh) == (gw, gh)
    assert gpx == dpx


JPEGIO_DECODE_CODE = """\
import board
import displayio
import time
from jpegio import JpegDecoder

decoder = JpegDecoder()
width, height = decoder.open("/test.jpg")
print('size', width, height)

bitmap = displayio.Bitmap(width, height, 65535)
decoder.decode(bitmap)

for i in range(min(width, 8)):
    print('px', i, hex(bitmap[i, 0]))

scale = 10
tg = displayio.TileGrid(
    bitmap,
    pixel_shader=displayio.ColorConverter(
        input_colorspace=displayio.Colorspace.RGB565_SWAPPED
    ),
)
g = displayio.Group(scale=scale)
g.x = (board.DISPLAY.width - width * scale) // 2
g.y = (board.DISPLAY.height - height * scale) // 2
g.append(tg)

board.DISPLAY.auto_refresh = False
board.DISPLAY.root_group = g
board.DISPLAY.refresh()
print('rendered')
while True:
    time.sleep(1)
"""


_JPEGIO_DRIVE = {"code.py": JPEGIO_DECODE_CODE, "test.jpg": _TEST_JPG_BYTES}


@pytest.mark.circuitpy_drive(_JPEGIO_DRIVE)
@pytest.mark.display(capture_times_ns=[14_000_000_000])
@pytest.mark.duration(18)
def test_jpegio_decode(request, circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "size 20 20" in output
    assert "rendered" in output

    golden = Path(__file__).parent / "golden" / "jpegio_test_pattern_320x240.png"
    _golden_compare_or_update(request, circuitpython.display_capture_paths(), golden)
