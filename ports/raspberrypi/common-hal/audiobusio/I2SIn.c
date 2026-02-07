// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>

#include "mpconfigport.h"

#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/audiobusio/I2SIn.h"
#include "shared-bindings/audiobusio/I2SIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "bindings/rp2pio/StateMachine.h"

#include "audio_dma.h"

#if CIRCUITPY_AUDIOBUSIO_I2SIN && !CIRCUITPY_AUDIOBUSIO_I2SOUT

const uint16_t i2sin_program_mono[] = {
//     pull block   side 0b11 ; Load OSR with bits_per_sample-2
    0x98a0,
//     out y 8      side 0b11 ; Save the value in y
    0x7848,
//     nop          side 0b01
    0xa842,
//     mov x y      side 0b01
    0xa822,
// lbit:
//     nop          side 0b00 [1]
    0xa142,
//     in pins 1    side 0b01
    0x4801,
//     jmp x-- lbit side 0b01
    0x0844,
//     nop          side 0b10 [1]
    0xb142,
//     in pins 1    side 0b11
    0x5801,
//     mov x y      side 0b11
    0xb822,
// rbit:
//     nop          side 0b10 [1]
    0xb142,
//     nop          side 0b11
    0xb842,
//     jmp x-- rbit side 0b11
    0x184a,
//     nop          side 0b00 [1]
    0xa142,
//     nop          side 0b01
    0xa842,
};

const uint16_t i2sin_program_mono_swap[] = {
//     pull block   side 0b11 ; Load OSR with bits_per_sample-2
    0x98a0,
//     out y 8      side 0b11 ; Save the value in y
    0x7848,
//     nop          side 0b10
    0xb042,
//     mov x y      side 0b10
    0xb022,
// lbit:
//     nop          side 0b00 [1]
    0xa142,
//     in pins 1    side 0b10
    0x5001,
//     jmp x-- lbit side 0b10
    0x1044,
//     nop          side 0b01 [1]
    0xa942,
//     in pins 1    side 0b11
    0x5801,
//     mov x y      side 0b11
    0xb822,
// rbit:
//     nop          side 0b01 [1]
    0xa942,
//     nop          side 0b11
    0xb842,
//     jmp x-- rbit side 0b11
    0x184a,
//     nop          side 0b00 [1]
    0xa142,
//     nop          side 0b10
    0xb042,
};

const uint16_t i2sin_program_stereo[] = {
// ;                       /--- LRCLK
// ;                       |/-- BCLK
// ;                       ||
//     pull block   side 0b11 ; Load OSR with bits_per_sample-2
    0x98a0,
//     out y 8      side 0b11 ; Save the value in y
    0x7848,
//     nop          side 0b01
    0xa842,
//     mov x y      side 0b01
    0xa822,
// lbit:
//     nop          side 0b00 [1]
    0xa142,
//     in pins 1    side 0b01
    0x4801,
//     jmp x-- lbit side 0b01
    0x0844,
//     nop          side 0b10 [1]
    0xb142,
//     in pins 1    side 0b11
    0x5801,
//     mov x y      side 0b11
    0xb822,
// rbit:
//     nop          side 0b10 [1]
    0xb142,
//     in pins 1    side 0b11
    0x5801,
//     jmp x-- rbit side 0b11
    0x184a,
//     nop          side 0b00 [1]
    0xa142,
//     in pins 1    side 0b01
    0x4801,
};

const uint16_t i2sin_program_stereo_swap[] = {
// ;                       /--- LRCLK
// ;                       |/-- BCLK
// ;                       ||
//     pull block   side 0b11 ; Load OSR with bits_per_sample-2
    0x98a0,
//     out y 8      side 0b11 ; Save the value in y
    0x7848,
//     nop          side 0b10
    0xb042,
//     mov x y      side 0b10
    0xb022,
// lbit:
//     nop          side 0b00 [1]
    0xa142,
//     in pins 1    side 0b10
    0x5001,
//     jmp x-- lbit side 0b10
    0x1044,
//     nop          side 0b01 [1]
    0xa942,
//     in pins 1    side 0b11
    0x5801,
//     mov x y      side 0b11
    0xb822,
// rbit:
//     nop          side 0b01 [1]
    0xa942,
//     in pins 1    side 0b11
    0x5801,
//     jmp x-- rbit side 0b11
    0x184a,
//     nop          side 0b00 [1]
    0xa142,
//     in pins 1    side 0b10
    0x5001,
};

