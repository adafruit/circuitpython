# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Filesystem tests that run against every filesystem the port supports.

Each test is parameterized over the two native_sim builds so the same
expectations are checked against the FAT filesystem (``native_native_sim``)
and littlefs (``native_native_sim_lfs``). These tests boot from an erased
flash image so they see the filesystem that CircuitPython creates itself,
not one preloaded from the host (see ``conftest`` for that).
"""

import time

import pytest

from .conftest import FILESYSTEM_SIMULATORS


def _enter_repl(circuitpython):
    """Run the default code.py, then enter the REPL from the reload prompt."""
    circuitpython.serial.wait_for("Press any key to enter the REPL")
    circuitpython.serial.write("\r")
    circuitpython.serial.wait_for(">>>")


def _repl(circuitpython, line):
    """Send one line to the REPL and give it time to echo and run."""
    # The native sim console needs CR, not LF, to submit the line.
    circuitpython.serial.write(line.rstrip("\n") + "\r")
    time.sleep(0.5)


@pytest.mark.parametrize("board", FILESYSTEM_SIMULATORS, indirect=True)
@pytest.mark.circuitpy_drive(None)
@pytest.mark.duration(30)
@pytest.mark.port_resets(6)
def test_fresh_filesystem_defaults(circuitpython):
    """An erased flash image gets a filesystem with the default files."""
    _enter_repl(circuitpython)

    _repl(circuitpython, "import os\n")
    _repl(circuitpython, "entries = os.listdir('/')\n")
    _repl(circuitpython, "print('code.py:', 'code.py' in entries)\n")
    circuitpython.serial.wait_for("code.py: True")
    _repl(circuitpython, "print('lib:', 'lib' in entries)\n")
    circuitpython.serial.wait_for("lib: True")


@pytest.mark.parametrize("board", FILESYSTEM_SIMULATORS, indirect=True)
@pytest.mark.circuitpy_drive(None)
@pytest.mark.duration(60)
@pytest.mark.port_resets(6)
def test_filesystem_write_read_delete(circuitpython):
    """Files survive write, close, reopen and delete."""
    _enter_repl(circuitpython)

    _repl(circuitpython, "with open('/fs_test.txt', 'w') as f:\n")
    _repl(circuitpython, "    f.write('hello filesystem')\n")
    _repl(circuitpython, "\n")
    _repl(circuitpython, "print('read:', open('/fs_test.txt').read())\n")
    circuitpython.serial.wait_for("read: hello filesystem")

    _repl(circuitpython, "import os\n")
    _repl(circuitpython, "print('exists:', 'fs_test.txt' in os.listdir('/'))\n")
    circuitpython.serial.wait_for("exists: True")

    _repl(circuitpython, "os.remove('/fs_test.txt')\n")
    _repl(circuitpython, "print('exists:', 'fs_test.txt' in os.listdir('/'))\n")
    circuitpython.serial.wait_for("exists: False")


@pytest.mark.parametrize("board", FILESYSTEM_SIMULATORS, indirect=True)
@pytest.mark.circuitpy_drive(None)
@pytest.mark.duration(60)
@pytest.mark.port_resets(6)
def test_filesystem_mkdir_rename_stat(circuitpython):
    """Directories, renames and os.stat work."""
    _enter_repl(circuitpython)

    _repl(circuitpython, "import os\n")
    _repl(circuitpython, "os.mkdir('/subdir')\n")
    _repl(circuitpython, "with open('/moved.txt', 'w') as f:\n")
    _repl(circuitpython, "    f.write('moved')\n")
    _repl(circuitpython, "\n")
    _repl(circuitpython, "os.rename('/moved.txt', '/subdir/renamed.txt')\n")
    _repl(circuitpython, "print('dir:', 'subdir' in os.listdir('/'))\n")
    circuitpython.serial.wait_for("dir: True")
    _repl(circuitpython, "print('renamed:', open('/subdir/renamed.txt').read())\n")
    circuitpython.serial.wait_for("renamed: moved")
    _repl(circuitpython, "print('stat size:', os.stat('/subdir/renamed.txt')[6])\n")
    circuitpython.serial.wait_for("stat size: 5")


@pytest.mark.parametrize("board", FILESYSTEM_SIMULATORS, indirect=True)
@pytest.mark.circuitpy_drive(None)
@pytest.mark.duration(60)
@pytest.mark.port_resets(6)
def test_storage_remount(circuitpython):
    """storage.remount('/') toggles writability on the root filesystem."""
    _enter_repl(circuitpython)

    _repl(circuitpython, "import storage\n")
    _repl(circuitpython, "storage.remount('/', readonly=False)\n")
    _repl(circuitpython, "with open('/remount.txt', 'w') as f:\n")
    _repl(circuitpython, "    f.write('writable')\n")
    _repl(circuitpython, "\n")
    _repl(circuitpython, "print('read:', open('/remount.txt').read())\n")
    circuitpython.serial.wait_for("read: writable")

    _repl(circuitpython, "storage.remount('/', readonly=True)\n")
    _repl(circuitpython, "try:\n")
    _repl(circuitpython, "    with open('/remount2.txt', 'w') as f:\n")
    _repl(circuitpython, "        f.write('nope')\n")
    _repl(circuitpython, "    print('unexpected: write succeeded')\n")
    _repl(circuitpython, "except OSError as e:\n")
    _repl(circuitpython, "    print('caught OSError:', e.errno)\n")
    _repl(circuitpython, "\n")
    circuitpython.serial.wait_for("caught OSError:")

    _repl(circuitpython, "import os\n")
    _repl(circuitpython, "print('blocked:', 'remount2.txt' in os.listdir('/'))\n")
    circuitpython.serial.wait_for("blocked: False")


@pytest.mark.parametrize("board", FILESYSTEM_SIMULATORS, indirect=True)
@pytest.mark.circuitpy_drive(None)
@pytest.mark.duration(120)
@pytest.mark.port_resets(8)
def test_filesystem_persists_across_reset(circuitpython):
    """Files written before microcontroller.reset() survive the reboot."""
    _enter_repl(circuitpython)

    _repl(circuitpython, "with open('/persist.txt', 'w') as f:\n")
    _repl(circuitpython, "    f.write('still here')\n")
    _repl(circuitpython, "\n")
    _repl(circuitpython, "import microcontroller\n")
    _repl(circuitpython, "microcontroller.reset()\n")

    assert circuitpython.reconnect_serial(timeout=30), "simulator did not reboot"

    _enter_repl(circuitpython)
    _repl(circuitpython, "print('persisted:', open('/persist.txt').read())\n")
    circuitpython.serial.wait_for("persisted: still here")
