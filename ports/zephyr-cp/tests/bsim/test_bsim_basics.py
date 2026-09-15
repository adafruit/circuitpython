# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Basic BabbleSim connectivity tests for bsim."""

import pytest

from .conftest import get_library_files

BSIM_CODE = """\
print("bsim ready")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_CODE})
@pytest.mark.circuitpy_drive({"code.py": BSIM_CODE})
@pytest.mark.duration(20)
def test_bsim_dual_instance_connect(bsim_phy, circuitpython1, circuitpython2, board):
    """Run two bsim instances on the same sim id and verify UART output."""

    # Wait for both devices to complete before checking output.
    circuitpython1.wait_until_done()
    circuitpython2.wait_until_done()

    output0 = circuitpython1.serial.all_output
    output1 = circuitpython2.serial.all_output

    assert f"Board ID:{board}" in output0
    assert f"Board ID:{board}" in output1
    assert "bsim ready" in output0
    assert "bsim ready" in output1


# --- adafruit_ble library import ---

BSIM_BLE_IMPORT_CODE = """\
import adafruit_ble

print("adafruit_ble version", adafruit_ble.__version__)
print("adafruit_ble repo", adafruit_ble.__repo__)
print("done")
"""


@pytest.mark.circuitpy_drive(
    {"code.py": BSIM_BLE_IMPORT_CODE, **get_library_files("adafruit_ble")}
)
def test_bsim_ble_library_import(bsim_phy, circuitpython):
    """Import adafruit_ble from CIRCUITPY and verify basic attributes."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "adafruit_ble version" in output
    assert "adafruit_ble repo" in output
    assert "done" in output
