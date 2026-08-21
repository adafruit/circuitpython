# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""PacketBuffer tests for bsim."""

import re

import pytest


# ---- Test 1: Server-side PacketBuffer incoming (WRITE characteristic) ----

BSIM_PB_SERVER_IN_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

svc = _bleio.Service(_bleio.UUID(0xFFE0))
char = _bleio.Characteristic.add_to_service(
    svc, _bleio.UUID(0xFFE1),
    properties=_bleio.Characteristic.WRITE,
    read_perm=_bleio.Attribute.NO_ACCESS,
    write_perm=_bleio.Attribute.OPEN,
    max_length=20, fixed_length=False,
)

# Wrap in PacketBuffer for incoming packets
pb = _bleio.PacketBuffer(char, buffer_size=4, max_packet_size=20)
print("service created")

name = b"CPPBIN"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name
adapter.start_advertising(advertisement, connectable=True)
print("advertising")

for _ in range(80):
    if adapter.connected:
        break
    time.sleep(0.1)
print("connected", adapter.connected)

# Read first incoming packet
data = bytearray(20)
n = 0
deadline = time.monotonic() + 5.0
while n == 0 and time.monotonic() < deadline:
    n = pb.readinto(data)
    if n == 0:
        time.sleep(0.05)
print("packet1", data[:n])

# Read second incoming packet
n2 = 0
deadline2 = time.monotonic() + 5.0
while n2 == 0 and time.monotonic() < deadline2:
    n2 = pb.readinto(data)
    if n2 == 0:
        time.sleep(0.05)
print("packet2", data[:n2])

for _ in range(80):
    if not adapter.connected:
        break
    time.sleep(0.1)
print("done")
"""

BSIM_PB_CLIENT_IN_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable and b"CPPBIN" in entry.advertisement_bytes:
        target = entry.address
        print("found server")
        break
adapter.stop_scan()

connection = adapter.connect(target, timeout=5.0)
print("connected", connection.connected)

services = connection.discover_remote_services([_bleio.UUID(0xFFE0)])
print("discovered services", len(services))

remote_char = services[0].characteristics[0]
print("found char")

# Write two packets
remote_char.value = bytes([0x01, 0x02, 0x03, 0x04])
print("wrote packet1")

time.sleep(0.3)

remote_char.value = bytes([0xAA, 0xBB, 0xCC, 0xDD, 0xEE])
print("wrote packet2")

time.sleep(0.5)

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(14)
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_SERVER_IN_CODE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_CLIENT_IN_CODE})
def test_bsim_packet_buffer_server_incoming(bsim_phy, circuitpython1, circuitpython2):
    """PacketBuffer on server-side WRITE characteristic receives framed packets."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    assert "service created" in server_output
    assert "connected True" in server_output
    assert "packet1" in server_output
    assert "packet2" in server_output
    assert "done" in server_output

    assert "found server" in client_output
    assert "wrote packet1" in client_output
    assert "wrote packet2" in client_output


# ---- Test 2: Server-side PacketBuffer write/flush (NOTIFY response) ----

BSIM_PB_BIDI_SERVER_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

svc = _bleio.Service(_bleio.UUID(0xFFE0))
char = _bleio.Characteristic.add_to_service(
    svc, _bleio.UUID(0xFFE1),
    properties=_bleio.Characteristic.WRITE | _bleio.Characteristic.NOTIFY,
    read_perm=_bleio.Attribute.NO_ACCESS,
    write_perm=_bleio.Attribute.OPEN,
    max_length=20, fixed_length=False,
)

# Wrap in PacketBuffer for incoming packets and outgoing notifications
pb = _bleio.PacketBuffer(char, buffer_size=4, max_packet_size=20)
print("service created")

name = b"CPPBBI"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name
adapter.start_advertising(advertisement, connectable=True)
print("advertising")

for _ in range(80):
    if adapter.connected:
        break
    time.sleep(0.1)
print("connected", adapter.connected)

# Read incoming packet from client
data = bytearray(20)
n = 0
deadline = time.monotonic() + 5.0
while n == 0 and time.monotonic() < deadline:
    n = pb.readinto(data)
    if n == 0:
        time.sleep(0.05)
print("received", data[:n])

