#!/usr/bin/env python3
"""
run_serial_tests.py — Automated REPL-based tests for STM32F405 audioio.

Automates Tests 1, 2, 3, and 4 from README.md by:
  1. Copying WAV files and test scripts to the board via mpremote.
  2. Running each test on the device via the CircuitPython REPL.
  3. Comparing captured output to expected patterns and reporting PASS/FAIL.

Test 5 (soft-reset cleanup) still requires manual interaction.

Usage:
    python3 run_serial_tests.py
    python3 run_serial_tests.py --port /dev/cu.usbmodemXXX
    python3 run_serial_tests.py --circuitpy /Volumes/CIRCUITPY   # macOS
    python3 run_serial_tests.py --circuitpy /media/user/CIRCUITPY  # Linux
    python3 run_serial_tests.py --circuitpy D:\\                    # Windows
    python3 run_serial_tests.py --no-copy --tests 3,4

Requirements:
    pip install mpremote
"""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
import time

# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../../.."))
AUDIOCORE_DIR = os.path.join(REPO_ROOT, "tests", "circuitpython-manual", "audiocore")

WAV_FILES = [
    "jeplayer-splash-8000-8bit-mono-unsigned.wav",
    "jeplayer-splash-8000-16bit-mono-signed.wav",
    "jeplayer-splash-44100-16bit-mono-signed.wav",
    "jeplayer-splash-8000-16bit-stereo-signed.wav",
    "jeplayer-splash-44100-16bit-stereo-signed.wav",
]

TEST_SCRIPTS = [
    "wavefile_playback.py",
    "wavefile_pause_resume.py",
    "single_buffer_loop.py",
    "stereo_playback.py",
]

DEINIT_TEST_CODE = (
    "import audioio, analogio, board\n"
    "dac = audioio.AudioOut(board.A0)\n"
    "dac.deinit()\n"
    "aout = analogio.AnalogOut(board.A0)\n"
    "aout.value = 32768\n"
    "aout.deinit()\n"
    "dac2 = audioio.AudioOut(board.A0)\n"
    "dac2.deinit()\n"
    'print("pass")\n'
)

# ---------------------------------------------------------------------------
# mpremote helpers
# ---------------------------------------------------------------------------

def _mpremote(args: list, timeout: float = 30.0):
    """Run an mpremote command, return (stdout, stderr). Raises on timeout."""
    try:
        result = subprocess.run(
            ["mpremote"] + args,
            capture_output=True,
            text=True,
            timeout=timeout,
        )
        return result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        raise TimeoutError(f"mpremote timed out after {timeout}s")
    except FileNotFoundError:
        sys.exit("mpremote not found. Run: pip install mpremote")


def _interrupt_running_code(port: str, soft_reset: bool = False) -> None:
    """Halt any code running on the device so mpremote can enter raw REPL.

    Strategy:
      1. Ctrl-C burst — usually breaks a busy print loop.
      2. Optional Ctrl-D soft reset, followed by a Ctrl-C flood through the
         reboot window so code.py is interrupted *before* it gets busy again.
    """
    try:
        import serial  # type: ignore[import-not-found]
    except ImportError:
        return
    try:
        with serial.Serial(port, 115200, timeout=0.1) as ser:
            for _ in range(5):
                ser.write(b"\x03")
                ser.flush()
                time.sleep(0.05)
            if soft_reset:
                ser.write(b"\x04")  # Ctrl-D → soft reboot
                ser.flush()
                # Flood Ctrl-C while CircuitPython reboots so code.py can't
                # get past its first iteration before we break in.
                deadline = time.time() + 3.0
                while time.time() < deadline:
                    ser.write(b"\x03")
                    ser.flush()
                    time.sleep(0.05)
            time.sleep(0.3)
            ser.reset_input_buffer()
    except (serial.SerialException, OSError):
        pass


def find_port() -> str:
    """Return the port of the first Adafruit device found by mpremote devs."""
    stdout, _ = _mpremote(["devs"])
    for line in stdout.splitlines():
        parts = line.split()
        if not parts:
            continue
        # mpremote devs output: <port> <serial> <vid:pid> <manufacturer> <product>
        # Filter for Adafruit VID (239a)
        if any("239a" in p for p in parts):
            return parts[0]
    # Fall back to any USB serial port that isn't Bluetooth/wlan
    for line in stdout.splitlines():
        parts = line.split()
        if parts and parts[0].startswith("/dev/") and "Bluetooth" not in line and "wlan" not in line:
            return parts[0]
    raise RuntimeError(
        "No board detected. Connect the board and/or pass --port.\n"
        f"mpremote devs output:\n{stdout}"
    )


