# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""BLE advertising tests for bsim."""

import logging
import re

import pytest


logger = logging.getLogger(__name__)

BSIM_ADV_CODE = """\
import _bleio
import time

name = b"CPADV"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name

adapter = _bleio.adapter
print("adv start")
adapter.start_advertising(advertisement, connectable=False)
print("adv started")
time.sleep(4)
adapter.stop_advertising()
print("adv stop")
"""

BSIM_ADV_INTERRUPT_RELOAD_CODE = """\
import _bleio
import time

name = b"CPADV"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name

adapter = _bleio.adapter
print("adv run start")
adapter.start_advertising(advertisement, connectable=False)
print("adv running")
time.sleep(10)
adapter.stop_advertising()
print("adv run done")
"""


BSIM_TX_POWER_DEFAULT_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

name = b"CPTXPWR"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name

print("advertising default")
adapter.start_advertising(advertisement)
time.sleep(4)
adapter.stop_advertising()
print("done")
"""

BSIM_TX_POWER_LOW_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

name = b"CPTXPWR"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name

print("advertising low")
adapter.start_advertising(advertisement, tx_power=-20)
time.sleep(4)
adapter.stop_advertising()
print("done")
"""


# Advertiser: primary advert carries a distinctive marker; the scan response
# carries a different marker. If the adapter merges them they'd arrive as one
# entry, but the adapter does not merge — they come as separate ScanEntry
# objects distinguished by `scan_response`.
BSIM_ADVERT_SCAN_RESPONSE_ADV_CODE = """\
import _bleio
import time

adapter = _bleio.adapter

name = b"ADVADV"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name

sname = b"SCANRSP"
scan_response = bytes((len(sname) + 1, 0x09)) + sname

print("adv start")
adapter.start_advertising(advertisement, scan_response=scan_response, connectable=True)
time.sleep(10)
adapter.stop_advertising()
print("adv done")
"""

BSIM_ADVERT_SCAN_RESPONSE_SCAN_CODE = """\
import _bleio

adapter = _bleio.adapter
print("scan start")
saw_primary = False
saw_scan_response = False
for entry in adapter.start_scan(active=True, timeout=12.0):
    data = entry.advertisement_bytes
    if b"ADVADV" in data and not entry.scan_response:
        saw_primary = True
        print("primary")
    if b"SCANRSP" in data and entry.scan_response:
        saw_scan_response = True
        print("scan_response")
    if saw_primary and saw_scan_response:
        print("both")
        break
adapter.stop_scan()
print("scan done", saw_primary, saw_scan_response)
"""

# Disable the workflow on the advertiser so only its custom advert is on air.
BSIM_ADVERT_SCAN_RESPONSE_SETTINGS = "CIRCUITPY_BLE_WORKFLOW = false\n"


@pytest.mark.zephyr_sample("tests/bsim/samples/observer")
@pytest.mark.circuitpy_drive({"code.py": BSIM_ADV_CODE})
@pytest.mark.duration(60)
def test_bsim_advertise_and_scan(bsim_phy, circuitpython, zephyr_sample):
    """Advertise from CircuitPython and verify Zephyr observer sees traffic."""
    observer = zephyr_sample

    circuitpython.wait_until_done()

    cp_output = circuitpython.serial.all_output
    observer_output = observer.serial.all_output
    assert "adv start" in cp_output
    assert "adv started" in cp_output
    assert "adv stop" in cp_output
    assert "Device found:" in observer_output
    assert "AD data len 10" in observer_output


@pytest.mark.zephyr_sample("tests/bsim/samples/observer")
@pytest.mark.port_resets(3)
@pytest.mark.duration(60)
@pytest.mark.circuitpy_drive({"code.py": BSIM_ADV_INTERRUPT_RELOAD_CODE})
def test_bsim_advertise_ctrl_c_reload(bsim_phy, circuitpython, zephyr_sample):
    """Ensure advertising resumes after Ctrl-C and a reload."""
    observer = zephyr_sample

    circuitpython.serial.wait_for("adv running")
    observer.serial.wait_for("Device found:")
    observer_count_before = observer.serial.all_output.count("Device found:")

    circuitpython.serial.write("\x03")
    # Real AES for the link layer adds CPU overhead that can delay the
    # interpreter noticing SIGINT beyond the default wait_for timeout.
    circuitpython.serial.wait_for("KeyboardInterrupt", timeout=20)

    circuitpython.serial.write("\x04")
    circuitpython.wait_until_done()

    cp_output = circuitpython.serial.all_output
    observer_output = observer.serial.all_output
    logger.info(observer_output)
    logger.info(cp_output)

    assert "adv run start" in cp_output
    assert "KeyboardInterrupt" in cp_output
    assert cp_output.count("adv running") >= 2
    assert cp_output.count("adv run done") >= 1
    assert observer_output.count("Device found:") >= observer_count_before + 1
    assert "Already advertising" not in cp_output


