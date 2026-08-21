// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "shared-module/picogame/pg_compat.h"
#include "shared-bindings/picogame/Particles.h"
#include "shared-module/picogame/Particles.h"

static void reset_dirty(picogame_particles_obj_t *self) {
    // INT32 sentinels (not int16): bbox fields are int32 + positions 24.8, so a big-world emitter past
    // +-32767 px must still accumulate. (Matches picogame_dirty_reset; update()/clear() use it too.)
    self->cx1 = self->px1 = 0x7fffffff;
    self->cy1 = self->py1 = 0x7fffffff;
    self->cx2 = self->px2 = -0x7fffffff - 1;
    self->cy2 = self->py2 = -0x7fffffff - 1;
}

//| class Particles:
//|     """A pooled particle layer (small moving dots), drawn as one Scene layer.
//|     Add it to a Scene, ``emit()`` bursts, and call ``tick()`` each frame."""
//|
//|     def __init__(
//|         self, capacity: int, *, size: int = 1, gravity: float = 0.0, fade: bool = False
//|     ) -> None: ...
//|
static mp_obj_t picogame_particles_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_capacity, ARG_size, ARG_gravity, ARG_fade };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_capacity, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_gravity, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_fade, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t cap = mp_arg_validate_int_range(args[ARG_capacity].u_int, 1, 4096, MP_QSTR_capacity);
    mp_int_t size = mp_arg_validate_int_range(args[ARG_size].u_int, 1, 8, MP_QSTR_size);
    mp_float_t gravity = (args[ARG_gravity].u_obj == mp_const_none)
        ? 0.0f : mp_obj_get_float(args[ARG_gravity].u_obj);
    if (gravity > 127.99f) {                       // clamp to the int16 8.8 range: gravity*256 must fit
        gravity = 127.99f;                         // [-32768, 32767]; |g|>=128 would flip the sign
    } else if (gravity < -128.0f) {
        gravity = -128.0f;
    }

    picogame_particles_obj_t *self = mp_obj_malloc(picogame_particles_obj_t, type);
    self->cap = cap;
    self->count = 0;
    self->size = size;
    self->gravity = (int16_t)(gravity * 256);
    self->fade = args[ARG_fade].u_bool;
    // Pure numeric arrays, no Python pointers -> exempt from the conservative GC scan
    // (shorter gc.collect() pauses; fixed at construction, never m_renew'd).
    self->px = m_malloc_without_collect(cap * sizeof(int32_t));
    self->py = m_malloc_without_collect(cap * sizeof(int32_t));
    self->vx = m_malloc_without_collect(cap * sizeof(int16_t));
    self->vy = m_malloc_without_collect(cap * sizeof(int16_t));
    self->life = m_malloc_without_collect(cap * sizeof(uint16_t));
    self->life0 = m_malloc_without_collect(cap * sizeof(uint16_t));
    self->color = m_malloc_without_collect(cap * sizeof(uint16_t));
    reset_dirty(self);
    return MP_OBJ_FROM_PTR(self);
}

//|
//|     def emit(
//|         self, x: int, y: int, count: int, speed: int = 1, life: int = 30, color: int = 0xFFFF
//|     ) -> None:
//|         """Spawn ``count`` particles at (x, y) with random velocity up to ``speed``
//|         px/tick, living ``life`` ticks, in wire-order ``color``."""
//|         ...
//|
static mp_obj_t picogame_particles_emit_fun(size_t n_args, const mp_obj_t *args) {
    picogame_particles_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int x = mp_obj_get_int(args[1]);
    int y = mp_obj_get_int(args[2]);
    int count = mp_arg_validate_int_min(mp_obj_get_int(args[3]), 0, MP_QSTR_count);
    // speed*256 must fit the int16_t velocity (8.8) -> cap 127; life is stored as uint16_t.
    int speed = (n_args > 4) ? mp_arg_validate_int_range(mp_obj_get_int(args[4]), 0, 127, MP_QSTR_speed) : 1;
    int life = (n_args > 5) ? mp_arg_validate_int_range(mp_obj_get_int(args[5]), 1, 65535, MP_QSTR_life) : 30;
    uint16_t color = (n_args > 6)
        ? mp_arg_validate_int_range(mp_obj_get_int(args[6]), 0, 0xFFFF, MP_QSTR_color) : 0xFFFF;
    picogame_particles_emit(self, x, y, count, speed, life, color);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picogame_particles_emit_obj, 4, 7, picogame_particles_emit_fun);

//|     def tick(self) -> None:
//|         """Advance all particles one step (move, gravity, ageing)."""
//|         ...
//|
static mp_obj_t picogame_particles_tick(mp_obj_t self_in) {
    picogame_particles_update(MP_OBJ_TO_PTR(self_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(picogame_particles_tick_obj, picogame_particles_tick);

//|     def clear(self) -> None:
//|         """Remove all particles."""
//|         ...
//|
//|
static mp_obj_t picogame_particles_clear_method(mp_obj_t self_in) {
    picogame_particles_clear(MP_OBJ_TO_PTR(self_in));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(picogame_particles_clear_obj, picogame_particles_clear_method);

static const mp_rom_map_elem_t picogame_particles_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_emit), MP_ROM_PTR(&picogame_particles_emit_obj) },
    { MP_ROM_QSTR(MP_QSTR_tick), MP_ROM_PTR(&picogame_particles_tick_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&picogame_particles_clear_obj) },
};
static MP_DEFINE_CONST_DICT(picogame_particles_locals_dict, picogame_particles_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picogame_particles_type,
    MP_QSTR_Particles,
    MP_TYPE_FLAG_NONE,
    make_new, picogame_particles_make_new,
    locals_dict, &picogame_particles_locals_dict
    );
