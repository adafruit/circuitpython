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
#include "common-hal/audiobusio/I2S.h"
#include "shared-bindings/audiobusio/I2S.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/audiocore/__init__.h"
#include "bindings/rp2pio/StateMachine.h"

#include "audio_dma.h"

#include "src/rp2_common/hardware_pio/include/hardware/pio_instructions.h"

#define I2S_CODE(bits_per_sample, out, in, left_justified, swap) \
    { \
/* 00 */ pio_encode_set(pio_y, bits_per_sample - 2) | pio_encode_sideset(2, 0b01 << swap | left_justified << !swap) | pio_encode_delay(1), \
        /* .wrap_target */ \
/* 01 */ (out ? pio_encode_pull(false, false) : pio_encode_nop()) | pio_encode_sideset(2, 0b01 << swap | left_justified << !swap), \
/* 02 */ (out ? pio_encode_mov(pio_x, pio_osr) : pio_encode_nop()) | pio_encode_sideset(2, 0b01 << swap | left_justified << !swap), \
/* 03 */ (out ? pio_encode_out(pio_pins, 1) : pio_encode_nop()) | pio_encode_sideset(2, 0b00) | pio_encode_delay(3), \
/* 04 */ (in ? pio_encode_in(pio_pins, 1) : pio_encode_nop()) | pio_encode_sideset(2, 0b01 << swap), \
/* 05 */ pio_encode_jmp_y_dec(3) | pio_encode_sideset(2, 0b01 << swap) | pio_encode_delay(2), \
/* 06 */ (out ? pio_encode_out(pio_pins, 1) : pio_encode_nop()) | pio_encode_sideset(2, 0b00 | !left_justified << !swap) | pio_encode_delay(3), \
/* 07 */ (in ? pio_encode_in(pio_pins, 1) : pio_encode_nop()) | pio_encode_sideset(2, 0b01 << swap | !left_justified << !swap), \
/* 08 */ pio_encode_set(pio_y, bits_per_sample - 2) | pio_encode_sideset(2, 0b01 << swap | !left_justified << !swap) | pio_encode_delay(2), \
/* 09 */ (out ? pio_encode_out(pio_pins, 1) : pio_encode_nop()) | pio_encode_sideset(2, 0b10 >> swap) | pio_encode_delay(3), \
/* 10 */ (in ? pio_encode_in(pio_pins, 1) : pio_encode_nop()) | pio_encode_sideset(2, 0b11), \
/* 11 */ pio_encode_jmp_y_dec(9) | pio_encode_sideset(2, 0b11) | pio_encode_delay(2), \
/* 12 */ (out ? pio_encode_out(pio_pins, 1) : pio_encode_nop()) | pio_encode_sideset(2, 0b00 | left_justified << !swap) | pio_encode_delay(2), \
/* 13 */ pio_encode_set(pio_y, bits_per_sample - 2) | pio_encode_sideset(2, 0b00 | left_justified << !swap), \
/* 13 */ (in ? pio_encode_in(pio_pins, 1) : pio_encode_nop()) | pio_encode_sideset(2, 0b01 << swap | left_justified << !swap), \
/* 14 */ (in ? pio_encode_push(false, false) : pio_encode_nop()) | pio_encode_sideset(2, 0b01 << swap | left_justified << !swap), \
        /* .wrap */ \
    }

