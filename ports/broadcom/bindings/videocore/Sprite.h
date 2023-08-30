#pragma once

#include "shared-bindings/displayio/Bitmap.h"
#include "bindings/videocore/hvs.h"

typedef struct {
  mp_obj_base_t base;
  displayio_bitmap_t *bitmap;
  bool dirty;
  uint32_t width;
  uint32_t height;
  uint32_t x;
  uint32_t y;
  uint32_t dlist[32];
  enum alpha_mode alpha_mode;
  uint32_t color_order;
  enum hvs_pixel_format pixel_format;
} sprite_t;

extern const mp_obj_type_t hvs_sprite_type;
mp_obj_t c_maybe_regen(mp_obj_t self_in);
