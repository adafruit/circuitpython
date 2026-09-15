# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""File Transfer service test for the supervisor BLE workflow (bsim).

A second CircuitPython device connects to the supervisor's BLE workflow
(advertised with the File Transfer service UUID 0xFEBB), pairs, and uses the
Adafruit_CircuitPython_BLE_File_Transfer library to list the workflow device's
root directory.
"""

import pytest

from .conftest import get_library_files

_ADAFRUIT_BLE = get_library_files("adafruit_ble")
_ADAFRUIT_BLE_FILE_TRANSFER = get_library_files("adafruit_ble_file_transfer")

# Device 1: idle code.py; its workflow exposes the File Transfer service.
WORKFLOW_IDLE_CODE = """\
import time
time.sleep(25)
"""

# Device 2: connect, pair, and listdir("/") over the File Transfer service.
CLIENT_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.uuid import StandardUUID
from adafruit_ble_file_transfer import FileTransferService, FileTransferClient

ble = BLERadio()

# Wait for the workflow device's file transfer server to finish starting up
# before scanning.
time.sleep(5)
print("scan start")
target = None
for adv in ble.start_scan(ProvideServicesAdvertisement, timeout=15, active=True):
    if StandardUUID(0xFEBB) in adv.services:
        target = adv
        print("found workflow")
        break
ble.stop_scan()
if target is None:
    print("no workflow")
    raise SystemExit(1)

connection = ble.connect(target, timeout=10)
print("connected", connection.connected)
connection.pair()
print("paired", connection.paired)

service = connection[FileTransferService]
print("ft version", service.version)
client = FileTransferClient(service)

entries = client.listdir("/")
names = [e[0] for e in entries]
print("ft names", names)
if "code.py" in names:
    print("ft code.py listed")
print("ft done")
connection.disconnect()
"""

CLIENT_SETTINGS = "CIRCUITPY_BLE_WORKFLOW = false\n"


@pytest.mark.duration(30)
@pytest.mark.circuitpy_drive({"code.py": WORKFLOW_IDLE_CODE})
@pytest.mark.circuitpy_drive(
    {
        "code.py": CLIENT_CODE,
        "settings.toml": CLIENT_SETTINGS,
        **_ADAFRUIT_BLE_FILE_TRANSFER,
        **_ADAFRUIT_BLE,
    }
)
def test_bsim_workflow_file_transfer(board, bsim_phy, circuitpython1, circuitpython2):
    """The supervisor BLE workflow's File Transfer service can list files."""
    workflow = circuitpython1
    client = circuitpython2

    client.wait_until_done()

    client_output = client.serial.all_output
    assert "found workflow" in client_output, f"client never found the workflow: {client_output}"
    assert "paired True" in client_output, f"pairing did not succeed: {client_output}"
    assert "ft version 4" in client_output, (
        f"File Transfer version characteristic is not 4: {client_output}"
    )
    assert "ft code.py listed" in client_output, (
        f"listdir did not include code.py: {client_output}"
    )
    assert "ft done" in client_output, f"file transfer did not complete: {client_output}"

    workflow_output = workflow.serial.all_output
    assert "safe mode" not in workflow_output.lower(), (
        f"workflow device entered safe mode: {workflow_output}"
    )
