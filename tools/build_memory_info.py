#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import os
import re
import sys
import json


# Handle size constants with K or M suffixes (allowed in .ld but not in Python).
K_PATTERN = re.compile(r"([0-9]+)[kK]")
K_REPLACE = r"(\1*1024)"

M_PATTERN = re.compile(r"([0-9]+)[mM]")
M_REPLACE = r"(\1*1024*1024)"

# A linker map lists every region with the lengths already resolved:
#     Name             Origin             Length             Attributes
#     FLASH_FIRMWARE   0x10000000         0x0017f000         xr
MAP_REGION = re.compile(r"^(\w+)\s+0x[0-9a-f]+\s+0x([0-9a-f]+)\s+\S*$", re.MULTILINE)

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
    """Region sizes from the Memory Configuration table of a linker map."""
    start = contents.find("Memory Configuration")
    if start < 0:
        return None
    end = contents.find("Linker script and memory map", start)
    table = contents[start : end if end > 0 else len(contents)]
    regions = {}
    for name, length in MAP_REGION.findall(table):
        if name not in ("Name", "Origin", "Length"):
            regions[name] = int(length, 16)
    return regions or None


def regions_from_linker_script(contents):
    """Region sizes from a linker script, for the ports that pass one."""
    regions = {}
    for line in contents.split("\n"):
        line = line.strip()
        if line.startswith(("FLASH_FIRMWARE", "RAM")):
            regions[line.split()[0]] = line.split("=")[-1]
    for region, space in regions.items():
        if "/*" in space:
            space = space.split("/*")[0]
        space = K_PATTERN.sub(K_REPLACE, space)
        space = M_PATTERN.sub(M_REPLACE, space)
        regions[region] = int(eval(space))
    return regions


# This file is either a linker map or the linker script.
try:
    with open(argv[0], "r") as f:
        contents = f.read()
except FileNotFoundError:
    print()
    print(f"No {argv[0]} to read the flash region from.")
    print()
    sys.exit(0)
regions = regions_from_map(contents) or regions_from_linker_script(contents)

firmware_region = None
for name in flash_names:
    if name in regions:
        firmware_region = regions[name]
        break

if image is not None:
    try:
        text = os.stat(image).st_size
    except FileNotFoundError:
        print()
        print(f"No {image} to measure.")
        print()
        sys.exit(0)

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
    ram_region = regions["RAM"]
    print(
        "{} bytes used, {} bytes free in ram for stack and heap out of {} bytes ({}kB).".format(
            used_ram, ram_region - used_ram, ram_region, ram_region / 1024
        )
    )
print()
