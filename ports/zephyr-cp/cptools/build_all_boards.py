#!/usr/bin/env python3
"""
Build all CircuitPython boards for the Zephyr port.

This script discovers all boards by finding circuitpython.toml files
and builds each one sequentially, passing through the jobserver for
parallelism within each build.
"""

import argparse
import pathlib
import subprocess
import sys
import time


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


def build_board(port_dir, vendor, board, extra_args=None):
    """
    Build a single board using make.

    Args:
        port_dir: Path to the zephyr-cp port directory
        vendor: Board vendor name
        board: Board name
        extra_args: Additional arguments to pass to make

    Returns:
        (success: bool, elapsed_time: float)
    """
    board_id = f"{vendor}_{board}"
    start_time = time.time()

    cmd = ["make", f"BOARD={board_id}"]

    # Add extra arguments (like -j)
    if extra_args:
        cmd.extend(extra_args)

    try:
        subprocess.run(
            cmd,
            cwd=port_dir,
            check=True,
            # Inherit stdin to pass through jobserver file descriptors
            stdin=sys.stdin,
            # Show output in real-time
            stdout=None,
            stderr=None,
            capture_output=True,
        )
        elapsed = time.time() - start_time
        return True, elapsed
    except subprocess.CalledProcessError:
        elapsed = time.time() - start_time
        return False, elapsed
    except KeyboardInterrupt:
        raise


def main():
    parser = argparse.ArgumentParser(
        description="Build all CircuitPython boards for the Zephyr port",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Build all boards sequentially with 32 parallel jobs per board
  %(prog)s -j32

  # Build all boards using make's jobserver (recommended)
  make -j32 all
""",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=str,
        default=None,
        help="Number of parallel jobs for each board build (passed to make)",
    )
    parser.add_argument(
        "--continue-on-error",
        action="store_true",
        help="Continue building remaining boards even if one fails",
    )

    args = parser.parse_args()

    # Get the port directory
    port_dir = pathlib.Path(__file__).parent.resolve().parent

    # Discover all boards
    boards = discover_boards(port_dir)

    if not boards:
        print("ERROR: No boards found!")
        return 1

    # Prepare extra make arguments
    extra_args = []
    if args.jobs:
        extra_args.append(f"-j{args.jobs}")

    # Build all boards
    start_time = time.time()
    results = []

    try:
        for index, (vendor, board) in enumerate(boards):
            board_id = f"{vendor}_{board}"
            print(f"{index + 1}/{len(boards)} {board_id}: ", end="", flush=True)

            success, elapsed = build_board(port_dir, vendor, board, extra_args)
            if success:
                print(f"✅ SUCCESS ({elapsed:.1f}s)")
            else:
                print(f"❌ FAILURE ({elapsed:.1f}s)")
            results.append((vendor, board, success, elapsed))

            if not success and not args.continue_on_error:
                print("\nStopping due to build failure.")
                break
    except KeyboardInterrupt:
        print("\n\nBuild interrupted by user.")
        return 130  # Standard exit code for SIGINT

    # Print summary
    total_time = time.time() - start_time
    print(f"\n{'=' * 80}")
    print("Build Summary")
    print(f"{'=' * 80}")

    successful = [r for r in results if r[2]]
    failed = [r for r in results if not r[2]]

    print(f"\nTotal boards: {len(results)}/{len(boards)}")
    print(f"Successful: {len(successful)}")
    print(f"Failed: {len(failed)}")
    print(f"Total time: {total_time:.1f}s")

    return 0 if len(failed) == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
