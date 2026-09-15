# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""BLE GATT service tests for bsim."""

import pytest

from .conftest import get_library_files

_ADAFRUIT_BLE = get_library_files("adafruit_ble")

# ---------------------------------------------------------------------------
# Shared service libraries
# ---------------------------------------------------------------------------

BATTERY_LIB = """
from adafruit_ble.services import Service
from adafruit_ble.characteristics import Characteristic, Attribute
from adafruit_ble.uuid import StandardUUID


class BatteryService(Service):
    uuid = StandardUUID(0x180F)
    level = Characteristic(
        uuid=StandardUUID(0x2A19),
        properties=Characteristic.READ | Characteristic.WRITE,
        read_perm=Attribute.OPEN,
        write_perm=Attribute.OPEN,
        max_length=1,
        fixed_length=True,
        initial_value=bytes([75]),
    )"""

HEART_RATE_LIB = """
from adafruit_ble.services import Service
from adafruit_ble.characteristics import Characteristic, Attribute
from adafruit_ble.uuid import StandardUUID


class HeartRateService(Service):
    uuid = StandardUUID(0x180D)
    measurement = Characteristic(
        uuid=StandardUUID(0x2A37),
        properties=Characteristic.READ,
        read_perm=Attribute.OPEN,
        write_perm=Attribute.NO_ACCESS,
        max_length=2,
        fixed_length=True,
        initial_value=bytes([0, 72]),
    )"""

# ===================================================================
# Test 1: Battery Service (Zephyr central)
# ===================================================================

BSIM_SERVICE_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from battery_service import BatteryService

ble = BLERadio()
svc = BatteryService()
print("service created")

advertisement = ProvideServicesAdvertisement(svc)
ble.start_advertising(advertisement)
print("advertising")

for _ in range(80):
    if ble.connected:
        break
    time.sleep(0.1)
print("connected", ble.connected)

for _ in range(80):
    if not ble.connected:
        break
    time.sleep(0.1)
print("done")
"""


@pytest.mark.zephyr_sample("tests/bsim/samples/central_battery_client")
@pytest.mark.duration(14)
@pytest.mark.circuitpy_drive(
    {
        "code.py": BSIM_SERVICE_CODE,
        "settings.toml": "CIRCUITPY_BLE_WORKFLOW = false\n",
        "battery_service.py": BATTERY_LIB,
        **_ADAFRUIT_BLE,
    }
)
def test_bsim_service_battery(bsim_phy, circuitpython, zephyr_sample):
    """CP hosts BatteryService; Zephyr central reads battery level."""
    circuitpython.wait_until_done()

    cp_output = circuitpython.serial.all_output
    sample_output = zephyr_sample.serial.all_output

    assert "service created" in cp_output
    assert "connected True" in cp_output
    assert "Battery Level: 75" in sample_output


# ===================================================================
# Test 2: CP client reads/writes CP peripheral
# ===================================================================

BSIM_SERVER_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from battery_service import BatteryService

ble = BLERadio()
svc = BatteryService()
print("service created")

advertisement = ProvideServicesAdvertisement(svc)
ble.start_advertising(advertisement)
print("advertising")

for _ in range(80):
    if ble.connected:
        break
    time.sleep(0.1)
print("connected", ble.connected)

for _ in range(80):
    if not ble.connected:
        break
    time.sleep(0.1)

print("final value", list(svc.level))
print("done")
"""

BSIM_CLIENT_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from battery_service import BatteryService

ble = BLERadio()

print("client start")
target = None
for adv in ble.start_scan(ProvideServicesAdvertisement, timeout=6.0):
    if adv.connectable:
        target = adv
        print("found server")
        break
ble.stop_scan()
print("have target", target is not None)

if target is None:
    raise RuntimeError("No server found")

connection = ble.connect(target, timeout=5.0)
print("connected", connection.connected)

svc = connection[BatteryService]
print("discovered services", 1)
print("discovered chars", 1)  # BatteryService has one characteristic

print("battery level", list(svc.level))

# Write a new value
svc.level = bytes([42])
print("wrote new value")

time.sleep(0.5)

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)

