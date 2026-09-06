#!/usr/bin/env python3
"""Check the flash partition layout a board's devicetree actually resolved to.

Board overlays in ``boards/`` routinely rebuild the ``partitions`` node so the
board gets a CIRCUITPY filesystem. Rebuilding it is easy to get subtly wrong in
a way nothing reports: drop the ``ranges;`` the board DTS declared and
devicetree stops translating partition addresses into the SoC's address space,
so every partition resolves to a bare offset instead. The build still succeeds.
What changes is anything keyed off the address -- on RP2040,
``RP2_REQUIRES_SECOND_STAGE_BOOT`` matches the code partition against
0x10000100, so a partition left at 0x100 silently turns it off and the UF2 is
built for the wrong address with no second stage bootloader linked in.

A board that CircuitPython also builds from another port (``counterpart`` in its
``circuitpython.toml``) is additionally compared with that build: nvm and the
CIRCUITPY drive must sit exactly where ``ports/<port>`` puts them, or switching
firmware between the two builds loses the user's data. The expected placement
is derived from the counterpart's configuration files the same way its
``mpconfigport.h`` derives it.

This reads the ``edt.pickle`` a build already produced, so it costs no build
time -- point it at build directories after building, or run it with no
arguments to check every build directory in the port.

    python cptools/check_partitions.py                       # every build-* dir
    python cptools/check_partitions.py build-raspberrypi_rpi_pico_zephyr
    python cptools/check_partitions.py --board raspberrypi_rpi_pico_zephyr build-x

Exits non-zero when a layout has problems.
"""

import argparse
import pathlib
import pickle
import re
import sys
import tomllib

import board_tools

PORT_DIR = pathlib.Path(__file__).resolve().parent.parent
TOP = PORT_DIR.parent.parent
EDT_MODULE = PORT_DIR / "zephyr" / "scripts" / "dts" / "python-devicetree" / "src"

# Parity with the non-Zephyr build. A board that CircuitPython also builds from another
# port (ports/raspberrypi, ports/nordic) names it in circuitpython.toml:
#
#     counterpart = "raspberrypi/raspberry_pi_pico_w"
#
# The nvm and CIRCUITPY drive of the Zephyr build then have to sit exactly where that
# build puts them, so that switching firmware between the two never loses user data.
# The expected placement is derived from the counterpart's own configuration files, the
# same way its mpconfigport.h derives it, rather than copied into the overlay by hand.
DEFINE_RE = re.compile(r"^\s*#\s*define\s+([A-Za-z_]\w*)\s+(.+?)\s*(?://.*|/\*.*)?$", re.MULTILINE)
CFLAG_DEFINE_RE = re.compile(r"-D([A-Za-z_]\w*)=(?:'([^']*)'|\"([^\"]*)\"|(\S+))")
MK_ASSIGN_RE = re.compile(r"^\s*([A-Za-z_]\w*)\s*[?:+]?=\s*(.*?)\s*$", re.MULTILINE)
IDENTIFIER_RE = re.compile(r"(?<!\w)[A-Za-z_]\w*")
SAFE_EXPR_RE = re.compile(r"^[\s0-9a-fA-FxX()*+\-/<>]+$")


def load_edt(build_dir):
    """Load the pickled devicetree a build produced, or None if there is none."""
    edt_path = build_dir / "zephyr-cp" / "zephyr" / "edt.pickle"
    if not edt_path.is_file():
        edt_path = build_dir / "zephyr" / "edt.pickle"
    if not edt_path.is_file():
        return None
    sys.path.insert(0, str(EDT_MODULE))
    with open(edt_path, "rb") as f:
        return pickle.load(f)


def partition_device(node):
    """Walk up to the NVM device owning a partition.

    The device is the first ancestor carrying ``reg``; the ``partitions``
    grouping node in between has none.
    """
    parent = getattr(node, "parent", None)
    while parent is not None:
        if parent.props.get("reg") is not None:
            return parent
        parent = getattr(parent, "parent", None)
    return None


