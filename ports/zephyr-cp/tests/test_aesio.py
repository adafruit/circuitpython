# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test the aesio module."""

import pytest

from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes


KEY = b"Sixteen byte key"
PLAINTEXT = b"CircuitPython!!!"  # 16 bytes


ECB_CODE = """\
import aesio

key = b'Sixteen byte key'
inp = b'CircuitPython!!!'
outp = bytearray(len(inp))
cipher = aesio.AES(key, aesio.MODE_ECB)
cipher.encrypt_into(inp, outp)
print(f"ciphertext_hex: {outp.hex()}")

decrypted = bytearray(len(outp))
cipher.decrypt_into(bytes(outp), decrypted)
print(f"decrypted: {bytes(decrypted)}")
print(f"match: {bytes(decrypted) == inp}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": ECB_CODE})
def test_aesio_ecb(circuitpython):
    """AES-ECB round-trips and matches CPython cryptography's output."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    encryptor = Cipher(algorithms.AES(KEY), modes.ECB()).encryptor()
    expected_hex = (encryptor.update(PLAINTEXT) + encryptor.finalize()).hex()
    assert f"ciphertext_hex: {expected_hex}" in output
    assert "match: True" in output
    assert "done" in output


IV = b"InitializationVe"  # 16 bytes
CBC_PLAINTEXT = b"CircuitPython!!!" * 2  # 32 bytes, multiple of 16
CTR_PLAINTEXT = b"CircuitPython is fun to use!"  # 28 bytes, arbitrary length


CBC_CODE = """\
import aesio

key = b'Sixteen byte key'
iv = b'InitializationVe'
inp = b'CircuitPython!!!' * 2
outp = bytearray(len(inp))
cipher = aesio.AES(key, aesio.MODE_CBC, iv)
print(f"mode: {cipher.mode}")
cipher.encrypt_into(inp, outp)
print(f"ciphertext_hex: {outp.hex()}")

# Re-create cipher to reset IV state for decryption.
cipher2 = aesio.AES(key, aesio.MODE_CBC, iv)
decrypted = bytearray(len(outp))
cipher2.decrypt_into(bytes(outp), decrypted)
print(f"match: {bytes(decrypted) == inp}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": CBC_CODE})
def test_aesio_cbc(circuitpython):
    """AES-CBC round-trips and matches CPython cryptography's output."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    encryptor = Cipher(algorithms.AES(KEY), modes.CBC(IV)).encryptor()
    expected_hex = (encryptor.update(CBC_PLAINTEXT) + encryptor.finalize()).hex()
    assert "mode: 2" in output
    assert f"ciphertext_hex: {expected_hex}" in output
    assert "match: True" in output
    assert "done" in output


CTR_CODE = """\
import aesio

key = b'Sixteen byte key'
iv = b'InitializationVe'
inp = b'CircuitPython is fun to use!'
outp = bytearray(len(inp))
cipher = aesio.AES(key, aesio.MODE_CTR, iv)
print(f"mode: {cipher.mode}")
cipher.encrypt_into(inp, outp)
print(f"ciphertext_hex: {outp.hex()}")

cipher2 = aesio.AES(key, aesio.MODE_CTR, iv)
decrypted = bytearray(len(outp))
cipher2.decrypt_into(bytes(outp), decrypted)
print(f"decrypted: {bytes(decrypted)}")
print(f"match: {bytes(decrypted) == inp}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": CTR_CODE})
def test_aesio_ctr(circuitpython):
    """AES-CTR handles arbitrary-length buffers and matches CPython output."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    encryptor = Cipher(algorithms.AES(KEY), modes.CTR(IV)).encryptor()
    expected_hex = (encryptor.update(CTR_PLAINTEXT) + encryptor.finalize()).hex()
    assert "mode: 6" in output
    assert f"ciphertext_hex: {expected_hex}" in output
    assert "match: True" in output
    assert "done" in output


REKEY_CODE = """\
import aesio

key1 = b'Sixteen byte key'
key2 = b'Another 16 byte!'
inp = b'CircuitPython!!!'

cipher = aesio.AES(key1, aesio.MODE_ECB)
out1 = bytearray(16)
cipher.encrypt_into(inp, out1)
print(f"ct1_hex: {out1.hex()}")

cipher.rekey(key2)
out2 = bytearray(16)
cipher.encrypt_into(inp, out2)
print(f"ct2_hex: {out2.hex()}")
print(f"different: {bytes(out1) != bytes(out2)}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": REKEY_CODE})
def test_aesio_rekey(circuitpython):
    """rekey() switches the active key; ciphertexts match CPython for both keys."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    enc1 = Cipher(algorithms.AES(b"Sixteen byte key"), modes.ECB()).encryptor()
    ct1 = (enc1.update(PLAINTEXT) + enc1.finalize()).hex()
    enc2 = Cipher(algorithms.AES(b"Another 16 byte!"), modes.ECB()).encryptor()
    ct2 = (enc2.update(PLAINTEXT) + enc2.finalize()).hex()
    assert f"ct1_hex: {ct1}" in output
    assert f"ct2_hex: {ct2}" in output
    assert "different: True" in output
    assert "done" in output


MODE_PROPERTY_CODE = """\
import aesio

key = b'Sixteen byte key'
iv = b'InitializationVe'
cipher = aesio.AES(key, aesio.MODE_ECB)
print(f"initial: {cipher.mode}")
print(f"ECB={aesio.MODE_ECB} CBC={aesio.MODE_CBC} CTR={aesio.MODE_CTR}")

for name, m in (("ECB", aesio.MODE_ECB), ("CBC", aesio.MODE_CBC), ("CTR", aesio.MODE_CTR)):
    cipher.mode = m
    print(f"set_{name}: {cipher.mode}")

try:
    cipher.mode = 99
except NotImplementedError as e:
    print(f"bad_mode: NotImplementedError")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": MODE_PROPERTY_CODE})
def test_aesio_mode_property(circuitpython):
    """The mode property is readable, writable, and rejects unsupported values."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "initial: 1" in output
    assert "ECB=1 CBC=2 CTR=6" in output
    assert "set_ECB: 1" in output
    assert "set_CBC: 2" in output
    assert "set_CTR: 6" in output
    assert "bad_mode: NotImplementedError" in output
    assert "done" in output


KEY_LENGTHS_CODE = """\
import aesio

inp = b'CircuitPython!!!'
for key in (b'A' * 16, b'B' * 24, b'C' * 32):
    cipher = aesio.AES(key, aesio.MODE_ECB)
    out = bytearray(16)
    cipher.encrypt_into(inp, out)
    print(f"len{len(key)}: {out.hex()}")

try:
    aesio.AES(b'too short', aesio.MODE_ECB)
except ValueError:
    print("bad_key: ValueError")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": KEY_LENGTHS_CODE})
def test_aesio_key_lengths(circuitpython):
    """AES-128/192/256 keys all work and match CPython; bad key length raises."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    for key in (b"A" * 16, b"B" * 24, b"C" * 32):
        encryptor = Cipher(algorithms.AES(key), modes.ECB()).encryptor()
        expected = (encryptor.update(PLAINTEXT) + encryptor.finalize()).hex()
        assert f"len{len(key)}: {expected}" in output
    assert "bad_key: ValueError" in output
    assert "done" in output
