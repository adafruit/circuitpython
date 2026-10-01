// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"

#include "shared-bindings/picotts/Engine.h"
#include "shared-module/picotts/Engine.h"

#define PICOTTS_VOICE ((const pico_Char *)"en-US")

// pico_getData is called with at most this many bytes, as in the SVOX examples.
#define PICOTTS_CHUNK_BYTES (1024)

static void check(pico_Status status) {
    if (status != PICO_OK) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q failure: %d"), MP_QSTR_picotts, (int)status);
    }
}

void common_hal_picotts_engine_construct(picotts_engine_obj_t *self,
    mp_obj_t ta, const uint8_t *ta_data, mp_obj_t sg, const uint8_t *sg_data, size_t memory_size) {
    // Pointers in the working memory point only into itself and into the voice buffers, which
    // self->ta and self->sg keep alive, so the GC need not scan it. It is large, so on boards
    // with PSRAM it lands there.
    self->memory = m_malloc_without_collect(memory_size);
    self->memory_size = memory_size;
    self->ta = ta;
    self->sg = sg;
    self->text = mp_const_none;
    self->text_data = NULL;
    self->text_len = 0;
    self->text_pos = 0;
    self->carry_len = 0;
    self->carry_pos = 0;
    check(pico_initialize(self->memory, memory_size, &self->system));
    check(pico_defineVoice(self->system, PICOTTS_VOICE, sg_data, ta_data));
    check(pico_newEngine(self->system, PICOTTS_VOICE, &self->engine));
}

bool common_hal_picotts_engine_deinited(picotts_engine_obj_t *self) {
    return self->memory == NULL;
}

void common_hal_picotts_engine_deinit(picotts_engine_obj_t *self) {
    if (common_hal_picotts_engine_deinited(self)) {
        return;
    }
    pico_disposeEngine(self->system, &self->engine);
    pico_terminate(&self->system);
    m_del(uint8_t, self->memory, self->memory_size);
    self->memory = NULL;
    self->ta = mp_const_none;
    self->sg = mp_const_none;
    self->text = mp_const_none;
    self->text_data = NULL;
}

void common_hal_picotts_engine_stop(picotts_engine_obj_t *self) {
    pico_resetEngine(self->engine, PICO_RESET_SOFT);
    self->text = mp_const_none;
    self->text_data = NULL;
    self->text_len = 0;
    self->text_pos = 0;
    self->carry_len = 0;
    self->carry_pos = 0;
}

void common_hal_picotts_engine_start(picotts_engine_obj_t *self, mp_obj_t text) {
    common_hal_picotts_engine_stop(self);
    size_t len;
    const char *data = mp_obj_str_get_data(text, &len);
    self->text = text;
    self->text_data = data;
    self->text_len = len + 1;  // str data is NUL terminated; the NUL flushes the engine
}

bool common_hal_picotts_engine_get_speaking(picotts_engine_obj_t *self) {
    return self->text_data != NULL || self->carry_pos < self->carry_len;
}

size_t common_hal_picotts_engine_render(picotts_engine_obj_t *self, int16_t *buffer, size_t length) {
    size_t n = 0;
    while (n < length) {
        if (self->carry_pos < self->carry_len) {
            size_t k = MIN(length - n, (size_t)(self->carry_len - self->carry_pos));
            memcpy(buffer + n, self->carry + self->carry_pos, k * sizeof(int16_t));
            self->carry_pos += k;
            n += k;
            continue;
        }
        if (self->text_data == NULL) {
            break;
        }
        if (self->text_pos < self->text_len) {
            size_t remaining = self->text_len - self->text_pos;
            pico_Int16 sent = 0;
            check(pico_putTextUtf8(self->engine,
                (const pico_Char *)self->text_data + self->text_pos,
                (pico_Int16)MIN(remaining, 32767), &sent));
            self->text_pos += sent;
        }
        // Straight into the caller's buffer when an item surely fits, else through carry.
        bool direct = (length - n) * sizeof(int16_t) >= sizeof(self->carry);
        int16_t *dest = direct ? buffer + n : self->carry;
        size_t want = direct ? MIN((length - n) * sizeof(int16_t), PICOTTS_CHUNK_BYTES) : sizeof(self->carry);
        pico_Int16 got = 0;
        pico_Int16 type = 0;
        pico_Status status = pico_getData(self->engine, dest, (pico_Int16)want, &got, &type);
        if (direct) {
            n += got / sizeof(int16_t);
        } else {
            self->carry_len = got / sizeof(int16_t);
            self->carry_pos = 0;
        }
        if (status == PICO_STEP_IDLE && self->text_pos >= self->text_len) {
            // All text is in and the engine has nothing more to give: done once carry drains.
            self->text = mp_const_none;
            self->text_data = NULL;
        } else if (status != PICO_STEP_BUSY && status != PICO_STEP_IDLE) {
            common_hal_picotts_engine_stop(self);
            check(status);
        }
    }
    return n;
}
