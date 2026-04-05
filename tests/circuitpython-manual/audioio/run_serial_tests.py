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
    python3 run_serial_tests.py --no-copy --tests 3,4

Requirements:
    pip install mpremote
"""

import argparse
import os
import subprocess
import sys

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
]

TEST_SCRIPTS = [
    "wavefile_playback.py",
    "wavefile_pause_resume.py",
    "single_buffer_loop.py",
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


def copy_files(port: str):
    """Copy WAV samples and test scripts to the board."""
    print("Copying files to board ...")
    files = (
        [(os.path.join(AUDIOCORE_DIR, w), w) for w in WAV_FILES]
        + [(os.path.join(SCRIPT_DIR, s), s) for s in TEST_SCRIPTS]
    )
    missing = []
    for src, dst in files:
        if not os.path.exists(src):
            missing.append(src)
            continue
        stdout, stderr = _mpremote(
            ["connect", port, "fs", "cp", src, f":{dst}"], timeout=30
        )
        # mpremote prints "Up to date: <file>" if unchanged, "cp ..." otherwise
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


def _run_exec(port: str, code: str, label: str, timeout: float):
    """Execute *code* on the device via mpremote exec and print the output."""
    print(f"\n{'=' * 60}")
    print(f"  {label}")
    print("=" * 60)
    try:
        stdout, stderr = _mpremote(["connect", port, "exec", code], timeout=timeout)
    except TimeoutError as exc:
        print(f"  [FAIL] {exc}")
        return False, "", ""
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
    code = 'import os; os.chdir("/")\nexec(open("wavefile_playback.py").read())'
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
    code = 'exec(open("wavefile_pause_resume.py").read())'
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
    code = 'exec(open("single_buffer_loop.py").read())'
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
        "--no-copy",
        action="store_true",
        help="Skip copying files to the board (assume they are already present)",
    )
    parser.add_argument(
        "--tests",
        default="1,2,3,4",
        help="Comma-separated list of test numbers to run (default: 1,2,3,4)",
    )
    args = parser.parse_args()

    selected = set(args.tests.split(","))

    port = args.port or find_port()
    print(f"Using port: {port}\n")

    if not args.no_copy:
        copy_files(port)

    results: dict[str, bool] = {}
    if "1" in selected:
        results["Test 1 — WAV Playback"] = test1_wavefile_playback(port)
    if "2" in selected:
        results["Test 2 — Pause/Resume"] = test2_pause_resume(port)
    if "3" in selected:
        results["Test 3 — Looping Sine"] = test3_single_buffer_loop(port)
    if "4" in selected:
        results["Test 4 — deinit/Re-init"] = test4_deinit(port)

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
        print("Remaining manual step: Test 5 (soft-reset) and audio/oscilloscope verification.")
        sys.exit(0)
    else:
        print("One or more tests FAILED — see details above.")
        sys.exit(1)


if __name__ == "__main__":
    main()
