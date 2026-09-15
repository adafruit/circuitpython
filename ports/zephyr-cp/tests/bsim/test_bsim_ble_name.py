# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""BLE adapter name tests for bsim.

Covers reading the default adapter name (derived from the device address),
setting a custom name and reading it back, and verifying the default name is
re-applied every time the adapter is re-enabled.
"""

import re

import pytest


# The address prints as <Address aa:bb:cc:dd:ee:ff> in big-endian/expected order.
# The default name is "CIRCUITPY" plus the last two octets (ee, ff) concatenated
# without colons, e.g. an address of <Address ...:d2:24> -> "CIRCUITPYd224".
ADDRESS_RE = re.compile(
    r"<Address ([0-9a-f]{2}):([0-9a-f]{2}):([0-9a-f]{2}):"
    r"([0-9a-f]{2}):([0-9a-f]{2}):([0-9a-f]{2})>"
)

# Strip ANSI/OSC escape sequences (e.g. terminal title updates) and carriage
# returns that CircuitPython interleaves with code.py output.
_ANSI_RE = re.compile(r"\x1b\][^\x07\x1b]*(?:\x07|\x1b\\)|\x1b\[[0-9;?]*[A-Za-z]")


def _clean(output: str) -> str:
    return _ANSI_RE.sub("", output).replace("\r", "")


def _expected_default_name(address_repr: str) -> str:
    m = ADDRESS_RE.search(address_repr)
    assert m is not None, f"unexpected address repr: {address_repr!r}"
    return "CIRCUITPY" + m.group(5) + m.group(6)


# --- Default name is derived from the adapter address ---

BSIM_DEFAULT_NAME_CODE = """\
import _bleio

adapter = _bleio.adapter
adapter.enabled = True
print("default name", adapter.name)
print("address", adapter.address)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_DEFAULT_NAME_CODE})
def test_bsim_default_name_matches_address(bsim_phy, circuitpython):
    """The default name is CIRCUITPY plus the last two address octets."""
    circuitpython.wait_until_done()

    output = _clean(circuitpython.serial.all_output)
    name = re.search(r"(?m)^default name (\S+)$", output).group(1)
    address_repr = re.search(r"(?m)^address (.+)$", output).group(1).strip()

    assert name == _expected_default_name(address_repr)
    assert re.search(r"(?m)^done$", output) is not None


# --- Set a custom name and read it back ---

BSIM_SET_NAME_CODE = """\
import _bleio

adapter = _bleio.adapter
adapter.enabled = True
adapter.name = "CPNAME"
print("name", adapter.name)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_SET_NAME_CODE})
def test_bsim_set_name(bsim_phy, circuitpython):
    """Set the BLE name and read it back on bsim."""
    circuitpython.wait_until_done()

    output = _clean(circuitpython.serial.all_output)
    assert re.search(r"(?m)^name CPNAME$", output) is not None
    assert re.search(r"(?m)^done$", output) is not None


# --- Re-enabling the adapter restores the default name ---

BSIM_NAME_RESET_ON_ENABLE_CODE = """\
import _bleio

adapter = _bleio.adapter

adapter.enabled = True
print("default", adapter.name)

adapter.name = "CUSTOM"
print("custom", adapter.name)

# Re-enabling re-applies the default name derived from the address.
adapter.enabled = False
adapter.enabled = True
print("reset", adapter.name)
print("address", adapter.address)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BSIM_NAME_RESET_ON_ENABLE_CODE})
def test_bsim_name_resets_on_enable(bsim_phy, circuitpython):
    """Disabling and re-enabling the adapter restores the default name."""
    circuitpython.wait_until_done()

    output = _clean(circuitpython.serial.all_output)
    default_name = re.search(r"(?m)^default (\S+)$", output).group(1)
    assert re.search(r"(?m)^custom CUSTOM$", output) is not None
    reset_name = re.search(r"(?m)^reset (\S+)$", output).group(1)
    address_repr = re.search(r"(?m)^address (.+)$", output).group(1).strip()

    # The custom name is gone and the address-derived default is back.
    assert reset_name != "CUSTOM"
    assert reset_name == _expected_default_name(address_repr)
    # The re-applied default matches the boot-time default.
    assert reset_name == default_name
    assert re.search(r"(?m)^done$", output) is not None
