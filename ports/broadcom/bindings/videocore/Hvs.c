#include <stdlib.h>
#include <stdio.h>

#include "py/obj.h"
#include "py/objproperty.h"

#include "bindings/videocore/Sprite.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "shared-module/displayio/Group.h"
#include "shared-bindings/displayio/Group.h"
#include "py/runtime.h"
#include "shared-bindings/displayio/TileGrid.h"
#include "shared-bindings/displayio/Palette.h"

#include "bindings/videocore/hvs.h"
#include "bindings/videocore/Hvs.h"

#if BCM_VERSION == 2711
volatile uint32_t* dlist_memory = (volatile uint32_t*)SCALER5_LIST_MEMORY;
#else
volatile uint32_t* dlist_memory = (volatile uint32_t*)SCALER_LIST_MEMORY;
#endif

volatile struct hvs_channel *hvs_hw_channels = (volatile struct hvs_channel*)SCALER_DISPCTRL0;
uint32_t dlist_slot = 128; // start a bit in, to not trash the firmware list

hvs_channel_t hvs_channels[3] = {
  [0] = {
    .base.type = &hvs_channel_type,
    .channel = 0,
  },
  [1] = {
    .base.type = &hvs_channel_type,
    .channel = 1,
  },
  [2] = {
    .base.type = &hvs_channel_type,
    .channel = 2,
  },
};

static mp_obj_t c_set_sprite_list(mp_obj_t self_in, mp_obj_t list) {
  mp_obj_t *unique_palettes = mp_obj_new_set(0, NULL);
  hvs_channel_t *self = MP_OBJ_TO_PTR(self_in);
  mp_obj_t sprite_list = mp_arg_validate_type(list, &mp_type_list, MP_QSTR_sprites);
  size_t len = 0;
  mp_obj_t *items;
  mp_obj_list_get(sprite_list, &len, &items);
  // first loop, find unique palettes
  for (uint32_t i=0; i<len; i++) {
    mp_obj_t sprite = mp_arg_validate_type(items[i], &hvs_sprite_type, MP_QSTR_todo);
    sprite_t *s = MP_OBJ_TO_PTR(sprite);
    if (s->palette) mp_obj_set_store(unique_palettes, s->palette);
  }
  // TODO, copy each palette to the dlist memory and set sprite->palette_addr to the addr its copied to
  uint32_t needed_slots = 1; // one more, to terminate the list
  // second loop, regenerate any display lists and count the total size
  for (uint32_t i=0; i<len; i++) {
    mp_obj_t sprite = mp_arg_validate_type(items[i], &hvs_sprite_type, MP_QSTR_todo);
    sprite_t *s = MP_OBJ_TO_PTR(sprite);
    c_maybe_regen(sprite);
    uint8_t length = (s->dlist[0] >> 24) & 0x3f;
    needed_slots += length;
  }
  if (needed_slots > (4096/2)) {
    mp_raise_ValueError(translate("too many sprites, unable to pageflip reliably"));
  }
  if ((dlist_slot + needed_slots) > 4096) {
    dlist_slot = 128;
  }
  uint32_t starting_slot = dlist_slot;
  // third loop, copy dlist fragments to hw
  for (uint32_t i=0; i<len; i++) {
    mp_obj_t sprite = mp_arg_validate_type(items[i], &hvs_sprite_type, MP_QSTR_todo);
    sprite_t *s = MP_OBJ_TO_PTR(sprite);
    uint8_t length = (s->dlist[0] >> 24) & 0x3f;
    for (int j=0; j<length; j++) {
      dlist_memory[dlist_slot++] = s->dlist[j];
    }
  }
  dlist_memory[dlist_slot++] = CONTROL_END;
  switch (self->channel) {
  case 0:
    *((volatile uint32_t*)SCALER_DISPLIST0) = starting_slot;
    break;
  case 1:
    *((volatile uint32_t*)SCALER_DISPLIST1) = starting_slot;
    break;
  case 2:
    *((volatile uint32_t*)SCALER_DISPLIST2) = starting_slot;
    break;
  }
  return mp_const_none;
}

