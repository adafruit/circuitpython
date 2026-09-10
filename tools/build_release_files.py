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

# On pull requests the translations other than en_US are built only to prove that they
# still fit in flash. The compiled code is identical for every translation; only three
# generated data files differ: the compressed strings, the compression dictionary and
# the terminal font. So instead of relinking the firmware for each translation, generate
# those files, count their bytes and predict the flash usage. Only translations predicted
# within LANGUAGE_MARGIN of the region limit, or whose build configuration differs, are
# really built. LANGUAGE_PREDICT=dryrun builds everything and prints the prediction
# error; LANGUAGE_PREDICT=off restores the old behaviour.
LANGUAGE_MARGIN = int(os.environ.get("LANGUAGE_MARGIN", 1024))
LANGUAGE_PREDICT = os.environ.get("LANGUAGE_PREDICT", "skip")

C_TYPE_SIZES = {
    "char": 1,
    "int8_t": 1,
    "uint8_t": 1,
    "int16_t": 2,
    "uint16_t": 2,
    "int32_t": 4,
    "uint32_t": 4,
}
C_ARRAY_RE = re.compile(r"const\s+(\w+)\s+\w+\[\d*\]\s*=\s*\{([^}]*)\}")
TRANSLATION_RE = re.compile(r"\.data = \d+, \.tail = \{([^}]*)\}")


def flash_usage(port, build_dir):
    """Return (used, region) bytes of the firmware flash region, or (None, None) if unknown."""
    try:
        with open(f"../ports/{port}/{build_dir}/firmware.size.json", "r") as f:
            firmware = json.load(f)
        return firmware["used_flash"], firmware["firmware_region"]
    except FileNotFoundError:
        return None, None


def c_array_bytes(path):
    """Sum the bytes of the const arrays initialised in a generated C file."""
    if not path.exists():
        return 0
    text = path.read_text()
    total = 0
    for match in C_ARRAY_RE.finditer(text):
        c_type, body = match.groups()
        if c_type not in C_TYPE_SIZES:
            typedef = re.search(r"typedef\s+(\w+)\s+" + c_type + ";", text)
            c_type = typedef.group(1) if typedef else "uint8_t"
        total += C_TYPE_SIZES[c_type] * len([x for x in body.split(",") if x.strip()])
    return total


def translation_bytes(port, build_dir, language):
    """Bytes of flash that depend on the translation: strings, dictionary and font."""
    build = pathlib.Path(f"../ports/{port}/{build_dir}")
    strings = 0
    for match in TRANSLATION_RE.finditer(
        (build / "py" / f"translations-{language}.c").read_text()
    ):
        strings += 1 + len([x for x in match.group(1).split(",") if x.strip()])
    dictionary = c_array_bytes(build / "genhdr" / "compressed_translations.generated.h")
    font = c_array_bytes(build / f"autogen_display_resources-{language}.c")
    return strings + dictionary + font


def generate_translation(port, board, build_dir, language):
    """Generate the translation data files of a language without compiling anything."""
    targets = [f"{build_dir}/py/translations-{language}.c"]
    if os.path.exists(f"../ports/{port}/{build_dir}/autogen_display_resources-{LANGUAGE_FIRST}.c"):
        targets.append(f"{build_dir}/autogen_display_resources-{language}.c")
    result = subprocess.run(
        "make -C ../ports/{port} TRANSLATION={language} BOARD={board} BUILD={build} -j {cores} {targets}".format(
            port=port,
            language=language,
            board=board,
            build=build_dir,
            cores=cores,
            targets=" ".join(targets),
        ),
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    if result.returncode != 0:
        print(result.stdout.decode("utf-8"))
    return result.returncode == 0


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

    # Set after the first language's build when its flash usage is known and too tight to
    # skip the other languages outright: the flash that build used, the flash the region
    # holds, and how many of the used bytes are translation data.
    baseline_flash = None
    flash_region = 0
    baseline_translation_bytes = 0

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

        predicted_flash = None
        if baseline_flash is not None and language != LANGUAGE_FIRST and not clean_build:
            if generate_translation(board_info["port"], board, build_dir, language):
                translation_growth = (
                    translation_bytes(board_info["port"], build_dir, language)
                    - baseline_translation_bytes
                )
                predicted_flash = baseline_flash + translation_growth
                fits = predicted_flash + LANGUAGE_MARGIN <= flash_region
                skip = fits and LANGUAGE_PREDICT == "skip"
                print(
                    "Predicted flash size for {board} {language}: {predicted} of {region} bytes"
                    " ({free} free, {growth:+d} vs {first}) -> {action}".format(
                        board=board,
                        language=language,
                        predicted=predicted_flash,
                        region=flash_region,
                        free=flash_region - predicted_flash,
                        growth=translation_growth,
                        first=LANGUAGE_FIRST,
                        action="skip" if skip else "build",
                    ),
                    flush=True,
                )
                if skip:
                    continue

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

        print(make_result.stdout.decode("utf-8"))
        print(other_output)

        if predicted_flash is not None and make_result.returncode == 0:
            actual_flash, _ = flash_usage(board_info["port"], build_dir)
            if actual_flash is not None:
                print(
                    "Flash size check {board} {language}: predicted {predicted},"
                    " actual {actual}, error {error:+d}".format(
                        board=board,
                        language=language,
                        predicted=predicted_flash,
                        actual=actual_flash,
                        error=predicted_flash - actual_flash,
                    )
                )

        # Flush so we will see something before 10 minutes has passed.
        print(flush=True)

        if (not build_all) and (language == LANGUAGE_FIRST) and (exit_status == 0):
            if extensions == ["exe"]:
                # The board builds a host executable, so there is no flash for a
                # translation to overflow and nothing for the other 16 to prove.
                print("Skipping languages")
                break
            used_flash, flash_region = flash_usage(board_info["port"], build_dir)
            if used_flash is None:
                print("Flash usage unknown, building all languages")
            elif used_flash + LANGUAGE_THRESHOLD < flash_region:
                print("Skipping languages")
                break
            elif LANGUAGE_PREDICT != "off" and board_info["port"] != "zephyr-cp":
                baseline_flash = used_flash
                baseline_translation_bytes = translation_bytes(
                    board_info["port"], build_dir, LANGUAGE_FIRST
                )

sys.exit(exit_status)
