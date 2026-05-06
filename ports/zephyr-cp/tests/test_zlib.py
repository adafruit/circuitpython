# SPDX-FileCopyrightText: 2026 Tim Cocks for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test the zlib module against CPython zlib.

CircuitPython's zlib only implements ``decompress``; these tests compress data
with CPython and verify the zephyr-cp port decodes it back to the same bytes.
"""

import zlib

import pytest


PLAIN_TEXT = b"CircuitPython running on Zephyr says hello!"
REPEATED_TEXT = b"The quick brown fox jumps over the lazy dog. " * 32
BINARY_BLOB = bytes(range(256)) * 4


def _make_code(compressed: bytes, wbits: int, expected_len: int) -> str:
    return (
        "import zlib\n"
        f"compressed = {compressed!r}\n"
        f"out = zlib.decompress(compressed, {wbits})\n"
        f'print(f"out_len: {{len(out)}}")\n'
        f'print(f"expected_len: {expected_len}")\n'
        'print(f"out_hex: {out.hex()}")\n'
        'print("done")\n'
    )


ZLIB_COMPRESSED = zlib.compress(PLAIN_TEXT)
ZLIB_CODE = _make_code(ZLIB_COMPRESSED, wbits=15, expected_len=len(PLAIN_TEXT))


@pytest.mark.circuitpy_drive({"code.py": ZLIB_CODE})
def test_zlib_decompress_zlib_format(circuitpython):
    """Data produced by CPython zlib.compress() round-trips through CircuitPython zlib.decompress()."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert f"out_len: {len(PLAIN_TEXT)}" in output
    assert f"out_hex: {PLAIN_TEXT.hex()}" in output
    assert "done" in output


REPEATED_COMPRESSED = zlib.compress(REPEATED_TEXT)
REPEATED_CODE = _make_code(REPEATED_COMPRESSED, wbits=15, expected_len=len(REPEATED_TEXT))


@pytest.mark.circuitpy_drive({"code.py": REPEATED_CODE})
def test_zlib_decompress_repeated(circuitpython):
    """Highly-compressible repeated input decompresses correctly (back-references)."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert f"out_len: {len(REPEATED_TEXT)}" in output
    # Repeated text has many back-references; shrinks a lot.
    assert len(REPEATED_COMPRESSED) < len(REPEATED_TEXT) // 4
    assert f"out_hex: {REPEATED_TEXT.hex()}" in output
    assert "done" in output


BINARY_COMPRESSED = zlib.compress(BINARY_BLOB)
BINARY_CODE = _make_code(BINARY_COMPRESSED, wbits=15, expected_len=len(BINARY_BLOB))


@pytest.mark.circuitpy_drive({"code.py": BINARY_CODE})
def test_zlib_decompress_binary_bytes(circuitpython):
    """Decompression preserves every byte value (0x00-0xFF)."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert f"out_len: {len(BINARY_BLOB)}" in output
    assert f"out_hex: {BINARY_BLOB.hex()}" in output
    assert "done" in output


# Raw DEFLATE stream (no zlib header/trailer).
_raw_compressor = zlib.compressobj(wbits=-15)
RAW_COMPRESSED = _raw_compressor.compress(PLAIN_TEXT) + _raw_compressor.flush()
RAW_CODE = _make_code(RAW_COMPRESSED, wbits=-15, expected_len=len(PLAIN_TEXT))


@pytest.mark.circuitpy_drive({"code.py": RAW_CODE})
def test_zlib_decompress_raw_deflate(circuitpython):
    """Raw DEFLATE streams (wbits=-15) decompress to the original bytes."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert f"out_len: {len(PLAIN_TEXT)}" in output
    assert f"out_hex: {PLAIN_TEXT.hex()}" in output
    assert "done" in output


# Gzip wrapper (wbits=31).
_gzip_compressor = zlib.compressobj(wbits=31)
GZIP_COMPRESSED = _gzip_compressor.compress(PLAIN_TEXT) + _gzip_compressor.flush()
GZIP_CODE = _make_code(GZIP_COMPRESSED, wbits=31, expected_len=len(PLAIN_TEXT))


@pytest.mark.circuitpy_drive({"code.py": GZIP_CODE})
def test_zlib_decompress_gzip_format(circuitpython):
    """Gzip-wrapped streams (wbits=31) decompress to the original bytes."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert f"out_len: {len(PLAIN_TEXT)}" in output
    assert f"out_hex: {PLAIN_TEXT.hex()}" in output
    assert "done" in output