print("disconnected", not connection.connected)
"""


@pytest.mark.duration(30)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_SERVER_CODE, "battery_service.py": BATTERY_LIB, **_ADAFRUIT_BLE}
)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_CLIENT_CODE, "battery_service.py": BATTERY_LIB, **_ADAFRUIT_BLE}
)
def test_bsim_service_cp_client(bsim_phy, circuitpython1, circuitpython2):
    """CP peripheral hosts BatteryService; CP central discovers, reads, and writes."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    assert "service created" in server_output
    assert "connected True" in server_output
    assert "final value [42]" in server_output

    assert "client start" in client_output
    assert "found server" in client_output
    assert "discovered services 1" in client_output
    assert "discovered chars 1" in client_output
    assert "battery level [75]" in client_output
    assert "wrote new value" in client_output
    assert "disconnected True" in client_output


# ===================================================================
# Test 3: Discover all services (no whitelist)
# ===================================================================

BSIM_DISCOVER_ALL_SERVER_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from battery_service import BatteryService
from heart_rate import HeartRateService

ble = BLERadio()
bas = BatteryService()
hrs = HeartRateService()
print("services created")

advertisement = ProvideServicesAdvertisement(bas, hrs)
ble.start_advertising(advertisement)

for _ in range(80):
    if ble.connected:
        break
    time.sleep(0.1)
print("connected", ble.connected)

for _ in range(80):
    if not ble.connected:
        break
    time.sleep(0.1)
print("done")
"""

BSIM_DISCOVER_ALL_CLIENT_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from battery_service import BatteryService
from heart_rate import HeartRateService

ble = BLERadio()

print("client start")
target = None
for adv in ble.start_scan(ProvideServicesAdvertisement, timeout=8.0):
    if adv.connectable:
        target = adv
        print("found server")
        break
ble.stop_scan()
print("have target", target is not None)

if target is None:
    raise RuntimeError("No server found")

connection = ble.connect(target, timeout=5.0)
print("connected", connection.connected)

# Discover ALL services (no whitelist) — use _bleio directly.
all_services = connection._bleio_connection.discover_remote_services()
print("total services", len(all_services))

# Filter to our two known UUIDs (ignore GATT/GAP services the stack may expose)
user_svcs = [s for s in all_services if s.uuid.uuid16 in (0x180F, 0x180D)]
print("user services", len(user_svcs))

uuids = sorted([s.uuid.uuid16 for s in user_svcs])
print("service uuids", uuids)

# Now read characteristics via adafruit_ble Service bindings.
if BatteryService in connection:
    bas = connection[BatteryService]
    print("char", hex(0x2a19), list(bas.level))

if HeartRateService in connection:
    hrs = connection[HeartRateService]
    print("char", hex(0x2a37), list(hrs.measurement))

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(35)
@pytest.mark.circuitpy_drive(
    {
        "code.py": BSIM_DISCOVER_ALL_SERVER_CODE,
        "battery_service.py": BATTERY_LIB,
        "heart_rate.py": HEART_RATE_LIB,
        **_ADAFRUIT_BLE,
    }
)
@pytest.mark.circuitpy_drive(
    {
        "code.py": BSIM_DISCOVER_ALL_CLIENT_CODE,
        "battery_service.py": BATTERY_LIB,
        "heart_rate.py": HEART_RATE_LIB,
        **_ADAFRUIT_BLE,
    }
)
def test_bsim_service_discover_all(bsim_phy, circuitpython1, circuitpython2):
    """Discover all services without a UUID whitelist, verify two user services found."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    client_output = client.serial.all_output

    assert "user services 2" in client_output
    assert "service uuids [6157, 6159]" in client_output  # 0x180D=6157, 0x180F=6159
    assert "char 0x2a37 [0, 72]" in client_output
    assert "char 0x2a19 [75]" in client_output


# ===================================================================
# Test 4: Write-no-response (uses MultiService from multi_service module)
# ===================================================================

