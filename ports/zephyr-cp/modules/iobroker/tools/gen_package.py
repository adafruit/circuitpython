#!/usr/bin/env python3
"""Transcribe a package pin assignment table into a iobroker package TOML.

Input is a `pdftotext -layout` dump of a Nordic SoC datasheet (the module's
datasheets/ directory keeps the PDFs; run pdftotext -layout yourself). The
script locates one "X.Y.Z <PACKAGE> (<CODE>) package pin assignments" section,
parses its pin table and cross-checks the GPIO pads it finds against the pads
named in the section's pin assignment figure.

The pin tables are laid out with the pin number vertically centered in its
row, so a pin's GPIO pad name can appear on the line before its number. The
parser therefore assigns every GPIO pad (P<port>.<pin>) to the next pin
number at or after it in reading order; sequential pin numbering makes that
unambiguous. Every mapped pin is printed for manual review -- the datasheet
figure remains the source of truth, so diff the output against it.

Usage (nRF54 datasheets, "<PACKAGE> (<CODE>) package pin assignments" with
numeric QFN pin numbers or a ball grid):

  gen_package.py DATASHEET.txt --section 10.1.4 --name nrf54l15_qfn48 \
      --socs SOC_NRF54L15,SOC_NRF54L10,SOC_NRF54L05 --pins 48 \
      --source nRF54L15_nRF54L10_nRF54L05_Datasheet_v1.0.pdf \
      --out ../packages/nrf54l15_qfn48.toml

Ball grids label their pins "A5"-style instead of numbering them; pass the
grid's row letters with --rows (comma-separated when the grid has two-letter
rows, as the aQFN grids do: --rows A,B,...,Y,AA,...,AL) and --cols.

nRF52/nRF53 product specifications name their sections "<PACKAGE> pin/ball
assignments" without a package code and lay the table out the same way (the
"Pin" column holds the ball label, there is no Clock column, and the aQFN
tables follow the main table with a "Corner pads" subsection); the same
options transcribe them:

  gen_package.py DATASHEET.txt --section 9.1.1 --name nrf5340_qkaa \
      --socs SOC_NRF5340_CPUAPP --pins 94 \
      --rows A,B,C,D,E,F,G,H,J,K,L,M,N,P,R,T,U,V,W,Y,AA,AB,AC,AD,AE,AF,AG,AH,AJ,AK,AL \
      --cols 31 --source nRF5340_PS_v1.6.pdf --out ../packages/nrf5340_qkaa.toml
"""

import argparse
import re
import sys
from pathlib import Path

PAD_RE = re.compile(r"P(\d+)\.(\d+)")
# Section headings: "10.1.4 QFN48 (QFAA) package pin assignments" (nRF54
# datasheets; title plus package code) or "9.1.1 aQFN94 pin assignments" /
# "7.1.1 aQFN73 ball assignments" (nRF52840/nRF5340 product specifications; no
# package code). The heading must end in the assignments phrase, which keeps
# table-of-contents lines (trailing dot leaders and page numbers) out.
SECTION_RE = re.compile(
    r"^\s*(\d+\.\d+\.\d+)\s+(.+?)\s+((?:package\s+)?(?:pin|ball)\s+assignments)$"
)
# Optional "(<CODE>)" at the end of a section title, as in the nRF54 headings.
TITLE_CODE_RE = re.compile(r"^(.*?)\s*\((\S+)\)$")
FIGURE_END_RE = re.compile(r"^\s*Figure \d+:")
# A pin number line: optionally preceded by the "Yes" clock-pin marker, either
# alone or with the row's first name/function entry on the same line.
PIN_LINE_RE = re.compile(r"^\s*(?:Yes\s+)?(\d+)(?:\s+(\S.*))?$")
# A ball name line (aQFN/CSP/BGA packages): row letter(s) + column at column 0.
BALL_LINE_RE = re.compile(r"^([A-Z]+)(\d+)(?:\s+|$)")
TABLE_HEADER_RE = re.compile(r"^\s*Pin\s+(?:Clock\s+)?Name\s+Function\s+Description")
# The pin table ends at its "Table N:" caption, or -- for aQFN packages,
# which list their corner pads right after the main table -- at the
# "Corner pads" heading. Neither belongs to the table.
TABLE_END_RE = re.compile(r"^\s*(?:Table\s+\d+:|Corner pads\b)")


