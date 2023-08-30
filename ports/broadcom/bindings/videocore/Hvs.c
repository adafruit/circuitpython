#include <stdlib.h>
#include <stdio.h>

#include "py/obj.h"
#include "py/objproperty.h"

#include "bindings/videocore/Sprite.h"
#include "shared-bindings/displayio/Bitmap.h"
#include "py/runtime.h"

#include "bindings/videocore/hvs.h"
#include "bindings/videocore/Hvs.h"

#if BCM_VERSION == 2711
volatile uint32_t* dlist_memory = (volatile uint32_t*)SCALER5_LIST_MEMORY;
#else
volatile uint32_t* dlist_memory = (volatile uint32_t*)SCALER_LIST_MEMORY;
#endif

extern const mp_obj_type_t hvs_channel_type;
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
  hvs_channel_t *self = MP_OBJ_TO_PTR(self_in);
  mp_obj_t sprite_list = mp_arg_validate_type(list, &mp_type_list, MP_QSTR_sprites);
  size_t len = 0;
  mp_obj_t *items;
  mp_obj_list_get(sprite_list, &len, &items);
  uint32_t needed_slots = 1; // one more, to terminate the list
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
    // early loop)
    dlist_slot = 128;
  }
  uint32_t starting_slot = dlist_slot;
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
MP_DEFINE_CONST_FUN_OBJ_2(fun_set_sprite_list, c_set_sprite_list);

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

simpleprop(width);
simpleprop(height);
simpleprop(enabled);

static const mp_rom_map_elem_t hvs_channel_locals_dict_table[] = {
  prop_entry(width),
  prop_entry(height),
  prop_entry(enabled),
  { MP_ROM_QSTR(MP_QSTR_set_sprite_list), MP_ROM_PTR(&fun_set_sprite_list) },
};

static MP_DEFINE_CONST_DICT(hvs_channel_locals_dict, hvs_channel_locals_dict_table);

const mp_obj_type_t hvs_channel_type = {
  { &mp_type_type },
  .name = MP_QSTR_HvsChannel,
  .locals_dict = (mp_obj_dict_t*)&hvs_channel_locals_dict,
};

