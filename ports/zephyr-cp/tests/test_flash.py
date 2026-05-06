# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test flash filesystem with various erase block sizes."""

import subprocess

import pytest


def read_file_from_flash(flash_file, path):
    """Extract a file from the FAT filesystem in the flash image."""
    result = subprocess.run(
        ["mcopy", "-i", str(flash_file), f"::{path}", "-"],
        capture_output=True,
    )
    if result.returncode != 0:
        raise FileNotFoundError(
            f"Failed to read ::{path} from {flash_file}: {result.stderr.decode()}"
        )
    return result.stdout.decode()


WRITE_READ_CODE = """\
import storage
storage.remount("/", readonly=False)
with open("/test.txt", "w") as f:
    f.write("hello flash")
with open("/test.txt", "r") as f:
    content = f.read()
print(f"content: {content}")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": WRITE_READ_CODE})
def test_flash_default_erase_size(circuitpython):
    """Test filesystem write/read with default 4KB erase blocks."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "content: hello flash" in output
    assert "done" in output

    content = read_file_from_flash(circuitpython.flash_file, "test.txt")
    assert content == "hello flash"


@pytest.mark.circuitpy_drive({"code.py": WRITE_READ_CODE})
@pytest.mark.flash_config(erase_block_size=65536)
def test_flash_64k_erase_blocks(circuitpython):
    """Test filesystem write/read with 64KB erase blocks (128 blocks per page)."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "content: hello flash" in output
    assert "done" in output

    content = read_file_from_flash(circuitpython.flash_file, "test.txt")
    assert content == "hello flash"


@pytest.mark.circuitpy_drive({"code.py": WRITE_READ_CODE})
@pytest.mark.flash_config(erase_block_size=262144, total_size=4 * 1024 * 1024)
def test_flash_256k_erase_blocks(circuitpython):
    """Test filesystem write/read with 256KB erase blocks (like RA8D1 OSPI)."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "content: hello flash" in output
    assert "done" in output

    content = read_file_from_flash(circuitpython.flash_file, "test.txt")
    assert content == "hello flash"


MULTI_FILE_CODE = """\
import storage
storage.remount("/", readonly=False)
for i in range(5):
    name = f"/file{i}.txt"
    with open(name, "w") as f:
        f.write(f"data{i}" * 100)
print("multi_file_ok")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": MULTI_FILE_CODE})
@pytest.mark.flash_config(erase_block_size=262144, total_size=4 * 1024 * 1024)
def test_flash_256k_multi_file(circuitpython):
    """Test multiple file writes with 256KB erase blocks to exercise cache flushing."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "multi_file_ok" in output

    for i in range(5):
        content = read_file_from_flash(circuitpython.flash_file, f"file{i}.txt")
        assert content == f"data{i}" * 100, f"file{i}.txt mismatch"


EXISTING_DATA_CODE = """\
import storage
storage.remount("/", readonly=False)

# Write a file to populate the erase page.
original_data = "A" * 4000
with open("/original.txt", "w") as f:
    f.write(original_data)

# Force a flush so the data is written to flash.
storage.remount("/", readonly=True)
storage.remount("/", readonly=False)

# Now write a small new file. This updates FAT metadata and directory
# entries that share an erase page with original.txt, exercising the
# read-modify-write cycle on the cached page.
with open("/small.txt", "w") as f:
    f.write("tiny")

# Read back original to check it survived.
with open("/original.txt", "r") as f:
    readback = f.read()
if readback == original_data:
    print("existing_data_ok")
else:
    print(f"MISMATCH: got {len(readback)} bytes")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": EXISTING_DATA_CODE})
@pytest.mark.flash_config(erase_block_size=262144, total_size=4 * 1024 * 1024)
def test_flash_256k_existing_data_survives(circuitpython):
    """Test that existing data in an erase page survives when new data is written.

    With 256KB erase blocks (512 blocks per page), writing to any block in
    the page triggers an erase-rewrite of the entire page. Existing blocks
    must be preserved through the read-modify-write cycle.
    """
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "existing_data_ok" in output

    # Verify both files survived on the actual flash image.
    original = read_file_from_flash(circuitpython.flash_file, "original.txt")
    assert original == "A" * 4000, f"original.txt corrupted: got {len(original)} bytes"

    small = read_file_from_flash(circuitpython.flash_file, "small.txt")
    assert small == "tiny"


OVERWRITE_CODE = """\
import storage
storage.remount("/", readonly=False)
with open("/overwrite.txt", "w") as f:
    f.write("first version")
with open("/overwrite.txt", "w") as f:
    f.write("second version")
print("overwrite_ok")
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": OVERWRITE_CODE})
@pytest.mark.flash_config(erase_block_size=262144, total_size=4 * 1024 * 1024)
def test_flash_256k_overwrite(circuitpython):
    """Test overwriting a file with 256KB erase blocks to exercise erase-rewrite cycle."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "overwrite_ok" in output

    content = read_file_from_flash(circuitpython.flash_file, "overwrite.txt")
    assert content == "second version"
