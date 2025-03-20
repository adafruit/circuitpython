# Called by the Makefile before calling out to `west`.
import pathlib
import subprocess
import sys
import tomllib

import board_tools

portdir = pathlib.Path(__file__).resolve().parent.parent

board = sys.argv[-1]

mpconfigboard = board_tools.find_mpconfigboard(portdir, board)
if mpconfigboard is None:
    # Assume it doesn't need any prep.
    sys.exit(0)

with mpconfigboard.open("rb") as f:
    mpconfigboard = tomllib.load(f)

blobs = mpconfigboard.get("BLOBS", [])
for blob in blobs:
    subprocess.run(["west", "blobs", "fetch", blob], check=True)
