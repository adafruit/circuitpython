// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "common-hal/audioi2sin/I2SIn.h"
#include "shared-bindings/audioi2sin/I2SIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "bindings/rp2pio/StateMachine.h"
#include "supervisor/port.h"

#include "hardware/dma.h"

#if CIRCUITPY_AUDIOI2SIN

// 16-bit programs sample a stereo frame of 16+16 = 32 bits and rely on
// auto-push at 32 to deliver one packed (right<<16 | left) word per frame.
// 32-bit programs sample 32 bits per channel and produce two FIFO words per
// frame (right first, then left). Each bit takes 6 PIO clocks (a [2]-delay
// pair). 24-bit recordings reuse the 32-bit programs because most 24-bit
// MEMS mics (SPH0645LM4H, INMP441, ICS-43434) transmit their 24 valid bits
// left-justified inside a 32-bit slot.
//
// `in pins 1` runs on a cycle where side-set drives BCLK high. The slave
// updates the data line on the BCLK falling edge, so by the rising edge it
// has settled and is safe to sample. Sampling at BCLK low (the previous,
// incorrect arrangement) catches the data mid-transition and the result is
// effectively noise.
#define PIO_CLOCKS_PER_BIT (6)

// Master-mode RX, regular pin order (BCLK = WS - 1), Philips alignment.
static const uint16_t i2sin_program[] = {
    0xb842, //  0: nop                    side 3
            //     .wrap_target
    0xf94e, //  1: set    y, 14           side 3 [1]
    0xb242, //  2: nop                    side 2 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xa242, //  5: nop                    side 0 [2]
    0x4801, //  6: in     pins, 1         side 1
    0xe94e, //  7: set    y, 14           side 1 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x4801, //  9: in     pins, 1         side 1
    0x0988, // 10: jmp    y--, 8          side 1 [1]
    0xb242, // 11: nop                    side 2 [2]
    0x5801, // 12: in     pins, 1         side 3
            //     .wrap
};

// Master-mode RX, regular pin order, left-justified.
static const uint16_t i2sin_program_left_justified[] = {
    0xa842, //  0: nop                    side 1
            //     .wrap_target
    0xe94e, //  1: set    y, 14           side 1 [1]
    0xb242, //  2: nop                    side 2 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xb242, //  5: nop                    side 2 [2]
    0x5801, //  6: in     pins, 1         side 3
    0xf94e, //  7: set    y, 14           side 3 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x4801, //  9: in     pins, 1         side 1
    0x0988, // 10: jmp    y--, 8          side 1 [1]
    0xa242, // 11: nop                    side 0 [2]
    0x4801, // 12: in     pins, 1         side 1
            //     .wrap
};

// Master-mode RX, swapped pin order (BCLK = WS + 1), Philips alignment.
static const uint16_t i2sin_program_swap[] = {
    0xb842, //  0: nop                    side 3
            //     .wrap_target
    0xf94e, //  1: set    y, 14           side 3 [1]
    0xaa42, //  2: nop                    side 1 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xa242, //  5: nop                    side 0 [2]
    0x5001, //  6: in     pins, 1         side 2
    0xf14e, //  7: set    y, 14           side 2 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x5001, //  9: in     pins, 1         side 2
    0x1188, // 10: jmp    y--, 8          side 2 [1]
    0xaa42, // 11: nop                    side 1 [2]
    0x5801, // 12: in     pins, 1         side 3
            //     .wrap
};

// Master-mode RX, swapped pin order, left-justified.
static const uint16_t i2sin_program_left_justified_swap[] = {
    0xb042, //  0: nop                    side 2
            //     .wrap_target
    0xf14e, //  1: set    y, 14           side 2 [1]
    0xaa42, //  2: nop                    side 1 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xaa42, //  5: nop                    side 1 [2]
    0x5801, //  6: in     pins, 1         side 3
    0xf94e, //  7: set    y, 14           side 3 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x5001, //  9: in     pins, 1         side 2
    0x1188, // 10: jmp    y--, 8          side 2 [1]
    0xa242, // 11: nop                    side 0 [2]
    0x5001, // 12: in     pins, 1         side 2
            //     .wrap
};

