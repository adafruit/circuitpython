#!/usr/bin/env python3
"""
run_serial_tests.py — Automated REPL-based tests for STM32F405 audioio.

Automates Tests 1–5 from README.md by:
  1. Copying WAV files and test scripts to the board via mpremote.
  2. Running each test on the device via the CircuitPython REPL.
  3. Comparing captured output to expected patterns and reporting PASS/FAIL.

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


# stderr substrings that indicate a transient host/USB issue worth retrying.
# Matched case-insensitively. CircuitPython USB-CDC briefly disappears during
# soft-reset and after some heavy DMA activity; mpremote's next invocation then
# either can't open the port or sees the kernel still holding the previous
# descriptor.
_RETRYABLE_STDERR = (
    "could not enter raw repl",
    "failed to access",
    "device not configured",
    "errno 6",
    "errno 16",
    "resource busy",
    "could not open",
    "no such file or directory",
    "serialexception",
    "device disconnected",
    "could not exclusively lock",
)


def _is_retryable(stderr: str) -> bool:
    s = stderr.lower()
    return any(needle in s for needle in _RETRYABLE_STDERR)


def _port_holders(port: str) -> list[str]:
    """Return a human-readable list of processes holding *port* (Unix only).

    macOS / Linux only — uses `lsof`. Returns lines like "Code Helper (51298)".
    Empty list when nothing holds the port or `lsof` is unavailable.
    """
    if not port.startswith("/"):
        return []
    holders: list[str] = []
    # Both /dev/cu.* (callout) and /dev/tty.* (dial-in) refer to the same UART
    # on macOS — VS Code typically opens /dev/tty.* while we ask for /dev/cu.*,
    # so check both.
    candidates = {port}
    if "/cu." in port:
        candidates.add(port.replace("/cu.", "/tty.", 1))
    elif "/tty." in port:
        candidates.add(port.replace("/tty.", "/cu.", 1))
    for path in candidates:
        if not os.path.exists(path):
            continue
        try:
            result = subprocess.run(
                ["lsof", "-Fcp", path],
                capture_output=True,
                text=True,
                timeout=5,
            )
        except (FileNotFoundError, subprocess.TimeoutExpired):
            return []
        pid = command = None
        for line in result.stdout.splitlines():
            if line.startswith("p"):
                pid = line[1:]
            elif line.startswith("c"):
                command = line[1:]
                if pid:
                    holders.append(f"{command} (PID {pid}) on {path}")
                    pid = command = None
    return holders


def _wait_for_port(port: str, timeout: float = 10.0) -> bool:
    """Block until *port* exists in the filesystem, or *timeout* elapses.

    macOS / Linux expose serial ports as /dev nodes; Windows uses COMx which is
    not a filesystem path, so on Windows we just sleep briefly and trust the
    next mpremote call to surface the real error.
    """
    if not port.startswith("/"):
        time.sleep(0.5)
        return True
    deadline = time.time() + timeout
    while time.time() < deadline:
        if os.path.exists(port):
            return True
        time.sleep(0.1)
    return False


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
        if (
            parts
            and parts[0].startswith("/dev/")
            and "Bluetooth" not in line
            and "wlan" not in line
        ):
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
        # Glob so a second CIRCUITPY volume (mounted as "CIRCUITPY 1") is found.
        import glob

        candidates = sorted(glob.glob("/Volumes/CIRCUITPY*"))
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
    files = [(os.path.join(AUDIOCORE_DIR, w), w) for w in WAV_FILES] + [
        (os.path.join(SCRIPT_DIR, s), s) for s in TEST_SCRIPTS
    ]
    # Check if device has enough space for test files
    if mount:
        needed = 0
        for src, dst in files:
            if not os.path.exists(src):
                continue
            needed += os.path.getsize(src)
            dest_path = os.path.join(mount, dst)
            if os.path.exists(dest_path):
                needed -= os.path.getsize(dest_path)
        free = shutil.disk_usage(mount).free
        if needed > free:
            short = needed - free
            print(f"ERROR: not enough space on {mount}.")
            print(f"  need:  {needed / 1024:.1f} KiB")
            print(f"  free:  {free / 1024:.1f} KiB")
            print(f"  short: {short / 1024:.1f} KiB")
            print(f"  Delete unrelated files (e.g. an old code.py or stale WAVs) and re-run.")
            sys.exit(1)
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
            stdout, stderr = _mpremote(["connect", port, "fs", "cp", src, f":/{dst}"], timeout=30)
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


def _run_exec(port: str, code: str, label: str, timeout: float, retries: int = 3):
    """Execute *code* on the device via mpremote exec and print the output.

    Retries cover three failure modes:
      * raw-REPL handshake blocked by a running code.py → Ctrl-C burst.
      * Persistent handshake failure → soft-reset + Ctrl-C flood.
      * Host-side serial drop ("device in use", "Errno 6", etc.) → wait for the
        port node to re-appear, then retry. CircuitPython's USB-CDC can briefly
        vanish after heavy DMA traffic or soft-reset.
    """
    print(f"\n{'=' * 60}")
    print(f"  {label}")
    print("=" * 60)
    stdout = ""
    stderr = ""
    for attempt in range(retries + 1):
        if not _wait_for_port(port, timeout=10.0):
            print(f"  [retry {attempt}/{retries}] port {port} not present — waited 10s")
            continue
        try:
            stdout, stderr = _mpremote(["connect", port, "exec", code], timeout=timeout)
        except TimeoutError as exc:
            print(f"  [FAIL] {exc}")
            return False, "", ""
        if not _is_retryable(stderr):
            break
        if attempt < retries:
            handshake = "could not enter raw repl" in stderr.lower()
            # Soft-reset only on persistent raw-REPL failures, not on host drops
            # (a soft-reset there just makes the disconnect window longer).
            escalate = handshake and attempt >= 1
            if handshake:
                tactic = "soft-reset + Ctrl-C flood" if escalate else "Ctrl-C burst"
            else:
                tactic = f"port-drop recovery ({stderr.strip().splitlines()[-1] if stderr.strip() else '?'})"
            print(f"  [retry {attempt + 1}/{retries}] {tactic}")
            _wait_for_port(port, timeout=10.0)
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


def _settle_between_tests(port: str) -> None:
    """Drain any lingering REPL output and wait for the port to be ready.

    CircuitPython occasionally re-enumerates its USB-CDC after a heavy test;
    next mpremote call then races the kernel re-binding the tty. Polling for
    the port node and then sending a Ctrl-C burst gives the host a clean
    starting state before the next exec.
    """
    _wait_for_port(port, timeout=10.0)
    _interrupt_running_code(port, soft_reset=False)
    time.sleep(0.3)


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
    for wav in sorted(WAV_FILES):
        passed &= _check(f"playing {wav}" in stdout, f"played {wav}")
    passed &= _check("OSError" not in stdout, "No OSError reported during playback")
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
        passed &= _check(
            f"playing with pause/resume: {wav}" in stdout, f"pause/resume header for {wav}"
        )
    passed &= _check("paused" in stdout, "At least one 'paused' line printed")
    passed &= _check("resumed" in stdout, "At least one 'resumed' line printed")
    passed &= _check("TIMEOUT" not in stdout, "No pause/resume hang timeout")
    passed &= _check("OSError" not in stdout, "No OSError reported during playback")
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
    passed &= _check(
        "playing stereo: jeplayer-splash-44100-16bit-stereo-signed.wav" in stdout,
        "44100 Hz 16-bit stereo WAV played",
    )
    passed &= _check(
        "playing stereo: jeplayer-splash-8000-16bit-stereo-signed.wav" in stdout,
        "8000 Hz 16-bit stereo WAV played",
    )
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

    # Surface the common "VS Code has the serial monitor open" pitfall up front
    # — otherwise every test fails with the opaque "in use by another program".
    holders = _port_holders(port)
    if holders:
        print("ERROR: port is held by another process:")
        for h in holders:
            print(f"  {h}")
        print(
            "Close the offending app (e.g. VS Code Serial Monitor, screen, tio)\n"
            "and re-run. mpremote needs exclusive access."
        )
        sys.exit(2)

    if not args.no_copy:
        copy_files(port, circuitpy=args.circuitpy)

    # Halt any running code.py so the first test gets a clean raw-REPL entry.
    _interrupt_running_code(port)

    test_runners = [
        ("1", "Test 1 — WAV Playback", test1_wavefile_playback),
        ("2", "Test 2 — Pause/Resume", test2_pause_resume),
        ("3", "Test 3 — Looping Sine", test3_single_buffer_loop),
        ("4", "Test 4 — deinit/Re-init", test4_deinit),
        ("5", "Test 5 — Stereo Playback", test5_stereo_playback),
    ]
    results: dict[str, bool] = {}
    first = True
    for key, name, runner in test_runners:
        if key not in selected:
            continue
        if not first:
            _settle_between_tests(port)
        first = False
        results[name] = runner(port)

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
        print("Remaining manual step: audio/oscilloscope verification.")
        sys.exit(0)
    else:
        print("One or more tests FAILED — see details above.")
        sys.exit(1)


if __name__ == "__main__":
    main()