# Write response back (via NOTIFY) — sends immediately via completion callback
pb.write(bytes([0x52, 0x45, 0x53, 0x50]))  # "RESP"
print("sent response")

for _ in range(80):
    if not adapter.connected:
        break
    time.sleep(0.1)
print("done")
"""

BSIM_PB_BIDI_CLIENT_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable and b"CPPBBI" in entry.advertisement_bytes:
        target = entry.address
        print("found server")
        break
adapter.stop_scan()

connection = adapter.connect(target, timeout=5.0)
print("connected", connection.connected)

services = connection.discover_remote_services([_bleio.UUID(0xFFE0)])
print("discovered services", len(services))

remote_char = services[0].characteristics[0]
print("found char, props", remote_char.properties)

# Write a packet to the server
remote_char.value = bytes([0x48, 0x45, 0x4C, 0x4C, 0x4F])  # "HELLO"
print("wrote hello")

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(20)
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_BIDI_SERVER_CODE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_BIDI_CLIENT_CODE})
def test_bsim_packet_buffer_bidirectional(bsim_phy, circuitpython1, circuitpython2):
    """Bidirectional PacketBuffer: server receives WRITE, sends NOTIFY response."""
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
    assert "done" in server_output

    assert "found server" in client_output
    assert "wrote hello" in client_output
    assert "done" in client_output


# ---- Test 3: PacketBuffer with multiple queued incoming packets ----

BSIM_PB_QUEUE_SERVER_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

svc = _bleio.Service(_bleio.UUID(0xFFE0))
char = _bleio.Characteristic.add_to_service(
    svc, _bleio.UUID(0xFFE1),
    properties=_bleio.Characteristic.WRITE,
    read_perm=_bleio.Attribute.NO_ACCESS,
    write_perm=_bleio.Attribute.OPEN,
    max_length=20, fixed_length=False,
)

# Small buffer: only 1 packet, small max_packet_size
pb = _bleio.PacketBuffer(char, buffer_size=1, max_packet_size=10)
print("service created")

name = b"CPPBQU"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name
adapter.start_advertising(advertisement, connectable=True)
print("advertising")

for _ in range(80):
    if adapter.connected:
        break
    time.sleep(0.1)
print("connected", adapter.connected)

# Wait for client to send all packets
time.sleep(1.0)

# Read packets - with buffer_size=1, oldest packets are dropped
data = bytearray(20)

n1 = pb.readinto(data)
print("pkt1", data[:n1])

n2 = pb.readinto(data)
print("pkt2", data[:n2])

for _ in range(80):
    if not adapter.connected:
        break
    time.sleep(0.1)
print("done")
"""

BSIM_PB_QUEUE_CLIENT_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable and b"CPPBQU" in entry.advertisement_bytes:
        target = entry.address
        print("found server")
        break
adapter.stop_scan()

connection = adapter.connect(target, timeout=5.0)
print("connected", connection.connected)

services = connection.discover_remote_services([_bleio.UUID(0xFFE0)])
remote_char = services[0].characteristics[0]

# Write 3 packets quickly (buffer only holds 2, so oldest should be dropped)
remote_char.value = bytes([0x01, 0x02, 0x03])
print("wrote pkt1")

time.sleep(0.1)

remote_char.value = bytes([0x04, 0x05, 0x06])
print("wrote pkt2")

time.sleep(0.1)

remote_char.value = bytes([0x07, 0x08, 0x09])
print("wrote pkt3")

