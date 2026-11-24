import errno

import board
import displayio
import storage


answer = input("Expected mapping for this stage (enabled/disabled): ").strip().lower()
if answer not in ("enabled", "disabled"):
    raise ValueError("Enter enabled or disabled from the README stage table")
expected_enabled = answer == "enabled"

fixture = "/quadrants.bmp"
with open(fixture, "rb") as source:
    expected_bytes = source.read()

with open(fixture, "rb") as source:
    try:
        views = storage.map_file(source)
    except OSError as error:
        if expected_enabled or error.args[0] != errno.EOPNOTSUPP:
            raise
        print("PASS map_file disabled: EOPNOTSUPP")
    else:
        assert expected_enabled, "map_file succeeded while mapping should be disabled"
        assert views, "nonempty fixture returned no mapped runs"

if expected_enabled:
    # The views must remain readable after source.close().
    assert b"".join(bytes(view) for view in views) == expected_bytes
    print("PASS map_file enabled: exact bytes after closing file")

bitmap = displayio.OnDiskBitmap(fixture)
assert bitmap.width == 16 and bitmap.height == 16
assert isinstance(bitmap.pixel_shader, displayio.Palette)
for index, color in enumerate((0x000000, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF)):
    assert bitmap.pixel_shader[index] == color, (index, bitmap.pixel_shader[index], color)
print("PASS OnDiskBitmap dimensions and palette")

display = getattr(board, "DISPLAY", None)
if display is None:
    try:
        from display_setup import display
    except ImportError:
        print("SKIP visual check: configure display_setup.py exporting display")

if display is not None and not hasattr(display, "auto_refresh"):
    print("SKIP visual check: requires an LCD/framebuffer display")
    display = None

if display is not None:
    scale = max(1, min(display.width // 16, display.height // 16))
    group = displayio.Group(
        scale=scale,
        x=(display.width - 16 * scale) // 2,
        y=(display.height - 16 * scale) // 2,
    )
    group.append(displayio.TileGrid(bitmap, pixel_shader=bitmap.pixel_shader))
    display.root_group = group
    if not display.auto_refresh:
        display.refresh()
    print("VISUAL CHECK: red/green above blue/white; compare all stages")
