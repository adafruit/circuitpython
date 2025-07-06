"""
categories: Core
description: f-strings cannot support expressions that require parsing to resolve unbalanced nested braces and brackets
cause: MicroPython is optimised for code space.
workaround: Always use balanced braces and brackets in expressions inside f-strings
"""

# CIRCUITPY-CHANGE: add noqa so ruff won't complain about unmatched braces
print(f"{'hello { world'}")  # noqa
print(f"{'hello ] world'}")  # noqa
