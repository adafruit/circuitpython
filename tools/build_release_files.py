#! /usr/bin/env python3

# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import os
import multiprocessing
import re
import sys
import subprocess
import shutil
import build_board_info as build_info
import pathlib
import time
import json
import tomllib

sys.path.append("../docs")
from shared_bindings_matrix import get_settings_from_makefile

TOP = pathlib.Path(__file__).resolve().parent.parent

for port in build_info.SUPPORTED_PORTS:
    result = subprocess.run("rm -rf ../ports/{port}/build*".format(port=port), shell=True)

all_boards = build_info.get_board_mapping()
build_boards = list(all_boards.keys())
if "BOARDS" in os.environ:
    build_boards = os.environ["BOARDS"].split()

sha, version = build_info.get_version_info()

build_all = os.environ.get("GITHUB_EVENT_NAME") != "pull_request"

LANGUAGE_FIRST = "en_US"
LANGUAGE_THRESHOLD = 10 * 1024

# Ports without tools/build_memory_info.py print the GNU ld memory usage table.
LD_MEMORY_RE = re.compile(
    r"^\s*(?:FLASH_FIRMWARE|FLASH):\s+(\d+) ([KM]?B)\s+(\d+) ([KM]?B)", re.MULTILINE
)
UNITS = {"B": 1, "KB": 1024, "MB": 1024 * 1024}


def flash_usage(port, build_dir, make_output):
    """Return (used, region) bytes of the firmware flash region, or None if unknown."""
    try:
        with open(f"../ports/{port}/{build_dir}/firmware.size.json", "r") as f:
            firmware = json.load(f)
            return firmware["used_flash"], firmware["firmware_region"]
    except FileNotFoundError:
        pass
    match = LD_MEMORY_RE.search(make_output)
    if match:
        used = int(match.group(1)) * UNITS[match.group(2)]
        region = int(match.group(3)) * UNITS[match.group(4)]
        return used, region
    return None


languages = build_info.get_languages()

all_languages = build_info.get_languages(list_all=True)

print("Note: Not building languages", set(all_languages) - set(languages))

exit_status = 0
cores = multiprocessing.cpu_count()
print("building boards with parallelism {}".format(cores))
for board in build_boards:
    bin_directory = "../bin/{}/".format(board)
    os.makedirs(bin_directory, exist_ok=True)
    board_info = all_boards[board]
    if board_info["port"] == "zephyr-cp":
        # Split the vendor portion out of the board name.
        next_underscore = board.find("_")
        while next_underscore != -1:
            vendor = board[:next_underscore]
            target = board[next_underscore + 1 :]
            cp_toml = TOP / f"ports/zephyr-cp/boards/{vendor}/{target}/circuitpython.toml"
            if cp_toml.exists():
                break
            next_underscore = board.find("_", next_underscore + 1)
        board_settings = {"CLEAN_REBUILD_LANGUAGES": []}
        with cp_toml.open("rb") as f:
            board_settings.update(tomllib.load(f))
    else:
        board_settings = get_settings_from_makefile("../ports/" + board_info["port"], board)
        board_settings["CIRCUITPY_BUILD_EXTENSIONS"] = [
            extension.strip()
            for extension in board_settings["CIRCUITPY_BUILD_EXTENSIONS"].split(",")
        ]

    languages.remove(LANGUAGE_FIRST)
    languages.insert(0, LANGUAGE_FIRST)

    for language in languages:
        bin_directory = "../bin/{board}/{language}".format(board=board, language=language)
        os.makedirs(bin_directory, exist_ok=True)
        start_time = time.monotonic()

        if "CLEAN_REBUILD_LANGUAGES" in board_settings:
            clean_build = language in board_settings["CLEAN_REBUILD_LANGUAGES"]
        else:
            # Normally different language builds are all done based on the same set of compiled sources.
            # But sometimes a particular language needs to be built from scratch, if, for instance,
            # CFLAGS_INLINE_LIMIT is set for a particular language to make it fit.
            clean_build_check_result = subprocess.run(
                "make -C ../ports/{port} TRANSLATION={language} BOARD={board} check-release-needs-clean-build -j {cores} | fgrep 'RELEASE_NEEDS_CLEAN_BUILD = 1'".format(
                    port=board_info["port"], language=language, board=board, cores=cores
                ),
                shell=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
            )
            clean_build = clean_build_check_result.returncode == 0

        build_dir = "build-{board}".format(board=board)
        if clean_build:
            build_dir += "-{language}".format(language=language)

        extensions = board_settings["CIRCUITPY_BUILD_EXTENSIONS"]

        artifacts = [os.path.join(build_dir, "firmware." + extension) for extension in extensions]
        make_result = subprocess.run(
            "make -C ../ports/{port} TRANSLATION={language} BOARD={board} BUILD={build} -j {cores} {artifacts}".format(
                port=board_info["port"],
                language=language,
                board=board,
                build=build_dir,
                cores=cores,
                artifacts=" ".join(artifacts),
            ),
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )

        build_duration = time.monotonic() - start_time
        success = "\033[32msucceeded\033[0m"
        if make_result.returncode != 0:
            exit_status = make_result.returncode
            success = "\033[31mfailed\033[0m"

        other_output = ""

        for extension in extensions:
            temp_filename = "../ports/{port}/{build}/firmware.{extension}".format(
                port=board_info["port"], build=build_dir, extension=extension
            )
            for alias in board_info["aliases"] + [board]:
                bin_directory = "../bin/{alias}/{language}".format(alias=alias, language=language)
                os.makedirs(bin_directory, exist_ok=True)
                final_filename = (
                    "adafruit-circuitpython-{alias}-{language}-{version}.{extension}".format(
                        alias=alias, language=language, version=version, extension=extension
                    )
                )
                final_filename = os.path.join(bin_directory, final_filename)
                try:
                    shutil.copyfile(temp_filename, final_filename)
                except FileNotFoundError:
                    other_output = "Cannot find file {}".format(temp_filename)
                    if exit_status == 0:
                        exit_status = 1

        print(
            "Build {board} for {language}{clean_build} took {build_duration:.2f}s and {success}".format(
                board=board,
                language=language,
                clean_build=(" (clean_build)" if clean_build else ""),
                build_duration=build_duration,
                success=success,
            )
        )

        make_output = make_result.stdout.decode("utf-8")
        print(make_output)
        print(other_output)

        # Flush so we will see something before 10 minutes has passed.
        print(flush=True)

        if (not build_all) and (language == LANGUAGE_FIRST) and (exit_status == 0):
            usage = flash_usage(board_info["port"], build_dir, make_output)
            if usage is None:
                print("Flash usage unknown, building all languages")
            elif usage[0] + LANGUAGE_THRESHOLD < usage[1]:
                print("Skipping languages")
                break

sys.exit(exit_status)