static mp_obj_t c_hvs_show(mp_obj_t self_in, mp_obj_t root_group_in) {
  mp_obj_list_t *sprites = mp_obj_new_list(0, NULL);

  hvs_channel_t *self = MP_OBJ_TO_PTR(self_in);
  displayio_group_t *root_group = mp_arg_validate_type(root_group_in, &displayio_group_type, MP_QSTR_bitmap);
  printf("hvs channel: %ld\n", self->channel);
  printf("root group x: %d y: %d\n", root_group->x, root_group->y);
  printf("group scale (not implemented): %d\n", root_group->scale);
  size_t len = 0;
  mp_obj_t *items;
  mp_obj_list_get(root_group->members, &len, &items);
  printf("%d elements\n", len);
  for (uint i=0; i<len; i++) {
    printf("%d: %p\n", i, mp_obj_get_type(items[i]));
    if (mp_obj_get_type(items[i]) == &displayio_tilegrid_type) {
      displayio_tilegrid_t *tg = mp_arg_validate_type(items[i], &displayio_tilegrid_type, MP_QSTR_tilegrid);
      displayio_bitmap_t *bitmap = mp_arg_validate_type(tg->bitmap, &displayio_bitmap_type, MP_QSTR_bitmap);
      uint8_t *arr;
      if (tg->inline_tiles) arr = (uint8_t*)&tg->tiles;
      else arr = tg->tiles;
      /*
      printf("found tilegrid: %p\n", tg);
      printf("x %d, y %d\n", tg->x, tg->y);
      printf("w %d, h %d\n", tg->width_in_tiles, tg->height_in_tiles);
      printf("w %d, h %d\n", tg->pixel_width, tg->pixel_height);
      printf("w %d, h %d\n", tg->tile_width, tg->tile_height);
      printf("shader type: %p\n", mp_obj_get_type(tg->pixel_shader));
      */
      enum hvs_pixel_format format = HVS_PIXEL_FORMAT_RGB332;
      int order = HVS_PIXEL_ORDER_ABGR;
      if (mp_obj_get_type(tg->pixel_shader) == &displayio_palette_type) {
        puts("tg uses a palette");
        format = HVS_PIXEL_FORMAT_PALETTE;
      }
      for (uint row=0; row < tg->height_in_tiles; row++) {
        for (uint col=0; col < tg->width_in_tiles; col++) {
          uint tile_offset = col + (row * tg->width_in_tiles);
          sprite_t *sprite = mp_obj_malloc(sprite_t, &hvs_sprite_type);
          sprite->bitmap = bitmap;
          if (format == HVS_PIXEL_FORMAT_PALETTE) sprite->palette = tg->pixel_shader;
          else sprite->palette = NULL;
          sprite->x = root_group->x + tg->x + (col * tg->tile_width);
          sprite->y = root_group->y + tg->y + (row * tg->tile_height);
          uint tile = arr[tile_offset];
          sprite->x_offset = (tile % tg->bitmap_width_in_tiles) * tg->tile_width;
          sprite->y_offset = (tile / tg->bitmap_width_in_tiles) * tg->tile_height;
          sprite->width = tg->tile_width;
          sprite->height = tg->tile_height;
          sprite->dirty = true;
          sprite->color_order = order;
          sprite->pixel_format = format;
          mp_obj_list_append(sprites, sprite);
        }
      }
    } else {
      puts("unexpected item in group");
    }
  }
  c_set_sprite_list(self_in, sprites);
  return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(fun_set_sprite_list, c_set_sprite_list);
MP_DEFINE_CONST_FUN_OBJ_2(fun_hvs_show, c_hvs_show);

#define simpleprop(name) \
  static MP_DEFINE_CONST_FUN_OBJ_1(fun_get_##name, c_getter_##name); \
  static MP_PROPERTY_GETTER(prop_##name, (mp_obj_t)&fun_get_##name)

#define prop_entry(name) { MP_ROM_QSTR(MP_QSTR_##name), MP_ROM_PTR(&prop_##name) }

static mp_obj_t c_getter_width(mp_obj_t self_in) {
  hvs_channel_t *self = MP_OBJ_TO_PTR(self_in);
  uint32_t ctrl = hvs_hw_channels[self->channel].dispctrl;
  return MP_OBJ_NEW_SMALL_INT((ctrl >> 12) & 0xfff);
}
static mp_obj_t c_getter_height(mp_obj_t self_in) {
  hvs_channel_t *self = MP_OBJ_TO_PTR(self_in);
  uint32_t ctrl = hvs_hw_channels[self->channel].dispctrl;
  return MP_OBJ_NEW_SMALL_INT(ctrl & 0xfff);
}
static mp_obj_t c_getter_enabled(mp_obj_t self_in) {
  hvs_channel_t *self = MP_OBJ_TO_PTR(self_in);
  uint32_t ctrl = hvs_hw_channels[self->channel].dispctrl;
  return mp_obj_new_bool(ctrl & SCALER_DISPCTRLX_ENABLE);
}

static mp_obj_t c_getter_frame(mp_obj_t self_in) {
  hvs_channel_t *self = MP_OBJ_TO_PTR(self_in);
  uint32_t stat = hvs_hw_channels[self->channel].dispstat;
  return MP_OBJ_NEW_SMALL_INT((stat >> 12) & 0x3f);
}

static mp_obj_t c_getter_scanline(mp_obj_t self_in) {
  hvs_channel_t *self = MP_OBJ_TO_PTR(self_in);
  uint32_t stat = hvs_hw_channels[self->channel].dispstat;
  return MP_OBJ_NEW_SMALL_INT(stat & 0xfff);
}

simpleprop(width);
simpleprop(height);
simpleprop(enabled);
simpleprop(frame);
simpleprop(scanline);

static const mp_rom_map_elem_t hvs_channel_locals_dict_table[] = {
  prop_entry(width),
  prop_entry(height),
  prop_entry(enabled),
  prop_entry(frame),
  prop_entry(scanline),
  { MP_ROM_QSTR(MP_QSTR_set_sprite_list), MP_ROM_PTR(&fun_set_sprite_list) },
  { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&fun_hvs_show) },
};

static MP_DEFINE_CONST_DICT(hvs_channel_locals_dict, hvs_channel_locals_dict_table);

const mp_obj_type_t hvs_channel_type = {
  { &mp_type_type },
  .name = MP_QSTR_HvsChannel,
  .locals_dict = (mp_obj_dict_t*)&hvs_channel_locals_dict,
};

