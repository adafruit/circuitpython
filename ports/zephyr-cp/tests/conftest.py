# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Pytest fixtures for CircuitPython native_sim testing."""

import logging
import os
import subprocess
from pathlib import Path

import pytest
import serial

from .perfetto_input_trace import write_input_trace

from perfetto.trace_processor import TraceProcessor

from . import NativeSimProcess

logger = logging.getLogger(__name__)


def pytest_addoption(parser):
    parser.addoption(
        "--update-goldens",
        action="store_true",
        default=False,
        help="Overwrite golden images with captured output instead of comparing.",
    )


def pytest_configure(config):
    config.addinivalue_line(
        "markers", "circuitpy_drive(files): run CircuitPython with files in the flash image"
    )
    config.addinivalue_line(
        "markers", "disable_i2c_devices(*names): disable native_sim I2C emulator devices"
    )
    config.addinivalue_line(
        "markers", "circuitpython_board(board_id): which board id to use in the test"
    )
    config.addinivalue_line(
        "markers",
        "zephyr_sample(sample, board='nrf52_bsim', device_id=1): build and run a Zephyr sample for bsim tests",
    )
    config.addinivalue_line(
        "markers",
        "duration(seconds): native_sim timeout and bsim PHY simulation duration",
    )
    config.addinivalue_line(
        "markers",
        "code_py_runs(count): stop native_sim after count code.py runs (default: 1)",
    )
    config.addinivalue_line(
        "markers",
        "input_trace(trace): inject input signal trace data into native_sim",
    )
    config.addinivalue_line(
        "markers",
        "native_sim_rt: run native_sim in realtime mode (-rt instead of -no-rt)",
    )
    config.addinivalue_line(
        "markers",
        "display(capture_times_ns=None): run test with SDL display; "
        "capture_times_ns is a list of nanosecond timestamps for trace-triggered captures",
    )
    config.addinivalue_line(
        "markers",
        "display_pixel_format(format): override the display pixel format "
        "(e.g. 'RGB_565', 'ARGB_8888')",
    )
    config.addinivalue_line(
        "markers",
        "display_mono_vtiled(value): override the mono vtiled screen_info flag (True or False)",
    )
    config.addinivalue_line(
        "markers",
        "flash_config(erase_block_size=N, total_size=N): override flash simulator parameters",
    )


ZEPHYR_CP = Path(__file__).parent.parent
BUILD_DIR = ZEPHYR_CP / "build-native_native_sim"
BINARY = BUILD_DIR / "zephyr-cp/zephyr/zephyr.exe"


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
def board(request):
    board = request.node.get_closest_marker("circuitpython_board")
    if board is not None:
        board = board.args[0]
    else:
        board = "native_native_sim"
    return board


@pytest.fixture
def native_sim_binary(request, board):
    """Return path to native_sim binary, skip if not built."""
    build_dir = ZEPHYR_CP / f"build-{board}"
    binary = build_dir / "zephyr-cp/zephyr/zephyr.exe"

    if not binary.exists():
        pytest.skip(f"binary not built: {binary}")
    return binary


@pytest.fixture
def native_sim_env() -> dict[str, str]:
    return {}


PIXEL_FORMAT_BITMASK = {
    "RGB_888": 1 << 0,
    "MONO01": 1 << 1,
    "MONO10": 1 << 2,
    "ARGB_8888": 1 << 3,
    "RGB_565": 1 << 4,
    "BGR_565": 1 << 5,
    "L_8": 1 << 6,
    "AL_88": 1 << 7,
}


@pytest.fixture
def pixel_format(request) -> str:
    """Indirect-parametrize fixture: adds display_pixel_format marker."""
    fmt = request.param
    request.node.add_marker(pytest.mark.display_pixel_format(fmt))
    return fmt


@pytest.fixture
def sim_id(request) -> str:
    return request.node.nodeid.replace("/", "_")


