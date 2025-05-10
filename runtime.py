"""Utility script to clean up redundant header includes in CircuitPython source files.

This script removes translate.h includes when they appear together with runtime.h,
as runtime.h already includes the necessary translation functionality.
"""

import pathlib

# Find all C source files recursively
paths = pathlib.Path(".").glob("**/*.c")
translate_h = '#include "supervisor/shared/translate/translate.h"'

for p in paths:
    # Skip files in esp-idf directories
    if "esp-idf" in str(p):
        continue
        
    # Process file if it contains both runtime.h and translate.h
    lines = p.read_text().split("\n")
    if '#include "py/runtime.h"' in lines and translate_h in lines:
        # Remove redundant translate.h include
        lines.remove(translate_h)
        p.write_text("\n".join(lines))
