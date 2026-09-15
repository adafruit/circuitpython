# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""BLE peripheral connection tests for bsim."""

import pytest


BSIM_PERIPHERAL_CODE = """\
import _bleio
import time
import sys

adapter = _bleio.adapter

name = b"CPPERIPH"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name

print("peripheral start")
adapter.start_advertising(advertisement, connectable=True)
print("advertising", adapter.advertising)

was_connected = False
timeout = time.monotonic() + 8.0
while not was_connected and time.monotonic() < timeout:
    time.sleep(0.01)
    was_connected = adapter.connected

if not was_connected:
    print("connect timed out")
    sys.exit(-1)

print("connected", was_connected, "advertising", adapter.advertising)

if was_connected:
    timeout = time.monotonic() + 8.0
    while adapter.connected and time.monotonic() < timeout:
        time.sleep(0.1)

print("disconnected", adapter.connected, len(adapter.connections))
"""

BSIM_CENTRAL_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

print("central start")
target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable and b"CPPERIPH" in entry.advertisement_bytes:
        target = entry.address
        print("found peripheral")
        break
adapter.stop_scan()
print("have target", target is not None)

if target is None:
    raise RuntimeError("No connectable peripheral found")

connection = adapter.connect(target, timeout=5.0)
print("connected", connection.connected, adapter.connected, len(adapter.connections))
connection.disconnect()

timeout = time.monotonic() + 4.0
while (connection.connected or adapter.connected) and time.monotonic() < timeout:
    time.sleep(0.1)

print("disconnected", connection.connected, adapter.connected, len(adapter.connections))
"""


@pytest.mark.zephyr_sample("bluetooth/central")
@pytest.mark.duration(14)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_PERIPHERAL_CODE, "settings.toml": "CIRCUITPY_BLE_WORKFLOW = false\n"}
)
def test_bsim_peripheral_zephyr_central(bsim_phy, circuitpython, zephyr_sample):
    """Advertise as connectable from CP; Zephyr central connects and disconnects."""
    central = zephyr_sample

    circuitpython.wait_until_done()

    cp_output = circuitpython.serial.all_output
    central_output = central.serial.all_output

    assert "peripheral start" in cp_output
    assert "advertising True" in cp_output
    assert "connected True advertising False" in cp_output
    assert "disconnected False 0" in cp_output

    assert "Scanning successfully started" in central_output
    assert "Connected:" in central_output
    assert "Disconnected:" in central_output


@pytest.mark.duration(20)
@pytest.mark.circuitpy_drive({"code.py": BSIM_PERIPHERAL_CODE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_CENTRAL_CODE})
def test_bsim_peripheral_cp_central(bsim_phy, circuitpython1, circuitpython2):
    """Two CP instances: device 0 peripheral, device 1 central."""
    peripheral = circuitpython1
    central = circuitpython2

    central.wait_until_done()
    peripheral.wait_until_done()

    periph_output = peripheral.serial.all_output
    central_output = central.serial.all_output

    assert "peripheral start" in periph_output
    assert "advertising True" in periph_output
    assert "connected True advertising False" in periph_output
    assert "disconnected False 0" in periph_output

    assert "central start" in central_output
    assert "found peripheral" in central_output
    assert "have target True" in central_output
    assert "connected True True 1" in central_output
    assert "disconnected False False 0" in central_output