// 32-bit-per-channel variants: identical to the 16-bit programs above except
// the loop counter is set to 30 (so each `bitloop` runs 31 in's, plus one
// outside the loop = 32 in's per channel).
static const uint16_t i2sin_program_32[] = {
    0xb842, //  0: nop                    side 3
            //     .wrap_target
    0xf95e, //  1: set    y, 30           side 3 [1]
    0xb242, //  2: nop                    side 2 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xa242, //  5: nop                    side 0 [2]
    0x4801, //  6: in     pins, 1         side 1
    0xe95e, //  7: set    y, 30           side 1 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x4801, //  9: in     pins, 1         side 1
    0x0988, // 10: jmp    y--, 8          side 1 [1]
    0xb242, // 11: nop                    side 2 [2]
    0x5801, // 12: in     pins, 1         side 3
            //     .wrap
};

static const uint16_t i2sin_program_left_justified_32[] = {
    0xa842, //  0: nop                    side 1
            //     .wrap_target
    0xe95e, //  1: set    y, 30           side 1 [1]
    0xb242, //  2: nop                    side 2 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xb242, //  5: nop                    side 2 [2]
    0x5801, //  6: in     pins, 1         side 3
    0xf95e, //  7: set    y, 30           side 3 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x4801, //  9: in     pins, 1         side 1
    0x0988, // 10: jmp    y--, 8          side 1 [1]
    0xa242, // 11: nop                    side 0 [2]
    0x4801, // 12: in     pins, 1         side 1
            //     .wrap
};

static const uint16_t i2sin_program_swap_32[] = {
    0xb842, //  0: nop                    side 3
            //     .wrap_target
    0xf95e, //  1: set    y, 30           side 3 [1]
    0xaa42, //  2: nop                    side 1 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xa242, //  5: nop                    side 0 [2]
    0x5001, //  6: in     pins, 1         side 2
    0xf15e, //  7: set    y, 30           side 2 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x5001, //  9: in     pins, 1         side 2
    0x1188, // 10: jmp    y--, 8          side 2 [1]
    0xaa42, // 11: nop                    side 1 [2]
    0x5801, // 12: in     pins, 1         side 3
            //     .wrap
};

static const uint16_t i2sin_program_left_justified_swap_32[] = {
    0xb042, //  0: nop                    side 2
            //     .wrap_target
    0xf15e, //  1: set    y, 30           side 2 [1]
    0xaa42, //  2: nop                    side 1 [2]
    0x5801, //  3: in     pins, 1         side 3
    0x1982, //  4: jmp    y--, 2          side 3 [1]
    0xaa42, //  5: nop                    side 1 [2]
    0x5801, //  6: in     pins, 1         side 3
    0xf95e, //  7: set    y, 30           side 3 [1]
    0xa242, //  8: nop                    side 0 [2]
    0x5001, //  9: in     pins, 1         side 2
    0x1188, // 10: jmp    y--, 8          side 2 [1]
    0xa242, // 11: nop                    side 0 [2]
    0x5001, // 12: in     pins, 1         side 2
            //     .wrap
};

