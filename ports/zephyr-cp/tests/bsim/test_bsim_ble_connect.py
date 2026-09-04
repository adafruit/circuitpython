# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""BLE central connection tests for bsim."""

import pytest


BSIM_CONNECT_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

print("connect start")
target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable:
        target = entry.address
        print("found target")
        break
adapter.stop_scan()
print("have target", target is not None)

if target is None:
    raise RuntimeError("No connectable target found")

connection = adapter.connect(target, timeout=5.0)
print("connected", connection.connected, adapter.connected, len(adapter.connections))
connection.disconnect()

for _ in range(40):
    if not connection.connected and not adapter.connected:
        break
    time.sleep(0.1)

print("disconnected", connection.connected, adapter.connected, len(adapter.connections))
"""

BSIM_RECONNECT_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

print("run start")
target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable:
        target = entry.address
        print("run found target")
        break
adapter.stop_scan()
print("run have target", target is not None)

if target is None:
    raise RuntimeError("No connectable target found")

connection = adapter.connect(target, timeout=5.0)
print("run connected", connection.connected, adapter.connected, len(adapter.connections))
connection.disconnect()

for _ in range(50):
    if not connection.connected and not adapter.connected and len(adapter.connections) == 0:
        break
    time.sleep(0.1)

print("run disconnected", connection.connected, adapter.connected, len(adapter.connections))
"""


@pytest.mark.zephyr_sample("bluetooth/peripheral")
@pytest.mark.duration(14)
@pytest.mark.circuitpy_drive({"code.py": BSIM_CONNECT_CODE})
def test_bsim_connect_zephyr_peripheral(bsim_phy, circuitpython, zephyr_sample):
    """Connect to the Zephyr peripheral sample and disconnect cleanly."""
    peripheral = zephyr_sample

    circuitpython.wait_until_done()

    cp_output = circuitpython.serial.all_output
    peripheral_output = peripheral.serial.all_output

    assert "connect start" in cp_output
    assert "found target" in cp_output
    assert "have target True" in cp_output
    assert "connected True True 1" in cp_output
    assert "disconnected False False 0" in cp_output

    assert "Advertising successfully started" in peripheral_output
    assert "Connected" in peripheral_output


@pytest.mark.zephyr_sample("bluetooth/peripheral_sc_only")
@pytest.mark.port_resets(3)
@pytest.mark.duration(26)
@pytest.mark.circuitpy_drive({"code.py": BSIM_RECONNECT_CODE})
def test_bsim_reconnect_zephyr_peripheral(bsim_phy, circuitpython, zephyr_sample):
    """Connect/disconnect, soft reload, then connect/disconnect again."""
    peripheral = zephyr_sample

    circuitpython.serial.wait_for("run disconnected")
    circuitpython.serial.wait_for("Press any key to enter the REPL")
    circuitpython.serial.write("\x04")

    circuitpython.wait_until_done()

    cp_output = circuitpython.serial.all_output
    peripheral_output = peripheral.serial.all_output

    assert cp_output.count("run start") >= 2
    assert cp_output.count("run found target") >= 2
    assert cp_output.count("run have target True") >= 2
    assert cp_output.count("run connected True True 1") >= 2
    assert cp_output.count("run disconnected False False 0") >= 2

    assert "Advertising successfully started" in peripheral_output
    assert peripheral_output.count("Connected") >= 2
