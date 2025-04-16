// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "bindings/rp2pio/StateMachine.h"

#include "common-hal/microcontroller/__init__.h"
#include "shared-bindings/digitalio/Pull.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/memorymap/AddressRange.h"

#include "hardware/platform_defs.h"
#include "hardware/structs/iobank0.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/pio_instructions.h"
#include "hardware/irq.h"

#include "shared/runtime/interrupt_char.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#define NO_DMA_CHANNEL (-1)

// Count how many state machines are using each pin.
static uint8_t _pin_reference_count[NUM_BANK0_GPIOS];
static uint32_t _current_program_id[NUM_PIOS][NUM_PIO_STATE_MACHINES];
static uint8_t _current_program_offset[NUM_PIOS][NUM_PIO_STATE_MACHINES];
static uint8_t _current_program_len[NUM_PIOS][NUM_PIO_STATE_MACHINES];
static bool _never_reset[NUM_PIOS][NUM_PIO_STATE_MACHINES];

static pio_pinmask_t _current_pins[NUM_PIOS];
static pio_pinmask_t _current_sm_pins[NUM_PIOS][NUM_PIO_STATE_MACHINES];

static int8_t _sm_dma_plus_one_write[NUM_PIOS][NUM_PIO_STATE_MACHINES];
static int8_t _sm_dma_plus_one_read[NUM_PIOS][NUM_PIO_STATE_MACHINES];

#define SM_DMA_ALLOCATED_WRITE(pio_index, sm) (_sm_dma_plus_one_write[(pio_index)][(sm)] != 0)
#define SM_DMA_GET_CHANNEL_WRITE(pio_index, sm) (_sm_dma_plus_one_write[(pio_index)][(sm)] - 1)
#define SM_DMA_CLEAR_CHANNEL_WRITE(pio_index, sm) (_sm_dma_plus_one_write[(pio_index)][(sm)] = 0)
#define SM_DMA_SET_CHANNEL_WRITE(pio_index, sm, channel) (_sm_dma_plus_one_write[(pio_index)][(sm)] = (channel) + 1)

#define SM_DMA_ALLOCATED_READ(pio_index, sm) (_sm_dma_plus_one_read[(pio_index)][(sm)] != 0)
#define SM_DMA_GET_CHANNEL_READ(pio_index, sm) (_sm_dma_plus_one_read[(pio_index)][(sm)] - 1)
#define SM_DMA_CLEAR_CHANNEL_READ(pio_index, sm) (_sm_dma_plus_one_read[(pio_index)][(sm)] = 0)
#define SM_DMA_SET_CHANNEL_READ(pio_index, sm, channel) (_sm_dma_plus_one_read[(pio_index)][(sm)] = (channel) + 1)

typedef void (*interrupt_handler_type)(void *);
static interrupt_handler_type _interrupt_handler[NUM_PIOS][NUM_PIO_STATE_MACHINES];
static void *_interrupt_arg[NUM_PIOS][NUM_PIO_STATE_MACHINES];

static void rp2pio_statemachine_interrupt_handler(void);

static void rp2pio_statemachine_set_pull(pio_pinmask_t pull_pin_up, pio_pinmask_t pull_pin_down, pio_pinmask_t pins_we_use) {
    for (size_t i = 0; i < NUM_BANK0_GPIOS; i++) {
        bool used = PIO_PINMASK_IS_SET(pins_we_use, i);
        if (used) {
            bool pull_up = PIO_PINMASK_IS_SET(pull_pin_up, i);
            bool pull_down = PIO_PINMASK_IS_SET(pull_pin_down, i);
            gpio_set_pulls(i, pull_up, pull_down);
        }
    }
}

static void rp2pio_statemachine_clear_dma_write(int pio_index, int sm) {
    if (SM_DMA_ALLOCATED_WRITE(pio_index, sm)) {
        int channel_write = SM_DMA_GET_CHANNEL_WRITE(pio_index, sm);
        uint32_t channel_mask_write = 1u << channel_write;
        dma_hw->inte0 &= ~channel_mask_write;
        if (!dma_hw->inte0) {
            irq_set_mask_enabled(1 << DMA_IRQ_0, false);
        }
        MP_STATE_PORT(background_pio_write)[channel_write] = NULL;
        dma_channel_abort(channel_write);
        dma_channel_unclaim(channel_write);
    }
    SM_DMA_CLEAR_CHANNEL_WRITE(pio_index, sm);
}

static void rp2pio_statemachine_clear_dma_read(int pio_index, int sm) {
    if (SM_DMA_ALLOCATED_READ(pio_index, sm)) {
        int channel_read = SM_DMA_GET_CHANNEL_READ(pio_index, sm);
        uint32_t channel_mask_read = 1u << channel_read;
        dma_hw->inte0 &= ~channel_mask_read;
        if (!dma_hw->inte0) {
            irq_set_mask_enabled(1 << DMA_IRQ_0, false);
        }
        MP_STATE_PORT(background_pio_read)[channel_read] = NULL;
        dma_channel_abort(channel_read);
        dma_channel_unclaim(channel_read);
    }
    SM_DMA_CLEAR_CHANNEL_READ(pio_index, sm);
}

static void _reset_statemachine(PIO pio, uint8_t sm, bool leave_pins) {
    uint8_t pio_index = pio_get_index(pio);
    rp2pio_statemachine_clear_dma_write(pio_index, sm);
    rp2pio_statemachine_clear_dma_read(pio_index, sm);
    uint32_t program_id = _current_program_id[pio_index][sm];
    if (program_id == 0) {
        return;
    }
    _current_program_id[pio_index][sm] = 0;
    bool program_in_use = false;
    for (size_t i = 0; i < NUM_PIO_STATE_MACHINES; i++) {
        if (_current_program_id[pio_index][i] == program_id) {
            program_in_use = true;
            break;
        }
    }
    if (!program_in_use) {
        uint8_t offset = _current_program_offset[pio_index][sm];
        pio_program_t program_struct = {
            .length = _current_program_len[pio_index][sm]
        };
        pio_remove_program(pio, &program_struct, offset);
    }

    pio_pinmask_t pins = _current_sm_pins[pio_index][sm];
    for (size_t pin_number = 0; pin_number < NUM_BANK0_GPIOS; pin_number++) {
        if (!PIO_PINMASK_IS_SET(pins, pin_number)) {
            continue;
        }
        _pin_reference_count[pin_number]--;
        if (_pin_reference_count[pin_number] == 0) {
            if (!leave_pins) {
                reset_pin_number(pin_number);
            }
            PIO_PINMASK_CLEAR(_current_pins[pio_index], pin_number);
        }
    }
    _current_sm_pins[pio_index][sm] = PIO_PINMASK_NONE;
    pio->inte0 &= ~((PIO_IRQ0_INTF_SM0_RXNEMPTY_BITS | PIO_IRQ0_INTF_SM0_TXNFULL_BITS | PIO_IRQ0_INTF_SM0_BITS) << sm);
    pio_sm_unclaim(pio, sm);
}

void reset_rp2pio_statemachine(void) {
    for (size_t i = 0; i < NUM_PIOS; i++) {
        PIO pio = pio_get_instance(i);
        for (size_t j = 0; j < NUM_PIO_STATE_MACHINES; j++) {
            if (_never_reset[i][j]) {
                continue;
            }
            _reset_statemachine(pio, j, false);
        }
    }
    for (uint8_t irq = PIO0_IRQ_0; irq <= PIO1_IRQ_1; irq++) {
        irq_handler_t int_handler = irq_get_exclusive_handler(irq);
        if (int_handler > 0) {
            irq_set_enabled(irq, false);
            irq_remove_handler(irq, int_handler);
        }
    }
}

static pio_pinmask_t _check_pins_free(const mcu_pin_obj_t *first_pin, uint8_t pin_count, bool exclusive_pin_use) {
    pio_pinmask_t pins_we_use = PIO_PINMASK_NONE;
    if (first_pin != NULL) {
        for (size_t i = 0; i < pin_count; i++) {
            uint8_t pin_number = first_pin->number + i;
            if (pin_number >= NUM_BANK0_GPIOS) {
                mp_raise_ValueError(MP_ERROR_TEXT("Pin count too large"));
            }
            const mcu_pin_obj_t *pin = mcu_get_pin_by_number(pin_number);
            if (!pin) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_Pin);
            }

            if (exclusive_pin_use || _pin_reference_count[pin_number] == 0) {
                assert_pin_free(pin);
            }
            PIO_PINMASK_SET(pins_we_use, pin_number);
        }
    }
    return pins_we_use;
}