// Caller validates that pins are free.
void common_hal_audioi2sin_i2sin_construct(audioi2sin_i2sin_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, const mcu_pin_obj_t *main_clock,
    uint32_t sample_rate, uint8_t bit_depth, uint8_t output_bit_depth,
    bool mono, bool left_justified, bool samples_signed) {

    if (main_clock != NULL) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_main_clock);
    }
    if (bit_depth != 16 && bit_depth != 24 && bit_depth != 32) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be 16, 24, or 32"), MP_QSTR_bit_depth);
    }

    // 24- and 32-bit recordings both clock 32 bits per channel; 24-bit MEMS
    // mics deliver their data left-justified inside that 32-bit slot.
    bool wide = (bit_depth != 16);
    uint32_t bits_per_channel = wide ? 32 : 16;
    uint32_t pio_clocks_per_frame = bits_per_channel * 2 * PIO_CLOCKS_PER_BIT;

    const mcu_pin_obj_t *sideset_pin = NULL;
    const uint16_t *program = NULL;
    size_t program_len = 0;

    if (bit_clock->number == word_select->number - 1) {
        sideset_pin = bit_clock;
        if (left_justified) {
            program = wide ? i2sin_program_left_justified_32 : i2sin_program_left_justified;
            program_len = wide ? MP_ARRAY_SIZE(i2sin_program_left_justified_32)
                               : MP_ARRAY_SIZE(i2sin_program_left_justified);
        } else {
            program = wide ? i2sin_program_32 : i2sin_program;
            program_len = wide ? MP_ARRAY_SIZE(i2sin_program_32)
                               : MP_ARRAY_SIZE(i2sin_program);
        }
    } else if (bit_clock->number == word_select->number + 1) {
        sideset_pin = word_select;
        if (left_justified) {
            program = wide ? i2sin_program_left_justified_swap_32 : i2sin_program_left_justified_swap;
            program_len = wide ? MP_ARRAY_SIZE(i2sin_program_left_justified_swap_32)
                               : MP_ARRAY_SIZE(i2sin_program_left_justified_swap);
        } else {
            program = wide ? i2sin_program_swap_32 : i2sin_program_swap;
            program_len = wide ? MP_ARRAY_SIZE(i2sin_program_swap_32)
                               : MP_ARRAY_SIZE(i2sin_program_swap);
        }
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("Bit clock and word select must be sequential GPIO pins"));
    }

    common_hal_rp2pio_statemachine_construct(
        &self->state_machine,
        program, program_len,
        sample_rate * pio_clocks_per_frame,
        NULL, 0, // init
        NULL, 0, // may_exec
        NULL, 0, PIO_PINMASK32_NONE, PIO_PINMASK32_NONE, // out pin
        data, 1, // in pins
        PIO_PINMASK32_NONE, PIO_PINMASK32_NONE, // in pulls
        NULL, 0, PIO_PINMASK32_NONE, PIO_PINMASK32_FROM_VALUE(0x1f), // set pins
        sideset_pin, 2, false, PIO_PINMASK32_NONE, PIO_PINMASK32_FROM_VALUE(0x1f), // sideset pins
        false, // No sideset enable
        NULL, PULL_NONE, // jump pin
        PIO_PINMASK_NONE, // wait gpio pins
        true, // exclusive pin use
        false, 32, false, // out settings (unused)
        false, // Wait for txstall
        true, 32, false, // in settings: auto-push at 32 bits, shift left (MSB first)
        false, // Not user-interruptible.
        1, -1, // wrap settings
        PIO_ANY_OFFSET,
        PIO_FIFO_TYPE_DEFAULT,
        PIO_MOV_STATUS_DEFAULT,
        PIO_MOV_N_DEFAULT);

    uint32_t actual_frequency = common_hal_rp2pio_statemachine_get_frequency(&self->state_machine);
    self->sample_rate = actual_frequency / pio_clocks_per_frame;
    self->bit_depth = bit_depth;
    self->output_bit_depth = output_bit_depth;
    self->mono = mono;
    self->samples_signed = samples_signed;
    self->left_justified = left_justified;
    self->settled = false;
    self->ring = NULL;
    self->ring_size = 0;
    self->half_size = 0;
    self->read_pos = 0;
    self->dma_channel = -1;
    self->overflow = false;

    // Each PIO frame produces 4 bytes in the FIFO regardless of bit depth
    // (16-bit auto-pushes one packed stereo word, 24/32-bit pushes two
    // separate 32-bit words). One stereo frame is therefore either 4 or
    // 8 bytes; size the half-buffer for ~40 ms of audio so a slow consumer
    // (SD card flush etc.) can complete without overrunning.
    size_t bytes_per_frame = (bit_depth == 16) ? 4 : 8;
    size_t target = (size_t)self->sample_rate * bytes_per_frame * 40 / 1000;
    size_t half_size = (target > 4096) ? target : 4096;
    // Round up to a multiple of 8 so 24/32-bit pair reads never straddle
    // the half boundary.
    half_size = (half_size + 7u) & ~(size_t)7u;

    self->ring = (uint8_t *)port_malloc(2 * half_size, true);
    if (self->ring == NULL) {
        common_hal_rp2pio_statemachine_deinit(&self->state_machine);
        m_malloc_fail(2 * half_size);
    }
    self->half_size = half_size;
    self->ring_size = 2 * half_size;

    common_hal_rp2pio_statemachine_set_read_buffers_raw(&self->state_machine,
        NULL, 0,
        self->ring, half_size,
        self->ring + half_size, half_size);
    if (!common_hal_rp2pio_statemachine_background_read(&self->state_machine, 4, false)) {
        port_free(self->ring);
        self->ring = NULL;
        common_hal_rp2pio_statemachine_deinit(&self->state_machine);
        mp_raise_OSError(MP_EIO);
    }
    self->dma_channel = common_hal_rp2pio_statemachine_get_read_dma_channel(&self->state_machine);
}