def find_circuitpy() -> str | None:
    """Return the path to the mounted CIRCUITPY volume, or None if not found."""
    import platform
    system = platform.system()

    if system == "Darwin":
        candidates = ["/Volumes/CIRCUITPY"]
    elif system == "Windows":
        # Scan all drive letters for a CIRCUITPY volume label.
        import string
        import ctypes
        candidates = []
        kernel32 = ctypes.windll.kernel32  # type: ignore[attr-defined]
        buf = ctypes.create_unicode_buffer(256)
        for letter in string.ascii_uppercase:
            root = f"{letter}:\\"
            if kernel32.GetVolumeInformationW(root, buf, 256, None, None, None, None, 0):
                if buf.value == "CIRCUITPY":
                    candidates.append(root)
    else:
        # Linux: common udev/udisks mount points
        candidates = ["/media/CIRCUITPY", "/run/media/CIRCUITPY"]
        try:
            import pwd
            user = pwd.getpwuid(os.getuid()).pw_name
            candidates.insert(0, f"/run/media/{user}/CIRCUITPY")
            candidates.insert(0, f"/media/{user}/CIRCUITPY")
        except Exception:
            pass

    for path in candidates:
        if os.path.isdir(path):
            return path
    return None


def copy_files(port: str, circuitpy: str | None = None):
    """Copy WAV samples and test scripts to the board."""
    mount = circuitpy or find_circuitpy()
    if mount:
        print(f"Copying files to board via {mount} ...")
    else:
        print("Copying files to board via mpremote ...")
    files = (
        [(os.path.join(AUDIOCORE_DIR, w), w) for w in WAV_FILES]
        + [(os.path.join(SCRIPT_DIR, s), s) for s in TEST_SCRIPTS]
    )
    missing = []
    for src, dst in files:
        if not os.path.exists(src):
            missing.append(src)
            continue
        if mount:
            dest_path = os.path.join(mount, dst)
            shutil.copy2(src, dest_path)
            print(f"  {dst} (copied)")
        else:
            stdout, stderr = _mpremote(
                ["connect", port, "fs", "cp", src, f":/{dst}"], timeout=30
            )
            if stderr and "Error" in stderr:
                print(f"  {dst} (FAILED: {stderr.strip()})")
            else:
                status = "up to date" if "Up to date" in stdout else "copied"
                print(f"  {dst} ({status})")
    if missing:
        print("WARNING: source files not found:")
        for m in missing:
            print(f"  {m}")
    print()


# ---------------------------------------------------------------------------
# Test runner
# ---------------------------------------------------------------------------

PASS_TAG = "PASS"
FAIL_TAG = "FAIL"


def _check(condition: bool, message: str) -> bool:
    print(f"  [{PASS_TAG if condition else FAIL_TAG}] {message}")
    return condition


def _run_exec(port: str, code: str, label: str, timeout: float, retries: int = 2):
    """Execute *code* on the device via mpremote exec and print the output.

    Retries on `could not enter raw repl` after sending a Ctrl-C burst — this
    handles the case where a busy code.py blocks mpremote's first handshake.
    """
    print(f"\n{'=' * 60}")
    print(f"  {label}")
    print("=" * 60)
    stdout = ""
    stderr = ""
    for attempt in range(retries + 1):
        try:
            stdout, stderr = _mpremote(
                ["connect", port, "exec", code], timeout=timeout
            )
        except TimeoutError as exc:
            print(f"  [FAIL] {exc}")
            return False, "", ""
        if "could not enter raw repl" not in stderr:
            break
        if attempt < retries:
            # Escalate: first attempt = Ctrl-C burst; second = Ctrl-D reboot.
            escalate = attempt >= 1
            tactic = "soft-reset + Ctrl-C flood" if escalate else "Ctrl-C burst"
            print(f"  [retry {attempt + 1}/{retries}] raw REPL busy — {tactic}")
            _interrupt_running_code(port, soft_reset=escalate)
            time.sleep(0.5)
    print("Output:")
    for line in stdout.splitlines():
        print(f"    {line}")
    if stderr:
        print("Stderr:")
        for line in stderr.splitlines():
            print(f"    {line}")
    return True, stdout, stderr


# ---------------------------------------------------------------------------
# Individual tests
# ---------------------------------------------------------------------------

def test1_wavefile_playback(port: str) -> bool:
    code = 'exec(open("/wavefile_playback.py").read())'
    ok, stdout, stderr = _run_exec(
        port, code, "Test 1 — WAV File Playback (wavefile_playback.py)", timeout=180
    )
    if not ok:
        return False
    passed = True
    passed &= _check("playing jeplayer-splash-44100-16bit-mono-signed.wav" in stdout, "44100 Hz 16-bit mono WAV played")
    passed &= _check("playing jeplayer-splash-8000-16bit-mono-signed.wav" in stdout, "8000 Hz 16-bit mono WAV played")
    passed &= _check("playing jeplayer-splash-8000-8bit-mono-unsigned.wav" in stdout, "8000 Hz 8-bit unsigned WAV played")
    passed &= _check("done" in stdout, "Script completed with 'done'")
    passed &= _check(not stderr, f"No exceptions (stderr={stderr!r})")
    return passed