@pytest.mark.zephyr_sample("tests/bsim/samples/observer")
@pytest.mark.duration(40)
@pytest.mark.circuitpy_drive({"code.py": BSIM_TX_POWER_DEFAULT_CODE})
def test_bsim_tx_power_default_rssi(board, bsim_phy, circuitpython, zephyr_sample):
    """Verify default TX power produces expected RSSI."""
    observer = zephyr_sample

    circuitpython.wait_until_done()

    cp_output = circuitpython.serial.all_output
    obs_output = observer.serial.all_output

    assert "advertising default" in cp_output
    assert "done" in cp_output

    # Observer: "Device found: <addr> (RSSI <n>), type <t>, AD data len <l>"
    # Advertisement is 12 bytes: flags (3) + name (9).
    # With 40 dB channel attenuation and 0 dBm TX → RSSI ~ -39.
    expected_rssi = -39
    rssi_pattern = re.compile(r"RSSI (-?\d+)\), type \d+, AD data len 12")
    all_rssi = [int(m.group(1)) for m in rssi_pattern.finditer(obs_output)]
    logger.info("RSSI values: %s", all_rssi)

    assert len(all_rssi) > 0, "Observer saw no advertisements"
    assert all_rssi[0] == expected_rssi, f"Expected RSSI {expected_rssi}, got {all_rssi[0]}"


@pytest.mark.zephyr_sample("tests/bsim/samples/observer")
@pytest.mark.duration(40)
@pytest.mark.circuitpy_drive({"code.py": BSIM_TX_POWER_LOW_CODE})
def test_bsim_tx_power_low_rssi(bsim_phy, circuitpython, zephyr_sample):
    """Verify low TX power reduces RSSI."""
    observer = zephyr_sample

    circuitpython.wait_until_done()

    cp_output = circuitpython.serial.all_output
    obs_output = observer.serial.all_output

    assert "advertising low" in cp_output
    assert "done" in cp_output

    # With 40 dB channel attenuation and -20 dBm TX → RSSI ~ -59
    rssi_pattern = re.compile(r"RSSI (-?\d+)\), type \d+, AD data len 12")
    all_rssi = [int(m.group(1)) for m in rssi_pattern.finditer(obs_output)]
    logger.info("RSSI values: %s", all_rssi)

    assert len(all_rssi) > 0, "Observer saw no advertisements"
    assert all_rssi[0] < -39, f"Expected lower RSSI with -20 dBm TX, got {all_rssi[0]}"


@pytest.mark.duration(20)
@pytest.mark.circuitpy_drive(
    {
        "code.py": BSIM_ADVERT_SCAN_RESPONSE_ADV_CODE,
        "settings.toml": BSIM_ADVERT_SCAN_RESPONSE_SETTINGS,
    }
)
@pytest.mark.circuitpy_drive({"code.py": BSIM_ADVERT_SCAN_RESPONSE_SCAN_CODE})
def test_bsim_scan_response_not_merged(bsim_phy, circuitpython1, circuitpython2):
    """The adapter does not merge advertisement and scan response.

    The advertiser sends a primary advert (marker "ADVADV") and a scan
    response (marker "SCANRSP") as separate packets. With duplicate filtering
    disabled and no merging in the adapter, the scanner observes them as two
    distinct ScanEntry objects: one with scan_response=False (the primary
    advert) and one with scan_response=True (the scan response).
    """
    scanner = circuitpython2

    scanner.wait_until_done()

    scanner_output = scanner.serial.all_output
    assert "primary" in scanner_output, (
        f"primary advert entry not observed separately: {scanner_output}"
    )
    assert "scan_response" in scanner_output, (
        f"scan response entry not observed separately: {scanner_output}"
    )
    assert "both" in scanner_output, (
        f"did not observe both advert and scan response: {scanner_output}"
    )
    assert "scan done True True" in scanner_output, f"scan did not confirm both: {scanner_output}"
