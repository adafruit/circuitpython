#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import os
import re
import sys
import json


# A linker map lists every region with the lengths already resolved:
#     Name             Origin             Length             Attributes
#     FLASH_FIRMWARE   0x10000000         0x0017f000         xr
MAP_REGION = re.compile(r"^(\w+)\s+0x([0-9a-f]+)\s+0x([0-9a-f]+)\s+\S*$", re.MULTILINE)

argv = sys.argv[1:]
flash_names = ["FLASH_FIRMWARE", "FLASH"]
if "--region" in argv:
    i = argv.index("--region")
    flash_names = argv[i + 1].split(",")
    del argv[i : i + 2]

# Ports whose toolchain this makefile cannot reach pass the image instead of piping
# size(1) in; what it occupies in flash is its size on disk.
image = None
if "--image" in argv:
    i = argv.index("--image")
    image = argv[i + 1]
    del argv[i : i + 2]

text = 0
data = 0
bss = 0

if image is None:
    # stdin is the linker output.
    for line in sys.stdin:
        # Uncomment to see linker output.
        # print(line)
        line = line.strip()
        if not line.startswith("text"):
            text, data, bss = map(int, line.split()[:3])


def regions_from_map(contents):
    """Region origins and sizes from the Memory Configuration table of a linker map."""
    start = contents.find("Memory Configuration")
    if start < 0:
        return None
    end = contents.find("Linker script and memory map", start)
    table = contents[start : end if end > 0 else len(contents)]
    regions = {}
    for name, origin, length in MAP_REGION.findall(table):
        if name not in ("Name", "Origin", "Length"):
            regions[name] = (int(origin, 16), int(length, 16))
    return regions or None


# The linker map lists the regions with every size the script computed resolved.
try:
    with open(argv[0], "r") as f:
        contents = f.read()
except FileNotFoundError:
    print()
    print(f"No {argv[0]} to read the flash region from.")
    print()
    sys.exit(0)
regions = regions_from_map(contents) or {}

firmware_region = None
flash_origin = 0
for name in flash_names:
    if name in regions:
        flash_origin, firmware_region = regions[name]
        break


def hex_bytes_in(path, start, length):
    """Bytes an Intel HEX file puts inside a region, or None if it can't be read."""
    total = 0
    base = 0
    try:
        with open(path, "r") as f:
            for line in f:
                if not line.startswith(":"):
                    continue
                count = int(line[1:3], 16)
                record = int(line[7:9], 16)
                if record == 0:
                    address = base + int(line[3:7], 16)
                    if start <= address < start + length:
                        total += count
                elif record == 4:
                    base = int(line[9:13], 16) << 16
                elif record == 2:
                    base = int(line[9:13], 16) << 4
    except (OSError, ValueError):
        return None
    return total


if image is not None:
    try:
        text = os.stat(image).st_size
    except FileNotFoundError:
        print()
        print(f"No {image} to measure.")
        print()
        sys.exit(0)
    if firmware_region is not None and text > firmware_region:
        # objcopy spans the whole image, so a board with a region far above the
        # firmware one (Renesas keeps its option bytes 16 MB up) gets a sparse file
        # whose size is the span, not the usage. The hex has the addresses.
        in_region = hex_bytes_in(
            os.path.splitext(image)[0] + ".hex", flash_origin, firmware_region
        )
        if in_region is None:
            print()
            print(f"{image} is larger than the firmware region and there is no hex to measure.")
            print()
            sys.exit(0)
        text = in_region

used_flash = data + text
used_ram = data + bss

if firmware_region is None:
    # Not knowing the size is not worth failing a build over: the tools that read
    # firmware.size.json fall back to assuming there is no headroom.
    print()
    print(
        "No {} region in {}. Regions found: {}.".format(
            " or ".join(flash_names), argv[0], ", ".join(sorted(regions)) or "none"
        )
    )
    print("{} bytes used in flash firmware space.".format(used_flash))
    print()
    sys.exit(0)

free_flash = firmware_region - used_flash

with open(f"{argv[1]}/firmware.size.json", "w") as f:
    json.dump({"used_flash": used_flash, "firmware_region": firmware_region}, f)

print()
print(
    "{} bytes used, {} bytes free in flash firmware space out of {} bytes ({}kB).".format(
        used_flash, free_flash, firmware_region, firmware_region / 1024
    )
)
if image is None and "RAM" in regions:
    _, ram_region = regions["RAM"]
    print(
        "{} bytes used, {} bytes free in ram for stack and heap out of {} bytes ({}kB).".format(
            used_ram, ram_region - used_ram, ram_region, ram_region / 1024
        )
    )
print()
