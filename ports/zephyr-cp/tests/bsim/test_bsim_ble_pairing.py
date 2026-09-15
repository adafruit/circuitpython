# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""BLE pairing tests for the bsim boards."""

import pytest

BSIM_PERIPHERAL_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

name = b"PAIRPERIPH"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name

print("peripheral start")
adapter.start_advertising(advertisement, connectable=True)
print("peripheral advertising", adapter.advertising)

# Wait for connection
timeout = time.monotonic() + 12.0
while not adapter.connected and time.monotonic() < timeout:
    time.sleep(0.01)

if not adapter.connected:
    print("peripheral connect timed out")
    raise SystemExit(1)

print("peripheral connected", adapter.connected)

# Wait for pairing to complete (central initiates it)
# The peripheral's SMP module responds automatically.
timeout = time.monotonic() + 10.0
paired = False
while time.monotonic() < timeout:
    if adapter.connected and len(adapter.connections) > 0:
        conn = adapter.connections[0]
        if conn.paired:
            paired = True
            break
    time.sleep(0.1)

print("peripheral paired", paired)

# Wait for disconnect
timeout = time.monotonic() + 10.0
while adapter.connected and time.monotonic() < timeout:
    time.sleep(0.1)

print("peripheral disconnected", adapter.connected, len(adapter.connections))
"""

BSIM_CENTRAL_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

print("central start")
target = None
for entry in adapter.start_scan(timeout=8.0, active=True):
    if entry.connectable and b"PAIRPERIPH" in entry.advertisement_bytes:
        target = entry.address
        print("central found target")
        break
adapter.stop_scan()
print("central have target", target is not None)

if target is None:
    raise RuntimeError("No connectable target found")

connection = adapter.connect(target, timeout=5.0)
print("central connected", connection.connected, adapter.connected, len(adapter.connections))

# Pair with the peripheral (Just Works pairing)
connection.pair()
print("central paired", connection.paired)

# Disconnect cleanly
connection.disconnect()

timeout = time.monotonic() + 4.0
while (connection.connected or adapter.connected) and time.monotonic() < timeout:
    time.sleep(0.1)

print("central disconnected", connection.connected, adapter.connected, len(adapter.connections))
"""


@pytest.mark.duration(22)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_PERIPHERAL_CODE, "settings.toml": "CIRCUITPY_BLE_WORKFLOW = false\n"}
)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_CENTRAL_CODE, "settings.toml": "CIRCUITPY_BLE_WORKFLOW = false\n"}
)
def test_bsim_pairing_cp_to_cp(bsim_phy, board, circuitpython1, circuitpython2):
    """Two CP instances: device 0 peripheral, device 1 central pairs to it."""
    peripheral = circuitpython1
    central = circuitpython2

    central.wait_until_done()
    peripheral.wait_until_done()

    periph_output = peripheral.serial.all_output
    central_output = central.serial.all_output

    # Peripheral assertions
    assert "peripheral start" in periph_output
    assert "peripheral advertising True" in periph_output
    assert "peripheral connected True" in periph_output
    assert "peripheral paired True" in periph_output
    assert "peripheral disconnected False 0" in periph_output

    # Central assertions
    assert "central start" in central_output
    assert "central found target" in central_output
    assert "central have target True" in central_output
    assert "central connected True True 1" in central_output
    assert "central paired True" in central_output
    assert "central disconnected False False 0" in central_output


BSIM_CENTRAL_PAIRED_PROPERTY_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

print("central start")
target = None
for entry in adapter.start_scan(timeout=8.0, active=True):
    if entry.connectable and b"PAIRPERIPH" in entry.advertisement_bytes:
        target = entry.address
        print("central found target")
        break
adapter.stop_scan()

if target is None:
    raise RuntimeError("No connectable target found")

connection = adapter.connect(target, timeout=5.0)
print("central connected", connection.connected)

# Check paired is False before pairing
print("central paired before", connection.paired)

connection.pair()
print("central paired after", connection.paired)

connection.disconnect()

timeout = time.monotonic() + 4.0
while (connection.connected or adapter.connected) and time.monotonic() < timeout:
    time.sleep(0.1)

