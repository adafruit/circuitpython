import micropython as micropython

# check that level 3 doesn't store line numbers
# the expected output is that any line is printed as "line 1"
micropython.opt_level(3)
# CIRCUITPY-CHANGE: use traceback.print_exception() instead of sys.print_exception()
exec("try:\n xyz\nexcept NameError as er:\n import traceback\n traceback.print_exception(er)")
