# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""BLE scanning tests for bsim."""

import pytest


BSIM_SCAN_CODE = """\
import _bleio

adapter = _bleio.adapter
print("scan start")
scan = adapter.start_scan(timeout=4.0, active=True)
found = False
for entry in scan:
    if b"zephyrproject" in entry.advertisement_bytes:
        print("found beacon")
        found = True
        break
adapter.stop_scan()
print("scan done", found)
"""

BSIM_SCAN_RELOAD_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

print("scan run start")
found = False
for entry in adapter.start_scan(active=True):
    if b"zephyrproject" in entry.advertisement_bytes:
        print("found beacon run")
        found = True
        break
adapter.stop_scan()
print("scan run done", found)
"""

BSIM_SCAN_RELOAD_NO_STOP_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

print("scan run start")
found = False
for entry in adapter.start_scan(active=True):
    if b"zephyrproject" in entry.advertisement_bytes:
        print("found beacon run")
        found = True
        break
print("scan run done", found)
"""

BSIM_SCAN_ENTRY_PROPS_CODE = """\
import _bleio

adapter = _bleio.adapter
print("scan start")
found = None
for entry in adapter.start_scan(timeout=4.0, active=True):
    if b"zephyrproject" in entry.advertisement_bytes:
        found = entry
        break
adapter.stop_scan()

if found is not None:
    print("rssi", found.rssi <= 0 and found.rssi > -100)
    print("connectable", found.connectable)
    print("scan_response", found.scan_response)
    print("address_bytes", len(found.address.address_bytes))
    print("address_type", found.address.type in (0, 1, 2, 3))
    print("adv_bytes_type", isinstance(found.advertisement_bytes, bytes))
else:
    print("no entry found")
print("done")
"""

BSIM_SCAN_PASSIVE_CODE = """\
import _bleio

adapter = _bleio.adapter
print("scan passive start")
found = False
for entry in adapter.start_scan(timeout=4.0, active=False):
    if b"zephyrproject" in entry.advertisement_bytes:
        print("found beacon passive")
        found = True
        break
adapter.stop_scan()
print("scan passive done", found)
"""


@pytest.mark.zephyr_sample("bluetooth/beacon")
@pytest.mark.circuitpy_drive({"code.py": BSIM_SCAN_CODE})
def test_bsim_scan_zephyr_beacon(bsim_phy, circuitpython, zephyr_sample):
    """Scan for Zephyr beacon sample advertisement using bsim."""
    _ = zephyr_sample

    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "scan start" in output
    assert "found beacon" in output
    assert "scan done True" in output


@pytest.mark.zephyr_sample("bluetooth/beacon")
@pytest.mark.port_resets(3)
@pytest.mark.duration(20)
@pytest.mark.circuitpy_drive({"code.py": BSIM_SCAN_RELOAD_CODE})
def test_bsim_scan_zephyr_beacon_reload(bsim_phy, circuitpython, zephyr_sample):
    """Scan for Zephyr beacon, soft reload, and scan again."""
    _ = zephyr_sample

    circuitpython.serial.wait_for("scan run done")
    circuitpython.serial.wait_for("Press any key to enter the REPL")
    circuitpython.serial.write("\x04")

    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert output.count("scan run start") >= 2
    assert output.count("found beacon run") >= 2
    assert output.count("scan run done True") >= 2


@pytest.mark.zephyr_sample("bluetooth/beacon")
@pytest.mark.circuitpy_drive({"code.py": BSIM_SCAN_ENTRY_PROPS_CODE})
def test_bsim_scan_entry_properties(bsim_phy, circuitpython, zephyr_sample):
    """Verify ScanEntry properties: rssi, connectable, scan_response, address, advertisement_bytes."""
    _ = zephyr_sample

    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "rssi True" in output
    assert "connectable False" in output  # beacon is non-connectable
    assert "scan_response False" in output
    assert "address_bytes 6" in output
    assert "address_type True" in output
    assert "adv_bytes_type True" in output
    assert "done" in output


@pytest.mark.zephyr_sample("bluetooth/beacon")
@pytest.mark.circuitpy_drive({"code.py": BSIM_SCAN_PASSIVE_CODE})
def test_bsim_scan_passive(bsim_phy, circuitpython, zephyr_sample):
    """Passive scan finds Zephyr beacon."""
    _ = zephyr_sample

    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "scan passive start" in output
    assert "found beacon passive" in output
    assert "scan passive done True" in output