print("central disconnected", connection.connected)
"""


@pytest.mark.duration(22)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_PERIPHERAL_CODE, "settings.toml": "CIRCUITPY_BLE_WORKFLOW = false\n"}
)
@pytest.mark.circuitpy_drive(
    {
        "code.py": BSIM_CENTRAL_PAIRED_PROPERTY_CODE,
        "settings.toml": "CIRCUITPY_BLE_WORKFLOW = false\n",
    }
)
def test_bsim_pairing_paired_property(bsim_phy, board, circuitpython1, circuitpython2):
    """Verify connection.paired transitions from False to True after pairing."""
    peripheral = circuitpython1
    central = circuitpython2

    central.wait_until_done()
    peripheral.wait_until_done()

    periph_output = peripheral.serial.all_output
    central_output = central.serial.all_output

    assert "peripheral paired True" in periph_output

    assert "central paired before False" in central_output
    assert "central paired after True" in central_output
    assert "central disconnected False" in central_output


BSIM_PERIPHERAL_BOND_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

name = b"BONDPERIPH"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name

print("peripheral start")

# First connection - pair
adapter.start_advertising(advertisement, connectable=True)
print("peripheral advertising", adapter.advertising)

timeout = time.monotonic() + 12.0
while not adapter.connected and time.monotonic() < timeout:
    time.sleep(0.01)

if not adapter.connected:
    print("peripheral connect1 timed out")
    raise SystemExit(1)

print("peripheral connected1", adapter.connected)

# Wait for pairing
timeout = time.monotonic() + 10.0
paired = False
while time.monotonic() < timeout:
    if adapter.connected and len(adapter.connections) > 0:
        conn = adapter.connections[0]
        if conn.paired:
            paired = True
            break
    time.sleep(0.1)

print("peripheral paired1", paired)

# Wait for disconnect
timeout = time.monotonic() + 10.0
while adapter.connected and time.monotonic() < timeout:
    time.sleep(0.1)

print("peripheral disconnected1", adapter.connected, len(adapter.connections))

# Second connection - bond should persist through storage.
# Calling pair() uses the stored bond keys without re-pairing.
time.sleep(0.5)
adapter.start_advertising(advertisement, connectable=True)
print("peripheral advertising2", adapter.advertising)

timeout = time.monotonic() + 12.0
while not adapter.connected and time.monotonic() < timeout:
    time.sleep(0.01)

if not adapter.connected:
    print("peripheral connect2 timed out")
    raise SystemExit(1)

print("peripheral connected2", adapter.connected)

# Request security to restore bond encryption
conn = adapter.connections[0]
conn.pair()
print("peripheral paired2", conn.paired)

# Wait for disconnect
timeout = time.monotonic() + 10.0
while adapter.connected and time.monotonic() < timeout:
    time.sleep(0.1)

print("peripheral disconnected2", adapter.connected)
"""

BSIM_CENTRAL_BOND_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

print("central start")
target_addr = None
for entry in adapter.start_scan(timeout=8.0, active=True):
    if entry.connectable and b"BONDPERIPH" in entry.advertisement_bytes:
        target_addr = entry.address
        print("central found target")
        break
adapter.stop_scan()
print("central have target", target_addr is not None)

if target_addr is None:
    raise RuntimeError("No connectable target found")

# First connection - pair
connection = adapter.connect(target_addr, timeout=5.0)
print("central connected1", connection.connected, adapter.connected, len(adapter.connections))

print("central paired before1", connection.paired)
connection.pair()
print("central paired after1", connection.paired)

connection.disconnect()

timeout = time.monotonic() + 4.0
while (connection.connected or adapter.connected) and time.monotonic() < timeout:
    time.sleep(0.1)

print("central disconnected1", connection.connected, adapter.connected)

# Second connection - bond should be restored from storage.
# Calling pair() uses the stored bond keys without re-pairing.
time.sleep(1.0)
connection2 = adapter.connect(target_addr, timeout=5.0)
print("central connected2", connection2.connected, adapter.connected, len(adapter.connections))

connection2.pair()
print("central paired bond", connection2.paired)

connection2.disconnect()

timeout = time.monotonic() + 4.0
while (connection2.connected or adapter.connected) and time.monotonic() < timeout:
    time.sleep(0.1)

print("central disconnected2", connection2.connected, adapter.connected)
"""


@pytest.mark.duration(35)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_PERIPHERAL_BOND_CODE, "settings.toml": "CIRCUITPY_BLE_WORKFLOW = false\n"}
)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_CENTRAL_BOND_CODE, "settings.toml": "CIRCUITPY_BLE_WORKFLOW = false\n"}
)
def test_bsim_bonding_persistence(bsim_phy, board, circuitpython1, circuitpython2):
    """Verify bonds are saved to storage and survive disconnect/reconnect.

    Central pairs with peripheral, disconnects, then reconnects.
    On the second connection the bond should be restored from storage
    and connection.paired should be True without calling pair() again.
    """
    peripheral = circuitpython1
    central = circuitpython2

    central.wait_until_done()
    peripheral.wait_until_done()

    periph_output = peripheral.serial.all_output
    central_output = central.serial.all_output

    # Peripheral assertions - first connection
    assert "peripheral start" in periph_output
    assert "peripheral advertising True" in periph_output
    assert "peripheral connected1 True" in periph_output
    assert "peripheral paired1 True" in periph_output
    assert "peripheral disconnected1 False 0" in periph_output
    # Peripheral - second connection (private advertising, bond restored from storage)
    assert "peripheral advertising2 True" in periph_output
    assert "peripheral connected2 True" in periph_output
    assert "peripheral paired2 True" in periph_output
    assert "peripheral disconnected2 False" in periph_output

    # Central assertions - first connection
    assert "central start" in central_output
    assert "central found target" in central_output
    assert "central have target True" in central_output
    assert "central connected1 True True 1" in central_output
    assert "central paired before1 False" in central_output
    assert "central paired after1 True" in central_output
    assert "central disconnected1 False False" in central_output
    # Central - second connection (bond restored from storage)
    assert "central connected2 True True 1" in central_output
    assert "central paired bond True" in central_output
    assert "central disconnected2 False False" in central_output