// Caller validates that pins are free.
void common_hal_audiobusio_i2sin_construct(audiobusio_i2sin_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select, const mcu_pin_obj_t *data,
    uint32_t buffer_size, uint8_t channel_count, uint32_t sample_rate, uint8_t bits_per_sample,
    bool samples_signed) {

    const mcu_pin_obj_t *sideset_pin = NULL;
    const uint16_t *program = NULL;
    size_t program_len = 0;

    if (bit_clock->number == word_select->number - 1) {
        sideset_pin = bit_clock;

        if (channel_count == 1) {
            program_len = MP_ARRAY_SIZE(i2sin_program_mono);
            program = i2sin_program_mono;
        } else {
            program_len = MP_ARRAY_SIZE(i2sin_program_stereo);
            program = i2sin_program_stereo;
        }

    } else if (bit_clock->number == word_select->number + 1) {
        sideset_pin = word_select;

        if (channel_count == 1) {
            program_len = MP_ARRAY_SIZE(i2sin_program_mono_swap);
            program = i2sin_program_mono_swap;
        } else {
            program_len = MP_ARRAY_SIZE(i2sin_program_stereo_swap);
            program = i2sin_program_stereo_swap;
        }

    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Bit clock and word select must be sequential GPIO pins"));
    }

    // Use the state machine to manage pins.
    common_hal_rp2pio_statemachine_construct(
        &self->state_machine,
        program, program_len,
        sample_rate * bits_per_sample * 2 * 4, // Frequency based on sample rate and bit width
        NULL, 0, // init
        NULL, 0, // may_exec
        NULL, 1, PIO_PINMASK32_NONE, PIO_PINMASK32_NONE, // out pin
        data, 1, // in pins
        PIO_PINMASK32_NONE, PIO_PINMASK32_NONE, // in pulls
        NULL, 1, PIO_PINMASK32_NONE, PIO_PINMASK32_NONE, // set pins
        sideset_pin, 2, false, PIO_PINMASK32_NONE, PIO_PINMASK32_FROM_VALUE(0x1f), // sideset pins
        false, // No sideset enable
        NULL, PULL_NONE, // jump pin
        PIO_PINMASK_NONE, // wait gpio pins
        true, // exclusive pin use
        false, 8, false, // out settings
        false, // Wait for txstall
        true, bits_per_sample, false, // in settings
        false, // Not user-interruptible.
        3, -1, // wrap settings
        PIO_ANY_OFFSET,
        PIO_FIFO_TYPE_DEFAULT,
        PIO_MOV_STATUS_DEFAULT,
        PIO_MOV_N_DEFAULT
        );

    audio_dma_init(&self->dma);

    self->buffer_size = buffer_size;
    self->channel_count = channel_count;
    self->sample_rate = sample_rate;
    self->bits_per_sample = bits_per_sample;
    self->samples_signed = samples_signed;
}

bool common_hal_audiobusio_i2sin_deinited(audiobusio_i2sin_obj_t *self) {
    return common_hal_rp2pio_statemachine_deinited(&self->state_machine);
}

void common_hal_audiobusio_i2sin_deinit(audiobusio_i2sin_obj_t *self) {
    if (common_hal_audiobusio_i2sin_deinited(self)) {
        return;
    }

    common_hal_rp2pio_statemachine_deinit(&self->state_machine);

    audio_dma_deinit(&self->dma);
}

uint32_t common_hal_audiobusio_i2sin_get_sample_rate(audiobusio_i2sin_obj_t *self) {
    return self->sample_rate;
}

uint8_t common_hal_audiobusio_i2sin_get_channel_count(audiobusio_i2sin_obj_t *self) {
    return self->channel_count;
}

uint8_t common_hal_audiobusio_i2sin_get_bits_per_sample(audiobusio_i2sin_obj_t *self) {
    return self->bits_per_sample;
}

void audiobusio_i2sin_reset_buffer(audiobusio_i2sin_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    common_hal_rp2pio_statemachine_restart(&self->state_machine);

    // Send bit width
    const uint8_t bit_width_data[1] = { self->bits_per_sample - 2 };
    common_hal_rp2pio_statemachine_write(&self->state_machine, bit_width_data, 1, 1, false);

    audio_dma_result result = audio_dma_setup_record(
        &self->dma,
        self,
        true,
        single_channel_output, // single channel
        channel, // audio channel
        true, // output signed
        self->bits_per_sample, // output resolution
        (uint32_t)&self->state_machine.pio->rxf[self->state_machine.state_machine],  // input register
        self->state_machine.rx_dreq, // data request line
        false); // swap channel

    if (result == AUDIO_DMA_DMA_BUSY) {
        common_hal_rp2pio_statemachine_stop(&self->state_machine);
        mp_raise_RuntimeError(MP_ERROR_TEXT("No DMA channel found"));
    } else if (result == AUDIO_DMA_MEMORY_ERROR) {
        common_hal_rp2pio_statemachine_stop(&self->state_machine);
        mp_raise_RuntimeError(MP_ERROR_TEXT("Unable to allocate buffers for signed conversion"));
    }

    self->last_index = -1;
}

audioio_get_buffer_result_t audiobusio_i2sin_get_buffer(audiobusio_i2sin_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length) {

    // Do other things while we wait for the buffer to fill.
    while (self->last_index == self->dma.input_index) {
        RUN_BACKGROUND_TASKS;
    }

    *buffer_length = self->buffer_size;
    *buffer = audio_dma_get_buffer(&self->dma);
    return GET_BUFFER_MORE_DATA;
}

void audiobusio_i2sin_get_buffer_structure(audiobusio_i2sin_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing) {

    *single_buffer = false;
    *samples_signed = self->samples_signed;
    *max_buffer_length = self->buffer_size;
    if (single_channel_output) {
        *spacing = self->channel_count;
    } else {
        *spacing = 1;
    }
}

#endif // CIRCUITPY_AUDIOBUSIO_I2SIN && !CIRCUITPY_AUDIOBUSIO_I2SOUT
