# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test adafruit_bus_device.i2c_device.I2CDevice against the AT24 EEPROM emulator."""

import pytest


PROBE_OK_CODE = """\
import board
from adafruit_bus_device.i2c_device import I2CDevice

i2c = board.I2C()
device = I2CDevice(i2c, 0x50)
print(f"device created: {type(device).__name__}")
i2c.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": PROBE_OK_CODE})
def test_i2cdevice_probe_success(circuitpython):
    """Constructing I2CDevice with probe=True succeeds when AT24 is present at 0x50."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "device created: I2CDevice" in output
    assert "done" in output


PROBE_FAIL_CODE = """\
import board
from adafruit_bus_device.i2c_device import I2CDevice

i2c = board.I2C()
try:
    device = I2CDevice(i2c, 0x51)
    print("unexpected_success")
except ValueError as e:
    print(f"probe_failed: {e}")
except OSError as e:
    print(f"probe_failed: {e}")
i2c.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": PROBE_FAIL_CODE})
def test_i2cdevice_probe_failure(circuitpython):
    """Constructing I2CDevice with probe=True raises when no device responds."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "probe_failed" in output
    assert "unexpected_success" not in output
    assert "done" in output


PROBE_DISABLED_CODE = """\
import board
from adafruit_bus_device.i2c_device import I2CDevice

i2c = board.I2C()
# probe=False should not raise even without a device at this address
device = I2CDevice(i2c, 0x51, probe=False)
print(f"device created: {type(device).__name__}")
i2c.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": PROBE_DISABLED_CODE})
def test_i2cdevice_probe_disabled(circuitpython):
    """Constructing I2CDevice with probe=False succeeds regardless of device presence."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "device created: I2CDevice" in output
    assert "done" in output


READ_CODE = """\
import board
from adafruit_bus_device.i2c_device import I2CDevice

i2c = board.I2C()
device = I2CDevice(i2c, 0x50)

# Read first byte by writing the memory address, then reading.
buf = bytearray(1)
with device:
    device.write(bytes([0x00]))
    device.readinto(buf)
print(f"AT24 byte 0: 0x{buf[0]:02X}")
if buf[0] == 0xFF:
    print("eeprom_valid")
i2c.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": READ_CODE})
def test_i2cdevice_write_then_readinto_separate(circuitpython):
    """write() followed by readinto() inside a single context manager block reads EEPROM data."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "AT24 byte 0: 0xFF" in output
    assert "eeprom_valid" in output
    assert "done" in output


WRITE_THEN_READINTO_CODE = """\
import board
from adafruit_bus_device.i2c_device import I2CDevice

i2c = board.I2C()
device = I2CDevice(i2c, 0x50)

out_buf = bytes([0x00])
in_buf = bytearray(4)
with device:
    device.write_then_readinto(out_buf, in_buf)
print(f"first 4 bytes: {[hex(b) for b in in_buf]}")
if all(b == 0xFF for b in in_buf):
    print("eeprom_valid")
i2c.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": WRITE_THEN_READINTO_CODE})
def test_i2cdevice_write_then_readinto_atomic(circuitpython):
    """write_then_readinto() performs an atomic write+read against the EEPROM."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "first 4 bytes:" in output
    assert "eeprom_valid" in output
    assert "done" in output


WRITE_READBACK_CODE = """\
import board
import time
from adafruit_bus_device.i2c_device import I2CDevice

i2c = board.I2C()
device = I2CDevice(i2c, 0x50)

# Write four bytes starting at EEPROM address 0x10.
payload = bytes([0xDE, 0xAD, 0xBE, 0xEF])
with device:
    device.write(bytes([0x10]) + payload)

# EEPROM needs a moment to commit the write internally.
time.sleep(0.01)

readback = bytearray(4)
with device:
    device.write_then_readinto(bytes([0x10]), readback)
print(f"readback: {[hex(b) for b in readback]}")
if bytes(readback) == payload:
    print("readback_ok")
i2c.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": WRITE_READBACK_CODE})
def test_i2cdevice_write_then_read_roundtrip(circuitpython):
    """Writing bytes to the EEPROM and reading them back returns the written values."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "readback_ok" in output
    assert "done" in output


SLICE_CODE = """\
import board
from adafruit_bus_device.i2c_device import I2CDevice

i2c = board.I2C()
device = I2CDevice(i2c, 0x50)

# Use start/end kwargs to send only a slice of the outgoing buffer.
out = bytearray([0xAA, 0x00, 0xBB])
dest = bytearray(4)
with device:
    # Only send the middle byte (the memory address 0x00).
    device.write_then_readinto(out, dest, out_start=1, out_end=2, in_start=0, in_end=2)
print(f"partial read: {[hex(b) for b in dest]}")
# Only the first two entries should have been written by the read.
if dest[0] == 0xFF and dest[1] == 0xFF and dest[2] == 0x00 and dest[3] == 0x00:
    print("slice_ok")
i2c.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": SLICE_CODE})
def test_i2cdevice_buffer_slices(circuitpython):
    """write_then_readinto honors out_start/out_end and in_start/in_end bounds."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "slice_ok" in output
    assert "done" in output


DISABLED_CODE = """\
import board
from adafruit_bus_device.i2c_device import I2CDevice

i2c = board.I2C()
try:
    device = I2CDevice(i2c, 0x50)
    print("unexpected_success")
except (ValueError, OSError) as e:
    print(f"probe_failed: {e}")
i2c.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": DISABLED_CODE})
@pytest.mark.disable_i2c_devices("eeprom@50")
def test_i2cdevice_probe_fails_when_device_disabled(circuitpython):
    """Probe fails when the AT24 emulator device is disabled on the bus."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "probe_failed" in output
    assert "unexpected_success" not in output
    assert "done" in output
