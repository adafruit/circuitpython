#!/usr/bin/env python3
"""Render a iobroker package TOML as the C pin map translation unit.

Run at build time by the module's CMakeLists.txt for the package selected via
CONFIG_IOBROKER_PACKAGE_*; the output lands in the build directory, so
nothing generated is committed. The declarations live in iobroker.h.

Usage:
  gen_package_c.py packages/nrf54l15_qfn48.toml package_pins.c
"""

import sys
import tomllib
from pathlib import Path


def main():
    if len(sys.argv) != 3:
        raise SystemExit(__doc__)
    toml_path, out_path = Path(sys.argv[1]), Path(sys.argv[2])
    with toml_path.open("rb") as f:
        data = tomllib.load(f)

    lines = [
        f"// Generated from {toml_path.name} by tools/gen_package_c.py -- do not edit.",
        f"// Package pin map '{data['name']}' for {', '.join(data['socs'])}.",
        "",
        "#include <stddef.h>",
        "#include <stdint.h>",
        "",
        "#include <iobroker/iobroker.h>",
        "",
        "const iobroker_package_pin_t iobroker_package_pins[] = {",
    ]
    for entry in data["pins"]:
        if "pad_name" not in entry and "pad" not in entry:
            # Informational module pins (e.g. a castellation bonded to GND):
            # they carry no SoC pad, so they map nothing.
            continue
        # The SoC pad is the pad id the package numbers its pins by (row
        # -major over the ball grid, or the datasheet's pin number); module
        # packages remap it per entry and SoC packages default to the pin.
        pad = entry.get("pad", entry["pin"])
        # The pad's GPIO attachment, or IOBROKER_PAD_NO_GPIO for analog-only
        # pads.
        gpio = entry.get("gpio", 0xFFFF)
        ball = f" ({entry['ball']})" if "ball" in entry else ""
        pad_name = f" {entry['pad_name']}" if "pad_name" in entry else ""
        comment = f"  //{ball}{pad_name}"
        lines.append(f"    {{ PACKAGE_PIN({entry['pin']}), {pad}, {gpio} }},{comment}")
    lines += [
        "};",
        f"const size_t iobroker_package_pin_count = {len(data['pins'])};",
        "",
    ]
    out_path.write_text("\n".join(lines))


if __name__ == "__main__":
    main()