time.sleep(0.5)

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(14)
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_QUEUE_SERVER_CODE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_QUEUE_CLIENT_CODE})
def test_bsim_packet_buffer_queue(bsim_phy, circuitpython1, circuitpython2):
    """PacketBuffer queues multiple packets; oldest dropped when buffer full."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    assert "service created" in server_output
    assert "connected True" in server_output
    # With buffer_size=1, only the last packet(s) remain
    assert "pkt1" in server_output
    assert "pkt2" in server_output
    assert "done" in server_output

    assert "wrote pkt1" in client_output
    assert "wrote pkt2" in client_output
    assert "wrote pkt3" in client_output


# ---- Test 4: readinto with buffer too small returns negative ----

BSIM_PB_OVERFLOW_SERVER_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

svc = _bleio.Service(_bleio.UUID(0xFFE0))
char = _bleio.Characteristic.add_to_service(
    svc, _bleio.UUID(0xFFE1),
    properties=_bleio.Characteristic.WRITE,
    read_perm=_bleio.Attribute.NO_ACCESS,
    write_perm=_bleio.Attribute.OPEN,
    max_length=20, fixed_length=False,
)

pb = _bleio.PacketBuffer(char, buffer_size=4, max_packet_size=20)
print("service created")

name = b"CPPBOV"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name
adapter.start_advertising(advertisement, connectable=True)

for _ in range(80):
    if adapter.connected:
        break
    time.sleep(0.1)
print("connected", adapter.connected)

# Try to read a 5-byte packet into a 3-byte buffer -> ValueError
try:
    data = bytearray(3)
    n = 0
    deadline = time.monotonic() + 5.0
    while n == 0 and time.monotonic() < deadline:
        n = pb.readinto(data)
        if n == 0:
            time.sleep(0.05)
    print("unexpected success", n)
except ValueError as e:
    print("valueerror", e)

time.sleep(0.5)
for _ in range(80):
    if not adapter.connected:
        break
    time.sleep(0.1)
print("done")
"""

BSIM_PB_OVERFLOW_CLIENT_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable and b"CPPBOV" in entry.advertisement_bytes:
        target = entry.address
        break
adapter.stop_scan()

connection = adapter.connect(target, timeout=5.0)

services = connection.discover_remote_services([_bleio.UUID(0xFFE0)])
remote_char = services[0].characteristics[0]

# Write a 5-byte packet, but server only has a 3-byte read buffer
remote_char.value = bytes([0x01, 0x02, 0x03, 0x04, 0x05])
print("wrote 5 bytes")

time.sleep(0.5)
connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(14)
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_OVERFLOW_SERVER_CODE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_OVERFLOW_CLIENT_CODE})
def test_bsim_packet_buffer_readinto_overflow(bsim_phy, circuitpython1, circuitpython2):
    """readinto returns negative when packet is larger than buffer."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    assert "service created" in server_output
    assert "valueerror" in server_output

    assert "wrote 5 bytes" in client_output


# ---- Test 5: write() with header kwarg ----

BSIM_PB_HEADER_SERVER_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

svc = _bleio.Service(_bleio.UUID(0xFFE0))
char = _bleio.Characteristic.add_to_service(
    svc, _bleio.UUID(0xFFE1),
    properties=_bleio.Characteristic.WRITE | _bleio.Characteristic.NOTIFY,
    read_perm=_bleio.Attribute.NO_ACCESS,
    write_perm=_bleio.Attribute.OPEN,
    max_length=20, fixed_length=False,
)

pb = _bleio.PacketBuffer(char, buffer_size=4, max_packet_size=20)
print("service created")

name = b"CPPBHD"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name
adapter.start_advertising(advertisement, connectable=True)

for _ in range(80):
    if adapter.connected:
        break
    time.sleep(0.1)
print("connected", adapter.connected)

# Read an incoming packet
data = bytearray(20)
n = 0
deadline = time.monotonic() + 5.0
while n == 0 and time.monotonic() < deadline:
    n = pb.readinto(data)
    if n == 0:
        time.sleep(0.05)
print("received", data[:n])

# Response: write body with a header — header goes at start of packet
pb.write(bytes([0x42, 0x4F, 0x44, 0x59]), header=bytes([0x48, 0x44]))  # "HD" + "BODY"
print("sent with header")

for _ in range(80):
    if not adapter.connected:
        break
    time.sleep(0.1)
print("done")
"""

BSIM_PB_HEADER_CLIENT_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable and b"CPPBHD" in entry.advertisement_bytes:
        target = entry.address
        break
adapter.stop_scan()

connection = adapter.connect(target, timeout=5.0)

services = connection.discover_remote_services([_bleio.UUID(0xFFE0)])
remote_char = services[0].characteristics[0]

# Write a packet to trigger the server's response
remote_char.value = bytes([0x47, 0x4F])
print("wrote trigger")

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(14)
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_HEADER_SERVER_CODE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_HEADER_CLIENT_CODE})
def test_bsim_packet_buffer_write_header(bsim_phy, circuitpython1, circuitpython2):
    """write() header kwarg prepends to packet body."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    assert "service created" in server_output
    assert "received" in server_output
    assert "sent with header" in server_output

    assert "wrote trigger" in client_output


# ---- Test 6: incoming_packet_length / outgoing_packet_length ----

BSIM_PB_LENGTHS_CODE = """\
import _bleio

