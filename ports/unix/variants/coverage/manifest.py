add_library("unix-ffi", "$(MPY_LIB_DIR)/unix-ffi")
freeze_as_str("frzstr")
freeze_as_mpy("frzmpy")
freeze_mpy("$(MPY_DIR)/tests/assets")
require("ssl")

# CIRCUITPY-CHANGE: no example user C modules; CircuitPython has no examples/usercmodule.