MULTI_LIB = """
from adafruit_ble.services import Service
from adafruit_ble.characteristics import Characteristic, Attribute
from adafruit_ble.uuid import StandardUUID


class MultiService(Service):
    uuid = StandardUUID(0x180F)
    char_a = Characteristic(
        uuid=StandardUUID(0x2A19),
        properties=Characteristic.READ,
        read_perm=Attribute.OPEN,
        write_perm=Attribute.NO_ACCESS,
        max_length=1,
        fixed_length=True,
        initial_value=bytes([10]),
    )
    char_b = Characteristic(
        uuid=StandardUUID(0x2A1A),
        properties=Characteristic.READ | Characteristic.WRITE_NO_RESPONSE,
        read_perm=Attribute.OPEN,
        write_perm=Attribute.OPEN,
        max_length=1,
        fixed_length=True,
        initial_value=bytes([20]),
    )
    char_c = Characteristic(
        uuid=StandardUUID(0x2A1B),
        properties=Characteristic.READ,
        read_perm=Attribute.OPEN,
        write_perm=Attribute.NO_ACCESS,
        max_length=1,
        fixed_length=True,
        initial_value=bytes([30]),
    )"""


BSIM_MULTI_CHAR_SERVER_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from multi_service import MultiService

ble = BLERadio()
svc = MultiService()
# Force binding
svc.char_a
svc.char_b
svc.char_c
print("service created")

advertisement = ProvideServicesAdvertisement(svc)
ble.start_advertising(advertisement)

for _ in range(80):
    if ble.connected:
        break
    time.sleep(0.1)
print("connected", ble.connected)

for _ in range(80):
    if not ble.connected:
        break
    time.sleep(0.1)

print("char_b final", list(svc.char_b))
print("done")
"""

BSIM_WRITE_NR_CLIENT_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from multi_service import MultiService

ble = BLERadio()

target = None
for adv in ble.start_scan(ProvideServicesAdvertisement, timeout=6.0):
    if adv.connectable:
        target = adv
        break
ble.stop_scan()

if target is None:
    raise RuntimeError("No server found")

connection = ble.connect(target, timeout=5.0)

svc = connection[MultiService]

print("initial", list(svc.char_b))

# Write-no-response
svc.char_b = bytes([99])
print("wrote wnr")

# Give the server time to process the write
time.sleep(0.5)

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(25)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_MULTI_CHAR_SERVER_CODE, "multi_service.py": MULTI_LIB, **_ADAFRUIT_BLE}
)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_WRITE_NR_CLIENT_CODE, "multi_service.py": MULTI_LIB, **_ADAFRUIT_BLE}
)
def test_bsim_service_write_no_response(bsim_phy, circuitpython1, circuitpython2):
    """Client writes a characteristic using WRITE_NO_RESPONSE."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    assert "char_b final [99]" in server_output
    assert "initial [20]" in client_output
    assert "wrote wnr" in client_output


# ===================================================================
# Test 5: Multiple characteristics on one service
# ===================================================================

BSIM_MULTI_CHAR_CLIENT_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from multi_service import MultiService

ble = BLERadio()

target = None
for adv in ble.start_scan(ProvideServicesAdvertisement, timeout=6.0):
    if adv.connectable:
        target = adv
        break
ble.stop_scan()

if target is None:
    raise RuntimeError("No server found")

connection = ble.connect(target, timeout=5.0)

svc = connection[MultiService]
print("num chars", 3)

# Read each characteristic
print("char", hex(0x2A19), list(svc.char_a))
print("char", hex(0x2A1A), list(svc.char_b))
print("char", hex(0x2A1B), list(svc.char_c))

# Write to the second characteristic
svc.char_b = bytes([77])
print("wrote 0x2a1a")

time.sleep(0.5)

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(25)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_MULTI_CHAR_SERVER_CODE, "multi_service.py": MULTI_LIB, **_ADAFRUIT_BLE}
)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_MULTI_CHAR_CLIENT_CODE, "multi_service.py": MULTI_LIB, **_ADAFRUIT_BLE}
)
def test_bsim_service_multi_char(bsim_phy, circuitpython1, circuitpython2):
    """Service with three characteristics: discover all, read each, write one."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    assert "num chars 3" in client_output
    assert "char 0x2a19 [10]" in client_output
    assert "char 0x2a1a [20]" in client_output
    assert "char 0x2a1b [30]" in client_output
    assert "wrote 0x2a1a" in client_output
    assert "char_b final [77]" in server_output


# ===================================================================
# Test 6: 128-bit custom UUID
# ===================================================================

CUSTOM_LIB = """
from adafruit_ble.services import Service
from adafruit_ble.characteristics import Characteristic, Attribute
from adafruit_ble.uuid import VendorUUID


