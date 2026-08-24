// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-module/synthio/Biquad.h"

typedef struct audiofilters_filter_chain {
    mp_obj_t obj;
    mp_obj_t *objs;
    size_t objs_len;
    biquad_filter_state *states;
} audiofilters_filter_chain_t;

void audiofilters_assign_filter_chain(audiofilters_filter_chain_t *filter_chain, mp_obj_t filter_in, uint8_t channel_count);
void audiofilters_reset_filter_chain(audiofilters_filter_chain_t *filter_chain, uint8_t channel_count);
void audiofilters_tick_filter_chain(audiofilters_filter_chain_t *filter_chain);
int32_t audiofilters_process_filter_chain(audiofilters_filter_chain_t *filter_chain, uint8_t channel_count, uint8_t channel, int32_t word);
void audiofilters_deinit_filter_chain(audiofilters_filter_chain_t *self);
