# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""vectorio tests that run against both displayio compositors.

native_sim is built twice: once with the legacy scanline compositor
(``native_native_sim``) and once with the LVGL compositor
(``native_native_sim_lvgl``). vectorio has an implementation for each one, so
these tests check that the LVGL implementation behaves like the legacy one: the
same API, the same shapes in the same place and the same pixels on the screen.

The golden image was captured from the legacy compositor and is compared by
both. The only pixels that are not compared are the ones along the outline of
the circle: LVGL draws a circle as a rounded rectangle and its rasterizer
supersamples the curve, so the outline is a blend of the shape and the
background there while the legacy compositor fills whole pixels. The band
around the outline is skipped, everything else has to match exactly.

Three differences between the two implementations are known and are not tested
here:

- LVGL antialiases the outline of a circle, the legacy compositor does not.
- The legacy compositor does not scale vectorio shapes with the scale of the
  group they are in, while the LVGL one does. A shape in a scaled group is
  therefore bigger on an LVGL display.
- An outline color index that names a transparent palette entry leaves the
  outline band unpainted on the legacy compositor, while LVGL treats it as no
  outline and lets the fill reach the edge. Set outline_color_index to None to
  remove an outline instead of making its palette entry transparent.
"""

import math
import shutil
from pathlib import Path

import pytest
from PIL import Image

_DISPLAY_HELPER = """\
import board
display = board.DISPLAY
"""

# The scene both compositors render. The shapes are placed so that none of them
# overlap and so that the circle is far away from the other two.
SCENE_CODE = (
    _DISPLAY_HELPER
    + """\
import displayio
import time
import vectorio

palette = displayio.Palette(4)
palette[0] = 0x000000
palette[1] = 0xFF0000
palette[2] = 0x00FF00
palette[3] = 0x0000FF

group = displayio.Group()

circle = vectorio.Circle(pixel_shader=palette, radius=20, x=40, y=40, color_index=1)
group.append(circle)

rectangle = vectorio.Rectangle(
    pixel_shader=palette, width=40, height=20, x=100, y=40, color_index=2
)
group.append(rectangle)

polygon = vectorio.Polygon(
    pixel_shader=palette,
    points=[(0, 0), (40, 10), (20, 40)],
    x=180,
    y=40,
    color_index=3,
)
group.append(polygon)

# Replacing the root group hides the console so the frame only has the shapes.
display.auto_refresh = False
display.root_group = group
display.refresh()
print('rendered')
while True:
    time.sleep(1)
"""
)

# The scene with the rectangle moved and the circle hidden. It is captured twice:
# once before the move and once after it.
MOVE_CODE = (
    _DISPLAY_HELPER
    + """\
import displayio
import time
import vectorio

palette = displayio.Palette(4)
palette[0] = 0x000000
palette[1] = 0xFF0000
palette[2] = 0x00FF00
palette[3] = 0x0000FF

group = displayio.Group()

circle = vectorio.Circle(pixel_shader=palette, radius=20, x=40, y=40, color_index=1)
circle.hidden = True
group.append(circle)

rectangle = vectorio.Rectangle(
    pixel_shader=palette, width=40, height=20, x=100, y=40, color_index=2
)
group.append(rectangle)

display.auto_refresh = False
display.root_group = group
display.refresh()
print('first')
time.sleep(2)
rectangle.x = 160
display.refresh()
print('second')
while True:
    time.sleep(1)
"""
)

API_CODE = """\
import displayio
import vectorio

palette = displayio.Palette(4)
palette[0] = 0x000000
palette[1] = 0xFF0000
palette[2] = 0x00FF00
palette[3] = 0x0000FF

circle = vectorio.Circle(pixel_shader=palette, radius=10, x=20, y=20)
rectangle = vectorio.Rectangle(pixel_shader=palette, width=30, height=20, x=5, y=5)
polygon = vectorio.Polygon(pixel_shader=palette, points=[(0, 0), (10, 0), (0, 10)])

print('circle', circle.x, circle.y, circle.radius, circle.color_index)
print('rectangle', rectangle.x, rectangle.y, rectangle.width, rectangle.height)
print('polygon', polygon.points, polygon.color_index)

