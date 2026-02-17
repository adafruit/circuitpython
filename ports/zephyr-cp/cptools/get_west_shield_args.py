#!/usr/bin/env python3
"""Resolve shield arguments for west build.

Priority:
1. SHIELD / SHIELDS make variables (if explicitly provided)
2. SHIELD / SHIELDS from boards/<vendor>/<board>/circuitpython.toml
"""

import argparse
import os
import pathlib
import re
import shlex

import board_tools


def split_shields(raw):
    if not raw:
        return []

    return [shield for shield in re.split(r"[,;\s]+", raw.strip()) if shield]


def dedupe(values):
    deduped = []
    seen = set()

    for value in values:
        if value in seen:
            continue
        seen.add(value)
        deduped.append(value)

    return deduped


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("board")
    args = parser.parse_args()

    portdir = pathlib.Path(__file__).resolve().parent.parent

    _, mpconfigboard = board_tools.load_mpconfigboard(portdir, args.board)

    shield_origin = os.environ.get("SHIELD_ORIGIN", "undefined")
    shields_origin = os.environ.get("SHIELDS_ORIGIN", "undefined")

    shield_override = os.environ.get("SHIELD", "")
    shields_override = os.environ.get("SHIELDS", "")

    override_requested = shield_origin != "undefined" or shields_origin != "undefined"

    if override_requested:
        shields = split_shields(shield_override) + split_shields(shields_override)
    else:
        shields = board_tools.get_shields(mpconfigboard)

    shields = dedupe(shields)

    west_shield_args = []
    for shield in shields:
        west_shield_args.extend(("--shield", shield))

    print(shlex.join(west_shield_args))


if __name__ == "__main__":
    main()