// Caller validates that pins are free.
void common_hal_audiobusio_i2s_construct(audiobusio_i2s_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data_out, const mcu_pin_obj_t *data_in,
    const mcu_pin_obj_t *main_clock, bool left_justified,
    uint32_t buffer_size, uint8_t channel_count, uint32_t sample_rate,
    uint8_t bits_per_sample, bool samples_signed) {
    if (main_clock != NULL) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_main_clock);
    }

    const mcu_pin_obj_t *sideset_pin = NULL;
    bool swap = false;

    if (bit_clock->number == word_select->number - 1) {
        sideset_pin = bit_clock;
    } else if (bit_clock->number == word_select->number + 1) {
        sideset_pin = word_select;
        swap = true;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Bit clock and word select must be sequential GPIO pins"));
    }

    uint16_t program[] = I2S_CODE(bits_per_sample, data_out != NULL, data_in != NULL, left_justified, swap);

    // Use the state machine to manage pins.
    common_hal_rp2pio_statemachine_construct(
        &self->state_machine,
        program, MP_ARRAY_SIZE(program),
        sample_rate * bits_per_sample * 16, // Frequency based on sample rate and bit width
        NULL, 0, // init
        NULL, 0, // may_exec
        data_out, 1, 0, 0xffffffff, // out pin
        data_in, 1, // in pins
        0, 0, // in pulls
        NULL, 1, 0, 0, // set pins
        sideset_pin, 2, false, 0, 0x1f, // sideset pins
        false, // No sideset enable
        NULL, PULL_NONE, // jump pin
        0, // wait gpio pins
        true, // exclusive pin use
        false, 32, false, // out settings
        false, // Wait for txstall
        false, 32, false, // in settings
        false, // Not user-interruptible.
        1, -1, // wrap settings
        PIO_ANY_OFFSET,
        PIO_FIFO_TYPE_DEFAULT,
        PIO_MOV_STATUS_DEFAULT,
        PIO_MOV_N_DEFAULT
        );

    self->buffer_size = buffer_size;
    self->channel_count = channel_count;
    self->sample_rate = sample_rate;
    self->bits_per_sample = bits_per_sample;
    self->samples_signed = samples_signed;

    self->playing = false;
    audio_dma_init(&self->dma);
}

void i2s_configure_audio_dma(audiobusio_i2s_obj_t *self, mp_obj_t sample, bool loop, uint32_t sample_rate, uint8_t bits_per_sample) {

    if (self->dma.output_channel[0] != NUM_DMA_CHANNELS || self->dma.input_channel[0] != NUM_DMA_CHANNELS) {
        if (self->state_machine.out) {
            audio_dma_stop_output(&self->dma);
        }
        if (self->state_machine.in) {
            audio_dma_stop_input(&self->dma);
        }
        audio_dma_deinit(&self->dma);
    }

    common_hal_rp2pio_statemachine_set_frequency(&self->state_machine, sample_rate * bits_per_sample * 16);
    common_hal_rp2pio_statemachine_restart(&self->state_machine);

    // On the RP2040, output registers are always written with a 32-bit write.
    // If the write is 8 or 16 bits wide, the data will be replicated in upper bytes.
    // See section 2.1.4 Narrow IO Register Writes in the RP2040 datasheet.
    // This means that identical 16-bit audio data will be written in both halves of the incoming PIO
    // FIFO register. Thus we get mono-to-stereo conversion for the I2S output for free.
    audio_dma_result result = audio_dma_setup(
        &self->dma,
        sample,
        loop,
        false, // single channel
        0, // audio channel
        true, // output signed
        bits_per_sample,
        (self->state_machine.out ? (uint32_t)&self->state_machine.pio->txf[self->state_machine.state_machine] : 0), // output register
        (self->state_machine.out ? self->state_machine.tx_dreq : 0), // output data request line
        (self->state_machine.in ? (uint32_t)&self->state_machine.pio->rxf[self->state_machine.state_machine] : 0), // input register
        (self->state_machine.in ? self->state_machine.rx_dreq : 0), // input data request line
        false); // swap channel

    if (result == AUDIO_DMA_DMA_BUSY) {
        common_hal_audiobusio_i2s_stop(self);
        mp_raise_RuntimeError(MP_ERROR_TEXT("No DMA channel found"));
    } else if (result == AUDIO_DMA_MEMORY_ERROR) {
        common_hal_audiobusio_i2s_stop(self);
        mp_raise_RuntimeError(MP_ERROR_TEXT("Unable to allocate buffers for signed conversion"));
    }
}

bool common_hal_audiobusio_i2s_deinited(audiobusio_i2s_obj_t *self) {
    return common_hal_rp2pio_statemachine_deinited(&self->state_machine);
}

void common_hal_audiobusio_i2s_deinit(audiobusio_i2s_obj_t *self) {
    if (common_hal_audiobusio_i2s_deinited(self)) {
        return;
    }

    common_hal_rp2pio_statemachine_deinit(&self->state_machine);

    audio_dma_deinit(&self->dma);
}

