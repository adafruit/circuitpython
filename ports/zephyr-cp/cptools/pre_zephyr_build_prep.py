# Called by the Makefile before calling out to `west`.
import pathlib
import subprocess
import sys

import board_tools

portdir = pathlib.Path(__file__).resolve().parent.parent

board = sys.argv[-1]

_, mpconfigboard = board_tools.load_mpconfigboard(portdir, board)
if not mpconfigboard:
    # Assume it doesn't need any prep.
    sys.exit(0)

blobs = mpconfigboard.get("BLOBS", [])
blob_fetch_args = mpconfigboard.get("blob_fetch_args", {})
for blob in blobs:
    args = blob_fetch_args.get(blob, [])
    subprocess.run(["west", "blobs", "fetch", blob, *args], check=True)

if board.endswith("bsim"):
    subprocess.run(
        ["make", "everything", "-j", "8"],
        cwd=portdir / "tools" / "bsim",
        check=True,
    )
