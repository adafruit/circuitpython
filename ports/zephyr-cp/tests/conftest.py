# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Pytest fixtures for CircuitPython native_sim testing."""

import logging
import os
import re
import select
import subprocess
import time
from dataclasses import dataclass
from pathlib import Path

import pytest
from perfetto.trace_processor import TraceProcessor

logger = logging.getLogger(__name__)

ZEPHYR_CP = Path(__file__).parent.parent
BUILD_DIR = ZEPHYR_CP / "build-native_native_sim"
BINARY = BUILD_DIR / "zephyr-cp/zephyr/zephyr.exe"


@dataclass
class InputTrigger:
    """A trigger for sending input to the simulator.

    Attributes:
        trigger: Text to match in output to trigger input, or None for immediate send.
        data: Bytes to send when triggered.
        sent: Whether this trigger has been sent (set internally).
    """

    trigger: str | None
    data: bytes
    sent: bool = False


@dataclass
class SimulatorResult:
    """Result from running CircuitPython on the simulator."""

    output: str
    trace_file: Path


def parse_gpio_trace(trace_file: Path, pin_name: str = "gpio_emul.00") -> list[tuple[int, int]]:
    """Parse GPIO trace from Perfetto trace file.

    Args:
        trace_file: Path to the Perfetto trace file.
        pin_name: Name of the GPIO pin track (e.g., "gpio_emul.00").

    Returns:
        List of (timestamp_ns, value) tuples for the specified GPIO pin.
    """
    tp = TraceProcessor(file_path=str(trace_file))
    result = tp.query(
        f'''
        SELECT c.ts, c.value
        FROM counter c
        JOIN track t ON c.track_id = t.id
        WHERE t.name = "{pin_name}"
        ORDER BY c.ts
        '''
    )
    return [(row.ts, int(row.value)) for row in result]


def _iter_uart_tx_slices(trace_file: Path) -> list[tuple[int, int, str, str]]:
    """Return UART TX slices as (timestamp_ns, duration_ns, text, device_name)."""
    tp = TraceProcessor(file_path=str(trace_file))
    result = tp.query(
        """
        SELECT s.ts, s.dur, s.name, dev.name AS device_name
        FROM slice s
        JOIN track tx ON s.track_id = tx.id
        JOIN track dev ON tx.parent_id = dev.id
        JOIN track uart ON dev.parent_id = uart.id
        WHERE tx.name = "TX" AND uart.name = "UART"
        ORDER BY s.ts
        """
    )
    return [
        (int(row.ts), int(row.dur or 0), row.name or "", row.device_name or "UART")
        for row in result
    ]


def log_uart_trace_output(trace_file: Path) -> None:
    """Log UART TX output from Perfetto trace with timestamps for line starts."""
    if not logger.isEnabledFor(logging.INFO):
        return
    slices = _iter_uart_tx_slices(trace_file)
    if not slices:
        return

    buffers: dict[str, list[str]] = {}
    line_start_ts: dict[str, int | None] = {}

    for ts, dur, text, device in slices:
        if device not in buffers:
            buffers[device] = []
            line_start_ts[device] = None

        if not text:
            continue

        char_step = dur / max(len(text), 1) if dur > 0 else 0.0
        for idx, ch in enumerate(text):
            if line_start_ts[device] is None:
                line_start_ts[device] = int(ts + idx * char_step)
            buffers[device].append(ch)
            if ch == "\n":
                line_text = "".join(buffers[device]).rstrip("\n")
                logger.info(
                    "UART trace %s @%d ns: %s",
                    device,
                    line_start_ts[device],
                    repr(line_text),
                )
                buffers[device] = []
                line_start_ts[device] = None

    for device, buf in buffers.items():
        if buf:
            logger.info(
                "UART trace %s @%d ns (partial): %s",
                device,
                line_start_ts[device] or 0,
                repr("".join(buf)),
            )


@pytest.fixture
def native_sim_binary():
    """Return path to native_sim binary, skip if not built."""
    if not BINARY.exists():
        pytest.skip(f"native_sim not built: {BINARY}")
    return BINARY


@pytest.fixture
def create_flash_image(tmp_path):
    """Factory fixture to create FAT flash images."""

    def _create(files: dict[str, str]) -> Path:
        flash = tmp_path / "flash.bin"

        # Create 2MB empty file
        flash.write_bytes(b"\x00" * (2 * 1024 * 1024))

        # Format as FAT (mformat)
        subprocess.run(["mformat", "-i", str(flash), "::"], check=True)

        # Copy files (mcopy)
        for name, content in files.items():
            src = tmp_path / name
            src.write_text(content)
            subprocess.run(["mcopy", "-i", str(flash), str(src), f"::{name}"], check=True)

        return flash

    return _create