static enum pio_fifo_join compute_fifo_type(int fifo_type_in, bool rx_fifo, bool tx_fifo) {
    if (fifo_type_in != PIO_FIFO_JOIN_AUTO) {
        return fifo_type_in;
    }
    if (!rx_fifo) {
        return PIO_FIFO_JOIN_TX;
    }
    if (!tx_fifo) {
        return PIO_FIFO_JOIN_RX;
    }
    return PIO_FIFO_JOIN_NONE;
}

static int compute_fifo_depth(enum pio_fifo_join join) {
    if (join == PIO_FIFO_JOIN_TX || join == PIO_FIFO_JOIN_RX) {
        return 8;
    }

    #if PICO_PIO_VERSION > 0
    if (join == PIO_FIFO_JOIN_PUTGET) {
        return 0;
    }
    #endif

    return 4;
}


// from pico-sdk/src/rp2_common/hardware_pio/pio.c
static bool is_gpio_compatible(PIO pio, uint32_t used_gpio_ranges) {
    #if PICO_PIO_VERSION > 0
    bool gpio_base = pio_get_gpio_base(pio);
    return !((gpio_base && (used_gpio_ranges & 1)) ||
        (!gpio_base && (used_gpio_ranges & 4)));
    #else
    ((void)pio);
    ((void)used_gpio_ranges);
    return true;
    #endif
}

static bool use_existing_program(PIO *pio_out, int *sm_out, int *offset_inout, uint32_t program_id, size_t program_len, uint gpio_base, uint gpio_count) {
    uint32_t required_gpio_ranges;
    if (gpio_count) {
        required_gpio_ranges = (1u << (gpio_base >> 4)) |
            (1u << ((gpio_base + gpio_count - 1) >> 4));
    } else {
        required_gpio_ranges = 0;
    }

    for (size_t i = 0; i < NUM_PIOS; i++) {
        PIO pio = pio_get_instance(i);
        if (!is_gpio_compatible(pio, required_gpio_ranges)) {
            continue;
        }
        for (size_t j = 0; j < NUM_PIO_STATE_MACHINES; j++) {
            if (_current_program_id[i][j] == program_id &&
                _current_program_len[i][j] == program_len &&
                (*offset_inout == -1 || *offset_inout == _current_program_offset[i][j])) {
                *sm_out = pio_claim_unused_sm(pio, false);
                if (*sm_out >= 0) {
                    *pio_out = pio;
                    *offset_inout = _current_program_offset[i][j];
                    return true;
                }
            }
        }
    }
    return false;
}

bool rp2pio_statemachine_construct(rp2pio_statemachine_obj_t *self,
    const uint16_t *program, size_t program_len,
    size_t frequency,
    const uint16_t *init, size_t init_len,
    const mcu_pin_obj_t *first_out_pin, uint8_t out_pin_count,
    const mcu_pin_obj_t *first_in_pin, uint8_t in_pin_count,
    pio_pinmask_t pull_pin_up, pio_pinmask_t pull_pin_down, // GPIO numbering
    const mcu_pin_obj_t *first_set_pin, uint8_t set_pin_count,
    const mcu_pin_obj_t *first_sideset_pin, uint8_t sideset_pin_count, bool sideset_pindirs,
    pio_pinmask_t initial_pin_state, pio_pinmask_t initial_pin_direction,
    const mcu_pin_obj_t *jmp_pin,
    pio_pinmask_t pins_we_use, bool tx_fifo, bool rx_fifo,
    bool auto_pull, uint8_t pull_threshold, bool out_shift_right,
    bool wait_for_txstall,
    bool auto_push, uint8_t push_threshold, bool in_shift_right,
    bool claim_pins,
    bool user_interruptible,
    bool sideset_enable,
    int wrap_target, int wrap,
    int offset,
    int fifo_type,
    int mov_status_type, int mov_status_n
    ) {
    // Create a program id that isn't the pointer so we can store it without storing the original object.
    uint32_t program_id = ~((uint32_t)program);

    uint gpio_base = 0, gpio_count = 0;
    #if NUM_BANK0_GPIOS > 32
    if (PIO_PINMASK_VALUE(pins_we_use) >> 32) {
        if (PIO_PINMASK_VALUE(pins_we_use) & 0xffff) {
            // Uses pins from 0-15 and 32-47. not possible
            return false;
        }
    }

    pio_pinmask_value_t v = PIO_PINMASK_VALUE(pins_we_use);
    if (v) {
        while (!(v & 1)) {
            gpio_base++;
            v >>= 1;
        }
        while (v) {
            gpio_count++;
            v >>= 1;
        }
    }
    #endif

    // Next, find a PIO and state machine to use.
    pio_program_t program_struct = {
        .instructions = (uint16_t *)program,
        .length = program_len,
        .origin = offset,
    };
    PIO pio;
    int state_machine;
    bool added = false;

    if (!use_existing_program(&pio, &state_machine, &offset, program_id, program_len, gpio_base, gpio_count)) {
        uint program_offset;
        bool r = pio_claim_free_sm_and_add_program_for_gpio_range(&program_struct, &pio, (uint *)&state_machine, &program_offset, gpio_base, gpio_count, true);
        if (!r) {
            return false;
        }
        offset = program_offset;
        added = true;
    }

    size_t pio_index = pio_get_index(pio);
    for (size_t i = 0; i < NUM_PIOS; i++) {
        if (i == pio_index) {
            continue;
        }
        pio_pinmask_t intersection = PIO_PINMASK_AND(_current_pins[i], pins_we_use);
        if (PIO_PINMASK_VALUE(intersection) != 0) {
            if (added) {
                pio_remove_program(pio, &program_struct, offset);
            }
            pio_sm_unclaim(pio, state_machine);
            // Pin in use by another PIO already.
            return false;
        }
    }

    // Sanity check that state_machine number is valid.
    assert(state_machine >= 0);
    self->pio = pio;
    self->state_machine = state_machine;
    self->offset = offset;
    _current_program_id[pio_index][state_machine] = program_id;
    _current_program_len[pio_index][state_machine] = program_len;
    _current_program_offset[pio_index][state_machine] = offset;
    _current_sm_pins[pio_index][state_machine] = pins_we_use;
    PIO_PINMASK_MERGE(_current_pins[pio_index], pins_we_use);

    pio_sm_set_pins_with_mask64(self->pio, state_machine, PIO_PINMASK_VALUE(initial_pin_state), PIO_PINMASK_VALUE(pins_we_use));
    pio_sm_set_pindirs_with_mask64(self->pio, state_machine, PIO_PINMASK_VALUE(initial_pin_direction), PIO_PINMASK_VALUE(pins_we_use));
    rp2pio_statemachine_set_pull(pull_pin_up, pull_pin_down, pins_we_use);
    self->initial_pin_state = initial_pin_state;
    self->initial_pin_direction = initial_pin_direction;
    self->pull_pin_up = pull_pin_up;
    self->pull_pin_down = pull_pin_down;

    for (size_t pin_number = 0; pin_number < NUM_BANK0_GPIOS; pin_number++) {
        if (!PIO_PINMASK_IS_SET(pins_we_use, pin_number)) {
            continue;
        }
        const mcu_pin_obj_t *pin = mcu_get_pin_by_number(pin_number);
        if (!pin) {
            // TODO: should be impossible, but free resources here anyway
            return false;
        }
        _pin_reference_count[pin_number]++;
        // Also claim the pin at the top level when we're the first to grab it.
        if (_pin_reference_count[pin_number] == 1) {
            if (claim_pins) {
                claim_pin(pin);
            }
            pio_gpio_init(self->pio, pin_number);
        }

        // Use lowest drive level for all State Machine outputs. (#7515
        // workaround). Remove if/when Pin objects get a drive_strength
        // property and use that value instead.
        gpio_set_drive_strength(pin_number, GPIO_DRIVE_STRENGTH_2MA);
    }

    pio_sm_config c = pio_get_default_sm_config();

    if (frequency == 0) {
        frequency = clock_get_hz(clk_sys);
    }
    uint64_t frequency256 = ((uint64_t)clock_get_hz(clk_sys)) * 256;
    uint64_t div256 = frequency256 / frequency;
    if (frequency256 % div256 > 0) {
        div256 += 1;
    }
    self->actual_frequency = frequency256 / div256;
    sm_config_set_clkdiv_int_frac(&c, div256 / 256, div256 % 256);

    if (first_out_pin != NULL) {
        sm_config_set_out_pins(&c, first_out_pin->number, out_pin_count);
    }
    if (first_in_pin != NULL) {
        sm_config_set_in_pins(&c, first_in_pin->number);
    }
    if (first_set_pin != NULL) {
        sm_config_set_set_pins(&c, first_set_pin->number, set_pin_count);
    }
    if (first_sideset_pin != NULL) {
        size_t total_sideset_bits = sideset_pin_count;
        if (sideset_enable) {
            total_sideset_bits += 1;
        }
        sm_config_set_sideset(&c, total_sideset_bits, sideset_enable, sideset_pindirs);
        sm_config_set_sideset_pins(&c, first_sideset_pin->number);
    }
    if (jmp_pin != NULL) {
        sm_config_set_jmp_pin(&c, jmp_pin->number);
    }

    mp_arg_validate_int_range(wrap, -1, program_len - 1, MP_QSTR_wrap);
    if (wrap == -1) {
        wrap = program_len - 1;
    }

    mp_arg_validate_int_range(wrap_target, 0, program_len - 1, MP_QSTR_wrap_target);

    wrap += offset;
    wrap_target += offset;

    sm_config_set_wrap(&c, wrap_target, wrap);
    sm_config_set_in_shift(&c, in_shift_right, auto_push, push_threshold);
    #if PICO_PIO_VERSION > 0
    sm_config_set_in_pin_count(&c, in_pin_count);
    #endif

    sm_config_set_out_shift(&c, out_shift_right, auto_pull, pull_threshold);
    sm_config_set_out_pin_count(&c, out_pin_count);

    sm_config_set_set_pin_count(&c, set_pin_count);

    enum pio_fifo_join join = compute_fifo_type(fifo_type, rx_fifo, tx_fifo);

    self->fifo_depth = compute_fifo_depth(join);

    #if PICO_PIO_VERSION > 0
    if (fifo_type == PIO_FIFO_JOIN_TXPUT || fifo_type == PIO_FIFO_JOIN_TXGET) {
        self->rxfifo_obj.base.type = &memorymap_addressrange_type;
        common_hal_memorymap_addressrange_construct(&self->rxfifo_obj, (uint8_t *)self->pio->rxf_putget[self->state_machine], 4 * sizeof(uint32_t));
    } else {
        self->rxfifo_obj.base.type = NULL;
    }
    #endif

    if (rx_fifo) {
        self->rx_dreq = pio_get_dreq(self->pio, self->state_machine, false);
    }
    if (tx_fifo) {
        self->tx_dreq = pio_get_dreq(self->pio, self->state_machine, true);
    }
    self->in = rx_fifo;
    self->out = tx_fifo;
    self->out_shift_right = out_shift_right;
    self->in_shift_right = in_shift_right;
    self->wait_for_txstall = wait_for_txstall;
    self->user_interruptible = user_interruptible;

    self->init = init;
    self->init_len = init_len;

    sm_config_set_fifo_join(&c, join);

    sm_config_set_mov_status(&c, mov_status_type, mov_status_n);

    // TODO: these arguments
    // int set_count, int out_count

    self->sm_config = c;

    // no DMA allocated
    SM_DMA_CLEAR_CHANNEL_READ(pio_index, state_machine);
    SM_DMA_CLEAR_CHANNEL_WRITE(pio_index, state_machine);

    pio_sm_init(self->pio, self->state_machine, offset, &c);
    common_hal_rp2pio_statemachine_run(self, init, init_len);

    common_hal_rp2pio_statemachine_set_frequency(self, frequency);
    pio_sm_set_enabled(self->pio, self->state_machine, true);
    return true;
}

