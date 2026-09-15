# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Nordic UART Service (NUS) tests for bsim — using adafruit_ble library."""

import pytest

from .conftest import get_library_files

_ADAFRUIT_BLE = get_library_files("adafruit_ble")

# ---- Test 1: CP peripheral hosts NUS, Zephyr central writes and reads ----

BSIM_NUS_PERIPHERAL_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

ble = BLERadio()
uart = UARTService()
print("service created")

advertisement = ProvideServicesAdvertisement(uart)
advertisement.complete_name = "CPNUS"
ble.start_advertising(advertisement)
print("advertising")

for _ in range(80):
    if ble.connected:
        break
    time.sleep(0.1)
print("connected", ble.connected)

# Wait for data to arrive from central
data = uart.read(5)
print("received", data)

# Send a response back
uart.write(b"World")
print("sent response")

time.sleep(1.0)

for _ in range(80):
    if not ble.connected:
        break
    time.sleep(0.1)
print("done")
"""


@pytest.mark.zephyr_sample("tests/bsim/samples/central_nus_client")
@pytest.mark.duration(14)
@pytest.mark.circuitpy_drive(
    {
        "code.py": BSIM_NUS_PERIPHERAL_CODE,
        "settings.toml": "CIRCUITPY_BLE_WORKFLOW = false\n",
        **_ADAFRUIT_BLE,
    }
)
def test_bsim_nus_peripheral(bsim_phy, circuitpython, zephyr_sample):
    """CP hosts NUS peripheral; Zephyr central writes to RX, reads TX notifications."""
    circuitpython.wait_until_done()

    cp_output = circuitpython.serial.all_output
    sample_output = zephyr_sample.serial.all_output

    assert "service created" in cp_output
    assert "connected True" in cp_output
    assert "received" in cp_output
    assert "sent response" in cp_output
    assert "NUS: received 'World'" in sample_output


# ---- Test 2: CP-to-CP NUS (peripheral + central) ----

BSIM_NUS_SERVER_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

ble = BLERadio()
uart = UARTService()
print("service created")

advertisement = ProvideServicesAdvertisement(uart)
advertisement.complete_name = "CP2CP"
ble.start_advertising(advertisement)
print("advertising")

for _ in range(80):
    if ble.connected:
        break
    time.sleep(0.1)
print("connected", ble.connected)

# Read incoming data from central
data = uart.read(6)
print("received", data)

# Respond back
uart.write(b"OK!")
print("sent response")

for _ in range(80):
    if not ble.connected:
        break
    time.sleep(0.1)
print("done")
"""

BSIM_NUS_CLIENT_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService

ble = BLERadio()

print("client start")
target = None
for adv in ble.start_scan(ProvideServicesAdvertisement, timeout=8.0):
    if adv.connectable and adv.complete_name == "CP2CP":
        target = adv
        print("found server")
        break
ble.stop_scan()
print("have target", target is not None)

if target is None:
    raise RuntimeError("No server found")

connection = ble.connect(target, timeout=5.0)
print("connected", connection.connected)

uart = connection[UARTService]
print("discovered services", 1)

# Write to server
uart.write(b"Hello!")
print("wrote to rx")

data = uart.read(3)
print("received", data)

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(30)
@pytest.mark.circuitpy_drive({"code.py": BSIM_NUS_SERVER_CODE, **_ADAFRUIT_BLE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_NUS_CLIENT_CODE, **_ADAFRUIT_BLE})
def test_bsim_nus_cp_to_cp(bsim_phy, circuitpython1, circuitpython2):
    """CP peripheral hosts NUS; CP central writes to RX, reads TX via notifications."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    assert "service created" in server_output
    assert "connected True" in server_output
    assert "received" in server_output
    assert "sent response" in server_output

    assert "client start" in client_output
    assert "found server" in client_output
    assert "have target True" in client_output
    assert "connected True" in client_output
    assert "discovered services 1" in client_output
    assert "wrote to rx" in client_output
    assert "received b'OK!'" in client_output
    assert "done" in client_output