# The circle is centered on its position and covers the pixels whose center is
# within the radius.
print('circle contains', circle.contains(20, 20), circle.contains(30, 20),
    circle.contains(31, 20), circle.contains(20, 31))

# The rectangle's position is its top left corner.
print('rectangle contains', rectangle.contains(5, 5), rectangle.contains(34, 24),
    rectangle.contains(35, 24), rectangle.contains(4, 5))

# The polygon's points are relative to its position.
print('polygon contains', polygon.contains(0, 0), polygon.contains(5, 1),
    polygon.contains(9, 9))

# Moving a shape moves its coverage with it.
circle.x = 50
print('moved circle contains', circle.contains(60, 20), circle.contains(20, 20))

# The color index selects the palette entry.
circle.color_index = 2
print('color index', circle.color_index)

# A shape can only be in one group.
group = displayio.Group()
group.append(circle)
print('in group', len(group))
other = displayio.Group()
try:
    other.append(circle)
    print('second group: no error')
except ValueError as e:
    print('second group: ValueError')
print('done')
"""


def _read_image(path):
    with Image.open(path) as img:
        rgb = img.convert("RGB")
        return rgb.width, rgb.height, rgb.tobytes()


def _pixel_at(image, x, y):
    return tuple(image.getpixel((x, y)))


def _circle_outline_band(width, height, center_x, center_y, radius):
    """Return the pixels LVGL may antialias when it draws a circle.

    LVGL draws the circle as a rounded rectangle and supersamples the curve, so
    the pixels along the outline are a blend of the shape and the background
    while the legacy compositor fills whole pixels. The pixels inside the
    outline are filled completely by both and the pixels outside it are not
    touched by either, so only the outline itself is skipped.
    """
    band = set()
    for y in range(height):
        for x in range(width):
            distance = math.hypot(x - center_x, y - center_y)
            if radius - 1 < distance <= radius + 1.5:
                band.add((x, y))
    return band


@pytest.mark.circuitpy_drive({"code.py": API_CODE})
@pytest.mark.duration(15)
def test_vectorio_api(circuitpython):
    """The vectorio API behaves the same on both compositors."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "circle 20 20 10 0" in output
    assert "rectangle 5 5 30 20" in output
    assert "polygon [(0, 0), (10, 0), (0, 10)] 0" in output
    # The circle covers the pixels whose center is within the radius.
    assert "circle contains True True False False" in output
    # The rectangle's position is its top left corner.
    assert "rectangle contains True True False False" in output
    assert "polygon contains True True False" in output
    # Moving a shape moves its coverage with it.
    assert "moved circle contains True False" in output
    assert "color index 2" in output
    assert "in group 1" in output
    assert "second group: ValueError" in output
    assert "done" in output


@pytest.mark.circuitpy_drive({"code.py": SCENE_CODE})
@pytest.mark.display(capture_times_ns=[4_000_000_000])
@pytest.mark.duration(8)
def test_vectorio_render_golden(request, circuitpython):
    """The shapes are rendered exactly like the legacy compositor renders them."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "rendered" in output

    captures = circuitpython.display_capture_paths()
    if not captures or not captures[0].exists():
        pytest.skip("display capture was not produced")

    golden = Path(__file__).parent / "vectorio" / "golden" / "vectorio_shapes_320x240.png"
    if request.config.getoption("--update-goldens"):
        golden.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(captures[0], golden)
        return

    gw, gh, gpx = _read_image(golden)
    dw, dh, dpx = _read_image(captures[0])
    assert (dw, dh) == (gw, gh)

    band = _circle_outline_band(dw, dh, 40, 40, 20)
    mismatches = []
    for y in range(dh):
        for x in range(dw):
            if (x, y) in band:
                continue
            i = (y * dw + x) * 3
            if gpx[i : i + 3] != dpx[i : i + 3]:
                mismatches.append((x, y, tuple(gpx[i : i + 3]), tuple(dpx[i : i + 3])))
    assert not mismatches, f"{len(mismatches)} pixels differ, first: {mismatches[:5]}"


@pytest.mark.circuitpy_drive({"code.py": MOVE_CODE})
@pytest.mark.display(capture_times_ns=[8_000_000_000, 10_000_000_000, 12_000_000_000])
@pytest.mark.duration(16)
def test_vectorio_move_and_hide(circuitpython):
    """Moving a shape updates the screen and a hidden shape is not drawn."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "first" in output
    assert "second" in output

    captures = circuitpython.display_capture_paths()
    if not captures or not captures[-1].exists():
        pytest.skip("display capture was not produced")

    # The last capture is the one taken after the shape was moved.
    with Image.open(captures[-1]) as img:
        after = img.convert("RGB")

    green = (0, 255, 0)
    black = (0, 0, 0)

    # The hidden circle is not drawn at all.
    assert _pixel_at(after, 40, 40) == black
    # After the move the rectangle is at its new position and the old one is
    # cleared again.
    assert _pixel_at(after, 110, 45) == black
    assert _pixel_at(after, 160, 45) == green


