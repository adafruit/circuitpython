try:
    import traceback
except ImportError:
    print("SKIP")
    raise SystemExit


def fun():
    raise Exception("test")


try:
    fun()
except Exception as exc:
    print("\nNo Trace:")
    traceback.print_exception(None, exc, None)
    print("\nDefault Trace:")
    traceback.print_exception(exc)
    print("\nLimit=1 Trace:")
    traceback.print_exception(None, exc, exc.__traceback__, limit=1)
    print("\nLimit=0 Trace:")
    traceback.print_exception(None, exc, exc.__traceback__, limit=0)
    print("\nLimit=-1 Trace:")
    print("".join(traceback.format_exception(None, exc, exc.__traceback__, limit=-1)), end="")

# value and tb must both be supplied or neither
print()
try:
    fun()
except Exception as exc:
    try:
        traceback.print_exception(None, value=exc)
        print("Should have raised ValueError for missing tb arg")
    except ValueError:
        print("ValueError for missing tb arg, as expected")
    try:
        traceback.print_exception(None, tb=None)
        print("Should have raised ValueError for missing value arg")
    except ValueError:
        print("ValueError for missing value arg, as expected")


class NonNativeException(Exception):
    pass


try:
    raise NonNativeException("test")
except Exception as e:
    print("\nNonNative Trace:")
    traceback.print_exception(None, e, e.__traceback__)
    print("")