bool common_hal_audioi2sin_i2sin_deinited(audioi2sin_i2sin_obj_t *self) {
    return common_hal_rp2pio_statemachine_deinited(&self->state_machine);
}

void common_hal_audioi2sin_i2sin_deinit(audioi2sin_i2sin_obj_t *self) {
    if (common_hal_audioi2sin_i2sin_deinited(self)) {
        return;
    }
    common_hal_rp2pio_statemachine_stop_background_read(&self->state_machine);
    common_hal_rp2pio_statemachine_deinit(&self->state_machine);
    if (self->ring != NULL) {
        port_free(self->ring);
        self->ring = NULL;
    }
    self->ring_size = 0;
    self->half_size = 0;
    self->dma_channel = -1;
}

uint8_t common_hal_audioi2sin_i2sin_get_bit_depth(audioi2sin_i2sin_obj_t *self) {
    return self->bit_depth;
}

uint8_t common_hal_audioi2sin_i2sin_get_output_bit_depth(audioi2sin_i2sin_obj_t *self) {
    return self->output_bit_depth;
}

uint32_t common_hal_audioi2sin_i2sin_get_sample_rate(audioi2sin_i2sin_obj_t *self) {
    return self->sample_rate;
}

bool common_hal_audioi2sin_i2sin_get_samples_signed(audioi2sin_i2sin_obj_t *self) {
    return self->samples_signed;
}

// In 16-bit mode, each PIO frame produces a single 32-bit FIFO word with bits
// 31..16 = right channel and bits 15..0 = left channel (both MSB-first signed
// 16-bit). In 24/32-bit mode each frame produces two FIFO words: right first,
// then left, each MSB-first in the full 32 bits. For mono we keep the left
// channel; for stereo we emit (left, right) pairs.
//
// `output_buffer_length` is the requested number of samples to write (sample
// width = 2 bytes for bit_depth=16, 4 bytes for bit_depth=24 or 32). Returns
// the number actually written.
// Compute the byte offset inside `ring` that the DMA is currently writing
// (one past the last word it finished). Always lies in [0, ring_size).
static size_t i2sin_write_pos(audioi2sin_i2sin_obj_t *self) {
    uintptr_t addr = (uintptr_t)dma_channel_hw_addr(self->dma_channel)->write_addr;
    uintptr_t base = (uintptr_t)self->ring;
    if (addr < base || addr >= base + self->ring_size) {
        // The ISR retargets write_addr to the start of the next half right
        // after a transfer completes; it should always be in-range, but if
        // we observe it mid-update just report "no new data".
        return self->read_pos;
    }
    return (size_t)(addr - base);
}

// 24-bit non-left-justified data arrives in the low 24 bits of the FIFO word
// with the sign bit at bit 23 and bits 31..24 zero. To make it decode
// correctly as int32 (array typecode "i"), lift the sign bit to bit 31.
static inline uint32_t movesign24(uint32_t val) {
    return ((val & 0x800000u) << 8) | (val & 0x7fffffu);
}

// Sign-extend a raw FIFO sample to a canonical int32 value. `raw` holds the
// sample as delivered by the PIO program for the given input bit depth and
// alignment. The returned int32 represents the same magnitude with the sign
// bit at bit 31.
static inline int32_t i2sin_normalize_signed(uint32_t raw, uint8_t in_depth,
    bool left_justified) {
    if (in_depth == 32) {
        return (int32_t)raw;
    }
    if (in_depth == 24) {
        if (left_justified) {
            // value in bits 31..8, sign at 31; arithmetic shift right 8
            return (int32_t)raw >> 8;
        }
        // value in bits 23..0, sign at 23
        uint32_t sign_bit = 0x800000u;
        return (int32_t)((raw ^ sign_bit) - sign_bit);
    }
    // 16-bit: low 16 bits, sign at 15
    return (int16_t)(raw & 0xffffu);
}

