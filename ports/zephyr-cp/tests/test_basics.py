# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test basic native_sim functionality."""

import pytest


@pytest.mark.circuitpy_drive(None)
def test_blank_flash_hello_world(circuitpython):
    """Test that an erased flash shows code.py output header."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "Board ID:native_native_sim" in output
    assert "UID:" in output
    assert "code.py output:" in output
    assert "Hello World" in output
    assert "done" in output


# --- PTY Input Tests ---


INPUT_CODE = """\
import sys

print("ready")
char = sys.stdin.read(1)
print(f"received: {repr(char)}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": INPUT_CODE})
def test_basic_serial_input(circuitpython):
    """Test reading single character from serial via PTY write."""
    circuitpython.serial.wait_for("ready")
    circuitpython.serial.write("A")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "ready" in output
    assert "received: 'A'" in output
    assert "done" in output


INPUT_FUNC_CODE = """\
print("ready")
name = input("Enter name: ")
print(f"hello {name}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": INPUT_FUNC_CODE})
def test_input_function(circuitpython):
    """Test the built-in input() function with PTY input."""
    circuitpython.serial.wait_for("Enter name:")
    circuitpython.serial.write("World\r")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "ready" in output
    assert "Enter name:" in output
    assert "hello World" in output
    assert "done" in output


INTERRUPT_CODE = """\
import time

print("starting")
for i in range(100):
    print(f"loop {i}")
    time.sleep(0.1)
print("completed")
"""


@pytest.mark.circuitpy_drive({"code.py": INTERRUPT_CODE})
def test_ctrl_c_interrupt(circuitpython):
    """Test sending Ctrl+C (0x03) to interrupt running code."""
    circuitpython.serial.wait_for("loop 5")
    circuitpython.serial.write("\x03")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "starting" in output
    assert "loop 5" in output
    assert "KeyboardInterrupt" in output
    assert "completed" not in output


RELOAD_CODE = """\
print("first run")
import time
time.sleep(1)
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": RELOAD_CODE})
@pytest.mark.code_py_runs(2)
def test_ctrl_d_soft_reload(circuitpython):
    """Test sending Ctrl+D (0x04) to trigger soft reload."""
    circuitpython.serial.wait_for("first run")
    circuitpython.serial.write("\x04")
    circuitpython.wait_until_done()

    # Should see "first run" appear multiple times due to reload
    # or see a soft reboot message
    output = circuitpython.serial.all_output
    assert "first run" in output
    # The soft reload should restart the code before "done" is printed
    assert "done" in output
    assert output.count("first run") > 1