@pytest.fixture
def run_circuitpython(native_sim_binary, create_flash_image, tmp_path):
    """Run CircuitPython with given code string and return output from PTY.

    Args:
        code: Python code to write to code.py, or None for no code.py.
        timeout: Timeout in seconds for the simulation.
        erase_flash: If True, erase flash before running.
        input_sequence: List of InputTrigger objects. When trigger text is seen
            in output, the corresponding data is written to the PTY. If trigger
            is None, the data is sent immediately when PTY is opened.
    """

    def _run(
        code: str | None,
        timeout: float = 5.0,
        erase_flash: bool = False,
        input_sequence: list[InputTrigger] | None = None,
        disabled_i2c_devices: list[str] | None = None,
    ) -> SimulatorResult:
        files = {"code.py": code} if code is not None else {}
        flash = create_flash_image(files)
        triggers = list(input_sequence) if input_sequence else []
        trace_file = tmp_path / "trace.perfetto"

        cmd = [
            str(native_sim_binary),
            f"--flash={flash}",
            "--flash_rm",
            "-no-rt",
            "-wait_uart",
            f"-stop_at={timeout}",
            f"--trace-file={trace_file}",
        ]
        if erase_flash:
            cmd.append("--flash_erase")
        if disabled_i2c_devices:
            for device in disabled_i2c_devices:
                cmd.append(f"--disable-i2c={device}")
        logger.info("Running: %s", " ".join(cmd))

        # Start the process
        proc = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )

        pty_path = None
        pty_fd = None
        output = []
        stdout_lines = []

        try:
            # Read stdout to find the PTY path
            start_time = time.time()
            while time.time() - start_time < timeout + 5:
                if proc.poll() is not None:
                    # Process exited
                    break

                # Check if stdout has data
                ready, _, _ = select.select([proc.stdout], [], [], 0.1)
                if ready:
                    line = proc.stdout.readline()
                    if not line:
                        break

                    stdout_lines.append(line.rstrip())

                    # Look for PTY path
                    match = re.search(r"uart connected to pseudotty: (/dev/pts/\d+)", line)
                    if match:
                        pty_path = match.group(1)
                        # Open the PTY for reading and writing
                        pty_fd = os.open(pty_path, os.O_RDWR | os.O_NONBLOCK)

                        # Send any immediate triggers (trigger=None)
                        for t in triggers:
                            if t.trigger is None and not t.sent:
                                os.write(pty_fd, t.data)
                                logger.info("PTY input (immediate): %r", t.data)
                                t.sent = True
                        break

            if pty_fd is None:
                raise RuntimeError("Failed to find PTY path in output")

            def check_triggers(accumulated_output: str) -> None:
                """Check accumulated output against triggers and send input."""
                for t in triggers:
                    if t.trigger is not None and not t.sent:
                        if t.trigger in accumulated_output:
                            os.write(pty_fd, t.data)
                            logger.info("PTY input (trigger %r): %r", t.trigger, t.data)
                            t.sent = True

            # Read from PTY until process exits or timeout
            while time.time() - start_time < timeout + 1:
                if proc.poll() is not None:
                    # Process exited, do one final read
                    try:
                        ready, _, _ = select.select([pty_fd], [], [], 0.1)
                        if ready:
                            data = os.read(pty_fd, 4096)
                            if data:
                                output.append(data.decode("utf-8", errors="replace"))
                    except (OSError, BlockingIOError):
                        pass
                    break

                # Check if PTY has data
                try:
                    ready, _, _ = select.select([pty_fd], [], [], 0.1)
                    if ready:
                        data = os.read(pty_fd, 4096)
                        if data:
                            output.append(data.decode("utf-8", errors="replace"))
                            check_triggers("".join(output))
                except (OSError, BlockingIOError):
                    pass

            # Read any remaining stdout
            remaining_stdout = proc.stdout.read()
            if remaining_stdout:
                stdout_lines.extend(remaining_stdout.rstrip().split("\n"))

            # Log stdout
            for line in stdout_lines:
                logger.info("stdout: %s", line)

            pty_output = "".join(output)
            for line in pty_output.split("\n"):
                logger.info("PTY output: %s", repr(line.strip()))
            log_uart_trace_output(trace_file)
            return SimulatorResult(output=pty_output, trace_file=trace_file)

        finally:
            if pty_fd is not None:
                os.close(pty_fd)
            proc.terminate()
            proc.wait(timeout=1)

    return _run