// Write `raw` (input-depth bits, just-read FIFO sample) to `buffer` at sample
// index `idx`, converting from `in_depth` to `out_depth` and (if needed)
// flipping the sign bit for the unsigned-WAV convention. Output element size
// follows `out_depth`: 1 byte at 8, 2 bytes at 16, 4 bytes at 24 or 32.
//
// For signed 24-bit output, the int32 slot holds the sign-extended value
// (range -2^23 .. 2^23-1) — unlike the default `output_bit_depth=bit_depth=24`
// path which uses `movesign24`, the converted path returns proper two's
// complement so the result decodes correctly as int32.
static inline void i2sin_write_converted(void *buffer, uint32_t idx,
    uint32_t raw, uint8_t in_depth, uint8_t out_depth,
    bool samples_signed, bool left_justified) {
    int32_t s = i2sin_normalize_signed(raw, in_depth, left_justified);
    int32_t shifted;
    if (out_depth >= in_depth) {
        // Bit-replicate the input across the wider output so that full-scale
        // input maps to full-scale output (e.g. 8-bit 0xFF -> 16-bit 0xFFFF),
        // rather than leaving the new low bits as zero.
        uint32_t in_mask = (in_depth >= 32) ? 0xffffffffu : ((1u << in_depth) - 1u);
        uint32_t in_bits = (uint32_t)s & in_mask;
        uint32_t result = 0;
        int remaining = out_depth;
        while (remaining > 0) {
            int take = (remaining >= (int)in_depth) ? (int)in_depth : remaining;
            result = (result << take) | (in_bits >> (in_depth - take));
            remaining -= take;
        }
        shifted = (int32_t)result;
    } else {
        shifted = s >> (in_depth - out_depth);
    }
    uint32_t u = (uint32_t)shifted;
    if (!samples_signed) {
        if (out_depth >= 32) {
            u ^= 0x80000000u;
        } else {
            uint32_t mask = (1u << out_depth) - 1u;
            u = (u & mask) ^ (1u << (out_depth - 1));
        }
    }
    switch (out_depth) {
        case 8:
            ((uint8_t *)buffer)[idx] = (uint8_t)(u & 0xffu);
            break;
        case 16:
            ((uint16_t *)buffer)[idx] = (uint16_t)(u & 0xffffu);
            break;
        default: // 24 or 32
            ((uint32_t *)buffer)[idx] = u;
            break;
    }
}