def device_size(node):
    """Total size of an NVM device.

    External SPI/QSPI NOR carries its capacity in the ``size`` property (in
    bits) and uses ``reg`` for the chip select, so reading ``reg`` alone gives
    0 for exactly the devices CIRCUITPY usually lives on.
    """
    size = node.props.get("size")
    if size:
        return size.val // 8
    reg = node.props.get("reg")
    if reg and len(reg.val) >= 2:
        return reg.val[1]
    return 0


def is_partition_child(node):
    """True for any node under a ``partitions`` grouping node.

    Membership is positional rather than by ``compatible``: an overlay may add
    a partition carrying neither ``zephyr,mapped-partition`` nor a
    ``fixed-partitions`` parent, and it still occupies the space and still is
    what the layout means to describe.
    """
    parent = getattr(node, "parent", None)
    return parent is not None and getattr(parent, "name", "") == "partitions"


def iter_partitions(edt):
    """Yield ``(node, device, offset, size, mapped)`` for every partition."""
    for node in edt.nodes:
        mapped = "zephyr,mapped-partition" in getattr(node, "compats", [])
        if not mapped and not is_partition_child(node):
            continue
        reg = node.props.get("reg")
        if not reg or len(reg.val) < 2:
            continue
        dev = partition_device(node)
        if dev is None:
            continue
        if mapped and (not getattr(node, "regs", None) or not getattr(dev, "regs", None)):
            continue
        yield node, dev, reg.val[0], reg.val[1], mapped


def check_layout(edt):
    """Return a list of problems with the resolved layout.

    Mapped partitions are checked for address translation; every partition is
    checked for overlap and for running past the end of its device. Erase-page
    alignment is not checked: RP2040 deliberately puts its code partition at
    0x100, directly behind the 256-byte second stage bootloader.
    """
    problems = []
    by_device = {}
    for node, dev, offset, size, mapped in iter_partitions(edt):
        label = node.labels[0] if node.labels else node.name
        dev_label = dev.labels[0] if dev.labels else dev.name
        total = device_size(dev)
        if mapped:
            # Test the resolved address against the device's own window rather
            # than against base + reg. Both forms are in use: a partition reg
            # is usually an offset, but some overlays write the absolute
            # address and leave the partitions node without ranges, which
            # resolves to the same correct address. What is never right is a
            # partition resolving outside the device it lives in -- which is
            # exactly what a rebuilt partitions node missing ranges; produces,
            # since the offset is then left untranslated.
            base = dev.regs[0].addr
            actual = node.regs[0].addr
            if total and not (base <= actual < base + total):
                problems.append(
                    f"{label}: resolves to 0x{actual:x}, outside {dev_label} "
                    f"(0x{base:x}-0x{base + total:x}) -- address translation is "
                    f"broken; does the partitions node declare ranges;?"
                )
            # Geometry below is compared in offsets from the device base, so a
            # partition declared either way lands in the same space. When the
            # translation is broken the difference is meaningless (and often
            # negative), so keep the declared offset rather than running the
            # geometry checks on nonsense.
            translated = actual - base
            if 0 <= translated and (not total or translated < total):
                offset = translated
        by_device.setdefault(dev_label, (dev, total, []))[2].append((label, offset, size))

    for dev_label, (dev, total, parts) in sorted(by_device.items()):
        ordered = sorted(parts, key=lambda p: p[1])
        for label, offset, size in ordered:
            if total and offset + size > total:
                problems.append(
                    f"{label}: ends at 0x{offset + size:x}, past the end of "
                    f"{dev_label} (0x{total:x})"
                )
        # A running high-water mark, not neighbouring pairs: a partition
        # spanning several later ones only overlaps its immediate successor in
        # a pairwise walk.
        high_label, high_end = None, 0
        for label, offset, size in ordered:
            if offset < high_end:
                problems.append(
                    f"{high_label} (ends 0x{high_end:x}) overlaps "
                    f"{label} (starts 0x{offset:x}) on {dev_label}"
                )
            if offset + size > high_end:
                high_label, high_end = label, offset + size
    return problems


