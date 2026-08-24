// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#include "shared-module/audiofilters/__init__.h"

void audiofilters_assign_filter_chain(audiofilters_filter_chain_t *self, mp_obj_t filter_in, uint8_t channel_count) {
    size_t n_items;
    mp_obj_t *items;

    if (filter_in == mp_const_none) {
        n_items = 0;
        items = NULL;
    } else if (mp_obj_is_type(filter_in, &mp_type_tuple)) {
        mp_obj_tuple_get(filter_in, &n_items, &items);
        for (size_t i = 0; i < n_items; i++) {
            if (!mp_obj_is_type(items[i], &synthio_biquad_type_obj)) {
                mp_raise_TypeError_varg(
                    MP_ERROR_TEXT("%q in %q must be of type %q, not %q"),
                    MP_QSTR_object,
                    MP_QSTR_filter,
                    MP_QSTR_Biquad,
                    mp_obj_get_type(items[i])->name);
            }
        }
    } else {
        n_items = 1;
        if (!mp_obj_is_type(filter_in, &synthio_biquad_type_obj)) {
            mp_raise_TypeError_varg(
                MP_ERROR_TEXT("%q must be of type %q or %q, not %q"),
                MP_QSTR_filter, MP_QSTR_Biquad, MP_QSTR_tuple, mp_obj_get_type(filter_in)->name);
        }
        items = &self->obj;
    }

    // everything has been checked, so we can do the following without fear

    self->obj = filter_in;
    self->states = m_renew(biquad_filter_state,
        self->states,
        self->objs_len * channel_count,
        n_items * channel_count);
    self->objs = items;
    self->objs_len = n_items;
}

void audiofilters_reset_filter_chain(audiofilters_filter_chain_t *self, uint8_t channel_count) {
    if (self->states) {
        for (uint8_t i = 0; i < self->objs_len * channel_count; i++) {
            synthio_biquad_filter_reset(&self->states[i]);
        }
    }
}

void audiofilters_tick_filter_chain(audiofilters_filter_chain_t *self) {
    for (uint8_t j = 0; j < self->objs_len; j++) {
        common_hal_synthio_biquad_tick(self->objs[j]);
    }
}

int32_t audiofilters_process_filter_chain(audiofilters_filter_chain_t *self, uint8_t channel_count, uint8_t channel, int32_t word) {
    // Process biquad filters
    for (uint8_t j = 0; j < self->objs_len; j++) {
        word = synthio_biquad_filter_sample(self->objs[j], &self->states[j * channel_count + channel], word);
    }
    return word;
}

void audiofilters_deinit_filter_chain(audiofilters_filter_chain_t *self) {
    self->obj = mp_const_none;
    self->objs = NULL;
    self->objs_len = 0;
    self->states = NULL;
}
