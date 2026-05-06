# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

import shutil
from pathlib import Path

import pytest
from PIL import Image


_TEST_GIF_PATH = Path(__file__).parent / "test.gif"
_TEST_GIF_BYTES = _TEST_GIF_PATH.read_bytes()


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


GIFIO_METADATA_CODE = """\
import gifio

odg = gifio.OnDiskGif('/test.gif')
print('size', odg.width, odg.height)
print('frame_count', odg.frame_count)
print('duration', round(odg.duration, 3))
print('min_delay', round(odg.min_delay, 3))
print('max_delay', round(odg.max_delay, 3))

delay = odg.next_frame()
print('delay', round(delay, 3))

bitmap = odg.bitmap
print('bitmap_size', bitmap.width, bitmap.height)

for i in range(min(odg.width, 8)):
    print('px', i, hex(bitmap[i, 0]))

odg.deinit()
print('deinited')

try:
    odg.next_frame()
except Exception as e:
    print('after_deinit', type(e).__name__)

print('done')
"""


GIFIO_DECODE_CODE = """\
import board
import displayio
import time
from gifio import OnDiskGif

odg = OnDiskGif('/test.gif')
print('size', odg.width, odg.height)
print('frame_count', odg.frame_count)

scale = 10
tg = displayio.TileGrid(
    odg.bitmap,
    pixel_shader=displayio.ColorConverter(
        input_colorspace=displayio.Colorspace.RGB565_SWAPPED
    ),
)
g = displayio.Group(scale=scale)
g.x = (board.DISPLAY.width - odg.width * scale) // 2
g.y = (board.DISPLAY.height - odg.height * scale) // 2
g.append(tg)

board.DISPLAY.auto_refresh = False
board.DISPLAY.root_group = g

# Pin the render loop to a known point in simulated time so that the
# capture_times_ns schedule below lands on each GIF frame while it is on screen.
LOOP_START = 10.0
FRAME_HOLD = 2.0
while time.monotonic() < LOOP_START:
    time.sleep(0.05)
print('loop_start', round(time.monotonic(), 3))

for frame_index in range(odg.frame_count):
    odg.next_frame()
    board.DISPLAY.refresh()
    print('rendered', frame_index, round(time.monotonic(), 3))
    # Hold this frame on screen; the capture for this frame is scheduled
    # for the middle of this window.
    target = LOOP_START + (frame_index + 1) * FRAME_HOLD
    while time.monotonic() < target:
        time.sleep(0.05)

print('done')
while True:
    time.sleep(1)
"""


# Keep these in sync with LOOP_START / FRAME_HOLD in GIFIO_DECODE_CODE.
_DECODE_LOOP_START_S = 10.0
_DECODE_FRAME_HOLD_S = 2.0
_DECODE_FRAME_COUNT = 13
# Capture at the midpoint of each frame's hold window.
_DECODE_CAPTURE_TIMES_NS = [
    int(
        (_DECODE_LOOP_START_S + i * _DECODE_FRAME_HOLD_S + _DECODE_FRAME_HOLD_S / 2)
        * 1_000_000_000
    )
    for i in range(_DECODE_FRAME_COUNT)
]


GIFIO_WRITER_CODE = """\
import displayio
import gifio
import os

width, height = 8, 4
buf = bytearray(width * height * 2)
for i in range(width * height):
    # RGB565 gradient
    value = (i * 0x0841) & 0xFFFF
    buf[2 * i] = (value >> 8) & 0xFF
    buf[2 * i + 1] = value & 0xFF

path = '/out.gif'
with gifio.GifWriter(path, width, height, displayio.Colorspace.RGB565, loop=True) as writer:
    writer.add_frame(buf, 0.1)
    writer.add_frame(buf, 0.2)

size = os.stat(path)[6]
print('wrote_size', size)

with open(path, 'rb') as f:
    header = f.read(6)
print('header', header)

print('done')
"""


_GIFIO_METADATA_DRIVE = {"code.py": GIFIO_METADATA_CODE, "test.gif": _TEST_GIF_BYTES}
_GIFIO_DECODE_DRIVE = {"code.py": GIFIO_DECODE_CODE, "test.gif": _TEST_GIF_BYTES}
_GIFIO_WRITER_DRIVE = {"code.py": GIFIO_WRITER_CODE}


@pytest.mark.circuitpy_drive(_GIFIO_METADATA_DRIVE)
def test_gifio_metadata(circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "size 16 16" in output
    assert "frame_count 13" in output
    assert "bitmap_size 16 16" in output
    assert "min_delay 0.08" in output
    assert "max_delay 0.08" in output
    # 13 frames * 0.08s = 1.04s
    assert "duration 1.04" in output
    assert "delay 0.08" in output
    assert "deinited" in output
    # Using a deinited OnDiskGif should raise an exception.
    assert "after_deinit" in output
    assert "done" in output


@pytest.mark.circuitpy_drive(_GIFIO_DECODE_DRIVE)
@pytest.mark.display(capture_times_ns=_DECODE_CAPTURE_TIMES_NS)
@pytest.mark.duration(60)
def test_gifio_decode(request, circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "size 16 16" in output
    assert "frame_count 13" in output
    for frame_index in range(_DECODE_FRAME_COUNT):
        assert f"rendered {frame_index}" in output
    assert "done" in output

    captures = circuitpython.display_capture_paths()
    assert len(captures) == _DECODE_FRAME_COUNT
    golden_dir = Path(__file__).parent / "golden"
    for frame_index, capture in enumerate(captures):
        golden = golden_dir / f"gifio_frame_{frame_index:02d}_320x240.png"
        _golden_compare_or_update(request, [capture], golden)