uint32_t common_hal_audioi2sin_i2sin_record_to_buffer(audioi2sin_i2sin_obj_t *self,
    void *buffer, uint32_t output_buffer_length) {
    uint32_t output_count = 0;
    const size_t ring_size = self->ring_size;
    const size_t half_size = self->half_size;

    // I2S delivers signed PCM. When the caller asked for unsigned samples,
    // flip the sign bit per sample (XOR with 0x8000 for 16-bit, 0x800000 for
    // 24-bit data in a 32-bit slot, 0x80000000 for 32-bit), matching the WAV
    // convention. The default (no-conversion) path applies the flip to the
    // raw FIFO word before splitting into channels; the conversion path
    // applies the flip per output sample at output bit width.
    const bool convert = self->output_bit_depth != self->bit_depth;
    const uint32_t flip16 = (!convert && !self->samples_signed) ? 0x80008000u : 0u;
    const uint32_t flip32 = (!convert && !self->samples_signed)
        ? (self->bit_depth == 24 ? 0x800000u : 0x80000000u)
        : 0u;
    const bool fix_sign24 = !convert
        && self->bit_depth == 24
        && self->samples_signed
        && !self->left_justified;
    const uint8_t in_depth = self->bit_depth;
    const uint8_t out_depth = self->output_bit_depth;
    const bool left_justified = self->left_justified;
    const bool samples_signed = self->samples_signed;

    if (self->bit_depth == 16) {
        // 16-bit mode auto-pushes one stereo frame per FIFO word. The DMA has
        // been streaming since construct time, so the ring already contains
        // settled data; drop the first 4 bytes once to discard a single
        // pre-record frame (matches the prior synchronous behaviour).
        uint16_t *output = convert ? NULL : (uint16_t *)buffer;
        while (output_count < output_buffer_length) {
            size_t write_pos = i2sin_write_pos(self);
            size_t avail = (write_pos + ring_size - self->read_pos) % ring_size;
            if (avail > half_size) {
                // DMA has filled more than one half ahead of us -- we lost
                // data. Snap to just behind the DMA on a 4-byte boundary.
                self->overflow = true;
                self->read_pos = write_pos & ~(size_t)3u;
                avail = 0;
            }
            if (!self->settled && avail >= 4) {
                self->read_pos = (self->read_pos + 4) % ring_size;
                avail -= 4;
                self->settled = true;
            }
            if (avail < 4) {
                RUN_BACKGROUND_TASKS;
                if (mp_hal_is_interrupted()) {
                    return output_count;
                }
                continue;
            }
            while (avail >= 4 && output_count < output_buffer_length) {
                uint32_t frame = *(volatile uint32_t *)(self->ring + self->read_pos) ^ flip16;
                uint16_t left = (uint16_t)(frame & 0xffff);
                uint16_t right = (uint16_t)(frame >> 16);
                if (!convert) {
                    if (self->mono) {
                        output[output_count++] = left;
                    } else {
                        output[output_count++] = left;
                        if (output_count >= output_buffer_length) {
                            self->read_pos = (self->read_pos + 4) % ring_size;
                            avail -= 4;
                            break;
                        }
                        output[output_count++] = right;
                    }
                } else {
                    i2sin_write_converted(buffer, output_count++, left,
                        in_depth, out_depth, samples_signed, left_justified);
                    if (!self->mono) {
                        if (output_count >= output_buffer_length) {
                            self->read_pos = (self->read_pos + 4) % ring_size;
                            avail -= 4;
                            break;
                        }
                        i2sin_write_converted(buffer, output_count++, right,
                            in_depth, out_depth, samples_signed, left_justified);
                    }
                }
                self->read_pos = (self->read_pos + 4) % ring_size;
                avail -= 4;
            }
        }
    } else {
        // 24-/32-bit mode emits two FIFO pushes per stereo frame (right then
        // left). The state machine was started at instruction 0 by the
        // constructor, so the very first push is the right channel and
        // alternation is preserved as long as we never touch the program
        // counter and always read an even number of words. half_size is a
        // multiple of 8, so reading 8-byte pairs stays aligned across the
        // ring wrap.
        uint32_t *output = convert ? NULL : (uint32_t *)buffer;
        while (output_count < output_buffer_length) {
            size_t write_pos = i2sin_write_pos(self);
            size_t avail = (write_pos + ring_size - self->read_pos) % ring_size;
            if (avail > half_size) {
                // Overflow: snap to a frame-aligned position one half behind
                // the DMA's current half so we resume on the right channel.
                self->overflow = true;
                size_t cur_half = (write_pos < half_size) ? 0 : half_size;
                self->read_pos = (cur_half + half_size) % ring_size;
                self->settled = false;
                avail = (write_pos + ring_size - self->read_pos) % ring_size;
            }
            if (!self->settled && avail >= 8) {
                self->read_pos = (self->read_pos + 8) % ring_size;
                avail -= 8;
                self->settled = true;
            }
            if (avail < 8) {
                RUN_BACKGROUND_TASKS;
                if (mp_hal_is_interrupted()) {
                    return output_count;
                }
                continue;
            }
            while (avail >= 8 && output_count < output_buffer_length) {
                uint32_t right = *(volatile uint32_t *)(self->ring + self->read_pos) ^ flip32;
                size_t next_pos = (self->read_pos + 4) % ring_size;
                uint32_t left = *(volatile uint32_t *)(self->ring + next_pos) ^ flip32;
                if (fix_sign24) {
                    right = movesign24(right);
                    left = movesign24(left);
                }
                if (!convert) {
                    if (self->mono) {
                        output[output_count++] = left;
                    } else {
                        output[output_count++] = left;
                        if (output_count >= output_buffer_length) {
                            self->read_pos = (self->read_pos + 8) % ring_size;
                            avail -= 8;
                            break;
                        }
                        output[output_count++] = right;
                    }
                } else {
                    i2sin_write_converted(buffer, output_count++, left,
                        in_depth, out_depth, samples_signed, left_justified);
                    if (!self->mono) {
                        if (output_count >= output_buffer_length) {
                            self->read_pos = (self->read_pos + 8) % ring_size;
                            avail -= 8;
                            break;
                        }
                        i2sin_write_converted(buffer, output_count++, right,
                            in_depth, out_depth, samples_signed, left_justified);
                    }
                }
                self->read_pos = (self->read_pos + 8) % ring_size;
                avail -= 8;
            }
        }
    }

    return output_count;
}

#endif // CIRCUITPY_AUDIOI2SIN
