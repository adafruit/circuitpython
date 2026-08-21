// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame particle system: a pooled set of small moving dots rendered as one
// Scene layer (individual sprites would be far too heavy). Positions/velocities
// are 24.8 / 8.8 fixed-point for sub-pixel motion. Tracks a dirty rect spanning
// the previous and current frames so moving particles leave no trails.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    int32_t *px, *py;     // position, 24.8 fixed-point, scene coords
    int16_t *vx, *vy;     // velocity, 8.8 fixed-point per tick
    uint16_t *life;       // ticks remaining
    uint16_t *life0;      // life at spawn (for the fade ramp)
    uint16_t *color;      // wire-order RGB565
    uint16_t cap, count;
    int16_t gravity;      // 8.8, added to vy each tick
    uint8_t size;         // particle size in pixels
    bool fade;            // dim each particle toward black as it ages
    // dirty bounding boxes (scene coords): previous frame and current frame.
    // int32 (not int16): positions are 24.8 in int32, so emitters past +-32767 px would truncate.
    int32_t cx1, cy1, cx2, cy2;
    int32_t px1, py1, px2, py2;
} picogame_particles_obj_t;

void picogame_particles_emit(picogame_particles_obj_t *ps, int x, int y,
    int count, int speed, int life, uint16_t color);
void picogame_particles_update(picogame_particles_obj_t *ps);
// Remove all particles, marking their last-drawn region dirty once so they get erased.
void picogame_particles_clear(picogame_particles_obj_t *ps);
// Returns true + the dirty rect (scene coords) spanning last+current frames.
bool picogame_particles_take_dirty(picogame_particles_obj_t *ps,
    int *x1, int *y1, int *x2, int *y2);
void picogame_blit_particles(
    uint16_t *buf, int region_w, int strip_top, int strip_h, int x0,
    picogame_particles_obj_t *ps, int ox, int oy);