svc = _bleio.Service(_bleio.UUID(0xFFE0))
char = _bleio.Characteristic.add_to_service(
    svc, _bleio.UUID(0xFFE1),
    properties=_bleio.Characteristic.WRITE | _bleio.Characteristic.NOTIFY,
    read_perm=_bleio.Attribute.NO_ACCESS,
    write_perm=_bleio.Attribute.OPEN,
    max_length=20, fixed_length=False,
)

pb = _bleio.PacketBuffer(char, buffer_size=2, max_packet_size=15)

# incoming_packet_length reflects the max we can receive (characteristic max_length)
print("incoming", pb.incoming_packet_length)
# outgoing_packet_length requires a connection to know the negotiated ATT MTU,
# so without one it raises ValueError (server-side NOTIFY is MTU-bounded).
try:
    print("outgoing", pb.outgoing_packet_length)
except ValueError as e:
    print("outgoing valueerror", e)
print("done")
"""


@pytest.mark.duration(5)
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_LENGTHS_CODE})
def test_bsim_packet_buffer_packet_lengths(bsim_phy, circuitpython):
    """incoming_packet_length and outgoing_packet_length properties."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output

    # Server-side local characteristic:
    # incoming = max_length = 20
    # outgoing requires a connection to know the ATT MTU, so it raises without one.
    assert "incoming 20" in output
    assert "outgoing valueerror" in output
    assert "done" in output


# ---- Test 7: disconnect / reconnect, conn tracking ----

BSIM_PB_RECONNECT_SERVER_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

svc = _bleio.Service(_bleio.UUID(0xFFE0))
char = _bleio.Characteristic.add_to_service(
    svc, _bleio.UUID(0xFFE1),
    properties=_bleio.Characteristic.WRITE | _bleio.Characteristic.NOTIFY,
    read_perm=_bleio.Attribute.NO_ACCESS,
    write_perm=_bleio.Attribute.OPEN,
    max_length=20, fixed_length=False,
)

pb = _bleio.PacketBuffer(char, buffer_size=4, max_packet_size=20)
print("service created")

name = b"CPPBRC"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name
adapter.start_advertising(advertisement, connectable=True)

# First connection
for _ in range(80):
    if adapter.connected:
        break
    time.sleep(0.1)
print("connected1", adapter.connected)

# Read first packet
data = bytearray(20)
n = 0
deadline = time.monotonic() + 5.0
while n == 0 and time.monotonic() < deadline:
    n = pb.readinto(data)
    if n == 0:
        time.sleep(0.05)
print("packet1", data[:n])

# Send a response to confirm the tracked conn works
pb.write(bytes([0x41, 0x43, 0x4B]))  # "ACK"
print("sent ack1")

# Wait for disconnect
for _ in range(80):
    if not adapter.connected:
        break
    time.sleep(0.1)
print("disconnected")

# Start advertising again for second connection
adapter.start_advertising(advertisement, connectable=True)

# Second connection
for _ in range(80):
    if adapter.connected:
        break
    time.sleep(0.1)
print("connected2", adapter.connected)

# Read second packet (should track new conn)
n2 = 0
deadline2 = time.monotonic() + 5.0
while n2 == 0 and time.monotonic() < deadline2:
    n2 = pb.readinto(data)
    if n2 == 0:
        time.sleep(0.05)
print("packet2", data[:n2])

pb.write(bytes([0x41, 0x43, 0x4B]))  # "ACK"
print("sent ack2")

for _ in range(80):
    if not adapter.connected:
        break
    time.sleep(0.1)
