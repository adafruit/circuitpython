# SPDX-FileCopyrightText: 2026 Tim Cocks for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test the hashlib module against CPython hashlib."""

import hashlib

import pytest


SHORT_DATA = b"CircuitPython!"
CHUNK_A = b"Hello, "
CHUNK_B = b"CircuitPython world!"
LONG_DATA = b"The quick brown fox jumps over the lazy dog." * 64


SHA256_CODE = """\
import hashlib

h = hashlib.new("sha256", b"CircuitPython!")
print(f"digest_size: {h.digest_size}")
print(f"digest_hex: {h.digest().hex()}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": SHA256_CODE})
def test_hashlib_sha256_basic(circuitpython):
    """sha256 digest on a small buffer matches CPython hashlib."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    expected = hashlib.sha256(SHORT_DATA).hexdigest()
    assert "digest_size: 32" in output
    assert f"digest_hex: {expected}" in output
    assert "done" in output


SHA1_CODE = """\
import hashlib

h = hashlib.new("sha1", b"CircuitPython!")
print(f"digest_size: {h.digest_size}")
print(f"digest_hex: {h.digest().hex()}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": SHA1_CODE})
def test_hashlib_sha1_basic(circuitpython):
    """sha1 digest on a small buffer matches CPython hashlib."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    expected = hashlib.sha1(SHORT_DATA).hexdigest()
    assert "digest_size: 20" in output
    assert f"digest_hex: {expected}" in output
    assert "done" in output


UPDATE_CODE = """\
import hashlib

h = hashlib.new("sha256")
h.update(b"Hello, ")
h.update(b"CircuitPython world!")
print(f"chunked_hex: {h.digest().hex()}")

h2 = hashlib.new("sha256", b"Hello, CircuitPython world!")
print(f"oneshot_hex: {h2.digest().hex()}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": UPDATE_CODE})
def test_hashlib_sha256_update_chunks(circuitpython):
    """Multiple update() calls produce the same digest as a single buffer, and match CPython."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    expected = hashlib.sha256(CHUNK_A + CHUNK_B).hexdigest()
    assert f"chunked_hex: {expected}" in output
    assert f"oneshot_hex: {expected}" in output
    assert "done" in output


LONG_CODE = """\
import hashlib

data = b"The quick brown fox jumps over the lazy dog." * 64
h = hashlib.new("sha256", data)
print(f"sha256_hex: {h.digest().hex()}")

h1 = hashlib.new("sha1", data)
print(f"sha1_hex: {h1.digest().hex()}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": LONG_CODE})
def test_hashlib_long_input(circuitpython):
    """Digests of a multi-block input match CPython for both sha1 and sha256."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert f"sha256_hex: {hashlib.sha256(LONG_DATA).hexdigest()}" in output
    assert f"sha1_hex: {hashlib.sha1(LONG_DATA).hexdigest()}" in output
    assert "done" in output


EMPTY_CODE = """\
import hashlib

h = hashlib.new("sha256", b"")
print(f"empty256: {h.digest().hex()}")

h = hashlib.new("sha1", b"")
print(f"empty1: {h.digest().hex()}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": EMPTY_CODE})
def test_hashlib_empty_input(circuitpython):
    """Empty-input digests match CPython well-known values."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert f"empty256: {hashlib.sha256(b'').hexdigest()}" in output
    assert f"empty1: {hashlib.sha1(b'').hexdigest()}" in output
    assert "done" in output


BAD_ALGO_CODE = """\
import hashlib

try:
    hashlib.new("md5", b"data")
except ValueError:
    print("bad_algo: ValueError")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": BAD_ALGO_CODE})
def test_hashlib_unsupported_algorithm(circuitpython):
    """Unsupported algorithm names raise ValueError."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "bad_algo: ValueError" in output
    assert "done" in output
