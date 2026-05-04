# Test supervisor.get_setting() parsing of settings.toml values.
# Copy settings.toml to the root of the board, then run this script.
# All lines should print PASS.

import math
import supervisor


def check(key, expected):
    got = supervisor.get_setting(key)
    if type(got) is not type(expected):
        print(
            f"FAIL {key}: expected type {type(expected).__name__}, got {type(got).__name__} ({got!r})"
        )
        return
    if isinstance(expected, float) and math.isnan(expected):
        ok = math.isnan(got)
    else:
        ok = got == expected
    if ok:
        print(f"PASS {key}")
    else:
        print(f"FAIL {key}: expected {expected!r}, got {got!r}")


check("str_val", "hello")
check("int_val", 42)
check("neg_int_val", -7)
check("hex_int_val", 31)
check("bool_true", True)
check("bool_false", False)
check("float_val", 3.14)
check("neg_float_val", -1.5)
check("sci_float_val", 6.626e-34)
# math.nan and math.inf are not always available
check("pos_inf", float("inf"))
check("neg_inf", -float("inf"))
check("pos_nan", float("nan"))


def check_bad(key):
    try:
        got = supervisor.get_setting(key)
        print(f"FAIL {key}: expected ValueError, got {got!r}")
    except ValueError:
        print(f"PASS {key} raises ValueError")


check_bad("bad_word")  # unquoted non-boolean word
check_bad("bad_float_junk")  # float with trailing garbage
check_bad("bad_inf_junk")  # inf with trailing characters
check_bad("bad_lone_sign")  # bare + with nothing after it

# Missing key returns default
got = supervisor.get_setting("no_such_key", "default")
if got == "default":
    print("PASS missing key default")
else:
    print(f"FAIL missing key default: got {got!r}")

# Invalid value raises ValueError
try:
    supervisor.get_setting("str_val")  # quoted string is valid
    print("PASS str via get_setting")
except ValueError:
    print("FAIL str via get_setting raised ValueError unexpectedly")
