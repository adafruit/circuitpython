# https://github.com/adafruit/circuitpython/issues/10614
# When a directory `shared_name/` (no __init__.py) and a module `shared_name.py`
# share a name, `import shared_name` must pick the .py module per PEP 420
# precedence: regular package > module > namespace package.
import shared_name

print("done")
