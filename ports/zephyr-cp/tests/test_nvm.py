# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test NVM functionality on native_sim."""

import pytest


NVM_BASIC_CODE = """\
import microcontroller

nvm = microcontroller.nvm
print(f"nvm length: {len(nvm)}")

# Write some bytes
nvm[0] = 42
nvm[1] = 99
print(f"nvm[0]: {nvm[0]}")
print(f"nvm[1]: {nvm[1]}")

# Write a slice
nvm[2:5] = b"\\x01\\x02\\x03"
print(f"nvm[2:5]: {list(nvm[2:5])}")

print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": NVM_BASIC_CODE})
def test_nvm_read_write(circuitpython):
    """Test basic NVM read and write operations."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "nvm length: 8192" in output
    assert "nvm[0]: 42" in output
    assert "nvm[1]: 99" in output
    assert "nvm[2:5]: [1, 2, 3]" in output
    assert "done" in output


NVM_PERSIST_CODE = """\
import microcontroller

nvm = microcontroller.nvm
value = nvm[0]
print(f"nvm[0]: {value}")

if value == 255:
    # First run: write a marker
    nvm[0] = 123
    print("wrote marker")
else:
    print(f"marker found: {value}")

print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": NVM_PERSIST_CODE})
@pytest.mark.code_py_runs(2)
def test_nvm_persists_across_reload(circuitpython):
    """Test that NVM data persists across soft reloads."""
    circuitpython.serial.wait_for("wrote marker")
    # Trigger soft reload
    circuitpython.serial.write("\x04")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "nvm[0]: 255" in output
    assert "wrote marker" in output
    assert "marker found: 123" in output
    assert "done" in output