def read_defines(paths):
    """Collect ``#define``s, ``-D`` flags and make assignments from configuration files.

    The first definition of a name wins, and files earlier in ``paths`` take
    precedence, so list the board's files before the port's defaults.
    """
    defines = {}
    for path in paths:
        if not path.is_file():
            continue
        text = path.read_text()
        if path.suffix == ".mk":
            for match in CFLAG_DEFINE_RE.finditer(text):
                value = next(v for v in match.groups()[1:] if v is not None)
                defines.setdefault(match.group(1), value)
            for match in MK_ASSIGN_RE.finditer(text):
                defines.setdefault(match.group(1), match.group(2))
        else:
            for match in DEFINE_RE.finditer(text):
                defines.setdefault(match.group(1), match.group(2))
    return defines


def evaluate(defines, name, default=None):
    """Integer value of a define, following references to other defines.

    Returns ``default`` when the name is undefined and None when it cannot be
    evaluated, so that a layout is never declared correct on a guess.
    """
    expr = defines.get(name)
    if expr is None:
        return default
    for ident in sorted(set(IDENTIFIER_RE.findall(expr)), key=len, reverse=True):
        value = evaluate(defines, ident)
        if value is None:
            return None
        expr = re.sub(rf"(?<!\w){ident}(?!\w)", str(value), expr)
    if not SAFE_EXPR_RE.match(expr):
        return None
    return int(eval(expr, {"__builtins__": {}}, {}))


def reference_layout(counterpart, flash_size):
    """Where the non-Zephyr build of ``counterpart`` keeps nvm and the CIRCUITPY drive.

    ``counterpart`` is ``<port>/<board>`` under ``ports/`` and ``flash_size`` the
    size of the internal flash. Returns ``{"nvm": (offset, size), "circuitpy":
    (offset, size)}``; ``"circuitpy"`` is the string ``"external"`` when that
    build uses a whole external flash chip as the drive. Raises ValueError when
    the port's rules are unknown or a value cannot be derived.
    """
    port, _, board = counterpart.partition("/")
    port_dir = TOP / "ports" / port
    board_dir = port_dir / "boards" / board
    if not board_dir.is_dir():
        raise ValueError(f"{counterpart}: no such board under ports/")
    defines = read_defines(
        [
            board_dir / "mpconfigboard.mk",
            board_dir / "mpconfigboard.h",
            port_dir / "mpconfigport.mk",
            port_dir / "mpconfigport.h",
        ]
    )

    def value(name, default=None):
        result = evaluate(defines, name, default)
        if result is None:
            raise ValueError(f"{counterpart}: cannot evaluate {name}")
        return result

    if port == "raspberrypi":
        # ports/raspberrypi/mpconfigport.h: nvm directly follows the firmware region,
        # the drive follows nvm (and the saves partition on boards that have one).
        firmware = value("CIRCUITPY_FIRMWARE_SIZE")
        nvm_size = value("CIRCUITPY_INTERNAL_NVM_SIZE")
        drive = firmware + nvm_size + value("CIRCUITPY_SAVES_PARTITION_SIZE", 0)
        return {"nvm": (firmware, nvm_size), "circuitpy": (drive, flash_size - drive)}
    if port == "nordic":
        # ports/nordic/mpconfigport.h: the bootloader sits at the top of flash, an
        # internal filesystem (when there is one) directly below it and nvm below that.
        bootloader = (
            flash_size
            - value("BOOTLOADER_SIZE")
            - value("BOOTLOADER_SETTINGS_SIZE")
            - value("BOOTLOADER_MBR_SIZE")
        )
        external = any(
            defines.get(flag, "0").strip() == "1"
            for flag in ("QSPI_FLASH_FILESYSTEM", "SPI_FLASH_FILESYSTEM")
        )
        fs_size = 0 if external else value("CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE")
        fs_start = bootloader - fs_size
        nvm_size = value("CIRCUITPY_INTERNAL_NVM_SIZE")
        return {
            "nvm": (fs_start - nvm_size, nvm_size),
            "circuitpy": "external" if external else (fs_start, fs_size),
        }
    raise ValueError(f"{counterpart}: no layout rules known for port {port}")


def is_internal_flash(node):
    return "soc-nv-flash" in getattr(node, "compats", [])


def is_external_flash(node):
    return node.props.get("size") is not None and any(
        "nor" in compat for compat in getattr(node, "compats", [])
    )


