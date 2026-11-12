# Boot-time flash mapping setting

Run on a board with `CIRCUITPY_STORAGE_MAP_FILE` compiled in and a FAT CIRCUITPY
drive. Copy `code.py` and `quadrants.bmp` to CIRCUITPY. Preserve your other
settings and boot/display configuration. This test does not edit files itself.

The script prompts for this stage's **expected boot value**, checks the public
`storage.map_file()` result, checks `OnDiskBitmap` dimensions and palette, and
displays four quadrants: red/green above blue/white. Enter `enabled` or `disabled`
from the table below; the script deliberately does not infer this from the current
`settings.toml`, which can differ from the boot snapshot after a soft reload.

For visual checks on an LCD/framebuffer display, use `board.DISPLAY`, or provide
a `display_setup.py` that exports your configured `display`. On Qualia, reuse your known working panel
setup in that module. Without a configured display, the script reports the
visual check as skipped; the serial checks still run. E-paper displays are also
skipped for the visual check.

| Stage | `CIRCUITPY_STORAGE_MAP_FILE` in `settings.toml` | Action | Enter |
| --- | --- | --- | --- |
| 1 | Key absent | Hardware reset / power cycle | `enabled` |
| 2 | `false` | Save, then Ctrl-D only | `enabled` |
| 3 | `false` | Hardware reset / power cycle | `disabled` |
| 4 | `true` | Save, then Ctrl-D only | `disabled` |
| 5 | `true` | Hardware reset / power cycle | `enabled` |
| 6 | Key absent | Hardware reset / power cycle | `enabled` |
| 7 | `"false"` (quoted, invalid boolean) | Hardware reset / power cycle | `enabled` |
| 8 | `0` (invalid boolean) | Hardware reset / power cycle | `enabled` |

Repeat Ctrl-D several times during stages 2 and 4. Saving may also autoreload;
that must preserve the same boot value. A hardware reset means the reset button
or a power cycle, not Ctrl-D or `supervisor.reload()`. Remove any duplicate copies
of this key. Invalid values may produce the normal settings parser diagnostic.

Every stage should print `PASS map_file ...` and
`PASS OnDiskBitmap dimensions and palette`.
Disabled mapping must raise `OSError` with `errno.EOPNOTSUPP`, while the bitmap
still renders identically. Observe the four quadrants at each stage with a
configured display; `OnDiskBitmap` does not expose pixel subscription, so the
serial checks do not prove pixel rendering or which internal read path was used.
The enabled case also checks the views after closing their file.
