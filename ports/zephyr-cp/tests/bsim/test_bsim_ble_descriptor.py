# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""BLE descriptor tests for bsim."""

import pytest

# ===================================================================
# Server: characteristic with user_description
# ===================================================================

BSIM_DESC_SERVER_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

svc = _bleio.Service(_bleio.UUID(0x180F))
char = _bleio.Characteristic.add_to_service(
    svc, _bleio.UUID(0x2A19),
    properties=_bleio.Characteristic.READ,
    read_perm=_bleio.Attribute.OPEN,
    write_perm=_bleio.Attribute.NO_ACCESS,
    max_length=1, fixed_length=True, initial_value=bytes([75]),
    user_description="Battery Level",
)
print("service created")

# Check local descriptors list
descs = char.descriptors
print("num descriptors", len(descs))
for d in descs:
    print("desc uuid", d.uuid.uuid16)
    print("desc value", list(d.value))

name = b"CPDESC"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name
adapter.start_advertising(advertisement, connectable=True)

for _ in range(80):
    if adapter.connected:
        break
    time.sleep(0.1)
print("connected", adapter.connected)

for _ in range(80):
    if not adapter.connected:
        break
    time.sleep(0.1)
print("done")
"""

# ===================================================================
# Client: connects and reads the remote user description
# ===================================================================

BSIM_DESC_CLIENT_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

print("client start")
target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable and b"CPDESC" in entry.advertisement_bytes:
        target = entry.address
        print("found server")
        break
adapter.stop_scan()
print("have target", target is not None)

if target is None:
    raise RuntimeError("No server found")

connection = adapter.connect(target, timeout=5.0)
print("connected", connection.connected)

services = connection.discover_remote_services([_bleio.UUID(0x180F)])
char = services[0].characteristics[0]

print("char value", list(char.value))

descs = char.descriptors
print("num descriptors", len(descs))
for d in descs:
    print("desc uuid", hex(d.uuid.uuid16))
    print("desc value", list(d.value))

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(30)
@pytest.mark.circuitpy_drive({"code.py": BSIM_DESC_SERVER_CODE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_DESC_CLIENT_CODE})
def test_bsim_descriptor_user_description(bsim_phy, circuitpython1, circuitpython2):
    """Server creates a characteristic with user_description;
    client discovers the service and reads the descriptor."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    # Server: user_description creates a CUD descriptor (0x2901) on the characteristic
    assert "service created" in server_output
    assert "num descriptors 1" in server_output
    assert "desc uuid 10497" in server_output  # 0x2901
    assert (
        "desc value [66, 97, 116, 116, 101, 114, 121, 32, 76, 101, 118, 101, 108]" in server_output
    )
    assert "connected True" in server_output

    # Client: discovers and reads the remote descriptor
    assert "found server" in client_output
    assert "char value [75]" in client_output
    assert "num descriptors 1" in client_output
    assert "desc uuid 0x2901" in client_output
    assert (
        "desc value [66, 97, 116, 116, 101, 114, 121, 32, 76, 101, 118, 101, 108]" in client_output
    )
    assert "done" in client_output
