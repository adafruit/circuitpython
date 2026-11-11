# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-FileCopyrightText: 2026 Adafruit Industries
# SPDX-License-Identifier: MIT

"""Choose the shared sdkconfig-flash-*.defaults file for a board.

ESP-IDF picks its partition table from a sdkconfig file shared by boards with
the same flash size and partition layout. The Makefile names the file for the
build and tools/update_sdkconfig.py names it again to write the file back, so
the rules live here instead of in both places.

An OTA slot has to fit the chip family's largest firmware image, so families
that can exceed 2MB use the 4MB-slot tables. There is a UF2 variant that also
matches tinyuf2, and a no-UF2 variant for boards without a USB device port.

This is also a script so the Makefile can query it:

    python3 tools/flash_layout.py --family esp32p4 --flash-size 16MB --uf2
"""

import argparse

SDKCONFIG_DIR = "esp-idf-config"

# The largest firmware image each chip family needs to fit. Families that are
# not listed use the default 2MB slots of the shared partition tables.
MAX_FIRMWARE_SIZE = {
    "esp32p4": "4MB",
    "esp32s31": "4MB",
}
DEFAULT_MAX_FIRMWARE_SIZE = "2MB"


def flash_layout(family):
    """Return the partition layout suffix, or "" for the default 2MB slots."""
    max_size = MAX_FIRMWARE_SIZE.get(family, DEFAULT_MAX_FIRMWARE_SIZE)
    if max_size == DEFAULT_MAX_FIRMWARE_SIZE:
        return ""
    return f"ota{max_size}"


def size_options(family, flash_size, uf2_bootloader, large_user_fs_layout=False):
    """Return the suffix after the flash size in a sdkconfig-flash name."""
    if flash_size == "2MB":
        # No 2MB board uses UF2, so there is never room for an OTA partition.
        return "-no-ota-no-uf2"
    if flash_size == "4MB":
        if uf2_bootloader:
            # S2 and S3 use a single OTA partition and do not do OTA updates.
            return "-no-ota"
        if large_user_fs_layout:
            # Some boards use a 2MB firmware and almost 2MB user filesystem,
            # for historical reasons.
            return "-no-ota-no-uf2-large-user_fs"
        return "-no-ota-no-uf2"
    # Everything with more than 4MB of flash.
    layout = flash_layout(family)
    if not uf2_bootloader:
        return f"-no-uf2-{layout}" if layout else "-no-uf2"
    if layout:
        return f"-{layout}"
    return ""


def sdkconfig_flash_filename(family, flash_size, uf2_bootloader, large_user_fs_layout=False):
    """Return e.g. esp-idf-config/sdkconfig-flash-16MB-ota4MB.defaults."""
    options = size_options(family, flash_size, uf2_bootloader, large_user_fs_layout)
    return f"{SDKCONFIG_DIR}/sdkconfig-flash-{flash_size}{options}.defaults"


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--family", required=True)
    parser.add_argument("--flash-size", required=True)
    parser.add_argument("--uf2", action="store_true", help="the board has a UF2 bootloader")
    parser.add_argument(
        "--large-user-fs",
        action="store_true",
        help="the board uses the large user filesystem layout",
    )
    args = parser.parse_args()
    print(
        sdkconfig_flash_filename(
            args.family,
            args.flash_size,
            args.uf2,
            args.large_user_fs,
        )
    )


if __name__ == "__main__":
    main()