static pio_pinmask_t mask_and_shift(const mcu_pin_obj_t *first_pin, uint32_t bit_count, pio_pinmask32_t value_in) {
    if (!first_pin) {
        return PIO_PINMASK_NONE;
    }
    pio_pinmask_value_t mask = (PIO_PINMASK_C(1) << bit_count) - 1;
    pio_pinmask_value_t value = (pio_pinmask_value_t)PIO_PINMASK32_VALUE(value_in);
    int shift = first_pin->number;
    return PIO_PINMASK_FROM_VALUE((value & mask) << shift);
}

typedef struct {
    struct {
        pio_pinmask_t pins_we_use;
        uint8_t in_pin_count, out_pin_count, pio_gpio_offset;
        bool has_jmp_pin, auto_push, auto_pull, has_in_pin, has_out_pin, has_set_pin;
    } inputs;
    struct {
        bool tx_fifo, rx_fifo, in_loaded, out_loaded, in_used, out_used;
    } outputs;
} introspect_t;

static void consider_instruction(introspect_t *state, uint16_t full_instruction, qstr what_program, size_t i) {
    uint16_t instruction = full_instruction & 0xe000;
    if (instruction == 0x8000) {
        if ((full_instruction & 0xe080) == pio_instr_bits_push) {
            state->outputs.rx_fifo = true;
            state->outputs.in_loaded = true;
        } else { // pull otherwise.
            state->outputs.tx_fifo = true;
            state->outputs.out_loaded = true;
        }
    }
    if (instruction == pio_instr_bits_jmp) {
        uint16_t condition = (full_instruction & 0x00e0) >> 5;
        if ((condition == 0x6) && !state->inputs.has_jmp_pin) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Missing jmp_pin. %q[%u] jumps on pin"), what_program, i);
        }
    }
    if (instruction == pio_instr_bits_wait) {
        uint16_t wait_source = (full_instruction & 0x0060) >> 5;
        uint16_t wait_index = (full_instruction & 0x001f) + state->inputs.pio_gpio_offset;
        if (wait_source == 0 && !PIO_PINMASK_IS_SET(state->inputs.pins_we_use, wait_index)) { // GPIO
            mp_raise_ValueError_varg(MP_ERROR_TEXT("%q[%u] uses extra pin"), what_program, i);
        } else if (wait_source == 1) { // Input pin
            if (!state->inputs.has_in_pin) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("Missing first_in_pin. %q[%u] waits based on pin"), what_program, i);
            }
            if (wait_index >= state->inputs.in_pin_count) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("%q[%u] waits on input outside of count"), what_program, i);
            }
        }
    }
    if (instruction == pio_instr_bits_in) {
        uint16_t source = (full_instruction & 0x00e0) >> 5;
        uint16_t bit_count = full_instruction & 0x001f;
        if (source == 0) {
            if (!state->inputs.has_in_pin) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("Missing first_in_pin. %q[%u] shifts in from pin(s)"), what_program, i);
            }
            if (bit_count > state->inputs.in_pin_count) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("%q[%u] shifts in more bits than pin count"), what_program, i);
            }
        }
        if (state->inputs.auto_push) {
            state->outputs.in_loaded = true;
            state->outputs.rx_fifo = true;
        }
        state->outputs.in_used = true;
    }
    if (instruction == pio_instr_bits_out) {
        uint16_t bit_count = full_instruction & 0x001f;
        uint16_t destination = (full_instruction & 0x00e0) >> 5;
        // Check for pins or pindirs destination.
        if (destination == 0x0 || destination == 0x4) {
            if (!state->inputs.has_out_pin) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("Missing first_out_pin. %q[%u] shifts out to pin(s)"), what_program, i);
            }
            if (bit_count > state->inputs.out_pin_count) {
                mp_raise_ValueError_varg(MP_ERROR_TEXT("%q[%u] shifts out more bits than pin count"), what_program, i);
            }
        }
        if (state->inputs.auto_pull) {
            state->outputs.out_loaded = true;
            state->outputs.tx_fifo = true;
        }
        state->outputs.out_used = true;
    }
    if (instruction == pio_instr_bits_set) {
        uint16_t destination = (full_instruction & 0x00e0) >> 5;
        // Check for pins or pindirs destination.
        if ((destination == 0x00 || destination == 0x4) && !state->inputs.has_set_pin) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Missing first_set_pin. %q[%u] sets pin(s)"), what_program, i);
        }
    }
    if (instruction == pio_instr_bits_mov) {
        uint16_t source = full_instruction & 0x0007;
        uint16_t destination = (full_instruction & 0x00e0) >> 5;
        // Check for pins or pindirs destination.
        if (destination == 0x0 && !state->inputs.has_out_pin) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Missing first_out_pin. %q[%u] writes pin(s)"), what_program, i);
        }
        if (source == 0x0 && !state->inputs.has_in_pin) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Missing first_in_pin. %q[%u] reads pin(s)"), what_program, i);
        }
        if (destination == 0x6) {
            state->outputs.in_loaded = true;
        } else if (destination == 0x7) {
            state->outputs.out_loaded = true;
        }
    }
}

