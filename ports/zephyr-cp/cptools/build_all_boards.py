#!/usr/bin/env python3
"""
Build all CircuitPython boards for the Zephyr port.

This script discovers all boards by finding circuitpython.toml files
and builds them in parallel while sharing a single jobserver across
all builds.

This is agent generated and works. Don't bother reading too closely because it
is just a tool for us.
"""

import argparse
import concurrent.futures
import os
import pathlib
import shlex
import subprocess
import sys
import time


class Jobserver:
    def __init__(self, read_fd, write_fd, jobs=None, owns_fds=False):
        self.read_fd = read_fd
        self.write_fd = write_fd
        self.jobs = jobs
        self.owns_fds = owns_fds

    def acquire(self):
        while True:
            try:
                os.read(self.read_fd, 1)
                return
            except InterruptedError:
                continue

    def release(self):
        while True:
            try:
                os.write(self.write_fd, b"+")
                return
            except InterruptedError:
                continue

    def pass_fds(self):
        return (self.read_fd, self.write_fd)

    def close(self):
        if self.owns_fds:
            os.close(self.read_fd)
            os.close(self.write_fd)


def _parse_makeflags_jobserver(makeflags):
    jobserver_auth = None
    jobs = None

    for token in shlex.split(makeflags):
        if token == "-j" or token == "--jobs":
            continue
        if token.startswith("-j") and token != "-j":
            try:
                jobs = int(token[2:])
            except ValueError:
                pass
        elif token.startswith("--jobs="):
            try:
                jobs = int(token.split("=", 1)[1])
            except ValueError:
                pass
        elif token.startswith("--jobserver-auth=") or token.startswith("--jobserver-fds="):
            jobserver_auth = token.split("=", 1)[1]

    if not jobserver_auth:
        return None, jobs, False

    if jobserver_auth.startswith("fifo:"):
        fifo_path = jobserver_auth[len("fifo:") :]
        read_fd = os.open(fifo_path, os.O_RDONLY)
        write_fd = os.open(fifo_path, os.O_WRONLY)
        os.set_inheritable(read_fd, True)
        os.set_inheritable(write_fd, True)
        return (read_fd, write_fd), jobs, True

    if "," in jobserver_auth:
        read_fd, write_fd = jobserver_auth.split(",", 1)
        return (int(read_fd), int(write_fd)), jobs, False

    return None, jobs, False


def _create_jobserver(jobs):
    read_fd, write_fd = os.pipe()
    os.set_inheritable(read_fd, True)
    os.set_inheritable(write_fd, True)
    for _ in range(jobs):
        os.write(write_fd, b"+")
    return Jobserver(read_fd, write_fd, jobs=jobs, owns_fds=True)


def _jobserver_from_env():
    makeflags = os.environ.get("MAKEFLAGS", "")
    fds, jobs, owns_fds = _parse_makeflags_jobserver(makeflags)
    if not fds:
        return None, jobs
    read_fd, write_fd = fds
    os.set_inheritable(read_fd, True)
    os.set_inheritable(write_fd, True)
    return Jobserver(read_fd, write_fd, jobs=jobs, owns_fds=owns_fds), jobs


def discover_boards(port_dir):
    """
    Discover all boards by finding circuitpython.toml files.

    Returns a list of (vendor, board) tuples.
    """
    boards = []
    boards_dir = port_dir / "boards"

    # Find all circuitpython.toml files
    for toml_file in boards_dir.glob("*/*/circuitpython.toml"):
        # Extract vendor and board from path: boards/vendor/board/circuitpython.toml
        parts = toml_file.relative_to(boards_dir).parts
        if len(parts) == 3:
            vendor = parts[0]
            board = parts[1]
            boards.append((vendor, board))

    return sorted(boards)