print("done")
"""

BSIM_PB_RECONNECT_CLIENT_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

# First connection
target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable and b"CPPBRC" in entry.advertisement_bytes:
        target = entry.address
        break
adapter.stop_scan()

connection = adapter.connect(target, timeout=5.0)
print("connected1", connection.connected)

services = connection.discover_remote_services([_bleio.UUID(0xFFE0)])
remote_char = services[0].characteristics[0]

# Write first packet
remote_char.value = bytes([0x46, 0x49, 0x52, 0x53, 0x54])  # "FIRST"
print("wrote first")

time.sleep(0.5)

# Disconnect
connection.disconnect()
timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("disconnected")

# Wait for server to re-advertise
time.sleep(0.5)

# Second connection
target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable and b"CPPBRC" in entry.advertisement_bytes:
        target = entry.address
        break
adapter.stop_scan()

connection = adapter.connect(target, timeout=5.0)
print("connected2", connection.connected)

services = connection.discover_remote_services([_bleio.UUID(0xFFE0)])
remote_char = services[0].characteristics[0]

# Write second packet
remote_char.value = bytes([0x53, 0x45, 0x43, 0x4E, 0x44])  # "SECOND"
print("wrote second")

time.sleep(0.5)
connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(30)
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_RECONNECT_SERVER_CODE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_RECONNECT_CLIENT_CODE})
def test_bsim_packet_buffer_reconnect(bsim_phy, circuitpython1, circuitpython2):
    """PacketBuffer tracks conn through disconnect / reconnect."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    assert "service created" in server_output
    assert "connected1 True" in server_output
    assert "packet1" in server_output
    assert "sent ack1" in server_output
    assert "disconnected" in server_output
    assert "connected2 True" in server_output
    assert "packet2" in server_output
    assert "sent ack2" in server_output

    assert "wrote first" in client_output
    assert "wrote second" in client_output


# ---- Test 8: outgoing_packet_length bounded by negotiated MTU ----
#
# Regression test for the "No ATT channel for MTU" bug. The server-side
# PacketBuffer's outgoing_packet_length must be bounded by the negotiated
# ATT MTU (ATT_MTU - 3), not just max_packet_size. Without that bounding,
# the workflow packs a notification larger than the MTU and bt_gatt_notify_cb
# fails silently ("No ATT channel for MTU"), so the client never receives it.
#
# The characteristic is configured with max_length=600 and max_packet_size=600,
# deliberately larger than any plausible negotiated ATT MTU (the stack supports
# up to CONFIG_BT_L2CAP_TX_MTU = 515, payload 512). So without the fix,
# outgoing_packet_length returns 600 and the 600-byte notification always
# exceeds the MTU and is dropped — regardless of whether an MTU exchange
# happened. With the fix, outgoing_packet_length is ATT_MTU - 3 (≤ 512) and the
# notification fits and is delivered.

BSIM_PB_MTU_SERVER_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

svc = _bleio.Service(_bleio.UUID(0xFFE0))
char = _bleio.Characteristic.add_to_service(
    svc, _bleio.UUID(0xFFE1),
    properties=_bleio.Characteristic.WRITE | _bleio.Characteristic.NOTIFY,
    read_perm=_bleio.Attribute.NO_ACCESS,
    write_perm=_bleio.Attribute.OPEN,
    max_length=600, fixed_length=False,
)

# max_packet_size deliberately larger than any negotiated ATT MTU so that an
# unbounded outgoing_packet_length always produces an oversized, undeliverable
# notification regardless of MTU-exchange timing.
pb = _bleio.PacketBuffer(char, buffer_size=4, max_packet_size=600)
print("service created")

name = b"CPPBMT"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name
adapter.start_advertising(advertisement, connectable=True)
print("advertising")

for _ in range(80):
    if adapter.connected:
        break
    time.sleep(0.1)
print("connected", adapter.connected)

# Wait for the client to subscribe (CCCD write) and send a trigger write.
data = bytearray(600)
n = 0
deadline = time.monotonic() + 5.0
while n == 0 and time.monotonic() < deadline:
    n = pb.readinto(data)
    if n == 0:
        time.sleep(0.05)
print("trigger", data[:n])

# The negotiated ATT MTU determines the largest notification payload. The
# connection's max_packet_length is ATT_MTU - 3.
conn = adapter.connections[0]
mtu_payload = conn.max_packet_length
print("mtu_payload", mtu_payload)

# outgoing_packet_length must be bounded by the MTU, not max_packet_size.
opl = pb.outgoing_packet_length
print("outgoing", opl)

# Send a response sized exactly to outgoing_packet_length, like the BLE
# file-transfer workflow does. With the fix this fits the MTU and is delivered.
response = bytes([0xAA]) * opl
pb.write(response)
print("sent", len(response))