static void consider_program(introspect_t *state, const uint16_t *program, size_t program_len, qstr what_program) {
    for (size_t i = 0; i < program_len; i++) {
        consider_instruction(state, program[i], what_program, i);
    }
}

void common_hal_rp2pio_statemachine_construct(rp2pio_statemachine_obj_t *self,
    const uint16_t *program, size_t program_len,
    size_t frequency,
    const uint16_t *init, size_t init_len,
    const uint16_t *may_exec, size_t may_exec_len,
    const mcu_pin_obj_t *first_out_pin, uint8_t out_pin_count, pio_pinmask32_t initial_out_pin_state32, pio_pinmask32_t initial_out_pin_direction32,
    const mcu_pin_obj_t *first_in_pin, uint8_t in_pin_count,
    pio_pinmask32_t in_pull_pin_up32, pio_pinmask32_t in_pull_pin_down32, // relative to first_in_pin
    const mcu_pin_obj_t *first_set_pin, uint8_t set_pin_count, pio_pinmask32_t initial_set_pin_state32, pio_pinmask32_t initial_set_pin_direction32,
    const mcu_pin_obj_t *first_sideset_pin, uint8_t sideset_pin_count, bool sideset_pindirs,
    pio_pinmask32_t initial_sideset_pin_state32, pio_pinmask32_t initial_sideset_pin_direction32,
    bool sideset_enable,
    const mcu_pin_obj_t *jmp_pin, digitalio_pull_t jmp_pull,
    pio_pinmask_t wait_gpio_mask,
    bool exclusive_pin_use,
    bool auto_pull, uint8_t pull_threshold, bool out_shift_right,
    bool wait_for_txstall,
    bool auto_push, uint8_t push_threshold, bool in_shift_right,
    bool user_interruptible,
    int wrap_target, int wrap,
    int offset,
    int fifo_type,
    int mov_status_type,
    int mov_status_n) {

    // First, check that all pins are free OR already in use by any PIO if exclusive_pin_use is false.
    pio_pinmask_t pins_we_use = wait_gpio_mask;
    PIO_PINMASK_MERGE(pins_we_use, _check_pins_free(first_out_pin, out_pin_count, exclusive_pin_use));
    PIO_PINMASK_MERGE(pins_we_use, _check_pins_free(first_in_pin, in_pin_count, exclusive_pin_use));
    PIO_PINMASK_MERGE(pins_we_use, _check_pins_free(first_set_pin, set_pin_count, exclusive_pin_use));
    PIO_PINMASK_MERGE(pins_we_use, _check_pins_free(first_sideset_pin, sideset_pin_count, exclusive_pin_use));
    PIO_PINMASK_MERGE(pins_we_use, _check_pins_free(jmp_pin, 1, exclusive_pin_use));

    int pio_gpio_offset = 0;
    #if NUM_BANK0_GPIOS > 32
    if (PIO_PINMASK_VALUE(pins_we_use) >> 32) {
        pio_gpio_offset = 16;
        if (PIO_PINMASK_VALUE(pins_we_use) & 0xffff) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("Cannot use GPIO0..15 together with GPIO32..47"));
        }
    }
    #endif

    // Look through the program to see what we reference and make sure it was provided.
    introspect_t state = {
        .inputs = {
            .pins_we_use = pins_we_use,
            .pio_gpio_offset = pio_gpio_offset,
            .has_jmp_pin = jmp_pin != NULL,
            .has_in_pin = first_in_pin != NULL,
            .has_out_pin = first_out_pin != NULL,
            .has_set_pin = first_set_pin != NULL,
            .in_pin_count = in_pin_count,
            .out_pin_count = out_pin_count,
            .auto_pull = auto_pull,
            .auto_push = auto_push,
        }
    };
    consider_program(&state, program, program_len, MP_QSTR_program);
    consider_program(&state, init, init_len, MP_QSTR_init);
    consider_program(&state, may_exec, may_exec_len, MP_QSTR_may_exec);

    if (!state.outputs.in_loaded && state.outputs.in_used) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Program does IN without loading ISR"));
    }
    if (!state.outputs.out_loaded && state.outputs.out_used) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Program does OUT without loading OSR"));
    }

    pio_pinmask_t initial_pin_state = mask_and_shift(first_out_pin, out_pin_count, initial_out_pin_state32);
    pio_pinmask_t initial_pin_direction = mask_and_shift(first_out_pin, out_pin_count, initial_out_pin_direction32);
    pio_pinmask_t initial_set_pin_state = mask_and_shift(first_set_pin, set_pin_count, initial_set_pin_state32);
    pio_pinmask_t initial_set_pin_direction = mask_and_shift(first_set_pin, set_pin_count, initial_set_pin_direction32);
    pio_pinmask_t set_out_overlap = PIO_PINMASK_AND(mask_and_shift(first_out_pin, out_pin_count, PIO_PINMASK32_ALL),
        mask_and_shift(first_set_pin, set_pin_count, PIO_PINMASK32_ALL));
    // Check that OUT and SET settings agree because we don't have a way of picking one over the other.
    if (!PIO_PINMASK_EQUAL(
        PIO_PINMASK_AND(initial_pin_state, set_out_overlap),
        PIO_PINMASK_AND(initial_set_pin_state, set_out_overlap))) {
        mp_raise_ValueError(MP_ERROR_TEXT("Initial set pin state conflicts with initial out pin state"));
    }
    if (!PIO_PINMASK_EQUAL(
        PIO_PINMASK_AND(initial_pin_direction, set_out_overlap),
        PIO_PINMASK_AND(initial_set_pin_direction, set_out_overlap))) {
        mp_raise_ValueError(MP_ERROR_TEXT("Initial set pin direction conflicts with initial out pin direction"));
    }
    PIO_PINMASK_MERGE(initial_pin_state, initial_set_pin_state);
    PIO_PINMASK_MERGE(initial_pin_direction, initial_set_pin_direction);

    // Sideset overrides OUT or SET so we always use its values.
    pio_pinmask_t sideset_mask = mask_and_shift(first_sideset_pin, sideset_pin_count, PIO_PINMASK32_FROM_VALUE(0x1f));
    initial_pin_state = PIO_PINMASK_OR(
        PIO_PINMASK_AND_NOT(initial_pin_state, sideset_mask),
        mask_and_shift(first_sideset_pin, sideset_pin_count, initial_sideset_pin_state32));
    initial_pin_direction = PIO_PINMASK_OR(
        PIO_PINMASK_AND_NOT(initial_pin_direction, sideset_mask),
        mask_and_shift(first_sideset_pin, sideset_pin_count, initial_sideset_pin_direction32));

    // Deal with pull up/downs
    pio_pinmask_t pull_up = mask_and_shift(first_in_pin, in_pin_count, in_pull_pin_up32);
    pio_pinmask_t pull_down = mask_and_shift(first_in_pin, in_pin_count, in_pull_pin_down32);

    if (jmp_pin) {
        pio_pinmask_t jmp_mask = mask_and_shift(jmp_pin, 1, PIO_PINMASK32_FROM_VALUE(0x1f));
        if (jmp_pull == PULL_UP) {
            PIO_PINMASK_MERGE(pull_up, jmp_mask);
        }
        if (jmp_pull == PULL_DOWN) {
            PIO_PINMASK_MERGE(pull_down, jmp_mask);
        }
    }
    if (PIO_PINMASK_VALUE(
        PIO_PINMASK_AND(initial_pin_direction,
            PIO_PINMASK_OR(pull_up, pull_down)))) {
        mp_raise_ValueError(MP_ERROR_TEXT("pull masks conflict with direction masks"));
    }

    bool ok = rp2pio_statemachine_construct(
        self,
        program, program_len,
        frequency,
        init, init_len,
        first_out_pin, out_pin_count,
        first_in_pin, in_pin_count,
        pull_up, pull_down,
        first_set_pin, set_pin_count,
        first_sideset_pin, sideset_pin_count, sideset_pindirs,
        initial_pin_state, initial_pin_direction,
        jmp_pin,
        pins_we_use, state.outputs.tx_fifo, state.outputs.rx_fifo,
        auto_pull, pull_threshold, out_shift_right,
        wait_for_txstall,
        auto_push, push_threshold, in_shift_right,
        true /* claim pins */,
        user_interruptible,
        sideset_enable,
        wrap_target, wrap, offset,
        fifo_type,
        mov_status_type, mov_status_n);
    if (!ok) {
        // indicate state machine never inited
        self->state_machine = NUM_PIO_STATE_MACHINES;
        mp_raise_RuntimeError(MP_ERROR_TEXT("All state machines in use"));
    }
}

