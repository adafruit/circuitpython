# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Serial (Nordic-UART-style) service test for the supervisor BLE workflow
(bsim).

The supervisor's BLE workflow exposes a CircuitPython serial service whose RX
characteristic accepts console stdin (write, encrypted) and whose TX
characteristic emits console stdout via notifications (encrypted). One
CircuitPython device runs code.py that reads a line with `input()` and echoes
it back with `print()`; a second CircuitPython device connects, pairs,
subscribes to TX, writes a line to RX, and verifies the echo comes back over TX.

The serial service requires encryption (Just Works pairing). nrf54lm20bsim LE
encryption is not yet functional in bsim, so this test is restricted to
native_nrf5340bsim (matching test_bsim_ble_pairing.py). Real hardware works on
both.
"""

import pytest

from .conftest import get_library_files

_ADAFRUIT_BLE = get_library_files("adafruit_ble")

# nrf54lm20bsim LE encryption is not yet functional in bsim. The bsim `board`
# fixture parametrizes over both boards (the circuitpython_board marker is
# documentation only here), so skip the non-functional board explicitly.
pytestmark = pytest.mark.circuitpython_board("native_nrf5340bsim")


def _skip_unless_nrf5340bsim(board):
    if board != "native_nrf5340bsim":
        pytest.skip(f"BLE encryption not functional on {board} in bsim")


# Device 1: read a line from console stdin (BLE RX) and echo it to console
# stdout (mirrored to BLE TX). `input()` blocks until the central writes, so the
# device stays alive for the exchange.
WORKFLOW_CODE = """\
import sys
print("nus ready")
sys.stdout.flush()
try:
    line = input()
except EOFError:
    line = ""
print("nus_echo:" + line)
"""

# Device 2: connect, pair, discover the CircuitPython serial service, subscribe
# to TX, write a line to RX, and read the echo back over TX.
CLIENT_CODE = """\
import time
import _bleio
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.uuid import StandardUUID

ble = BLERadio()
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

# CircuitPython serial service UUID (128-bit, "nhtyPtiucriC" base + 0x0001).
serial_uuid = _bleio.UUID(b"nhtyPtiucriC\\x01\\x00\\xaf\\xad")
services = connection._bleio_connection.discover_remote_services([serial_uuid])
svc = services[0]
print("serial service", svc.uuid.uuid16)

rx_char = None
tx_char = None
for ch in svc.characteristics:
    if ch.uuid.uuid16 == 0x0002:
        rx_char = ch
    elif ch.uuid.uuid16 == 0x0003:
        tx_char = ch
print("rx", rx_char is not None, "tx", tx_char is not None)

# Subscribe to TX notifications (client-side PacketBuffer writes the CCCD).
tx_pb = _bleio.PacketBuffer(tx_char, buffer_size=4, max_packet_size=128)

# Write a line to RX (console stdin). RX has WRITE_NO_RESPONSE, so `.value =`
# does a GATT write without response.
rx_char.value = b"ZZZ\\r"
print("rx written")

buf = bytearray(128)
got = b""
deadline = time.monotonic() + 10
while time.monotonic() < deadline:
    n = tx_pb.readinto(buf)
    if n:
        got += bytes(buf[:n])
        if b"nus_echo:ZZZ" in got:
            print("nus_received nus_echo:ZZZ")
            break
print("nus done", b"nus_echo:ZZZ" in got)
connection.disconnect()
"""

CLIENT_SETTINGS = "CIRCUITPY_BLE_WORKFLOW = false\n"


@pytest.mark.duration(30)
@pytest.mark.circuitpy_drive({"code.py": WORKFLOW_CODE})
@pytest.mark.circuitpy_drive(
    {
        "code.py": CLIENT_CODE,
        "settings.toml": CLIENT_SETTINGS,
        **_ADAFRUIT_BLE,
    }
)
def test_bsim_workflow_nus(board, bsim_phy, circuitpython1, circuitpython2):
    """The supervisor BLE workflow's serial service echoes console I/O over BLE."""
    _skip_unless_nrf5340bsim(board)
    workflow = circuitpython1
    client = circuitpython2

    client.wait_until_done()

    client_output = client.serial.all_output
    assert "found workflow" in client_output, f"client never found the workflow: {client_output}"
    assert "paired True" in client_output, f"pairing did not succeed: {client_output}"
    assert "serial service 1" in client_output, (
        f"CircuitPython serial service not discovered: {client_output}"
    )
    assert "rx True tx True" in client_output, (
        f"RX/TX characteristics not discovered: {client_output}"
    )
    assert "rx written" in client_output, f"RX write failed: {client_output}"
    assert "nus_received nus_echo:ZZZ" in client_output, (
        f"echo 'nus_echo:ZZZ' never received over TX: {client_output}"
    )
    assert "nus done True" in client_output, f"echo not confirmed: {client_output}"

    workflow_output = workflow.serial.all_output
    assert "safe mode" not in workflow_output.lower(), (
        f"workflow device entered safe mode: {workflow_output}"
    )
