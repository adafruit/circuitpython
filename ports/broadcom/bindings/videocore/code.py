import videocore
import gifio
import time
import displayio
import bitmaptools

def gif_animate():
  gif = gifio.OnDiskGif("/discord-minecraft.gif", le=True)
  sprite = videocore.Sprite()

  start = time.monotonic()
  next_delay = gif.next_frame()
  end = time.monotonic()
  overhead = end - start
  sprite.image = gif.bitmap
  sprite.color_order = 2
  videocore.HvsChannel1.set_sprite_list([sprite])
  xmax = videocore.HvsChannel1.width - gif.width
  ymax = videocore.HvsChannel1.height - gif.height

  xinc = 3;
  yinc = 4;

  x = 5
  y = 5
  lastframe = videocore.HvsChannel1.frame
  start = time.monotonic()

  while True:
    framedelta = videocore.HvsChannel1.frame - lastframe
    lastframe = videocore.HvsChannel1.frame
    #print(f"delta: {framedelta} scanline: {videocore.HvsChannel1.scanline}/{videocore.HvsChannel1.height} overhead: {overhead} next: {next_delay}")
    end = time.monotonic()
    overhead = end - start

    time.sleep(max(0, next_delay - overhead))

    start = time.monotonic()
    next_delay = gif.next_frame()
    if (x > 0) and (x < xmax):
      x += xinc
    else:
      xinc = xinc * -1
      x += xinc

    if (y > 0) and (y < ymax):
      y += yinc
    else:
      yinc = yinc * -1
      y += yinc

    if x < 0:
      x = 0
    if y < 0:
      y = 0
    sprite.x = x
    sprite.y = y
    videocore.HvsChannel1.set_sprite_list([sprite])
    #print("refresh time:")
    #print(refresh)

def make_color_sweep(pixel_format, y, color_order, red_start, red_max, green_start, green_max, blue_start, blue_max):
  bitmap = displayio.Bitmap(32 * 3, 512 , (1<<16)-1)
  red = red_max
  def green(n):
    return int(n) << green_start
  def blue(n):
    return int(n) << blue_start
  for row in range(512):
    bitmaptools.draw_line(bitmap, 0, row, 32, row, int(red * (row/512)))
    bitmaptools.draw_line(bitmap, 32, row, 64, row, int(green(green_max * (row/512))))
    bitmaptools.draw_line(bitmap, 64, row, 96, row, int(blue(blue_max * (row/512))))
  sprite = videocore.Sprite()
  sprite.image = bitmap
  sprite.color_order = color_order
  sprite.x = y
  sprite.pixel_format = pixel_format
  return sprite

#gif = gifio.OnDiskGif("/ResD1_720X480.gif")

def many_formats():
  dist = 32*4
  format1 = make_color_sweep(1, dist * 0, color_order=2, red_start=0, red_max=15, green_start=4, green_max=15, blue_start=8, blue_max=15)
  format2 = make_color_sweep(2, dist * 1, color_order=2, red_start=0, red_max=31, green_start=5, green_max=31, blue_start=10, blue_max=31)
  format3 = make_color_sweep(3, dist * 2, color_order=2, red_start=0, red_max=31, green_start=5, green_max=31, blue_start=10, blue_max=31)
  format4 = make_color_sweep(4, dist * 3, color_order=3, red_start=0, red_max=31, green_start=5, green_max=63, blue_start = 11, blue_max=31)
  videocore.HvsChannel1.set_sprite_list([format1, format2, format3, format4])

def tilegrid():
  import adafruit_imageload
  import displayio
  import board
  # https://learn.adafruit.com/circuitpython-display-support-using-displayio/sprite-sheet
  sprite_sheet, palette = adafruit_imageload.load("/cp_sprite_sheet.bmp", bitmap=displayio.Bitmap, palette=displayio.Palette)
  sprite = displayio.TileGrid(sprite_sheet, pixel_shader=palette, width=2, height=2, tile_width=16, tile_height=16)
  group = displayio.Group(scale=8)
  group.append(sprite)
  display = board.DISPLAY
  group.x = 30
  group.y = 30
  for i in range(30):
    sprite[0] = i % 6
    sprite[1] = (i+1) % 6
    sprite[2] = (i+2) % 6
    sprite[3] = (i+3) % 6
    for j in range(100):
      group.x = j
      display.show(group)
      return
      time.sleep(0.01)

#many_formats()
#gif_animate()
tilegrid()