void common_hal_rp2pio_statemachine_restart(rp2pio_statemachine_obj_t *self) {
    common_hal_rp2pio_statemachine_stop(self);
    // Reset program counter to the original offset. A JMP is 0x0000 plus
    // the desired offset, so we can just use self->offset.
    pio_sm_exec(self->pio, self->state_machine, self->offset);
    pio_sm_restart(self->pio, self->state_machine);
    uint8_t pio_index = pio_get_index(self->pio);
    pio_pinmask_t pins_we_use = _current_sm_pins[pio_index][self->state_machine];
    pio_sm_set_pins_with_mask64(self->pio, self->state_machine, PIO_PINMASK_VALUE(self->initial_pin_state), PIO_PINMASK_VALUE(pins_we_use));
    pio_sm_set_pindirs_with_mask64(self->pio, self->state_machine, PIO_PINMASK_VALUE(self->initial_pin_direction), PIO_PINMASK_VALUE(pins_we_use));
    rp2pio_statemachine_set_pull(self->pull_pin_up, self->pull_pin_down, pins_we_use);
    common_hal_rp2pio_statemachine_run(self, self->init, self->init_len);
    pio_sm_set_enabled(self->pio, self->state_machine, true);
}

void common_hal_rp2pio_statemachine_stop(rp2pio_statemachine_obj_t *self) {
    pio_sm_set_enabled(self->pio, self->state_machine, false);
}

void common_hal_rp2pio_statemachine_run(rp2pio_statemachine_obj_t *self, const uint16_t *instructions, size_t len) {
    for (size_t i = 0; i < len; i++) {
        pio_sm_exec(self->pio, self->state_machine, instructions[i]);
    }
}

uint32_t common_hal_rp2pio_statemachine_get_frequency(rp2pio_statemachine_obj_t *self) {
    return self->actual_frequency;
}

void common_hal_rp2pio_statemachine_set_frequency(rp2pio_statemachine_obj_t *self, uint32_t frequency) {
    if (frequency == 0) {
        frequency = clock_get_hz(clk_sys);
    }
    uint64_t frequency256 = ((uint64_t)clock_get_hz(clk_sys)) * 256;
    uint64_t div256 = frequency256 / frequency;
    if (frequency256 % div256 > 0) {
        div256 += 1;
    }
    // 0 is interpreted as 0x10000 so it's valid.
    if (div256 / 256 > 0x10000 || frequency > clock_get_hz(clk_sys)) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q out of range"), MP_QSTR_frequency);
    }
    self->actual_frequency = frequency256 / div256;

    pio_sm_set_clkdiv_int_frac(self->pio, self->state_machine, div256 / 256, div256 % 256);
    // Reset the clkdiv counter in case our new TOP is lower.
    pio_sm_clkdiv_restart(self->pio, self->state_machine);
}

void rp2pio_statemachine_reset_ok(PIO pio, int sm) {
    uint8_t pio_index = pio_get_index(pio);
    _never_reset[pio_index][sm] = false;
}

void rp2pio_statemachine_never_reset(PIO pio, int sm) {
    uint8_t pio_index = pio_get_index(pio);
    _never_reset[pio_index][sm] = true;
}

void rp2pio_statemachine_deinit(rp2pio_statemachine_obj_t *self, bool leave_pins) {
    common_hal_rp2pio_statemachine_stop(self);
    (void)common_hal_rp2pio_statemachine_stop_background_write(self);

    uint8_t sm = self->state_machine;
    uint8_t pio_index = pio_get_index(self->pio);
    common_hal_mcu_disable_interrupts();
    _interrupt_arg[pio_index][sm] = NULL;
    _interrupt_handler[pio_index][sm] = NULL;
    common_hal_mcu_enable_interrupts();
    _never_reset[pio_index][sm] = false;
    _reset_statemachine(self->pio, sm, leave_pins);
    self->state_machine = NUM_PIO_STATE_MACHINES;
}

void common_hal_rp2pio_statemachine_deinit(rp2pio_statemachine_obj_t *self) {
    rp2pio_statemachine_deinit(self, false);
}

void common_hal_rp2pio_statemachine_never_reset(rp2pio_statemachine_obj_t *self) {
    rp2pio_statemachine_never_reset(self->pio, self->state_machine);
    // TODO: never reset all the pins
}

bool common_hal_rp2pio_statemachine_deinited(rp2pio_statemachine_obj_t *self) {
    return self->state_machine == NUM_PIO_STATE_MACHINES;
}

static enum dma_channel_transfer_size _stride_to_dma_size(uint8_t stride) {
    switch (stride) {
        case 4:
            return DMA_SIZE_32;
        case 2:
            return DMA_SIZE_16;
        case 1:
        default:
            return DMA_SIZE_8;
    }
}