void common_hal_audiobusio_i2s_play(audiobusio_i2s_obj_t *self,
    mp_obj_t sample, bool loop) {
    if (!self->state_machine.out) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data out"));
    }

    if (common_hal_audiobusio_i2s_get_playing(self)) {
        common_hal_audiobusio_i2s_stop(self);
    }

    uint8_t bits_per_sample = audiosample_bits_per_sample(sample);
    uint32_t sample_rate = audiosample_sample_rate(sample);
    uint8_t channel_count = audiosample_channel_count(sample);
    if (channel_count > 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("Too many channels in sample."));
    }

    if (self->state_machine.in) {
        if (bits_per_sample > self->bits_per_sample) {
            mp_raise_ValueError(MP_ERROR_TEXT("Bits per sample cannot be greater than input."));
        }
        if (sample_rate != self->sample_rate) {
            mp_raise_ValueError(MP_ERROR_TEXT("Sample rate must match."));
        }
    }

    i2s_configure_audio_dma(self, sample, loop, sample_rate, bits_per_sample);
    self->playing = true;
}

void common_hal_audiobusio_i2s_pause(audiobusio_i2s_obj_t *self) {
    if (!self->state_machine.out) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data out"));
    }
    audio_dma_pause(&self->dma);
}

void common_hal_audiobusio_i2s_resume(audiobusio_i2s_obj_t *self) {
    if (!self->state_machine.out) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data out"));
    }
    // Maybe: Clear any overrun/underrun errors
    audio_dma_resume(&self->dma);
}

bool common_hal_audiobusio_i2s_get_paused(audiobusio_i2s_obj_t *self) {
    if (!self->state_machine.out) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data out"));
    }
    return audio_dma_get_paused(&self->dma);
}

void common_hal_audiobusio_i2s_stop(audiobusio_i2s_obj_t *self) {
    if (!self->state_machine.out) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data out"));
    }

    audio_dma_stop(&self->dma);

    common_hal_rp2pio_statemachine_stop(&self->state_machine);

    self->playing = false;
}

bool common_hal_audiobusio_i2s_get_playing(audiobusio_i2s_obj_t *self) {
    if (!self->state_machine.out) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data out"));
    }

    bool playing = audio_dma_get_playing(&self->dma);
    if (!playing && self->playing) {
        common_hal_audiobusio_i2s_stop(self);
    }
    return playing;
}

uint32_t common_hal_audiobusio_i2s_get_sample_rate(audiobusio_i2s_obj_t *self) {
    if (!self->state_machine.in) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data in"));
    }
    return self->sample_rate;
}

uint8_t common_hal_audiobusio_i2s_get_channel_count(audiobusio_i2s_obj_t *self) {
    if (!self->state_machine.in) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data in"));
    }
    return self->channel_count;
}

uint8_t common_hal_audiobusio_i2s_get_bits_per_sample(audiobusio_i2s_obj_t *self) {
    if (!self->state_machine.in) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data in"));
    }
    return self->bits_per_sample;
}

void audiobusio_i2s_reset_buffer(audiobusio_i2s_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {
    if (!self->state_machine.in) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data in"));
    }

    if (single_channel_output) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Single channel output not supported."));
    }

    i2s_configure_audio_dma(self, self, true, self->sample_rate, self->bits_per_sample);
}

audioio_get_buffer_result_t audiobusio_i2s_get_buffer(audiobusio_i2s_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer,
    uint32_t *buffer_length) {
    if (!self->state_machine.in) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data in"));
    }

    if (single_channel_output) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Single channel output not supported."));
    }

    // Do other things while we wait for the buffer to fill.
    while (!audio_dma_has_buffer(&self->dma)) {
        if (self->state_machine.out) {
            common_hal_mcu_delay_us(1000000 / self->sample_rate);
        } else {
            RUN_BACKGROUND_TASKS;
        }
    }

    *buffer_length = self->buffer_size;
    *buffer = audio_dma_get_buffer(&self->dma);
    return GET_BUFFER_MORE_DATA;
}

void audiobusio_i2s_get_buffer_structure(audiobusio_i2s_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing) {
    if (!self->state_machine.in) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No data in"));
    }

    *single_buffer = false;
    *samples_signed = self->samples_signed;
    *max_buffer_length = self->buffer_size;
    if (single_channel_output) {
        *spacing = self->channel_count;
    } else {
        *spacing = 1;
    }
}
