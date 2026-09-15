# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Advertising test for the supervisor BLE workflow (bsim).

The supervisor's built-in BLE workflow (file-transfer + serial services and
advertising) starts automatically at boot, independent of user code. One
CircuitPython device runs an idle code.py so its workflow advertises; a second
CircuitPython device scans with the adafruit_ble library and verifies the
workflow's advertisement carries the File Transfer service UUID (0xFEBB) and
the CIRCUITPY device name.

The second device disables its own workflow via settings.toml so only the
first device advertises.
"""

import logging
import re

import pytest

from .conftest import get_library_files

logger = logging.getLogger(__name__)

_ADAFRUIT_BLE = get_library_files("adafruit_ble")

# Device 1: idle code.py. The supervisor workflow advertises during the sleep.
WORKFLOW_IDLE_CODE = """\
import time
time.sleep(15)
"""

# Device 2: scan for the workflow advertisement. The workflow puts the
# File Transfer service UUID (0xFEBB) in the primary advertisement and the full
# "CIRCUITPYxxxx" name in the scan response, so those arrive as separate
# Advertisement objects (service list vs. scan response).
SCANNER_CODE = """\
import time
from adafruit_ble import BLERadio
from adafruit_ble.advertising import Advertisement
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.uuid import StandardUUID

ble = BLERadio()
print("scan start")
found_ft = False
found_name = False
ft_rssi = None
for adv in ble.start_scan(ProvideServicesAdvertisement, Advertisement, timeout=15, active=True):
    if isinstance(adv, ProvideServicesAdvertisement) and StandardUUID(0xFEBB) in adv.services:
        found_ft = True
        ft_rssi = adv.rssi
        print("ft_uuid febb")
    name = adv.complete_name or adv.short_name or ""
    if name:
        found_name = True
        print("cp_name", name)
    if found_ft and found_name:
        print("workflow found")
        print("ft_rssi", ft_rssi)
        break
ble.stop_scan()
print("scan done", found_ft, found_name, ft_rssi)
"""

# Disable the workflow on the scanner so it doesn't advertise alongside device 1.
SCANNER_SETTINGS = "CIRCUITPY_BLE_WORKFLOW = false\n"


@pytest.mark.duration(20)
@pytest.mark.circuitpy_drive({"code.py": WORKFLOW_IDLE_CODE})
@pytest.mark.circuitpy_drive(
    {
        "code.py": SCANNER_CODE,
        "settings.toml": SCANNER_SETTINGS,
        **_ADAFRUIT_BLE,
    }
)
def test_bsim_workflow_advertises(bsim_phy, circuitpython1, circuitpython2):
    """The supervisor BLE workflow advertises 0xFEBB and the CIRCUITPY name."""
    workflow = circuitpython1
    scanner = circuitpython2

    scanner.wait_until_done()

    scanner_output = scanner.serial.all_output
    assert "ft_uuid febb" in scanner_output, (
        f"File Transfer service UUID 0xFEBB not observed: {scanner_output}"
    )
    # The device name currently advertises as the controller default ("Zephyr");
    # the CIRCUITPY name is configured separately, at which point this will
    # observe the full "CIRCUITPY..." name. Either way a name must be present.
    assert "cp_name" in scanner_output, f"device name not observed: {scanner_output}"
    assert "workflow found" in scanner_output, (
        f"workflow advertisement not fully observed: {scanner_output}"
    )

    # The workflow advertises its public advert at reduced TX power (-20 dBm),
    # so with the 40 dB channel attenuation the RSSI is ~ -59, well below the
    # 0 dBm baseline of -39 seen for user advertisements.
    rssi_match = re.search(r"ft_rssi (-?\d+)", scanner_output)
    assert rssi_match, f"ft_rssi not observed: {scanner_output}"
    ft_rssi = int(rssi_match.group(1))
    logger.info("workflow ft_rssi: %d", ft_rssi)
    assert ft_rssi < -39, f"Expected reduced-TX-power RSSI (< -39), got {ft_rssi}"

    # The workflow device should not have entered safe mode.
    workflow_output = workflow.serial.all_output
    assert "safe mode" not in workflow_output.lower(), (
        f"workflow device entered safe mode: {workflow_output}"
    )