def build_board(
    port_dir,
    vendor,
    board,
    extra_args=None,
    jobserver=None,
    env=None,
    log_dir=None,
):
    """
    Build a single board using make.

    Args:
        port_dir: Path to the zephyr-cp port directory
        vendor: Board vendor name
        board: Board name
        extra_args: Additional arguments to pass to make
        jobserver: Jobserver instance to limit parallel builds
        env: Environment variables for the subprocess
        log_dir: Directory to write build logs

    Returns:
        (success: bool, elapsed_time: float, output: str, log_path: pathlib.Path)
    """
    board_id = f"{vendor}_{board}"
    start_time = time.time()
    log_path = None

    cmd = ["make", f"BOARD={board_id}"]

    # Add extra arguments (like -j)
    if extra_args:
        cmd.extend(extra_args)

    if jobserver:
        jobserver.acquire()

    try:
        result = subprocess.run(
            cmd,
            cwd=port_dir,
            # Inherit stdin alongside jobserver file descriptors
            stdin=sys.stdin,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            env=env,
            pass_fds=jobserver.pass_fds() if jobserver else (),
        )
        elapsed = time.time() - start_time
        output = result.stdout or ""
        if log_dir:
            log_path = log_dir / f"{board_id}.log"
            log_path.write_text(output)
        return result.returncode == 0, elapsed, output, log_path
    except KeyboardInterrupt:
        raise
    finally:
        if jobserver:
            jobserver.release()


def _format_status(status):
    state = status["state"]
    elapsed = status.get("elapsed")
    if state == "queued":
        return "QUEUED"
    if state == "running":
        return f"RUNNING {elapsed:.1f}s"
    if state == "success":
        return f"SUCCESS {elapsed:.1f}s"
    if state == "failed":
        return f"FAILED {elapsed:.1f}s"
    if state == "skipped":
        return "SKIPPED"
    return state.upper()


def _build_status_table(boards, statuses, start_time, stop_submitting):
    from rich.table import Table
    from rich.text import Text

    elapsed = time.time() - start_time
    title = f"Building {len(boards)} boards | Elapsed: {elapsed:.1f}s"
    if stop_submitting:
        title += " | STOPPING AFTER FAILURE"

    table = Table(title=title)
    table.add_column("#", justify="right")
    table.add_column("Board", no_wrap=True)
    table.add_column("Status", no_wrap=True)

    for i, (vendor, board) in enumerate(boards):
        board_id = f"{vendor}_{board}"
        status_text = _format_status(statuses[i])
        state = statuses[i]["state"]
        style = None
        if state == "success":
            style = "green"
        elif state == "failed":
            style = "red"
        table.add_row(
            f"{i + 1}/{len(boards)}",
            board_id,
            Text(status_text, style=style) if style else status_text,
        )

    return table


def _run_builds_tui(
    port_dir,
    boards,
    extra_args,
    jobserver,
    env,
    log_dir,
    max_workers,
    continue_on_error,
):
    from rich.live import Live

    statuses = [
        {"state": "queued", "elapsed": 0.0, "start": None, "log_path": None} for _ in boards
    ]
    results = []
    futures = {}
    next_index = 0
    stop_submitting = False
    start_time = time.time()

    with Live(
        _build_status_table(boards, statuses, start_time, stop_submitting),
        refresh_per_second=4,
        transient=False,
    ) as live:
        with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
            while next_index < len(boards) and len(futures) < max_workers:
                vendor, board = boards[next_index]
                statuses[next_index]["state"] = "running"
                statuses[next_index]["start"] = time.time()
                future = executor.submit(
                    build_board,
                    port_dir,
                    vendor,
                    board,
                    extra_args,
                    jobserver,
                    env,
                    log_dir,
                )
                futures[future] = next_index
                next_index += 1

            while futures:
                for status in statuses:
                    if status["state"] == "running":
                        status["elapsed"] = time.time() - status["start"]

                live.update(_build_status_table(boards, statuses, start_time, stop_submitting))

                done, _ = concurrent.futures.wait(
                    futures,
                    timeout=0.1,
                    return_when=concurrent.futures.FIRST_COMPLETED,
                )
                for future in done:
                    index = futures.pop(future)
                    vendor, board = boards[index]
                    success, elapsed, _output, log_path = future.result()
                    statuses[index]["elapsed"] = elapsed
                    statuses[index]["log_path"] = log_path
                    statuses[index]["state"] = "success" if success else "failed"
                    results.append((vendor, board, success, elapsed))

                    if not success and not continue_on_error:
                        stop_submitting = True

                    if not stop_submitting and next_index < len(boards):
                        vendor, board = boards[next_index]
                        statuses[next_index]["state"] = "running"
                        statuses[next_index]["start"] = time.time()
                        future = executor.submit(
                            build_board,
                            port_dir,
                            vendor,
                            board,
                            extra_args,
                            jobserver,
                            env,
                            log_dir,
                        )
                        futures[future] = next_index
                        next_index += 1

            if stop_submitting:
                for index in range(next_index, len(boards)):
                    if statuses[index]["state"] == "queued":
                        statuses[index]["state"] = "skipped"

            live.update(_build_status_table(boards, statuses, start_time, stop_submitting))

    return results, stop_submitting


