# SPDX-FileCopyrightText: 2026 Vladimir Smitka
# SPDX-License-Identifier: MIT

"""busio.SPI on native_sim, through the SPI loopback device."""

import pytest

LOOPBACK_CODE = """\
import board

spi = board.SPI()
while not spi.try_lock():
    pass
spi.configure(baudrate=1_000_000)
bad = 0
for n in (1, 31, 32, 33, 4096):
    out = bytes(i * 7 & 0xFF for i in range(n))
    inb = bytearray(n)
    spi.write_readinto(out, inb)
    if inb != out:
        bad += 1
        print("write_readinto", n, "differs")
    spi.readinto(inb, write_value=0xA5)
    if inb != b"\\xa5" * n:
        bad += 1
        print("readinto 0xA5", n, "differs")
    spi.readinto(inb)
    if inb != bytes(n):
        bad += 1
        print("readinto 0", n, "differs")
    spi.write(out)
out = bytes(range(64))
inb = bytearray(64)
spi.write_readinto(out, inb, out_start=8, out_end=40, in_start=8, in_end=40)
if inb[8:40] != out[8:40] or any(inb[:8]) or any(inb[40:]):
    bad += 1
    print("slices differ")
spi.unlock()
spi.deinit()
print("frequency", spi.frequency if False else "ok")
print("bad", bad)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": LOOPBACK_CODE})
def test_spi_loopback(circuitpython):
    """Blocking transfers come back through the loopback, including slices and reads."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "differs" not in output
    assert "bad 0" in output
    assert "done" in output