static bool _transfer(rp2pio_statemachine_obj_t *self,
    const uint8_t *data_out, size_t out_len, uint8_t out_stride_in_bytes,
    uint8_t *data_in, size_t in_len, uint8_t in_stride_in_bytes, bool swap_out, bool swap_in) {
    // This implementation is based on SPI but varies because the tx and rx buffers
    // may be different lengths and occur at different times or speeds.

    // Use DMA for large transfers if channels are available.
    // Don't exceed FIFO size.
    const size_t dma_min_size_threshold = self->fifo_depth;
    int chan_tx = -1;
    int chan_rx = -1;
    size_t len = MAX(out_len, in_len);
    bool tx = data_out != NULL;
    bool rx = data_in != NULL;
    bool use_dma = len >= dma_min_size_threshold || swap_out || swap_in;
    if (use_dma) {
        // Use DMA channels to service the two FIFOs
        if (tx) {
            chan_tx = dma_claim_unused_channel(false);
            // DMA allocation failed...
            if (chan_tx < 0) {
                return false;
            }
        }
        if (rx) {
            chan_rx = dma_claim_unused_channel(false);
            // DMA allocation failed...
            if (chan_rx < 0) {
                // may need to free tx channel
                if (chan_tx >= 0) {
                    dma_channel_unclaim(chan_tx);
                }
                return false;
            }
        }
    }
    volatile uint8_t *tx_destination = NULL;
    const volatile uint8_t *rx_source = NULL;
    if (tx) {
        tx_destination = (volatile uint8_t *)&self->pio->txf[self->state_machine];
        if (!self->out_shift_right) {
            tx_destination += 4 - out_stride_in_bytes;
        }
    }
    if (rx) {
        rx_source = (const volatile uint8_t *)&self->pio->rxf[self->state_machine];
        if (self->in_shift_right) {
            rx_source += 4 - in_stride_in_bytes;
        }
    }
    uint32_t stall_mask = 1 << (PIO_FDEBUG_TXSTALL_LSB + self->state_machine);
    if (use_dma) {
        dma_channel_config c;
        uint32_t channel_mask = 0;
        if (tx) {
            c = dma_channel_get_default_config(chan_tx);
            channel_config_set_transfer_data_size(&c, _stride_to_dma_size(out_stride_in_bytes));
            channel_config_set_dreq(&c, self->tx_dreq);
            channel_config_set_read_increment(&c, true);
            channel_config_set_write_increment(&c, false);
            channel_config_set_bswap(&c, swap_out);
            dma_channel_configure(chan_tx, &c,
                tx_destination,
                data_out,
                out_len / out_stride_in_bytes,
                false);
            channel_mask |= 1u << chan_tx;
        }
        if (rx) {
            c = dma_channel_get_default_config(chan_rx);
            channel_config_set_transfer_data_size(&c, _stride_to_dma_size(in_stride_in_bytes));
            channel_config_set_dreq(&c, self->rx_dreq);
            channel_config_set_read_increment(&c, false);
            channel_config_set_write_increment(&c, true);
            channel_config_set_bswap(&c, swap_in);
            dma_channel_configure(chan_rx, &c,
                data_in,
                rx_source,
                in_len / in_stride_in_bytes,
                false);
            channel_mask |= 1u << chan_rx;
        }

        dma_start_channel_mask(channel_mask);
        while ((rx && dma_channel_is_busy(chan_rx)) ||
               (tx && dma_channel_is_busy(chan_tx))) {
            // TODO: We should idle here until we get a DMA interrupt or something else.
            RUN_BACKGROUND_TASKS;
            if (self->user_interruptible && mp_hal_is_interrupted()) {
                if (rx && dma_channel_is_busy(chan_rx)) {
                    dma_channel_abort(chan_rx);
                }
                if (tx && dma_channel_is_busy(chan_tx)) {
                    dma_channel_abort(chan_tx);
                }
                break;
            }

        }
        // Clear the stall bit so we can detect when the state machine is done transmitting.
        self->pio->fdebug = stall_mask;
    }

    // If we have claimed only one channel successfully, we should release immediately. This also
    // releases the DMA after use_dma has been done.
    if (chan_rx >= 0) {
        dma_channel_unclaim(chan_rx);
    }
    if (chan_tx >= 0) {
        dma_channel_unclaim(chan_tx);
    }

    if (!use_dma && !(self->user_interruptible && mp_hal_is_interrupted())) {
        // Use software for small transfers, or if couldn't claim two DMA channels
        size_t rx_remaining = in_len / in_stride_in_bytes;
        size_t tx_remaining = out_len / out_stride_in_bytes;

        while (rx_remaining || tx_remaining) {
            while (tx_remaining && !pio_sm_is_tx_fifo_full(self->pio, self->state_machine)) {
                if (out_stride_in_bytes == 1) {
                    *tx_destination = *data_out;
                } else if (out_stride_in_bytes == 2) {
                    *((uint16_t *)tx_destination) = *((uint16_t *)data_out);
                } else if (out_stride_in_bytes == 4) {
                    *((uint32_t *)tx_destination) = *((uint32_t *)data_out);
                }
                data_out += out_stride_in_bytes;
                --tx_remaining;
            }
            while (rx_remaining && !pio_sm_is_rx_fifo_empty(self->pio, self->state_machine)) {
                if (in_stride_in_bytes == 1) {
                    *data_in = (uint8_t)*rx_source;
                } else if (in_stride_in_bytes == 2) {
                    *((uint16_t *)data_in) = *((uint16_t *)rx_source);
                } else if (in_stride_in_bytes == 4) {
                    *((uint32_t *)data_in) = *((uint32_t *)rx_source);
                }
                data_in += in_stride_in_bytes;
                --rx_remaining;
            }
            RUN_BACKGROUND_TASKS;
            if (self->user_interruptible && mp_hal_is_interrupted()) {
                break;
            }
        }
        // Clear the stall bit so we can detect when the state machine is done transmitting.
        self->pio->fdebug = stall_mask;
    }
    // Wait for the state machine to finish transmitting the data we've queued
    // up.
    if (tx) {
        while (!pio_sm_is_tx_fifo_empty(self->pio, self->state_machine) ||
               (self->wait_for_txstall && (self->pio->fdebug & stall_mask) == 0)) {
            RUN_BACKGROUND_TASKS;
            if (self->user_interruptible && mp_hal_is_interrupted()) {
                break;
            }
        }
    }
    return true;
}

// TODO: Provide a way around these checks in case someone wants to use the FIFO
// with manually run code.

bool common_hal_rp2pio_statemachine_write(rp2pio_statemachine_obj_t *self, const uint8_t *data, size_t len, uint8_t stride_in_bytes, bool swap) {
    if (!self->out) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No out in program"));
    }
    return _transfer(self, data, len, stride_in_bytes, NULL, 0, 0, swap, false);
}

bool common_hal_rp2pio_statemachine_readinto(rp2pio_statemachine_obj_t *self, uint8_t *data, size_t len, uint8_t stride_in_bytes, bool swap) {
    if (!self->in) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No in in program"));
    }
    return _transfer(self, NULL, 0, 0, data, len, stride_in_bytes, false, swap);
}

bool common_hal_rp2pio_statemachine_write_readinto(rp2pio_statemachine_obj_t *self,
    const uint8_t *data_out, size_t out_len, uint8_t out_stride_in_bytes,
    uint8_t *data_in, size_t in_len, uint8_t in_stride_in_bytes, bool swap_out, bool swap_in) {
    if (!self->in || !self->out) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("No in or out in program"));
    }
    return _transfer(self, data_out, out_len, out_stride_in_bytes, data_in, in_len, in_stride_in_bytes, swap_out, swap_in);
}

bool common_hal_rp2pio_statemachine_get_rxstall(rp2pio_statemachine_obj_t *self) {
    uint32_t stall_mask = 1 << (PIO_FDEBUG_RXSTALL_LSB + self->state_machine);
    return (self->pio->fdebug & stall_mask) != 0;
}

void common_hal_rp2pio_statemachine_clear_rxfifo(rp2pio_statemachine_obj_t *self) {
    uint8_t level = pio_sm_get_rx_fifo_level(self->pio, self->state_machine);
    uint32_t stall_mask = 1 << (PIO_FDEBUG_RXSTALL_LSB + self->state_machine);
    for (size_t i = 0; i < level; i++) {
        (void)self->pio->rxf[self->state_machine];
    }
    self->pio->fdebug = stall_mask;
}

bool common_hal_rp2pio_statemachine_get_txstall(rp2pio_statemachine_obj_t *self) {
    uint32_t stall_mask = 1 << (PIO_FDEBUG_TXSTALL_LSB + self->state_machine);
    return (self->pio->fdebug & stall_mask) != 0;
}

void common_hal_rp2pio_statemachine_clear_txstall(rp2pio_statemachine_obj_t *self) {
    (void)pio_sm_get_rx_fifo_level(self->pio, self->state_machine);
    uint32_t stall_mask = 1 << (PIO_FDEBUG_TXSTALL_LSB + self->state_machine);
    self->pio->fdebug = stall_mask;
}


size_t common_hal_rp2pio_statemachine_get_in_waiting(rp2pio_statemachine_obj_t *self) {
    uint8_t level = pio_sm_get_rx_fifo_level(self->pio, self->state_machine);
    return level;
}

void common_hal_rp2pio_statemachine_set_interrupt_handler(rp2pio_statemachine_obj_t *self, void (*handler)(void *), void *arg, int mask) {
    uint8_t pio_index = pio_get_index(self->pio);
    uint8_t sm = self->state_machine;

    common_hal_mcu_disable_interrupts();
    uint32_t inte = self->pio->inte0;
    inte &= ~((PIO_IRQ0_INTF_SM0_RXNEMPTY_BITS | PIO_IRQ0_INTF_SM0_TXNFULL_BITS | PIO_IRQ0_INTF_SM0_BITS) << sm);
    inte |= (mask << sm);
    self->pio->inte0 = inte;
    _interrupt_arg[pio_index][sm] = arg;
    _interrupt_handler[pio_index][sm] = handler;
    irq_set_exclusive_handler(PIO0_IRQ_0 + 2 * pio_index, rp2pio_statemachine_interrupt_handler);
    irq_set_enabled(PIO0_IRQ_0 + 2 * pio_index, true);
    common_hal_mcu_enable_interrupts();
}

static void rp2pio_statemachine_interrupt_handler(void) {
    for (size_t pio_index = 0; pio_index < NUM_PIOS; pio_index++) {
        PIO pio = pio_get_instance(pio_index);
        for (size_t sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
            if (!_interrupt_handler[pio_index][sm]) {
                continue;
            }
            uint32_t intf = (PIO_IRQ0_INTF_SM0_RXNEMPTY_BITS | PIO_IRQ0_INTF_SM0_TXNFULL_BITS | PIO_IRQ0_INTF_SM0_BITS) << sm;
            if (pio->ints0 & intf) {
                _interrupt_handler[pio_index][sm](_interrupt_arg[pio_index][sm]);
            }
        }
    }
}