def _run_builds_plain(
    port_dir,
    boards,
    extra_args,
    jobserver,
    env,
    log_dir,
    max_workers,
    continue_on_error,
):
    results = []
    futures = {}
    next_index = 0
    stop_submitting = False

    with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
        while next_index < len(boards) and len(futures) < max_workers:
            vendor, board = boards[next_index]
            future = executor.submit(
                build_board,
                port_dir,
                vendor,
                board,
                extra_args,
                jobserver,
                env,
                log_dir,
            )
            futures[future] = (vendor, board)
            next_index += 1

        while futures:
            done, _ = concurrent.futures.wait(
                futures,
                return_when=concurrent.futures.FIRST_COMPLETED,
            )
            for future in done:
                vendor, board = futures.pop(future)
                success, elapsed, _output, _log_path = future.result()
                board_id = f"{vendor}_{board}"
                status = "SUCCESS" if success else "FAILURE"
                print(f"{board_id}: {status} ({elapsed:.1f}s)")
                results.append((vendor, board, success, elapsed))

                if not success and not continue_on_error:
                    stop_submitting = True

                if not stop_submitting and next_index < len(boards):
                    vendor, board = boards[next_index]
                    future = executor.submit(
                        build_board,
                        port_dir,
                        vendor,
                        board,
                        extra_args,
                        jobserver,
                        env,
                        log_dir,
                    )
                    futures[future] = (vendor, board)
                    next_index += 1

    return results, stop_submitting


def main():
    parser = argparse.ArgumentParser(
        description="Build all CircuitPython boards for the Zephyr port",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Build all boards in parallel with 32 jobserver slots
  %(prog)s -j32

  # Build all boards using make's jobserver (recommended)
  make -j32 all
""",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=None,
        help="Number of shared jobserver slots across all board builds",
    )
    parser.add_argument(
        "--continue-on-error",
        action="store_true",
        help="Continue building remaining boards even if one fails",
    )
    parser.add_argument(
        "--vendor",
        type=str,
        default=None,
        help="Only build boards from this vendor (e.g. 'native' for sim boards)",
    )

    args = parser.parse_args()

    if args.jobs is not None and args.jobs < 1:
        print("ERROR: --jobs must be at least 1")
        return 2

    # Get the port directory
    port_dir = pathlib.Path(__file__).parent.resolve().parent

    # Discover all boards
    boards = discover_boards(port_dir)

    if args.vendor:
        boards = [(v, b) for v, b in boards if v == args.vendor]

    if not boards:
        print("ERROR: No boards found!")
        return 1

    # Prepare jobserver and extra make arguments
    jobserver, detected_jobs = _jobserver_from_env()
    env = os.environ.copy()

    extra_args = []
    jobserver_jobs = detected_jobs

    if not jobserver:
        jobserver_jobs = args.jobs if args.jobs else (os.cpu_count() or 1)
        jobserver = _create_jobserver(jobserver_jobs)
        env["MAKEFLAGS"] = (
            f"-j{jobserver_jobs} --jobserver-auth={jobserver.read_fd},{jobserver.write_fd}"
        )

    max_workers = jobserver_jobs
    if max_workers is None:
        max_workers = min(len(boards), os.cpu_count() or 1)
    max_workers = max(1, min(len(boards), max_workers))

    # Build all boards
    log_dir = port_dir / "build-logs"
    log_dir.mkdir(parents=True, exist_ok=True)

    try:
        use_tui = sys.stdout.isatty()
        if use_tui:
            try:
                import rich  # noqa: F401
            except ImportError:
                use_tui = False

        if use_tui:
            results, stop_submitting = _run_builds_tui(
                port_dir,
                boards,
                extra_args,
                jobserver,
                env,
                log_dir,
                max_workers,
                args.continue_on_error,
            )
        else:
            results, stop_submitting = _run_builds_plain(
                port_dir,
                boards,
                extra_args,
                jobserver,
                env,
                log_dir,
                max_workers,
                args.continue_on_error,
            )
    except KeyboardInterrupt:
        print("\n\nBuild interrupted by user.")
        return 130  # Standard exit code for SIGINT
    finally:
        if jobserver:
            jobserver.close()

    failed = [r for r in results if not r[2]]
    return 0 if len(failed) == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
