# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""BLE adapter state tests for bsim."""

import pytest


# --- Enable/disable cycle ---

BSIM_ENABLE_DISABLE_CODE = """\
import _bleio

adapter = _bleio.adapter

# Check initial state (should be enabled after boot)
print("enabled start", adapter.enabled)

# Disable
adapter.enabled = False
print("enabled false", adapter.enabled)

# Re-enable
adapter.enabled = True
print("enabled true", adapter.enabled)

# Verify it reports as enabled
print("enabled final", adapter.enabled)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_ENABLE_DISABLE_CODE})
def test_bsim_adapter_enable_disable(bsim_phy, circuitpython):
    """Toggle adapter.enabled and verify state."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "enabled start True" in output
    assert "enabled false False" in output
    assert "enabled true True" in output
    assert "enabled final True" in output
    assert "done" in output


# --- Disable stops advertising ---

BSIM_DISABLE_STOPS_ADV_CODE = """\
import _bleio

adapter = _bleio.adapter

name = b"CPADV"
advertisement = bytes((2, 0x01, 0x06, len(name) + 1, 0x09)) + name

adapter.start_advertising(advertisement, connectable=False)
print("advertising", adapter.advertising)

adapter.enabled = False
print("advertising after disable", adapter.advertising)

adapter.enabled = True
print("advertising after enable", adapter.advertising)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_DISABLE_STOPS_ADV_CODE})
def test_bsim_adapter_disable_stops_advertising(bsim_phy, circuitpython):
    """Disabling the adapter stops advertising."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "advertising True" in output
    assert "advertising after disable False" in output
    assert "advertising after enable False" in output
    assert "done" in output


# --- Adapter state across soft reload ---

BSIM_ENABLE_RELOAD_CODE = """\
import _bleio

adapter = _bleio.adapter

print("enabled", adapter.enabled)
print("advertising", adapter.advertising)
print("connected", adapter.connected)
print("done")
"""


@pytest.mark.port_resets(3)
@pytest.mark.circuitpy_drive({"code.py": BSIM_ENABLE_RELOAD_CODE})
def test_bsim_adapter_state_after_reload(bsim_phy, circuitpython):
    """Adapter state is clean after soft reload."""
    circuitpython.serial.wait_for("done")
    circuitpython.serial.wait_for("Press any key to enter the REPL")
    circuitpython.serial.write("\x04")

    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert output.count("enabled True") >= 2
    assert output.count("advertising False") >= 2
    assert output.count("connected False") >= 2


# --- Adapter name truncation ---

BSIM_NAME_TRUNCATION_CODE = """\
import _bleio

adapter = _bleio.adapter

# Set a very long name
long_name = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%"
adapter.name = long_name

# Read back — should be truncated to fit CONFIG_BT_DEVICE_NAME_MAX
name = adapter.name
print("name len", len(name))
print("name", name)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_NAME_TRUNCATION_CODE})
def test_bsim_adapter_name_truncation(bsim_phy, circuitpython):
    """Very long adapter.name is truncated to fit Zephyr limit."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "name len" in output
    # Should be truncated — length must be less than input (72 chars)
    name_line = [l for l in output.split("\n") if "name len" in l][0]
    name_len = int(name_line.split()[-1])
    assert name_len < 72
    assert "done" in output
