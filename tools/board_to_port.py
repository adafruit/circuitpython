#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

import sys
from pathlib import Path

docs = Path(__file__).parent.parent / "docs"
sys.path.append(str(docs))
from shared_bindings_matrix import get_board_mapping

board = sys.argv[1]

board_mapping = get_board_mapping()
if board in board_mapping:
    board_info = board_mapping[board]
    print(board_info["port"])
    sys.exit(0)

raise ValueError(f"No port directory associated with the board tag given {board}.")
