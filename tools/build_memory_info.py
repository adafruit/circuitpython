#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import re
import sys
import json

from elftools.elf.elffile import ELFFile


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

# The ELF says where each segment loads, so the firmware region's usage can exclude
# segments that load into other regions (nordic's FLASH_ISR, for example).
if "--elf" not in argv:
    sys.exit("usage: build_memory_info.py <map> <build dir> --elf <elf> [--region NAME,...]")
i = argv.index("--elf")
elf = argv[i + 1]
del argv[i : i + 2]


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


def flash_end_in_region(path, start, length):
    """Highest address a loadable ELF segment fills in the region, or None if none do.

    Measuring to the end, not summing segment sizes, counts alignment gaps between
    segments, which are as unusable as the segments themselves.
    """
    end = None
    with open(path, "rb") as f:
        for segment in ELFFile(f).iter_segments():
            if segment["p_type"] != "PT_LOAD" or segment["p_filesz"] == 0:
                continue
            if start <= segment["p_paddr"] < start + length:
                segment_end = segment["p_paddr"] + segment["p_filesz"]
                end = segment_end if end is None else max(end, segment_end)
    return end


def ram_used_in_region(path, start, length):
    """Bytes the loadable ELF segments occupy in the region once running."""
    used = 0
    with open(path, "rb") as f:
        for segment in ELFFile(f).iter_segments():
            if segment["p_type"] == "PT_LOAD" and start <= segment["p_vaddr"] < start + length:
                used += segment["p_memsz"]
    return used


# Not knowing the size is not worth failing a build over: the tools that read
# firmware.size.json fall back to assuming there is no headroom.
if firmware_region is None:
    print()
    print(
        "No {} region in {}. Regions found: {}.".format(
            " or ".join(flash_names), argv[0], ", ".join(sorted(regions)) or "none"
        )
    )
    print()
    sys.exit(0)

flash_end = flash_end_in_region(elf, flash_origin, firmware_region)
if flash_end is None:
    print()
    print(f"Nothing in {elf} loads into the {name} region.")
    print()
    sys.exit(0)
used_flash = flash_end - flash_origin
free_flash = firmware_region - used_flash

with open(f"{argv[1]}/firmware.size.json", "w") as f:
    json.dump({"used_flash": used_flash, "firmware_region": firmware_region}, f)

print()
print(
    "{} bytes used, {} bytes free in flash firmware space out of {} bytes ({}kB).".format(
        used_flash, free_flash, firmware_region, firmware_region / 1024
    )
)
if "RAM" in regions:
    _, ram_region = regions["RAM"]
    used_ram = ram_used_in_region(elf, *regions["RAM"])
    print(
        "{} bytes used, {} bytes free in ram for stack and heap out of {} bytes ({}kB).".format(
            used_ram, ram_region - used_ram, ram_region, ram_region / 1024
        )
    )
print()