uint8_t rp2pio_statemachine_program_offset(rp2pio_statemachine_obj_t *self) {
    uint8_t pio_index = pio_get_index(self->pio);
    uint8_t sm = self->state_machine;
    return _current_program_offset[pio_index][sm];
}

bool common_hal_rp2pio_statemachine_background_write(rp2pio_statemachine_obj_t *self, uint8_t stride_in_bytes, bool swap) {

    uint8_t pio_index = pio_get_index(self->pio);
    uint8_t sm = self->state_machine;

    self->switched_write_buffers = false;

    int pending_buffers_write = (self->once_write_buf_info.info.len != 0) + (self->loop_write_buf_info.info.len != 0) + (self->loop2_write_buf_info.info.len != 0);

    // If all buffer arguments have nonzero length, write once_write_buf, loop_write_buf, loop2_write_buf and repeat last two forever

    if (!(self->once_write_buf_info.info.len)) {
        if (!self->loop_write_buf_info.info.len) {
            // If once_write_buf and loop_write_buf have zero length, write loop2_write_buf forever
            self->once_write_buf_info = self->loop2_write_buf_info;
            self->loop_write_buf_info = self->loop2_write_buf_info;
        } else {
            if (!(self->loop2_write_buf_info.info.len)) {
                // If once_write_buf and loop2_write_buf have zero length, write loop_write_buf forever
                self->once_write_buf_info = self->loop_write_buf_info;
                self->loop2_write_buf_info = self->loop_write_buf_info;
            } else {
                // If only once_write_buf has zero length, write loop_write_buf, loop2_write_buf, and repeat last two forever
                self->once_write_buf_info = self->loop_write_buf_info;
                self->loop_write_buf_info = self->loop2_write_buf_info;
                self->loop2_write_buf_info = self->once_write_buf_info;
            }
        }
    } else {
        if (!(self->loop_write_buf_info.info.len)) {
            // If once_write_buf has nonzero length and loop_write_buf has zero length, write once_write_buf, loop2_write_buf and repeat last buf forever
            self->loop_write_buf_info = self->loop2_write_buf_info;
        } else {
            if (!self->loop2_write_buf_info.info.len) {
                // If once_write_buf has nonzero length and loop2_write_buf have zero length, write once_write_buf, loop_write_buf and repeat last buf forever
                self->loop2_write_buf_info = self->loop_write_buf_info;
            }
        }
    }

    // if DMA is already going (i.e. this is not the first call to background_write),
    // block until once_write_buf and loop_write_buf have each been written at least once
    if (SM_DMA_ALLOCATED_WRITE(pio_index, sm)) {
        if (stride_in_bytes != self->background_stride_in_bytes) {
            mp_raise_ValueError(MP_ERROR_TEXT("Mismatched data size"));
        }
        if (swap != self->byteswap) {
            mp_raise_ValueError(MP_ERROR_TEXT("Mismatched swap flag"));
        }

        while (self->pending_buffers_write) {
            RUN_BACKGROUND_TASKS;
            if (self->user_interruptible && mp_hal_is_interrupted()) {
                return false;
            }
        }

        common_hal_mcu_disable_interrupts();
        self->next_write_buf_1 = self->once_write_buf_info;
        self->next_write_buf_2 = self->loop_write_buf_info;
        self->next_write_buf_3 = self->loop2_write_buf_info;
        self->pending_buffers_write = pending_buffers_write;

        if (self->dma_completed_write && self->next_write_buf_1.info.len) {
            rp2pio_statemachine_dma_complete_write(self, SM_DMA_GET_CHANNEL_WRITE(pio_index, sm));
            self->dma_completed_write = false;
        }

        common_hal_mcu_enable_interrupts();

        return true;
    }

    int channel_write = dma_claim_unused_channel(false);
    if (channel_write == -1) {
        return false;
    }

    SM_DMA_SET_CHANNEL_WRITE(pio_index, sm, channel_write);

    volatile uint8_t *tx_destination = (volatile uint8_t *)&self->pio->txf[self->state_machine];

    self->tx_dreq = pio_get_dreq(self->pio, self->state_machine, true);

    dma_channel_config c_write;

    self->current_write_buf = self->once_write_buf_info;
    self->next_write_buf_1 = self->loop_write_buf_info;
    self->next_write_buf_2 = self->loop2_write_buf_info;
    self->next_write_buf_3 = self->loop_write_buf_info;

    self->pending_buffers_write = pending_buffers_write;
    self->dma_completed_write = false;

    self->background_stride_in_bytes = stride_in_bytes;
    self->byteswap = swap;

    c_write = dma_channel_get_default_config(channel_write);
    channel_config_set_transfer_data_size(&c_write, _stride_to_dma_size(stride_in_bytes));
    channel_config_set_dreq(&c_write, self->tx_dreq);
    channel_config_set_read_increment(&c_write, true);
    channel_config_set_write_increment(&c_write, false);
    channel_config_set_bswap(&c_write, swap);
    dma_channel_configure(channel_write, &c_write,
        tx_destination,
        self->once_write_buf_info.info.buf,
        self->once_write_buf_info.info.len / stride_in_bytes,
        false);

    common_hal_mcu_disable_interrupts();

    // Acknowledge any previous pending interrupt
    dma_hw->ints0 |= 1u << channel_write;
    MP_STATE_PORT(background_pio_write)[channel_write] = self;
    dma_hw->inte0 |= 1u << channel_write;

    irq_set_mask_enabled(1 << DMA_IRQ_0, true);
    dma_start_channel_mask(1u << channel_write);
    common_hal_mcu_enable_interrupts();

    return true;
}

void rp2pio_statemachine_dma_complete_write(rp2pio_statemachine_obj_t *self, int channel_write) {
    self->current_write_buf = self->next_write_buf_1;
    self->next_write_buf_1 = self->next_write_buf_2;
    self->next_write_buf_2 = self->next_write_buf_3;
    self->next_write_buf_3 = self->next_write_buf_1;

    if (self->current_write_buf.info.buf) {
        if (self->pending_buffers_write > 0) {
            self->pending_buffers_write--;
        }
        dma_channel_set_read_addr(channel_write, self->current_write_buf.info.buf, false);
        dma_channel_set_trans_count(channel_write, self->current_write_buf.info.len / self->background_stride_in_bytes, true);
    } else {
        self->dma_completed_write = true;
        self->pending_buffers_write = 0; // should be a no-op
    }

    self->switched_write_buffers = true;
}

bool common_hal_rp2pio_statemachine_stop_background_write(rp2pio_statemachine_obj_t *self) {
    uint8_t pio_index = pio_get_index(self->pio);
    uint8_t sm = self->state_machine;
    rp2pio_statemachine_clear_dma_write(pio_index, sm);
    memset(&self->current_write_buf, 0, sizeof(self->current_write_buf));
    memset(&self->next_write_buf_1, 0, sizeof(self->next_write_buf_1));
    memset(&self->next_write_buf_2, 0, sizeof(self->next_write_buf_2));
    memset(&self->next_write_buf_3, 0, sizeof(self->next_write_buf_3));
    self->pending_buffers_write = 0;
    return true;
}

bool common_hal_rp2pio_statemachine_get_writing(rp2pio_statemachine_obj_t *self) {
    return !self->dma_completed_write;
}

int common_hal_rp2pio_statemachine_get_pending_write(rp2pio_statemachine_obj_t *self) {
    return self->pending_buffers_write;
}

// =================================================================================