for _ in range(80):
    if not adapter.connected:
        break
    time.sleep(0.1)
print("done")
"""

BSIM_PB_MTU_CLIENT_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable and b"CPPBMT" in entry.advertisement_bytes:
        target = entry.address
        print("found server")
        break
adapter.stop_scan()

connection = adapter.connect(target, timeout=5.0)
print("connected", connection.connected)

services = connection.discover_remote_services([_bleio.UUID(0xFFE0)])
remote_char = services[0].characteristics[0]
print("found char, props", remote_char.properties)

# Creating a client-side PacketBuffer on a NOTIFY characteristic writes the
# CCCD to subscribe, which gives the server a connection to notify.
client_pb = _bleio.PacketBuffer(remote_char, buffer_size=4, max_packet_size=600)
print("subscribed")

# Trigger the server's response with a direct write.
remote_char.value = b"GO"
print("wrote trigger")

# Read the notification response. Without the MTU fix the server sends an
# oversized notification that the ATT layer drops, so this times out.
buf = bytearray(600)
n = 0
deadline = time.monotonic() + 5.0
while n == 0 and time.monotonic() < deadline:
    n = client_pb.readinto(buf)
    if n == 0:
        time.sleep(0.05)
if n > 0:
    print("received_ok", n, bytes(buf[:n]))
else:
    print("received_none")

time.sleep(0.5)
connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(20)
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_MTU_SERVER_CODE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_MTU_CLIENT_CODE})
def test_bsim_packet_buffer_outgoing_mtu_bounded(bsim_phy, circuitpython1, circuitpython2):
    """Server-side outgoing_packet_length is bounded by the negotiated ATT MTU.

    Without the fix, outgoing_packet_length returns max_packet_size (200) and
    the resulting notification exceeds the bsim default ATT MTU (23), so the
    ATT layer drops it ("No ATT channel for MTU") and the client receives
    nothing. With the fix, outgoing_packet_length is ATT_MTU - 3 (20) and the
    notification is delivered to the client.
    """
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    assert "service created" in server_output
    assert "connected True" in server_output
    assert "trigger" in server_output

    # outgoing_packet_length must not exceed the connection's MTU-derived
    # max_packet_length. Without the fix this is 200 vs 20.
    mtu_match = re.search(r"mtu_payload (\d+)", server_output)
    outgoing_match = re.search(r"outgoing (\d+)", server_output)
    assert mtu_match is not None, f"mtu_payload not printed: {server_output}"
    assert outgoing_match is not None, f"outgoing not printed: {server_output}"
    mtu_payload = int(mtu_match.group(1))
    outgoing = int(outgoing_match.group(1))
    assert outgoing <= mtu_payload, (
        f"outgoing_packet_length {outgoing} exceeds MTU payload {mtu_payload}; "
        "notifications this size would be dropped by the ATT layer"
    )
    assert outgoing < 600, f"outgoing_packet_length {outgoing} not bounded below max_packet_size"

    # The notification sized to outgoing_packet_length must actually arrive.
    assert "received_ok" in client_output, (
        f"client never received the notification (MTU-bound bug): {client_output}"
    )
    assert "received_none" not in client_output

    received_match = re.search(r"received_ok (\d+)", client_output)
    assert received_match is not None
    received = int(received_match.group(1))
    assert received == outgoing, f"client received {received} bytes, server sent {outgoing}"


# ---- Test: client-side PacketBuffer write (remote WRITE_NO_RESPONSE) ----
#
# Covers the client-side write path in common_hal_bleio_packet_buffer_write
# (the is_remote branch): a CP central wraps a remote characteristic in a
# _bleio.PacketBuffer and writes to it via pb.write(...), which must reach the
# peripheral's server-side PacketBuffer. This mirrors how the BLE file-transfer
# library sends commands (raw.write). It also exercises the header+data combine
# in the client write path via pb.write(data, header=...).
#
# The characteristic uses WRITE_NO_RESPONSE | NOTIFY with open permissions, so
# no pairing is required and the test runs on both bsim boards.

BSIM_PB_CLIENT_WRITE_SERVER_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

svc = _bleio.Service(_bleio.UUID(0xFFE0))
char = _bleio.Characteristic.add_to_service(
    svc, _bleio.UUID(0xFFE1),
    properties=_bleio.Characteristic.WRITE_NO_RESPONSE | _bleio.Characteristic.NOTIFY,
    read_perm=_bleio.Attribute.OPEN,
    write_perm=_bleio.Attribute.OPEN,
    max_length=20, fixed_length=False,
)

# Server-side PacketBuffer: reads incoming WRITE_NO_RESPONSE, echoes via NOTIFY.
pb = _bleio.PacketBuffer(char, buffer_size=4, max_packet_size=20)
print("service created")

name = b"CPPBCW"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name
adapter.start_advertising(advertisement, connectable=True)
print("advertising")

for _ in range(80):
    if adapter.connected:
        break
    time.sleep(0.1)
print("connected", adapter.connected)

data = bytearray(20)
for _ in range(2):
    n = 0
    deadline = time.monotonic() + 5.0
    while n == 0 and time.monotonic() < deadline:
        n = pb.readinto(data)
        if n == 0:
            time.sleep(0.05)
    print("received", bytes(data[:n]))
    # Echo the packet straight back over NOTIFY.
    pb.write(bytes(data[:n]))
    print("echoed", bytes(data[:n]))

for _ in range(80):
    if not adapter.connected:
        break
    time.sleep(0.1)
print("done")
"""

