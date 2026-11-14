import displayio

# L1: only the low bit counts, 0 is black and 1 is white
c = displayio.ColorConverter(input_colorspace=displayio.Colorspace.L1)
for v in (0, 1, 2, 3, 0xFF):
    print(v, hex(c.convert(v)))
