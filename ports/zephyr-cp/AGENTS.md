- Build a board by doing `make BOARD=<vendor>_<board_name>`.
- The corresponding configuration files are in `boards/<vendor>/<board_name>`, including the
  board's devicetree overlay (`board.overlay`) and Kconfig fragment (`board.conf`). The Makefile
  passes them to Zephyr; the overlay replaces `app.overlay`, so include it if the board wants it.
- To flash it on a board do `make BOARD=<vendor>_<board_name> flash`.
- Zephyr board docs are at `zephyr/boards/<vendor>/<board_name>`.
- Run zephyr-cp tests with `make test`.
- Do not add new translatable error strings (`MP_ERROR_TEXT`). Instead, use `raise_zephyr_error()` or `CHECK_ZEPHYR_RESULT()` from `bindings/zephyr_kernel/__init__.h` to convert Zephyr errno values into Python exceptions.
