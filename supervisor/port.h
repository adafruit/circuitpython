// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016-2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "py/obj.h"
#include "supervisor/shared/safe_mode.h"

// Provided by the linker;
extern uint32_t _ezero;

// This file defines core methods that must be implemented by a port.
extern uint32_t _estack;

// Stored at the end of the bss section (which includes the heap).
extern uint32_t _ebss;

safe_mode_t port_init(void);

// Reset the microcontroller completely.
void reset_cpu(void) NORETURN;

// Reset the microcontroller state.
void reset_port(void);

// Reset to the bootloader
void reset_to_bootloader(void) NORETURN;

// Get stack limit address
uint32_t *port_stack_get_limit(void);

// Get stack top address
uint32_t *port_stack_get_top(void);

// Get heap bottom address
uint32_t *port_heap_get_bottom(void);

// Get heap top address
uint32_t *port_heap_get_top(void);

// Save and retrieve a word from memory that is preserved over reset. Used for safe mode.
void port_set_saved_word(uint32_t);
uint32_t port_get_saved_word(void);

// Get the raw tick count since start up. A tick is 1/1024 of a second, a common low frequency
// clock rate. If subticks is not NULL then the port will fill in the number of subticks where each
// tick is 32 subticks (for a resolution of 1/32768 or 30.5ish microseconds.)
uint64_t port_get_raw_ticks(uint8_t *subticks);

// Enable 1/1024 second tick.
void port_enable_tick(void);

// Disable 1/1024 second tick.
void port_disable_tick(void);

// Wake the CPU after the given number of ticks or sooner. Only the last call to this will apply.
// Only the common sleep routine should use it.
void port_interrupt_after_ticks(uint32_t ticks);

// Sleep the CPU until an interrupt is received. We call this idle because it
// may not be a system level sleep.
void port_idle_until_interrupt(void);

// Execute port specific actions during background tick. Only if ticks are enabled.
void port_background_tick(void);

// Execute port specific actions during background tasks. This is before the
// background callback system and happens *very* often. Use
// port_background_tick() when possible.
void port_background_task(void);

// Take port specific actions at the beginning and end of background ticks.
// This is used e.g., to set a monitoring pin for debug purposes.  "Actual
// work" should be done in port_background_tick() instead.
void port_start_background_tick(void);
void port_finish_background_tick(void);

// Some ports need special handling to wake the main task from another task. The
// port must implement the necessary code in this function.  A default weak
// implementation is provided that does nothing.
void port_wake_main_task(void);

// Some ports need special handling to wake the main task from an interrupt
// context.  The port must implement the necessary code in this function.  A
// default weak implementation is provided that does nothing.
void port_wake_main_task_from_isr(void);

// Some ports may use real RTOS tasks besides the background task framework of
// CircuitPython. Calling this will yield to other tasks and then return to the
// CircuitPython task when others are done.
void port_yield(void);

// Some ports want to add information to boot_out.txt.
// A default weak implementation is provided that does nothing.
void port_boot_info(void);

// Some ports want to mark additional pointers as gc roots.
// A default weak implementation is provided that does nothing.
void port_gc_collect(void);

// Most ports that implement CIRCUITPY_BOOT_BUTTON use a generic version of
// this function to sense the button. Ports that need to can override this
// function to provide their own implementation.
bool port_boot_button_pressed(void);

// Allocating objects on the port heap, not the VM heap.
#define mp_obj_port_malloc(struct_type, obj_type) ((struct_type *)mp_obj_port_malloc_helper(sizeof(struct_type), obj_type))
#define mp_obj_port_malloc_var(struct_type, var_field, var_type, var_num, obj_type) ((struct_type *)mp_obj_port_malloc_helper(offsetof(struct_type, var_field) + sizeof(var_type) * (var_num), obj_type))

void *mp_obj_port_malloc_helper(size_t num_bytes, const mp_obj_type_t *type);
mp_obj_t mp_obj_new_port_tuple(size_t n, const mp_obj_t *items);

// Safely free an object that may have been allocated from either the GC heap or port heap.
// If the pointer is on the GC heap, it will be freed by the GC automatically.
// If the pointer is not on the GC heap, it will be freed with port_free.
void circuitpy_free_obj(mp_obj_t obj);