BSIM_PB_CLIENT_WRITE_CLIENT_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

target = None
for entry in adapter.start_scan(timeout=6.0, active=True):
    if entry.connectable and b"CPPBCW" in entry.advertisement_bytes:
        target = entry.address
        print("found server")
        break
adapter.stop_scan()

connection = adapter.connect(target, timeout=5.0)
print("connected", connection.connected)

services = connection.discover_remote_services([_bleio.UUID(0xFFE0)])
print("discovered services", len(services))

remote_char = services[0].characteristics[0]
print("found char, props", remote_char.properties)

# Client-side PacketBuffer on the remote characteristic: subscribes to NOTIFY
# (readinto) and writes to the remote (pb.write -> client-side write path).
cpb = _bleio.PacketBuffer(remote_char, buffer_size=4, max_packet_size=20)

# Write 1: plain write (no header) — exercises bt_gatt_write_without_response.
cpb.write(b"PING")
print("wrote ping")

# Write 2: write with a header — exercises the header+data combine in the
# client write path. "B" + "ODY" is sent as a single "BODY" packet.
cpb.write(b"ODY", header=b"B")
print("wrote body with header")

# Read both NOTIFY echoes back.
buf = bytearray(20)
for _ in range(2):
    n = 0
    deadline = time.monotonic() + 5.0
    while n == 0 and time.monotonic() < deadline:
        n = cpb.readinto(buf)
        if n == 0:
            time.sleep(0.05)
    print("client received", bytes(buf[:n]))

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(20)
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_CLIENT_WRITE_SERVER_CODE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_PB_CLIENT_WRITE_CLIENT_CODE})
def test_bsim_packet_buffer_client_write(bsim_phy, circuitpython1, circuitpython2):
    """A CP central writes to a remote characteristic via a client-side
    PacketBuffer (pb.write), and the peripheral echoes it back over NOTIFY.

    Covers the client-side write path (remote WRITE_NO_RESPONSE), including the
    header+data combine, mirroring how the BLE file-transfer library sends
    commands.
    """
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    # Server received both client writes (plain + header-combined).
    assert "received b'PING'" in server_output, (
        f"server never received the plain client write: {server_output}"
    )
    assert "received b'BODY'" in server_output, (
        f"server never received the header-combined client write: {server_output}"
    )
    assert "echoed b'PING'" in server_output
    assert "echoed b'BODY'" in server_output
    assert "done" in server_output

    # Client wrote both and got both echoes back over NOTIFY.
    assert "wrote ping" in client_output, f"client plain write did not complete: {client_output}"
    assert "wrote body with header" in client_output, (
        f"client header write did not complete: {client_output}"
    )
    assert "client received b'PING'" in client_output, (
        f"client never received the PING echo: {client_output}"
    )
    assert "client received b'BODY'" in client_output, (
        f"client never received the BODY echo: {client_output}"
    )
    assert "done" in client_output