# Corner radius, outlines and the geometry that display_shapes needs. Both
# compositors answer contains() from the same coverage test, so this is the same
# on both even though they rasterize differently.
ROUNDED_CODE = """\
import displayio
import vectorio

palette = displayio.Palette(4)
palette[0] = 0x000000
palette[1] = 0xFF0000
palette[2] = 0x00FF00
palette[3] = 0x0000FF

# A shape's position is its top left corner, so contains() at (10 + dx, 10 + dy)
# asks about the shape's own pixel (dx, dy).
rectangle = vectorio.Rectangle(pixel_shader=palette, width=80, height=40, x=10, y=10,
    radius=12, color_index=1, outline_width=4, outline_color_index=2)
print('defaults', rectangle.radius, rectangle.outline_width,
    rectangle.outline_color_index, rectangle.color_index)

# The corners are cut out of the coverage: the corner pixel is not covered, and
# the middle of each side still is.
print('corners', rectangle.contains(10, 10), rectangle.contains(12, 12),
    rectangle.contains(18, 18), rectangle.contains(10, 30), rectangle.contains(89, 30))

# Radius and outline width must not exceed half of the shortest side. Invalid
# construction and assignments raise ValueError and leave existing values unchanged.
try:
    vectorio.Rectangle(pixel_shader=palette, width=20, height=20, radius=11)
except ValueError:
    print('constructor radius limit: ValueError')
try:
    vectorio.Rectangle(pixel_shader=palette, width=20, height=20, outline_width=11)
except ValueError:
    print('constructor outline limit: ValueError')
try:
    rectangle.radius = 21
except ValueError:
    print('radius limit: ValueError')
try:
    rectangle.outline_width = 21
except ValueError:
    print('outline limit: ValueError')
try:
    rectangle.width = 20
except ValueError:
    print('resize limit: ValueError')
print('limits', rectangle.radius, rectangle.outline_width, rectangle.width)

# The outline is inside the bounds, so it covers the same pixels whether it is set
# or not: contains() only answers coverage, not which color the pixel gets.
rectangle.outline_width = 4
print('band', rectangle.contains(11, 30), rectangle.contains(13, 30),
    rectangle.contains(14, 30), rectangle.contains(14, 14), rectangle.contains(18, 18))
rectangle.outline_color_index = None
print('no outline color', rectangle.contains(11, 30), rectangle.outline_color_index)
rectangle.outline_color_index = 2

# Zero outline width is the same coverage with no outline at all.
rectangle.outline_width = 0
print('no outline', rectangle.contains(11, 30), rectangle.outline_width)

# A transparent palette entry draws nothing, which is how a shape gets a
# transparent fill.
palette.make_transparent(1)
print('transparent fill', rectangle.contains(50, 30))

# The circle ring behaves the same way. Its outline cannot be wider than its
# radius, and shrinking the radius cannot invalidate an existing outline.
try:
    vectorio.Circle(pixel_shader=palette, radius=20, outline_width=21)
except ValueError:
    print('circle constructor outline limit: ValueError')
circle = vectorio.Circle(pixel_shader=palette, radius=20, x=100, y=100,
    color_index=2, outline_width=4, outline_color_index=3)
try:
    circle.outline_width = 21
except ValueError:
    print('circle outline limit: ValueError')
try:
    circle.radius = 3
except ValueError:
    print('circle resize limit: ValueError')
print('circle limits', circle.radius, circle.outline_width)
# The maximum itself is valid.
circle.outline_width = circle.radius
circle.outline_width = 4
print('ring', circle.outline_width, circle.outline_color_index,
    circle.contains(100, 84), circle.contains(100, 100),
    circle.contains(100, 79), circle.contains(100, 121))
circle.outline_width = 0
print('no ring', circle.contains(100, 84))
print('done')
"""

