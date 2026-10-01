// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/picotts/Engine.h"

extern const mp_obj_type_t picotts_engine_type;

// Allocates memory_size bytes of engine working memory and loads the voice from ta_data and
// sg_data, the contents of the en-US_ta.bin and en-US_lh0_sg.bin files. The engine reads them in
// place for its whole life; ta and sg are the objects that own them, kept alive by the engine.
// Raises MemoryError if the memory cannot be allocated and RuntimeError if the engine fails.
void common_hal_picotts_engine_construct(picotts_engine_obj_t *self,
    mp_obj_t ta, const uint8_t *ta_data, mp_obj_t sg, const uint8_t *sg_data, size_t memory_size);
void common_hal_picotts_engine_deinit(picotts_engine_obj_t *self);
bool common_hal_picotts_engine_deinited(picotts_engine_obj_t *self);
// Starts speaking text, dropping any text not yet rendered.
void common_hal_picotts_engine_start(picotts_engine_obj_t *self, mp_obj_t text);
// Drops any text not yet rendered.
void common_hal_picotts_engine_stop(picotts_engine_obj_t *self);
// True until render() has returned all of the samples for the current text.
bool common_hal_picotts_engine_get_speaking(picotts_engine_obj_t *self);
// Renders up to length samples into buffer. Returns the number written; fewer than length
// only when the text is finished. Raises RuntimeError if the engine fails.
size_t common_hal_picotts_engine_render(picotts_engine_obj_t *self, int16_t *buffer, size_t length);
