// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/audiobusio/PDMIn.h"

#include "py/runtime.h"

#if CIRCUITPY_AUDIOBUSIO_PDMIN

void common_hal_audiobusio_pdmin_construct(audiobusio_pdmin_obj_t *self,
    const mcu_pin_obj_t *clock_pin, const mcu_pin_obj_t *data_pin,
    uint32_t sample_rate, uint8_t bit_depth, bool mono, uint8_t oversample) {
    mp_raise_NotImplementedError(NULL);
}

bool common_hal_audiobusio_pdmin_deinited(audiobusio_pdmin_obj_t *self) {
    return true;
}

void common_hal_audiobusio_pdmin_deinit(audiobusio_pdmin_obj_t *self) {
}

uint8_t common_hal_audiobusio_pdmin_get_bit_depth(audiobusio_pdmin_obj_t *self) {
    return 0;
}

uint32_t common_hal_audiobusio_pdmin_get_sample_rate(audiobusio_pdmin_obj_t *self) {
    return 0;
}

uint32_t common_hal_audiobusio_pdmin_record_to_buffer(audiobusio_pdmin_obj_t *self,
    uint16_t *output_buffer, uint32_t output_buffer_length) {
    return 0;
}

#endif // CIRCUITPY_AUDIOBUSIO_PDMIN