SUBCLASS_CODE = (
    _DISPLAY_HELPER
    + """\
import displayio
import time
import vectorio


class RoundRect(vectorio.Rectangle):
    \"\"\"A display_shapes style rounded rectangle built on vectorio.Rectangle.\"\"\"

    def __init__(self, x, y, width, height, r, *, fill=None, outline=None, stroke=1):
        if width <= 0 or height <= 0:
            raise ValueError("dimensions must be larger than 0")
        if r > min(width, height) / 2:
            raise ValueError("radius cannot exceed half of the smaller side")
        self._palette = displayio.Palette(2)
        self._palette.make_transparent(0)
        self._palette.make_transparent(1)
        super().__init__(pixel_shader=self._palette, width=width, height=height, x=x, y=y,
            color_index=0, radius=r, outline_width=stroke, outline_color_index=1)
        self.fill = fill
        self.outline = outline

    @property
    def fill(self):
        return None if self._palette.is_transparent(0) else self._palette[0]

    @fill.setter
    def fill(self, color):
        if color is None:
            self._palette.make_transparent(0)
        else:
            self._palette[0] = color
            self._palette.make_opaque(0)

    @property
    def outline(self):
        return None if self._palette.is_transparent(1) else self._palette[1]

    @outline.setter
    def outline(self, color):
        if color is None:
            self._palette.make_transparent(1)
        else:
            self._palette[1] = color
            self._palette.make_opaque(1)


# Use display_shapes.RoundRect's default one-pixel stroke, not only the thicker
# outlines exercised by the native API and rendering tests.
round_rect = RoundRect(10, 10, 80, 40, 12, fill=0xFF0000, outline=0x00FF00)
print('size', round_rect.x, round_rect.y, round_rect.width, round_rect.height)
print('shape', round_rect.radius, round_rect.outline_width,
    round_rect.outline_color_index, round_rect.color_index)
print('colors', hex(round_rect.fill), hex(round_rect.outline))
print('shader', round_rect.pixel_shader is round_rect._palette)

# Everything inherited from VectorShape resolves through the native Rectangle.
print('contains', round_rect.contains(50, 30), round_rect.contains(10, 10))
print('location', round_rect.location)
round_rect.x = 30
round_rect.width = 100
round_rect.radius = 20
print('mutated', round_rect.x, round_rect.width, round_rect.radius, round_rect.location)
round_rect.hidden = True
print('hidden', round_rect.hidden)
round_rect.hidden = False
round_rect.fill = None
print('cleared', round_rect.fill, round_rect.contains(50, 30))

group = displayio.Group()
group.append(round_rect)
print('in group', len(group))
other = displayio.Group()
try:
    other.append(round_rect)
except ValueError:
    print('second group: ValueError')


class SubclassedCircle(vectorio.Circle):
    pass


class SubclassedPolygon(vectorio.Polygon):
    pass


# Exercise the native-base conversion for every vectorio shape, not only Rectangle.
circle = SubclassedCircle(pixel_shader=round_rect._palette, radius=20, x=150, y=100,
    color_index=0, outline_width=4, outline_color_index=1)
polygon = SubclassedPolygon(pixel_shader=round_rect._palette,
    points=[(0, 0), (40, 0), (0, 40)], x=220, y=100, color_index=0)
print('other shapes', circle.radius, circle.outline_width, circle.outline_color_index,
    polygon.points, polygon.color_index)
circle.radius = 22
circle.outline_width = 3
polygon.points = [(0, 0), (50, 0), (0, 40)]
polygon.color_index = 1
print('other mutations', circle.radius, circle.outline_width, polygon.points,
    polygon.color_index, circle.contains(150, 100), polygon.contains(225, 105))

# Until super().__init__() runs, the instance has no native Rectangle. Native properties,
# inherited VectorShape properties and Group.append() must raise instead of treating the
# Python instance as a native shape.
class EarlyAccess(vectorio.Rectangle):
    def __init__(self, palette):
        self.early = []
        for name, attempt in (
            ('radius', lambda: self.radius),
            ('x', lambda: self.x),
            ('append', lambda: displayio.Group().append(self)),
        ):
            try:
                attempt()
                self.early.append(name + ' ok')
            except Exception as e:
                self.early.append(name + ' ' + type(e).__name__)
        super().__init__(pixel_shader=palette, width=20, height=20, radius=5)


early = EarlyAccess(round_rect._palette)
print('early access', ', '.join(early.early), early.radius)


# An __init__() that never calls super().__init__() gets its native Rectangle built
# afterwards from the constructor's arguments.
class NoSuperInit(vectorio.Rectangle):
    def __init__(self, **kwargs):
        pass


late = NoSuperInit(pixel_shader=round_rect._palette, width=20, height=20, radius=5)
print('no super init', late.width, late.radius)

# Restore the rectangle after the API checks and render all three Python subclasses. This
# verifies that Group's protocol lookup reaches the native wrapper on both compositors.
round_rect.x = 10
round_rect.width = 80
round_rect.radius = 12
round_rect.fill = 0xFF0000
group.append(circle)
group.append(polygon)
display.auto_refresh = False
display.root_group = group
display.refresh()
print('done')
print('rendered')
while True:
    time.sleep(1)
"""
)

