"""
categories: Core
description: f-strings cannot support expressions that require parsing to resolve unbalanced nested braces and brackets
cause: MicroPython is optimised for code space.
workaround: Always use balanced braces and brackets in expressions inside f-strings
"""

# fmt: off
print(f"{'hello { world'}")
print(f"{'hello ] world'}")