def check_parity(edt, counterpart):
    """Return problems where the layout differs from the non-Zephyr ``counterpart``."""
    internal = [node for node in edt.nodes if is_internal_flash(node)]
    if not internal:
        return [f"no soc-nv-flash device to compare with {counterpart}"]
    try:
        expected = reference_layout(counterpart, device_size(internal[0]))
    except ValueError as e:
        return [str(e)]

    partitions = {}
    for node, dev, offset, size, mapped in iter_partitions(edt):
        label = node.labels[0] if node.labels else node.name
        partitions[label] = (dev, offset, size)

    problems = []

    def compare(label, want):
        found = partitions.get(label)
        if found is None:
            problems.append(
                f"{label}: missing, {counterpart} has it at 0x{want[0]:x}+0x{want[1]:x}"
            )
            return
        dev, offset, size = found
        if not is_internal_flash(dev):
            problems.append(f"{label}: not on the internal flash, unlike {counterpart}")
        if (offset, size) != want:
            problems.append(
                f"{label}: 0x{offset:x}+0x{size:x}, {counterpart} has it at "
                f"0x{want[0]:x}+0x{want[1]:x}"
            )

    compare("nvm_partition", expected["nvm"])
    if expected["circuitpy"] == "external":
        # supervisor/flash.c uses the first flash device no partition covers as the
        # drive, which is the whole chip only while the chip carries no partitions.
        if "circuitpy_partition" in partitions:
            problems.append(
                f"circuitpy_partition: declared, but {counterpart} uses the whole "
                f"external flash as the drive"
            )
        external = [node for node in edt.nodes if is_external_flash(node)]
        if not external:
            problems.append(f"no external flash, but {counterpart} keeps the drive on one")
        for dev, offset, size in partitions.values():
            if is_external_flash(dev):
                problems.append(
                    f"{dev.labels[0] if dev.labels else dev.name}: carries partitions, "
                    f"but {counterpart} uses the whole chip as the drive"
                )
                break
    else:
        compare("circuitpy_partition", expected["circuitpy"])
    return problems


def counterpart_of(board_id):
    """The ``counterpart`` a board declares in its circuitpython.toml, if any."""
    toml_path = board_tools.find_mpconfigboard(PORT_DIR, board_id) if board_id else None
    if toml_path is None:
        return None
    with toml_path.open("rb") as f:
        return tomllib.load(f).get("counterpart")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument(
        "build_dirs",
        nargs="*",
        help="Build directories to check (default: every build-* in the port)",
    )
    parser.add_argument(
        "--board",
        help="Board built in the (single) build directory; default: taken from its name",
    )
    args = parser.parse_args()

    dirs = [pathlib.Path(d) for d in args.build_dirs]
    if not dirs:
        dirs = sorted(p for p in PORT_DIR.glob("build-*") if p.is_dir())
    if not dirs:
        print("No build directories found; build a board first.", file=sys.stderr)
        return 1
    if args.board and len(dirs) != 1:
        parser.error("--board applies to exactly one build directory")

    failed = []
    checked = 0
    for build_dir in dirs:
        edt = load_edt(build_dir)
        if edt is None:
            continue
        checked += 1
        n_parts = sum(1 for _ in iter_partitions(edt))
        problems = check_layout(edt)
        board = args.board or build_dir.name.removeprefix("build-")
        counterpart = counterpart_of(board)
        if counterpart:
            problems += check_parity(edt, counterpart)
        if problems:
            failed.append(build_dir.name)
            print(f"{build_dir.name}:")
            for problem in problems:
                print(f"  FAIL  {problem}")
        else:
            # Say how much was inspected: a board whose overlay defines no
            # partitions at all would otherwise be indistinguishable from a
            # verified-good one.
            parity = f", matches ports/{counterpart}" if counterpart else ""
            print(f"{build_dir.name}: ok ({n_parts} partitions checked{parity})")

    if not checked:
        print("No build directory held an edt.pickle; build a board first.", file=sys.stderr)
        return 1
    if failed:
        print(f"\n{len(failed)} board(s) with layout problems: {', '.join(failed)}")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
