# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test I2C functionality on native_sim."""

I2C_SCAN_CODE = """\
import board

i2c = board.I2C()
while not i2c.try_lock():
    pass
devices = i2c.scan()
print(f"I2C devices: {[hex(d) for d in devices]}")
i2c.unlock()
i2c.deinit()
print("done")
"""


def test_i2c_scan(run_circuitpython):
    """Test I2C bus scanning finds emulated devices.

    The AT24 EEPROM emulator responds to zero-length probe writes,
    so it should appear in scan results at address 0x50.
    """
    result = run_circuitpython(I2C_SCAN_CODE, timeout=5.0)

    assert "I2C devices:" in result.output
    # AT24 EEPROM should be at address 0x50
    assert "0x50" in result.output
    assert "done" in result.output


AT24_READ_CODE = """\
import board

i2c = board.I2C()
while not i2c.try_lock():
    pass

# AT24 EEPROM at address 0x50
AT24_ADDR = 0x50

# Read first byte from address 0
result = bytearray(1)
try:
    i2c.writeto_then_readfrom(AT24_ADDR, bytes([0x00]), result)
    value = result[0]
    print(f"AT24 byte 0: 0x{value:02X}")
    # Fresh EEPROM should be 0xFF
    if value == 0xFF:
        print("eeprom_valid")
    else:
        print(f"unexpected value: expected 0xFF, got 0x{value:02X}")
except OSError as e:
    print(f"I2C error: {e}")

i2c.unlock()
i2c.deinit()
print("done")
"""


def test_i2c_at24_read(run_circuitpython):
    """Test reading from AT24 EEPROM emulator."""
    result = run_circuitpython(AT24_READ_CODE, timeout=5.0)

    assert "AT24 byte 0: 0xFF" in result.output
    assert "eeprom_valid" in result.output
    assert "done" in result.output


def test_i2c_device_disabled(run_circuitpython):
    """Test that disabled I2C device doesn't appear in scan."""
    result = run_circuitpython(
        I2C_SCAN_CODE,
        timeout=5.0,
        disabled_i2c_devices=["eeprom@50"],
    )

    assert "I2C devices:" in result.output
    # AT24 at 0x50 should NOT appear when disabled
    assert "0x50" not in result.output
    assert "done" in result.output


def test_i2c_device_disabled_communication_fails(run_circuitpython):
    """Test that communication with disabled I2C device fails."""
    result = run_circuitpython(
        AT24_READ_CODE,
        timeout=5.0,
        disabled_i2c_devices=["eeprom@50"],
    )

    # Should get an I2C error when trying to communicate
    assert "I2C error" in result.output
    assert "eeprom_valid" not in result.output
    assert "done" in result.output