def test2_pause_resume(port: str) -> bool:
    code = 'exec(open("/wavefile_pause_resume.py").read())'
    ok, stdout, stderr = _run_exec(
        port, code, "Test 2 — Pause / Resume (wavefile_pause_resume.py)", timeout=180
    )
    if not ok:
        return False
    passed = True
    for wav in sorted(WAV_FILES):
        passed &= _check(f"playing with pause/resume: {wav}" in stdout, f"pause/resume header for {wav}")
    passed &= _check("paused" in stdout, "At least one 'paused' line printed")
    passed &= _check("resumed" in stdout, "At least one 'resumed' line printed")
    passed &= _check("done" in stdout, "Script completed with 'done'")
    passed &= _check(not stderr, f"No exceptions (stderr={stderr!r})")
    return passed


def test3_single_buffer_loop(port: str) -> bool:
    code = 'exec(open("/single_buffer_loop.py").read())'
    ok, stdout, stderr = _run_exec(
        port, code, "Test 3 — Looping Sine Wave (single_buffer_loop.py)", timeout=30
    )
    if not ok:
        return False
    passed = True
    for label in ("unsigned 8 bit", "signed 8 bit", "unsigned 16 bit", "signed 16 bit"):
        passed &= _check(label in stdout, f"'{label}' label printed")
    passed &= _check("done" in stdout, "Script completed with 'done'")
    passed &= _check(not stderr, f"No exceptions (stderr={stderr!r})")
    return passed


def test5_stereo_playback(port: str) -> bool:
    code = 'exec(open("/stereo_playback.py").read())'
    ok, stdout, stderr = _run_exec(
        port, code, "Test 5 — Stereo Playback (stereo_playback.py)", timeout=180
    )
    if not ok:
        return False
    passed = True
    passed &= _check("channel test: left only" in stdout, "Left-only channel tone played")
    passed &= _check("channel test: right only" in stdout, "Right-only channel tone played")
    passed &= _check("channel test: both channels" in stdout, "Both-channel tone played")
    passed &= _check("pan sweep: left to right" in stdout, "Pan sweep played")
    passed &= _check("playing stereo: jeplayer-splash-44100-16bit-stereo-signed.wav" in stdout, "44100 Hz 16-bit stereo WAV played")
    passed &= _check("playing stereo: jeplayer-splash-8000-16bit-stereo-signed.wav" in stdout, "8000 Hz 16-bit stereo WAV played")
    passed &= _check("done" in stdout, "Script completed with 'done'")
    passed &= _check(not stderr, f"No exceptions (stderr={stderr!r})")
    return passed


def test4_deinit(port: str) -> bool:
    ok, stdout, stderr = _run_exec(
        port, DEINIT_TEST_CODE, "Test 4 — deinit and Re-init (inline)", timeout=10
    )
    if not ok:
        return False
    passed = True
    passed &= _check("pass" in stdout, "Script printed 'pass'")
    passed &= _check(not stderr, f"No exceptions (stderr={stderr!r})")
    return passed


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("--port", help="Serial port (auto-detected if omitted)")
    parser.add_argument(
        "--circuitpy",
        metavar="PATH",
        help="Path to mounted CIRCUITPY volume (auto-detected if omitted)",
    )
    parser.add_argument(
        "--no-copy",
        action="store_true",
        help="Skip copying files to the board (assume they are already present)",
    )
    parser.add_argument(
        "--tests",
        default="1,2,3,4,5",
        help="Comma-separated list of test numbers to run (default: 1,2,3,4,5)",
    )
    args = parser.parse_args()

    selected = set(args.tests.split(","))

    port = args.port or find_port()
    print(f"Using port: {port}\n")

    if not args.no_copy:
        copy_files(port, circuitpy=args.circuitpy)

    # Halt any running code.py so the first test gets a clean raw-REPL entry.
    _interrupt_running_code(port)

    results: dict[str, bool] = {}
    if "1" in selected:
        results["Test 1 — WAV Playback"] = test1_wavefile_playback(port)
    if "2" in selected:
        results["Test 2 — Pause/Resume"] = test2_pause_resume(port)
    if "3" in selected:
        results["Test 3 — Looping Sine"] = test3_single_buffer_loop(port)
    if "4" in selected:
        results["Test 4 — deinit/Re-init"] = test4_deinit(port)
    if "5" in selected:
        results["Test 5 — Stereo Playback"] = test5_stereo_playback(port)

    print(f"\n{'=' * 60}")
    print("SUMMARY")
    print("=" * 60)
    all_passed = True
    for name, passed in results.items():
        print(f"  [{'PASS' if passed else 'FAIL'}] {name}")
        all_passed = all_passed and passed

    print()
    if all_passed:
        print("All automated tests passed.")
        print("Remaining manual step: Test 6 (soft-reset) and audio/oscilloscope verification.")
        sys.exit(0)
    else:
        print("One or more tests FAILED — see details above.")
        sys.exit(1)


if __name__ == "__main__":
    main()
