// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/memorymap/AddressRange.h"
#include "src/rp2_common/hardware_pio/include/hardware/pio.h"

enum { PIO_ANY_OFFSET = -1 };
enum { PIO_FIFO_JOIN_AUTO = -1, PIO_FIFO_TYPE_DEFAULT = PIO_FIFO_JOIN_AUTO };
enum { PIO_MOV_STATUS_DEFAULT = STATUS_TX_LESSTHAN };
enum { PIO_MOV_N_DEFAULT = 0 };

typedef struct sm_buf_info {
    mp_obj_t obj;
    mp_buffer_info_t info;
} sm_buf_info;

#define RP2PIO_STATEMACHINE_N_BUFS 3

typedef struct {
    mp_obj_base_t base;
    uint32_t pins; // Bitmask of what pins this state machine uses.
    int state_machine;
    PIO pio;
    const uint16_t *init;
    size_t init_len;
    uint32_t initial_pin_state;
    uint32_t initial_pin_direction;
    uint32_t pull_pin_up;
    uint32_t pull_pin_down;
    uint tx_dreq;
    uint rx_dreq;
    uint32_t actual_frequency;
    pio_sm_config sm_config;
    bool in;
    bool out;
    bool wait_for_txstall;
    bool out_shift_right;
    bool in_shift_right;
    bool user_interruptible;
    uint8_t offset;
    uint8_t fifo_depth;  // Either 4 if FIFOs are not joined, or 8 if they are.

    // dma-related items
    volatile int pending_buffers_write;
    volatile int pending_buffers_read;
    int write_buf_index, read_buf_index;
    sm_buf_info write_buf[RP2PIO_STATEMACHINE_N_BUFS];
    sm_buf_info read_buf[RP2PIO_STATEMACHINE_N_BUFS];

    sm_buf_info once_read_buf_info, loop_read_buf_info, loop2_read_buf_info;
    sm_buf_info current_read_buf, next_read_buf_1, next_read_buf_2, next_read_buf_3;
    sm_buf_info once_write_buf_info, loop_write_buf_info, loop2_write_buf_info;
    sm_buf_info current_write_buf, next_write_buf_1, next_write_buf_2, next_write_buf_3;

    bool switched_write_buffers, switched_read_buffers;

    int background_stride_in_bytes;
    bool dma_completed_write, byteswap;
    bool dma_completed_read;
    #if PICO_PIO_VERSION > 0
    memorymap_addressrange_obj_t rxfifo_obj;
    #endif
} rp2pio_statemachine_obj_t;

void reset_rp2pio_statemachine(void);

// Minimal internal version that only fails on pin error (not in use) or full PIO.
bool rp2pio_statemachine_construct(rp2pio_statemachine_obj_t *self,
    const uint16_t *program, size_t program_len,
    size_t frequency,
    const uint16_t *init, size_t init_len,
    const mcu_pin_obj_t *first_out_pin, uint8_t out_pin_count,
    const mcu_pin_obj_t *first_in_pin, uint8_t in_pin_count,
    uint32_t pull_pin_up, uint32_t pull_pin_down,
    const mcu_pin_obj_t *first_set_pin, uint8_t set_pin_count,
    const mcu_pin_obj_t *first_sideset_pin, uint8_t sideset_pin_count, bool sideset_pindirs,
    uint32_t initial_pin_state, uint32_t initial_pin_direction,
    const mcu_pin_obj_t *jmp_pin,
    uint32_t pins_we_use, bool tx_fifo, bool rx_fifo,
    bool auto_pull, uint8_t pull_threshold, bool out_shift_right,
    bool wait_for_txstall,
    bool auto_push, uint8_t push_threshold, bool in_shift_right,
    bool claim_pins,
    bool interruptible,
    bool sideset_enable,
    int wrap_target, int wrap, int offset,
    int fifo_type,
    int mov_status_type, int mov_status_n
    );

uint8_t rp2pio_statemachine_program_offset(rp2pio_statemachine_obj_t *self);

void rp2pio_statemachine_deinit(rp2pio_statemachine_obj_t *self, bool leave_pins);
void rp2pio_statemachine_dma_complete_write(rp2pio_statemachine_obj_t *self, int channel);
void rp2pio_statemachine_dma_complete_read(rp2pio_statemachine_obj_t *self, int channel);

void rp2pio_statemachine_reset_ok(PIO pio, int sm);
void rp2pio_statemachine_never_reset(PIO pio, int sm);

uint8_t rp2pio_statemachine_find_pio(int program_size, int sm_count);

extern const mp_obj_type_t rp2pio_statemachine_type;
