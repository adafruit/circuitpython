# SPDX-FileCopyrightText: 2026 Tim Cocks for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test the getpass core module."""

import pytest


DEFAULT_PROMPT_CODE = """\
import getpass

print("ready")
pw = getpass.getpass()
print(f"got: {pw}")
print(f"length: {len(pw)}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": DEFAULT_PROMPT_CODE})
def test_getpass_default_prompt(circuitpython):
    """getpass() with no args prints 'Password: ' and returns the typed string."""
    circuitpython.serial.wait_for("ready")
    circuitpython.serial.wait_for("Password:")
    circuitpython.serial.write("hunter2\r")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "Password:" in output
    assert "got: hunter2" in output
    assert "length: 7" in output
    assert "done" in output


CUSTOM_PROMPT_CODE = """\
import getpass

print("ready")
pw = getpass.getpass("Enter secret: ")
print(f"got: {pw}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": CUSTOM_PROMPT_CODE})
def test_getpass_custom_prompt(circuitpython):
    """A user-supplied prompt string is written before reading input."""
    circuitpython.serial.wait_for("Enter secret:")
    circuitpython.serial.write("s3cr3t\r")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "Enter secret:" in output
    assert "got: s3cr3t" in output
    assert "done" in output


NO_ECHO_CODE = """\
import getpass

print("ready")
pw = getpass.getpass("Pwd: ")
print("---boundary---")
print(f"got: {pw}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": NO_ECHO_CODE})
def test_getpass_does_not_echo(circuitpython):
    """Characters typed during getpass are not echoed back to the terminal."""
    circuitpython.serial.wait_for("Pwd:")
    secret = "TopSecret123"
    circuitpython.serial.write(secret + "\r")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    # Split around the boundary: nothing typed should appear before it,
    # but the printed value appears after.
    pre, _, post = output.partition("---boundary---")
    assert secret not in pre
    assert f"got: {secret}" in post
    assert "done" in post


BACKSPACE_CODE = """\
import getpass

print("ready")
pw = getpass.getpass("P: ")
print(f"got: {pw}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BACKSPACE_CODE})
def test_getpass_backspace(circuitpython):
    """Backspace (0x7f) removes the previous character from the buffer."""
    circuitpython.serial.wait_for("P:")
    # Type "abcX", then backspace, then "d" -> "abcd"
    circuitpython.serial.write("abcX\x7fd\r")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "got: abcd" in output
    assert "done" in output


CTRL_C_CODE = """\
import getpass

print("ready")
try:
    getpass.getpass("Pwd: ")
    print("no exception")
except KeyboardInterrupt:
    print("interrupted")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": CTRL_C_CODE})
def test_getpass_ctrl_c_raises_keyboard_interrupt(circuitpython):
    """Sending Ctrl+C while getpass is reading raises KeyboardInterrupt."""
    circuitpython.serial.wait_for("Pwd:")
    circuitpython.serial.write("\x03")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "interrupted" in output
    assert "no exception" not in output
    assert "done" in output


CTRL_D_EMPTY_CODE = """\
import getpass

print("ready")
try:
    getpass.getpass("Pwd: ")
    print("no exception")
except EOFError:
    print("eof")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": CTRL_D_EMPTY_CODE})
def test_getpass_ctrl_d_empty_raises_eof(circuitpython):
    """Ctrl+D with an empty buffer raises EOFError."""
    circuitpython.serial.wait_for("Pwd:")
    circuitpython.serial.write("\x04")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "eof" in output
    assert "no exception" not in output
    assert "done" in output


EMPTY_INPUT_CODE = """\
import getpass

print("ready")
pw = getpass.getpass("Pwd: ")
print(f"length: {len(pw)}")
print(f"empty: {pw == ''}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": EMPTY_INPUT_CODE})
def test_getpass_empty_input(circuitpython):
    """Pressing return immediately yields an empty string."""
    circuitpython.serial.wait_for("Pwd:")
    circuitpython.serial.write("\r")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "length: 0" in output
    assert "empty: True" in output
    assert "done" in output
