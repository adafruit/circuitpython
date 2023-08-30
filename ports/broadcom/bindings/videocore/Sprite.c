#include <stdlib.h>
#include <stdio.h>

#include "py/obj.h"
#include "py/objproperty.h"

#include "bindings/videocore/Sprite.h"
#include "py/runtime.h"


STATIC mp_obj_t hvs_sprite_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
  sprite_t *sprite = m_new_obj(sprite_t);
  sprite->base.type = &hvs_sprite_type;
  sprite->bitmap = NULL;
  sprite->dirty = true;
  sprite->alpha_mode = alpha_mode_fixed;
  sprite->color_order = HVS_PIXEL_ORDER_ABGR;
  sprite->pixel_format = HVS_PIXEL_FORMAT_RGB565;
  return MP_OBJ_FROM_PTR(sprite);
}

enum hvs_pixel_format bitmap_to_hvs(const displayio_bitmap_t *bitmap) {
  switch (bitmap->bits_per_value) {
  case 16:
    return HVS_PIXEL_FORMAT_RGB565;
  }
  return HVS_PIXEL_FORMAT_RGB332;
}

#if BCM_VERSION == 2711
#error not implemented yet
#else
void hvs_regen_noscale_noviewport(sprite_t *s) {
  uint32_t *d = s->dlist;
  // CTL0
  d[0] = CONTROL_VALID
    | CONTROL_PIXEL_ORDER(s->color_order)
    | CONTROL_UNITY
    | CONTROL_FORMAT(s->pixel_format)
    | CONTROL_WORDS(7);
  // POS0
  d[1] = POS0_X(s->x) | POS0_Y(s->y) | POS0_ALPHA(0xff);
  // POS2, input size
  d[2] = POS2_H(s->bitmap->height) | POS2_W(s->bitmap->width) | (s->alpha_mode << 30);
  // POS3, context
  d[3] = 0xDEADBEEF;
  // PTR0
  d[4] = ((uint32_t)s->bitmap->data) // assumes identity map, should be physical addr
    | 0xc0000000; // and tell HVS to do uncached reads
  // context 0
  d[5] = 0xDEADBEEF;
  // pitch 0
  d[6] = s->bitmap->stride * 4;

  //printf("w: %d, h: %d, stride: %d, bits per value: %d\n", s->bitmap->width, s->bitmap->height, s->bitmap->stride, s->bitmap->bits_per_value);
}
#endif

mp_obj_t c_maybe_regen(mp_obj_t self_in) {
  sprite_t *self = MP_OBJ_TO_PTR(self_in);
  if (self->dirty) {
    //printf("regen time\n");
    hvs_regen_noscale_noviewport(self);
    self->dirty = false;
  } else puts("not dirty");
  return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(fun_maybe_regen, c_maybe_regen);

STATIC mp_obj_t get_image_c(mp_obj_t self_in) {
  sprite_t *self = MP_OBJ_TO_PTR(self_in);
  if (self->bitmap) return MP_OBJ_FROM_PTR(self->bitmap);
  else return mp_const_none;
}
STATIC mp_obj_t set_image_c(mp_obj_t self_in, mp_obj_t value) {
  displayio_bitmap_t *bitmap = mp_arg_validate_type(value, &displayio_bitmap_type, MP_QSTR_bitmap);
  sprite_t *self = MP_OBJ_TO_PTR(self_in);
  self->bitmap = bitmap;
  self->dirty = true;
  if (self->width == 0) self->width = bitmap->width;
  if (self->height == 0) self->height = bitmap->height;
  return value;
}
MP_DEFINE_CONST_FUN_OBJ_1(get_image_fun, get_image_c);
MP_DEFINE_CONST_FUN_OBJ_2(set_image_fun, set_image_c);
MP_PROPERTY_GETSET(image_prop, (mp_obj_t)&get_image_fun, (mp_obj_t)&set_image_fun);

#define simpleprop(name) \
    static mp_obj_t c_getter_##name(mp_obj_t self_in) { sprite_t *self = MP_OBJ_TO_PTR(self_in); return MP_OBJ_NEW_SMALL_INT(self->name); } \
    static mp_obj_t c_setter_##name(mp_obj_t self_in, mp_obj_t value) { sprite_t *self = MP_OBJ_TO_PTR(self_in); self->name = mp_obj_get_int(value); self->dirty = true; return mp_const_none; } \
    static MP_DEFINE_CONST_FUN_OBJ_1(fun_get_##name, c_getter_##name); \
    static MP_DEFINE_CONST_FUN_OBJ_2(fun_set_##name, c_setter_##name); \
    static MP_PROPERTY_GETSET(prop_##name, (mp_obj_t)&fun_get_##name, (mp_obj_t)&fun_set_##name)

#define prop_entry(name) { MP_ROM_QSTR(MP_QSTR_##name), MP_ROM_PTR(&prop_##name) }

simpleprop(width);
simpleprop(height);
simpleprop(x);
simpleprop(y);
simpleprop(color_order);
simpleprop(pixel_format);

STATIC const mp_rom_map_elem_t hvs_sprite_locals_dict_table[] = {
  { MP_ROM_QSTR(MP_QSTR_image), MP_ROM_PTR(&image_prop) },
  prop_entry(width),
  prop_entry(height),
  prop_entry(x),
  prop_entry(y),
  prop_entry(color_order),
  prop_entry(pixel_format),
  { MP_ROM_QSTR(MP_QSTR_maybe_regen), MP_ROM_PTR(&fun_maybe_regen) },
};

STATIC MP_DEFINE_CONST_DICT(hvs_sprite_locals_dict, hvs_sprite_locals_dict_table);

const mp_obj_type_t hvs_sprite_type = {
    { &mp_type_type },
    .flags = 0, //MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_Sprite,
    .locals_dict = (mp_obj_dict_t*)&hvs_sprite_locals_dict,
    .make_new = hvs_sprite_make_new,
    //MP_TYPE_EXTENDED_FIELDS(
    //),
};