ROUNDED_SCENE_CODE = (
    _DISPLAY_HELPER
    + """\
import displayio
import time
import vectorio

palette = displayio.Palette(4)
palette.make_transparent(0)
palette[1] = 0xFF0000
palette[2] = 0x00FF00
palette[3] = 0x0000FF

group = displayio.Group()

# Rounded rectangle with a 4 pixel outline: x 10..89, y 10..49, radius 12.
rectangle = vectorio.Rectangle(pixel_shader=palette, width=80, height=40, x=10, y=10,
    radius=12, color_index=1, outline_width=4, outline_color_index=2)
group.append(rectangle)

# A hollow one: transparent fill, so only the outline is drawn.
hollow = vectorio.Rectangle(pixel_shader=palette, width=80, height=40, x=110, y=10,
    radius=12, color_index=0, outline_width=4, outline_color_index=2)
group.append(hollow)

# No outline color means no outline, so the fill reaches the edge.
invisible = vectorio.Rectangle(pixel_shader=palette, width=80, height=40, x=210, y=10,
    radius=12, color_index=1, outline_width=4)
group.append(invisible)

# A ring: center (40, 110), radius 20, 4 pixels thick.
circle = vectorio.Circle(pixel_shader=palette, radius=20, x=40, y=110,
    color_index=1, outline_width=4, outline_color_index=2)
group.append(circle)

display.auto_refresh = False
display.root_group = group
display.refresh()
print('rendered')
while True:
    time.sleep(1)
"""
)