@pytest.fixture
def circuitpython(request, board, sim_id, native_sim_binary, native_sim_env, tmp_path):
    """Run CircuitPython with given code string and return PTY output."""

    instance_count = 1
    if "circuitpython1" in request.fixturenames and "circuitpython2" in request.fixturenames:
        instance_count = 2

    drives = list(request.node.iter_markers_with_node("circuitpy_drive"))
    if len(drives) != instance_count:
        raise RuntimeError(f"not enough drives for {instance_count} instances")

    input_trace_markers = list(request.node.iter_markers_with_node("input_trace"))
    if len(input_trace_markers) > 1:
        raise RuntimeError("expected at most one input_trace marker")

    input_trace = None
    if input_trace_markers and len(input_trace_markers[0][1].args) == 1:
        input_trace = input_trace_markers[0][1].args[0]

    input_trace_file = None
    if input_trace is not None:
        input_trace_file = tmp_path / "input.perfetto"
        write_input_trace(input_trace_file, input_trace)

    marker = request.node.get_closest_marker("duration")
    if marker is None:
        timeout = 10
    else:
        timeout = marker.args[0]

    runs_marker = request.node.get_closest_marker("code_py_runs")
    if runs_marker is None:
        code_py_runs = 1
    else:
        code_py_runs = int(runs_marker.args[0])

    display_marker = request.node.get_closest_marker("display")
    if display_marker is None:
        display_marker = request.node.get_closest_marker("display_capture")

    capture_times_ns = None
    if display_marker is not None:
        capture_times_ns = display_marker.kwargs.get("capture_times_ns", None)

    pixel_format_marker = request.node.get_closest_marker("display_pixel_format")
    pixel_format = None
    if pixel_format_marker is not None and pixel_format_marker.args:
        pixel_format = pixel_format_marker.args[0]

    mono_vtiled_marker = request.node.get_closest_marker("display_mono_vtiled")
    mono_vtiled = None
    if mono_vtiled_marker is not None and mono_vtiled_marker.args:
        mono_vtiled = mono_vtiled_marker.args[0]

    # If capture_times_ns is set, merge display_capture track into input trace.
    if capture_times_ns is not None:
        if input_trace is None:
            input_trace = {}
        else:
            input_trace = dict(input_trace)
        input_trace["display_capture"] = list(capture_times_ns)
        if input_trace_file is None:
            input_trace_file = tmp_path / "input.perfetto"
        write_input_trace(input_trace_file, input_trace)

    use_realtime = request.node.get_closest_marker("native_sim_rt") is not None

    flash_config_marker = request.node.get_closest_marker("flash_config")
    flash_total_size = 2 * 1024 * 1024  # default 2MB
    flash_erase_block_size = None
    flash_write_block_size = None
    if flash_config_marker:
        flash_total_size = flash_config_marker.kwargs.get("total_size", flash_total_size)
        flash_erase_block_size = flash_config_marker.kwargs.get("erase_block_size", None)
        flash_write_block_size = flash_config_marker.kwargs.get("write_block_size", None)

    procs = []
    for i in range(instance_count):
        flash = tmp_path / f"flash-{i}.bin"
        flash.write_bytes(b"\xff" * flash_total_size)
        files = None
        if len(drives[i][1].args) == 1:
            files = drives[i][1].args[0]
        if files is not None:
            subprocess.run(["mformat", "-i", str(flash), "::"], check=True)
            tmp_drive = tmp_path / f"drive{i}"
            tmp_drive.mkdir(exist_ok=True)

            for name, content in files.items():
                src = tmp_drive / name
                if isinstance(content, bytes):
                    src.write_bytes(content)
                else:
                    src.write_text(content)
                subprocess.run(["mcopy", "-i", str(flash), str(src), f"::{name}"], check=True)

        trace_file = tmp_path / f"trace-{i}.perfetto"

        if "bsim" in board:
            cmd = [str(native_sim_binary), f"--flash_app={flash}"]
            if instance_count > 1:
                cmd.append("-disconnect_on_exit=1")
            cmd.extend(
                (
                    f"-s={sim_id}",
                    f"-d={i}",
                    "-uart0_pty",
                    "-uart0_pty_wait_for_readers",
                    "-uart_pty_wait",
                    f"--vm-runs={code_py_runs + 1}",
                )
            )
        else:
            cmd = [str(native_sim_binary), f"--flash={flash}"]
            # native_sim vm-runs includes the boot VM setup run.
            realtime_flag = "-rt" if use_realtime else "-no-rt"
            cmd.extend(
                (
                    realtime_flag,
                    "-display_headless",
                    "-i2s_earless",
                    "-wait_uart",
                    f"--vm-runs={code_py_runs + 1}",
                )
            )

        if flash_erase_block_size is not None:
            cmd.append(f"--flash_erase_block_size={flash_erase_block_size}")
        if flash_write_block_size is not None:
            cmd.append(f"--flash_write_block_size={flash_write_block_size}")
        if flash_config_marker and "total_size" in flash_config_marker.kwargs:
            cmd.append(f"--flash_total_size={flash_total_size}")

        if input_trace_file is not None:
            cmd.append(f"--input-trace={input_trace_file}")

        marker = request.node.get_closest_marker("disable_i2c_devices")
        if marker and len(marker.args) > 0:
            for device in marker.args:
                cmd.append(f"--disable-i2c={device}")

        if pixel_format is not None:
            cmd.append(f"--display_pixel_format={PIXEL_FORMAT_BITMASK[pixel_format]}")

        if mono_vtiled is not None:
            cmd.append(f"--display_mono_vtiled={'true' if mono_vtiled else 'false'}")

        env = os.environ.copy()
        env.update(native_sim_env)

        capture_png_pattern = None
        if capture_times_ns is not None:
            if instance_count == 1:
                capture_png_pattern = str(tmp_path / "frame_%d.png")
            else:
                capture_png_pattern = str(tmp_path / f"frame-{i}_%d.png")
            cmd.append(f"--display_capture_png={capture_png_pattern}")

        logger.info("Running: %s", " ".join(cmd))
        proc = NativeSimProcess(cmd, timeout, trace_file, env, flash_file=flash)
        proc.display_dump = None
        proc._capture_png_pattern = capture_png_pattern
        proc._capture_count = len(capture_times_ns) if capture_times_ns is not None else 0
        procs.append(proc)
    if instance_count == 1:
        yield procs[0]
    else:
        yield procs
    for i, proc in enumerate(procs):
        if instance_count > 1:
            print(f"---------- Instance {i} -----------")
        proc.shutdown()

        print("All serial output:")
        print(proc.serial.all_output)
        print()
        print("All debug serial output:")
        print(proc.debug_serial.all_output)
