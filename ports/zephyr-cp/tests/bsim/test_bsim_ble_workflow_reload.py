# SPDX-FileCopyrightText: 2026 Dan Halbert for Adafruit Industries
# SPDX-License-Identifier: MIT

"""The supervisor BLE workflow connection survives a VM reload (bsim).

A second CircuitPython device connects to the workflow, pairs, and lists the
root directory. The test then interrupts the workflow device's code.py and
reloads it with Ctrl-D, the same path as typing at the "Press any key" prompt
or saving a file from the web editor. The connection must stay up and file
transfer must still work afterwards.

In the variant where code.py created a `_bleio.Service`, that service must be
gone from the GATT table after the reload, while the workflow connection still
survives: Zephyr can unregister services individually, so no stack restart is
needed.
"""

import pytest

from .conftest import get_library_files

_ADAFRUIT_BLE = get_library_files("adafruit_ble")
_ADAFRUIT_BLE_FILE_TRANSFER = get_library_files("adafruit_ble_file_transfer")

# Device 1, plain: idle code.py until the test interrupts it.
WORKFLOW_IDLE_CODE = """\
import supervisor
import time
print("run", supervisor.runtime.run_reason)
print("workflow ready")
# Idle in short sleeps: on zephyr-cp a Ctrl-C from the UART console is only
# noticed when the current time.sleep() ends, so keep each one brief.
while True:
    time.sleep(0.5)
"""

# Device 1, with a user service: create a Battery Service only on the first
# run, so that after the reload it must have been removed by the supervisor,
# not recreated by code.py.
WORKFLOW_SERVICE_CODE = """\
import supervisor
import time
import _bleio
print("run", supervisor.runtime.run_reason)
if supervisor.runtime.run_reason == supervisor.RunReason.STARTUP:
    service = _bleio.Service(_bleio.UUID(0x180F))
    _bleio.Characteristic.add_to_service(
        service, _bleio.UUID(0x2A19),
        properties=_bleio.Characteristic.READ,
        read_perm=_bleio.Attribute.OPEN,
        write_perm=_bleio.Attribute.NO_ACCESS,
        max_length=1, fixed_length=True, initial_value=b"\\x64",
    )
    print("user service created")
print("workflow ready")
# Idle in short sleeps: on zephyr-cp a Ctrl-C from the UART console is only
# noticed when the current time.sleep() ends, so keep each one brief.
while True:
    time.sleep(0.5)
"""

# Device 2: connect, pair, list files, report whether the Battery Service is
# present, then wait through the reload and do it all again on the same
# connection.
CLIENT_CODE = """\
import time
import _bleio
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.uuid import StandardUUID
from adafruit_ble_file_transfer import FileTransferService, FileTransferClient

BATTERY = _bleio.UUID(0x180F)

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

def battery_present():
    services = connection._bleio_connection.discover_remote_services()
    return any(s.uuid == BATTERY for s in services)

def list_root(tag):
    service = connection[FileTransferService]
    client = FileTransferClient(service)
    names = [e[0] for e in client.listdir("/")]
    print("ft names", tag, names)
    print("ft code.py listed", tag, "code.py" in names)

print("battery before", battery_present())
list_root("before")
print("client ready for reload")

# The test reloads the workflow device now. Give it time to come back.
time.sleep(20)
print("still connected", connection.connected)
list_root("after")
print("battery after", battery_present())
print("client done")
connection.disconnect()
"""

CLIENT_SETTINGS = "CIRCUITPY_BLE_WORKFLOW = false\n"

CLIENT_DRIVE = {
    "code.py": CLIENT_CODE,
    "settings.toml": CLIENT_SETTINGS,
    **_ADAFRUIT_BLE_FILE_TRANSFER,
    **_ADAFRUIT_BLE,
}


def _reload_workflow_and_check(workflow, client):
    client.serial.wait_for("client ready for reload", timeout=90)

    # Interrupt code.py, then reload from the "Press any key" prompt. Before the
    # fix, the first byte at that prompt restarted the BLE stack and dropped the
    # workflow connection.
    workflow.serial.write("\x03")
    workflow.serial.wait_for("KeyboardInterrupt", timeout=30)
    workflow.serial.write("\x04")
    workflow.serial.wait_for("RunReason.REPL_RELOAD", timeout=30)

    client.serial.wait_for("client done", timeout=90)

    client_output = client.serial.all_output
    assert "paired True" in client_output, f"pairing did not succeed: {client_output}"
    assert "ft code.py listed before True" in client_output, (
        f"listdir before reload did not include code.py: {client_output}"
    )
    assert "still connected True" in client_output, (
        f"workflow connection did not survive the reload: {client_output}"
    )
    assert "ft code.py listed after True" in client_output, (
        f"listdir after reload did not include code.py: {client_output}"
    )

    workflow_output = workflow.serial.all_output
    assert "safe mode" not in workflow_output.lower(), (
        f"workflow device entered safe mode: {workflow_output}"
    )
    return client_output


@pytest.mark.port_resets(3)
@pytest.mark.duration(90)
@pytest.mark.circuitpy_drive({"code.py": WORKFLOW_IDLE_CODE})
@pytest.mark.circuitpy_drive(CLIENT_DRIVE)
def test_bsim_workflow_reload_keeps_connection(board, bsim_phy, circuitpython1, circuitpython2):
    """A Ctrl-D reload with no user services leaves the workflow connection up."""
    client_output = _reload_workflow_and_check(circuitpython1, circuitpython2)
    assert "battery before False" in client_output
    assert "battery after False" in client_output


@pytest.mark.port_resets(3)
@pytest.mark.duration(90)
@pytest.mark.circuitpy_drive({"code.py": WORKFLOW_SERVICE_CODE})
@pytest.mark.circuitpy_drive(CLIENT_DRIVE)
def test_bsim_workflow_reload_removes_user_service(
    board, bsim_phy, circuitpython1, circuitpython2
):
    """A reload removes a user-created service without dropping the connection."""
    workflow = circuitpython1
    client_output = _reload_workflow_and_check(workflow, circuitpython2)
    assert "user service created" in workflow.serial.all_output
    assert "battery before True" in client_output, (
        f"client did not see the user service before the reload: {client_output}"
    )
    assert "battery after False" in client_output, (
        f"user service still present after the reload: {client_output}"
    )