@pytest.mark.circuitpy_drive({"code.py": ROUNDED_CODE})
@pytest.mark.duration(15)
def test_vectorio_radius_and_outline_api(circuitpython):
    """Radius, outline width and outline color index are the same on both compositors."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "defaults 12 4 2 1" in output
    # Covered: the flat sides and the diagonal inside the corner arc. Not
    # covered: the corner pixel and everything a pixel beyond the shape.
    assert "corners False False True True True" in output
    # The corner radius and outline width are validated instead of clamped. A
    # resize that would make either one too large is rejected too.
    assert "constructor radius limit: ValueError" in output
    assert "constructor outline limit: ValueError" in output
    assert "radius limit: ValueError" in output
    assert "outline limit: ValueError" in output
    assert "resize limit: ValueError" in output
    assert "limits 12 4 80" in output
    # The 4 pixel outline band is the outline, and the fill starts inside it. An
    # outline covers the same pixels as the fill it replaces.
    assert "band True True True True True" in output
    assert "no outline color True None" in output
    assert "no outline True 0" in output
    assert "transparent fill True" in output
    # The ring covers radius-4..radius, the fill covers everything inside.
    assert "circle constructor outline limit: ValueError" in output
    assert "circle outline limit: ValueError" in output
    assert "circle resize limit: ValueError" in output
    assert "circle limits 20 4" in output
    assert "ring 4 3 True True False False" in output
    assert "no ring True" in output
    assert "done" in output


@pytest.mark.circuitpy_drive({"code.py": SUBCLASS_CODE})
@pytest.mark.display(capture_times_ns=[4_000_000_000])
@pytest.mark.duration(8)
def test_vectorio_python_subclass(circuitpython):
    """Python subclasses of all vectorio shapes work as rendered group members."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "size 10 10 80 40" in output
    assert "shape 12 1 1 0" in output
    assert "colors 0xff0000 0xff00" in output
    assert "shader True" in output
    assert "contains True False" in output
    assert "location (10, 10)" in output
    assert "mutated 30 100 20 (30, 10)" in output
    assert "hidden True" in output
    assert "cleared None True" in output
    assert "in group 1" in output
    assert "second group: ValueError" in output
    assert "other shapes 20 4 1 [(0, 0), (40, 0), (0, 40)] 0" in output
    assert "other mutations 22 3 [(0, 0), (50, 0), (0, 40)] 1 True True" in output
    # Before super().__init__(), every entry point raises instead of reading the instance
    # as a native shape. Without super().__init__(), the native base is built afterwards.
    assert (
        "early access radius NotImplementedError, x NotImplementedError, "
        "append NotImplementedError 5" in output
    )
    assert "no super init 20 5" in output
    assert "done" in output
    assert "rendered" in output

    captures = circuitpython.display_capture_paths()
    if not captures or not captures[0].exists():
        pytest.skip("display capture was not produced")

    with Image.open(captures[0]) as img:
        frame = img.convert("RGB")

    red = (255, 0, 0)
    green = (0, 255, 0)
    black = (0, 0, 0)

    # The subclassed rounded rectangle renders with its inherited native geometry.
    assert _pixel_at(frame, 50, 30) == red
    assert _pixel_at(frame, 50, 10) == green
    assert _pixel_at(frame, 10, 10) == black
    # Circle and Polygon subclasses also reach their native draw protocols.
    assert _pixel_at(frame, 150, 100) == red
    assert _pixel_at(frame, 150, 79) == green
    assert _pixel_at(frame, 225, 105) == green
    assert _pixel_at(frame, 260, 135) == black


@pytest.mark.circuitpy_drive({"code.py": ROUNDED_SCENE_CODE})
@pytest.mark.display(capture_times_ns=[4_000_000_000])
@pytest.mark.duration(8)
def test_vectorio_radius_and_outline_render(circuitpython):
    """Rounded corners and outlines are drawn inside the shape's bounds."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "rendered" in output

    captures = circuitpython.display_capture_paths()
    if not captures or not captures[0].exists():
        pytest.skip("display capture was not produced")

    with Image.open(captures[0]) as img:
        frame = img.convert("RGB")

    red = (255, 0, 0)
    green = (0, 255, 0)
    black = (0, 0, 0)

    def pixel(x, y):
        return tuple(frame.getpixel((x, y)))

    # The fill is the whole rectangle less the outline band.
    assert pixel(50, 30) == red
    # The outline is inside the bounds: 4 pixels in on every flat side, so it
    # never makes the rectangle bigger.
    assert pixel(50, 10) == green
    assert pixel(50, 13) == green
    assert pixel(50, 14) == red
    assert pixel(10, 30) == green
    assert pixel(13, 30) == green
    assert pixel(14, 30) == red
    # The corner is round: well outside the arc is background and the diagonal
    # inside the arc is outline.
    assert pixel(10, 10) == black
    assert pixel(12, 12) == black
    assert pixel(14, 14) == green
    assert pixel(18, 18) == red

    # A transparent fill leaves only the outline.
    assert pixel(150, 30) == black
    assert pixel(150, 11) == green
    assert pixel(111, 30) == green
    assert pixel(110, 10) == black

    # A transparent outline color draws no outline, so the fill reaches the edge.
    assert pixel(250, 10) == red
    assert pixel(211, 30) == red
    assert pixel(210, 10) == black

    # The ring is inside the circle's radius.
    assert pixel(40, 110) == red
    assert pixel(40, 97) == red
    assert pixel(40, 92) == green
    assert pixel(40, 89) == black