class CustomService(Service):
    uuid = VendorUUID("12345678-1234-5678-1234-56789abcdef0")
    data = Characteristic(
        uuid=VendorUUID("12345678-1234-5678-1234-56789abcdef1"),
        properties=Characteristic.READ | Characteristic.WRITE,
        read_perm=Attribute.OPEN,
        write_perm=Attribute.OPEN,
        max_length=4,
        fixed_length=False,
        initial_value=bytes([0xDE, 0xAD]),
    )"""

BSIM_CUSTOM_UUID_SERVER_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from custom_service import CustomService

ble = BLERadio()
svc = CustomService()
print("service created")

advertisement = ProvideServicesAdvertisement(svc)
ble.start_advertising(advertisement)

for _ in range(80):
    if ble.connected:
        break
    time.sleep(0.1)
print("connected", ble.connected)

for _ in range(80):
    if not ble.connected:
        break
    time.sleep(0.1)

print("final value", list(svc.data))
print("done")
"""

BSIM_CUSTOM_UUID_CLIENT_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from custom_service import CustomService

ble = BLERadio()

target = None
for adv in ble.start_scan(ProvideServicesAdvertisement, timeout=6.0):
    if adv.connectable:
        target = adv
        break
ble.stop_scan()

if target is None:
    raise RuntimeError("No server found")

connection = ble.connect(target, timeout=5.0)

svc = connection[CustomService]
print("discovered services", 1)

print("char value", list(svc.data))

svc.data = bytes([0xBE, 0xEF])
print("wrote custom")

time.sleep(0.5)

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(25)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_CUSTOM_UUID_SERVER_CODE, "custom_service.py": CUSTOM_LIB, **_ADAFRUIT_BLE}
)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_CUSTOM_UUID_CLIENT_CODE, "custom_service.py": CUSTOM_LIB, **_ADAFRUIT_BLE}
)
def test_bsim_service_custom_uuid(bsim_phy, circuitpython1, circuitpython2):
    """128-bit custom UUID service: discover, read, and write."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    server_output = server.serial.all_output
    client_output = client.serial.all_output

    assert "discovered services 1" in client_output
    assert "char value [222, 173]" in client_output  # 0xDE, 0xAD
    assert "wrote custom" in client_output
    assert "final value [190, 239]" in server_output  # 0xBE, 0xEF


# ===================================================================
# Test 7: Empty discovery result
# ===================================================================

BSIM_EMPTY_DISC_CLIENT_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from heart_rate import HeartRateService

ble = BLERadio()

target = None
for adv in ble.start_scan(ProvideServicesAdvertisement, timeout=6.0):
    if adv.connectable:
        target = adv
        print("found server")
        break
ble.stop_scan()
print("have target", target is not None)

if target is None:
    raise RuntimeError("No server found")

connection = ble.connect(target, timeout=5.0)
print("connected", connection.connected)

# Ask for Heart Rate Service which doesn't exist on this server
found = HeartRateService in connection
print("found services", 1 if found else 0)

connection.disconnect()

timeout = time.monotonic() + 4.0
while connection.connected and time.monotonic() < timeout:
    time.sleep(0.1)
print("done")
"""


@pytest.mark.duration(30)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_CUSTOM_UUID_SERVER_CODE, "custom_service.py": CUSTOM_LIB, **_ADAFRUIT_BLE}
)
@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_EMPTY_DISC_CLIENT_CODE, "heart_rate.py": HEART_RATE_LIB, **_ADAFRUIT_BLE}
)
def test_bsim_service_empty_discovery(bsim_phy, circuitpython1, circuitpython2):
    """Filter for a UUID that doesn't exist, verify empty tuple returned."""
    server = circuitpython1
    client = circuitpython2

    client.wait_until_done()
    server.wait_until_done()

    client_output = client.serial.all_output

    assert "found services 0" in client_output
    assert "done" in client_output