def find_section(lines, section):
    """Return (title, code, kind, section lines) for a numbered heading.

    code is the package code in the nRF54 "QFN48 (QFAA)" headings, or None
    when the heading carries none ("aQFN94 pin assignments"). kind is the
    heading's "package pin assignments"/"pin assignments"/"ball assignments"
    phrase, used in the output header comment.
    """
    starts = [(i, SECTION_RE.match(line)) for i, line in enumerate(lines)]
    starts = [(i, m) for i, m in starts if m]
    for index, (i, m) in enumerate(starts):
        if m.group(1) == section:
            end = starts[index + 1][0] if index + 1 < len(starts) else len(lines)
            title, code = m.group(2), None
            code_match = TITLE_CODE_RE.match(title)
            if code_match:
                title, code = code_match.group(1), code_match.group(2)
            return title, code, m.group(3), lines[i + 1 : end]
    raise SystemExit(f"section {section!r} not found")


def parse_table(section_lines, rows=None, cols=10):
    """Return ({pin_id: [(port, pin), ...]}, ordered pin ids) from the table.

    Numeric packages (QFN) use 1-based pin numbers validated against
    sequential numbering. Ball packages pass the grid's row letters (a list,
    since aQFN grids have two-letter rows); pins are (row, column) pairs,
    ordered row-major.
    """
    header = next((i for i, line in enumerate(section_lines) if TABLE_HEADER_RE.match(line)), None)
    if header is None:
        raise SystemExit("pin table header ('Pin Clock Name Function ...') not found")
    body = section_lines[header + 1 :]
    # The table ends at its caption line or at the "Corner pads" subsection
    # some packages list after it; neither is part of the table.
    end = next((i for i, line in enumerate(body) if TABLE_END_RE.match(line)), None)
    if end is not None:
        body = body[:end]

    # Pass 1: pin lines. Numeric mode validates against sequential numbering
    # so page numbers and stray digits cannot be mistaken for pins; ball mode
    # validates against the grid instead.
    number_lines = []
    if rows:
        valid = {(row, column) for row in rows for column in range(1, cols + 1)}
        for i, line in enumerate(body):
            m = BALL_LINE_RE.match(line)
            if m and (m.group(1), int(m.group(2))) in valid:
                ball = (m.group(1), int(m.group(2)))
                if ball in (b for _, b in number_lines):
                    raise SystemExit(f"duplicate ball {ball[0]}{ball[1]}")
                number_lines.append((i, ball))
    else:
        for i, line in enumerate(body):
            m = PIN_LINE_RE.match(line)
            if m and int(m.group(1)) == len(number_lines) + 1:
                number_lines.append((i, int(m.group(1))))
    if not number_lines:
        raise SystemExit("no pin numbers found; table layout mismatch?")

    # Pass 2: assign each GPIO pad to the next pin at or after it. The pin
    # number is vertically centered in its table row, so the row's GPIO pad
    # name can sit on the line before the number.
    pins = {pin: [] for _, pin in number_lines}
    for i, line in enumerate(body):
        for m in PAD_RE.finditer(line):
            owner = next((pin for j, pin in number_lines if j >= i), None)
            if owner is None:
                raise SystemExit(f"GPIO pad {m.group(0)!r} after the last pin")
            pins[owner].append((int(m.group(1)), int(m.group(2))))
    order = [pin for _, pin in number_lines]
    return pins, order