bool common_hal_rp2pio_statemachine_background_read(rp2pio_statemachine_obj_t *self, uint8_t stride_in_bytes, bool swap) {

    uint8_t pio_index = pio_get_index(self->pio);
    uint8_t sm = self->state_machine;

    self->switched_read_buffers = false;

    int pending_buffers_read = (self->once_read_buf_info.info.len != 0) + (self->loop_read_buf_info.info.len != 0) + (self->loop2_read_buf_info.info.len != 0);

    // If all buffer arguments have nonzero length, read once_read_buf, loop_read_buf, loop2_read_buf and repeat last two forever

    if (!(self->once_read_buf_info.info.len)) {
        if (!(self->loop_read_buf_info.info.len)) {
            // If once_read_buf and loop_read_buf have zero length, read loop2_read_buf forever
            self->once_read_buf_info = self->loop2_read_buf_info;
            self->loop_read_buf_info = self->loop2_read_buf_info;
        } else {
            if (!(self->loop2_read_buf_info.info.len)) {
                // If once_read_buf and loop2_read_buf have zero length, read loop_read_buf forever
                self->once_read_buf_info = self->loop_read_buf_info;
                self->loop2_read_buf_info = self->loop_read_buf_info;
            } else {
                // If only once_read_buf has zero length, read loop_read_buf, loop2_read_buf, and repeat last two forever
                self->once_read_buf_info = self->loop_read_buf_info;
                self->loop_read_buf_info = self->loop2_read_buf_info;
                self->loop2_read_buf_info = self->once_read_buf_info;
            }
        }
    } else {
        if (!(self->loop_read_buf_info.info.len)) {
            // If once_read_buf has nonzero length and loop_read_buf has zero length, read once_read_buf, loop2_read_buf and repeat last buf forever
            self->loop_read_buf_info = self->loop2_read_buf_info;
        } else {
            if (!(self->loop2_read_buf_info.info.len)) {
                // If once_read_buf has nonzero length and loop2_read_buf have zero length, read once_read_buf, loop_read_buf and repeat last buf forever
                self->loop2_read_buf_info = self->loop_read_buf_info;

            }
        }
    }

    if (SM_DMA_ALLOCATED_READ(pio_index, sm)) {
        if (stride_in_bytes != self->background_stride_in_bytes) {
            mp_raise_ValueError(MP_ERROR_TEXT("Mismatched data size"));
        }
        if (swap != self->byteswap) {
            mp_raise_ValueError(MP_ERROR_TEXT("Mismatched swap flag"));
        }

        while (self->pending_buffers_read) {
            RUN_BACKGROUND_TASKS;
            if (self->user_interruptible && mp_hal_is_interrupted()) {
                return false;
            }
        }

        common_hal_mcu_disable_interrupts();
        self->next_read_buf_1 = self->once_read_buf_info;
        self->next_read_buf_2 = self->loop_read_buf_info;
        self->next_read_buf_3 = self->loop2_read_buf_info;
        self->pending_buffers_read = pending_buffers_read;

        if (self->dma_completed_read && self->next_read_buf_1.info.len) {
            rp2pio_statemachine_dma_complete_read(self, SM_DMA_GET_CHANNEL_READ(pio_index, sm));
            self->dma_completed_read = false;
        }

        common_hal_mcu_enable_interrupts();

        return true;
    }

    int channel_read = dma_claim_unused_channel(false);
    if (channel_read == -1) {
        return false;
    }
    SM_DMA_SET_CHANNEL_READ(pio_index, sm, channel_read);

    // set up receive DMA

    volatile uint8_t *rx_source = (volatile uint8_t *)&self->pio->rxf[self->state_machine];

    self->rx_dreq = pio_get_dreq(self->pio, self->state_machine, false);

    dma_channel_config c_read;

    self->current_read_buf = self->once_read_buf_info;
    self->next_read_buf_1 = self->loop_read_buf_info;
    self->next_read_buf_2 = self->loop2_read_buf_info;
    self->next_read_buf_3 = self->loop_read_buf_info;

    self->pending_buffers_read = pending_buffers_read;
    self->dma_completed_read = false;

    self->background_stride_in_bytes = stride_in_bytes;
    self->byteswap = swap;

    c_read = dma_channel_get_default_config(channel_read);
    channel_config_set_transfer_data_size(&c_read, _stride_to_dma_size(stride_in_bytes));
    channel_config_set_dreq(&c_read, self->rx_dreq);
    channel_config_set_read_increment(&c_read, false);
    channel_config_set_write_increment(&c_read, true);
    channel_config_set_bswap(&c_read, swap);
    dma_channel_configure(channel_read, &c_read,
        self->once_read_buf_info.info.buf,
        rx_source,
        self->once_read_buf_info.info.len / stride_in_bytes,
        false);

    common_hal_mcu_disable_interrupts();
    // Acknowledge any previous pending interrupt
    dma_hw->ints0 |= 1u << channel_read;
    MP_STATE_PORT(background_pio_read)[channel_read] = self;
    dma_hw->inte0 |= 1u << channel_read;
    irq_set_mask_enabled(1 << DMA_IRQ_0, true);
    dma_start_channel_mask((1u << channel_read));
    common_hal_mcu_enable_interrupts();

    return true;
}

void rp2pio_statemachine_dma_complete_read(rp2pio_statemachine_obj_t *self, int channel_read) {

    self->current_read_buf = self->next_read_buf_1;
    self->next_read_buf_1 = self->next_read_buf_2;
    self->next_read_buf_2 = self->next_read_buf_3;
    self->next_read_buf_3 = self->next_read_buf_1;

    if (self->current_read_buf.info.buf) {
        if (self->pending_buffers_read > 0) {
            self->pending_buffers_read--;
        }
        dma_channel_set_write_addr(channel_read, self->current_read_buf.info.buf, false);
        dma_channel_set_trans_count(channel_read, self->current_read_buf.info.len / self->background_stride_in_bytes, true);
    } else {
        self->dma_completed_read = true;
        self->pending_buffers_read = 0; // should be a no-op
    }

    self->switched_read_buffers = true;
}

bool common_hal_rp2pio_statemachine_stop_background_read(rp2pio_statemachine_obj_t *self) {
    uint8_t pio_index = pio_get_index(self->pio);
    uint8_t sm = self->state_machine;
    rp2pio_statemachine_clear_dma_read(pio_index, sm);
    memset(&self->current_read_buf, 0, sizeof(self->current_read_buf));
    memset(&self->next_read_buf_1, 0, sizeof(self->next_read_buf_1));
    memset(&self->next_read_buf_2, 0, sizeof(self->next_read_buf_2));
    memset(&self->next_read_buf_3, 0, sizeof(self->next_read_buf_3));
    self->pending_buffers_read = 0;
    return true;
}

bool common_hal_rp2pio_statemachine_get_reading(rp2pio_statemachine_obj_t *self) {
    return !self->dma_completed_read;
}

int common_hal_rp2pio_statemachine_get_pending_read(rp2pio_statemachine_obj_t *self) {
    return self->pending_buffers_read;
}

int common_hal_rp2pio_statemachine_get_offset(rp2pio_statemachine_obj_t *self) {
    uint8_t pio_index = pio_get_index(self->pio);
    uint8_t sm = self->state_machine;
    uint8_t offset = _current_program_offset[pio_index][sm];
    return offset;
}

int common_hal_rp2pio_statemachine_get_pc(rp2pio_statemachine_obj_t *self) {
    uint8_t pio_index = pio_get_index(self->pio);
    PIO pio = pio_get_instance(pio_index);
    uint8_t sm = self->state_machine;
    return pio_sm_get_pc(pio, sm);
}

mp_obj_t common_hal_rp2pio_statemachine_get_rxfifo(rp2pio_statemachine_obj_t *self) {
    #if PICO_PIO_VERSION > 0
    if (self->rxfifo_obj.base.type) {
        return MP_OBJ_FROM_PTR(&self->rxfifo_obj);
    }
    #endif
    return mp_const_none;
}

mp_obj_t common_hal_rp2pio_statemachine_get_last_read(rp2pio_statemachine_obj_t *self) {
    if (self->switched_read_buffers) {
        self->switched_read_buffers = false;
        return self->next_read_buf_1.obj;
    }
    return mp_const_empty_bytes;
}

mp_obj_t common_hal_rp2pio_statemachine_get_last_write(rp2pio_statemachine_obj_t *self) {
    if (self->switched_write_buffers) {
        self->switched_write_buffers = false;
        return self->next_write_buf_1.obj;
    }
    return mp_const_empty_bytes;
}


// Use a compile-time constant for MP_REGISTER_POINTER so the preprocessor will
// not split the expansion across multiple lines.
MP_REGISTER_ROOT_POINTER(mp_obj_t background_pio_read[enum_NUM_DMA_CHANNELS]);
MP_REGISTER_ROOT_POINTER(mp_obj_t background_pio_write[enum_NUM_DMA_CHANNELS]);