def parse_figure(section_lines):
    """GPIO pads named anywhere in the section's pin assignment figure."""
    pads = set()
    for line in section_lines:
        if FIGURE_END_RE.match(line):
            break
        pads.update(PAD_RE.finditer(line))
    return {(int(m.group(1)), int(m.group(2))) for m in pads}


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("datasheet", type=Path, help="pdftotext -layout dump")
    parser.add_argument("--section", required=True, help="e.g. 10.1.4")
    parser.add_argument("--name", required=True, help="package map name, e.g. nrf54l15_qfn48")
    parser.add_argument(
        "--socs", required=True, help="comma-separated Kconfig SoC symbols the map applies to"
    )
    parser.add_argument(
        "--pins", type=int, required=True, help="expected number of package pins (or balls)"
    )
    parser.add_argument(
        "--rows",
        help="ball row letters, e.g. ABCDEFGHJK (comma-separated A,B,...,AA "
        "when the grid has two-letter rows), for ball grid packages; "
        "pins are numbered sequentially in row-major order",
    )
    parser.add_argument("--cols", type=int, default=10, help="ball grid columns (with --rows)")
    parser.add_argument(
        "--die-pad",
        action="store_true",
        help="the table lists the exposed die pad as one pin past --pins; "
        "it is bonded to VSS and never routable",
    )
    parser.add_argument(
        "--source", required=True, help="datasheet file name for the header comment"
    )
    parser.add_argument("--out", type=Path, required=True, help="output TOML path")
    args = parser.parse_args()

    # Ball row letters: comma-separated when the grid has two-letter rows.
    rows = None
    if args.rows:
        rows = args.rows.split(",") if "," in args.rows else list(args.rows)

    lines = args.datasheet.read_text().splitlines()
    title, code, kind, section_lines = find_section(lines, args.section)

    pins, order = parse_table(section_lines, rows=rows, cols=args.cols)
    expected_max = args.pins + (1 if args.die_pad else 0)
    if len(order) != expected_max:
        raise SystemExit(f"parsed {len(order)} pins, expected {expected_max}")
    if args.die_pad:
        die_pad = order[-1]
        if pins[die_pad]:
            raise SystemExit(f"die pad {die_pad} unexpectedly bonded to GPIO {pins[die_pad]}")
        del pins[die_pad]
        order = order[:-1]
    if not args.rows:
        missing = [n for n in range(1, args.pins + 1) if n not in pins]
        if missing:
            raise SystemExit(f"pin numbers not found: {missing}")

    # Every pin must bond at most one GPIO pad, and the set of bonded pads
    # must match the pads the figure shows for this package.
    multi = {n: p for n, p in pins.items() if len(p) > 1}
    if multi:
        raise SystemExit(f"pins with more than one GPIO pad: {multi}")
    table_pads = {p[0] for p in pins.values() if p}
    figure_pads = parse_figure(section_lines)
    if not figure_pads:
        # CSP/BGA figures are unlabeled ball grids; there is nothing to
        # cross-check against. The pad-count sanity check below still applies.
        print("note: figure names no GPIO pads; skipping table/figure cross-check")
    elif table_pads != figure_pads:
        raise SystemExit(
            "pad mismatch table vs figure:\n"
            f"  only in table:  {sorted(table_pads - figure_pads)}\n"
            f"  only in figure: {sorted(figure_pads - table_pads)}"
        )

    skipped = [pin for pin in order if not pins[pin]]

    def pin_label(pin):
        return f"{pin[0]}{pin[1]}" if isinstance(pin, tuple) else str(pin)

    header_notes = []
    if args.rows:
        header_notes += [
            "# Ball packages: 'pin' numbers the balls sequentially in row-major",
            "# order (the datasheet only labels them, e.g. B2).",
            "",
        ]
    out_lines = [
        f"# Package pin map for {args.name}, transcribed from {args.source},",
        f"# section {args.section} ({title}{f', {code}' if code else ''} {kind}),",
        "# by tools/gen_package.py. Review against the datasheet figure before",
        "# editing by hand; regenerate instead when the datasheet changes.",
        "",
        *header_notes,
        f'name = "{args.name}"',
        f"socs = [{', '.join(f'"{s}"' for s in args.socs.split(','))}]",
        "",
    ]
    for number, pin in enumerate(order, start=1):
        pad = pins[pin][0] if pins[pin] else None
        if not pad:
            continue
        soc_pad = pad[0] * 32 + pad[1]
        out_lines.append("[[pins]]")
        out_lines.append(f"pin = {number}")
        if isinstance(pin, tuple):
            out_lines.append(f'ball = "{pin[0]}{pin[1]}"')
        out_lines.append(f"pad = {soc_pad}")
        out_lines.append(f'pad_name = "P{pad[0]}.{pad[1]:02d}"')
        out_lines.append("")
    if skipped:
        out_lines.append("# Package pins without a GPIO pad (not routable):")
        out_lines.append("# " + ", ".join(pin_label(pin) for pin in skipped))
        out_lines.append("")

    args.out.write_text("\n".join(out_lines))

    print(
        f"{args.out}: {sum(1 for p in pins.values() if p)} GPIO pads "
        f"on {len(order)} package pins, {len(skipped)} skipped"
    )
    for number, pin in enumerate(order, start=1):
        pad = pins[pin][0] if pins[pin] else None
        if pad:
            print(f"  pin {pin_label(pin):>4} -> P{pad[0]}.{pad[1]:02d}")


if __name__ == "__main__":
    main()
