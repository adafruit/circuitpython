// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/port.h"

#include "mpconfigboard.h"
#include "supervisor/shared/tick.h"

#include <zephyr/autoconf.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>

#include "lib/tlsf/tlsf.h"
#include <zephyr/device.h>

static tlsf_t heap;

// Auto generated in pins.c
extern const struct device *const rams[];
extern const uint32_t *const ram_bounds[];
extern const size_t circuitpy_max_ram_size;

static pool_t pools[CIRCUITPY_RAM_DEVICE_COUNT];

static K_EVENT_DEFINE(main_needed);

static struct k_timer tick_timer;

static void _tick_function(struct k_timer *timer_id) {
    supervisor_tick();
}

safe_mode_t port_init(void) {
    k_timer_init(&tick_timer, _tick_function, NULL);
    return SAFE_MODE_NONE;
}

// Reset the microcontroller completely.
void reset_cpu(void) {
    // Try a warm reboot first. It won't return if it works but isn't always
    // implemented.
    sys_reboot(SYS_REBOOT_WARM);
    sys_reboot(SYS_REBOOT_COLD);
    printk("Failed to reboot. Looping.\n");
    while (true) {
    }
}

void reset_port(void) {

}

void reset_to_bootloader(void) {
    reset_cpu();
}

void port_wake_main_task(void) {
    k_event_set(&main_needed, 1);
}

void port_wake_main_task_from_isr(void) {
    k_event_set(&main_needed, 1);
}

void port_yield(void) {
    k_yield();
}

void port_boot_info(void) {
}

// Get stack limit address
uint32_t *port_stack_get_limit(void) {
    return (uint32_t *)k_current_get()->stack_info.start;
}

// Get stack top address
uint32_t *port_stack_get_top(void) {
    _thread_stack_info_t stack_info = k_current_get()->stack_info;

    return (uint32_t *)(stack_info.start + stack_info.size - stack_info.delta);
}

// Save and retrieve a word from memory that is preserved over reset. Used for safe mode.
void port_set_saved_word(uint32_t) {

}
uint32_t port_get_saved_word(void) {
    return 0;
}

uint64_t port_get_raw_ticks(uint8_t *subticks) {
    // Make sure time advances in the simulator.
    #if defined(CONFIG_ARCH_POSIX)
    k_busy_wait(100);
    #endif
    int64_t uptime = k_uptime_ticks() * 32768 / CONFIG_SYS_CLOCK_TICKS_PER_SEC;
    if (subticks != NULL) {
        *subticks = uptime % 32;
    }
    return uptime / 32;
}

// Enable 1/1024 second tick.
void port_enable_tick(void) {
    k_timer_start(&tick_timer, K_USEC(1000000 / 1024), K_USEC(1000000 / 1024));
}

// Disable 1/1024 second tick.
void port_disable_tick(void) {
    k_timer_stop(&tick_timer);
}

static k_timeout_t next_timeout;
static k_timepoint_t next_timepoint;

void port_interrupt_after_ticks(uint32_t ticks) {
    size_t zephyr_ticks = ticks * CONFIG_SYS_CLOCK_TICKS_PER_SEC / 1024;
    k_timeout_t maybe_next_timeout = K_TIMEOUT_ABS_TICKS(k_uptime_ticks() + zephyr_ticks);
    k_timepoint_t maybe_next_timepoint = sys_timepoint_calc(maybe_next_timeout);
    if (sys_timepoint_cmp(maybe_next_timepoint, next_timepoint) < 0) {
        next_timeout = maybe_next_timeout;
        next_timepoint = maybe_next_timepoint;
    }
}

void port_idle_until_interrupt(void) {
    k_event_wait(&main_needed, 0xffffffff, true, next_timeout);
    next_timeout = K_FOREVER;
    next_timepoint = sys_timepoint_calc(next_timeout);
}

// Zephyr doesn't maintain one multi-heap. So, make our own using TLSF.
void port_heap_init(void) {
    for (size_t i = 0; i < CIRCUITPY_RAM_DEVICE_COUNT; i++) {
        uint32_t *heap_bottom = ram_bounds[2 * i];
        uint32_t *heap_top = ram_bounds[2 * i + 1];
        size_t size = (heap_top - heap_bottom) * sizeof(uint32_t);

        printk("Init heap at %p - %p with size %d\n", heap_bottom, heap_top, size);
        // If this crashes, then make sure you've enabled all of the Kconfig needed for the drivers.
        if (i == 0) {
            heap = tlsf_create_with_pool(heap_bottom, size, circuitpy_max_ram_size);
            pools[i] = tlsf_get_pool(heap);
        } else {
            pools[i] = tlsf_add_pool(heap, heap_bottom + 1, size - sizeof(uint32_t));
        }
    }
    #if !DT_HAS_CHOSEN(zephyr_sram)
    #error "No SRAM!"
    #endif
}

void *port_malloc(size_t size, bool dma_capable) {
    void *block = tlsf_malloc(heap, size);
    return block;
}

void port_free(void *ptr) {
    tlsf_free(heap, ptr);
}

void *port_realloc(void *ptr, size_t size, bool dma_capable) {
    return tlsf_realloc(heap, ptr, size);
}

static bool max_size_walker(void *ptr, size_t size, int used, void *user) {
    size_t *max_size = (size_t *)user;
    if (!used && *max_size < size) {
        *max_size = size;
    }
    return true;
}

size_t port_heap_get_largest_free_size(void) {
    size_t max_size = 0;
    for (size_t i = 0; i < CIRCUITPY_RAM_DEVICE_COUNT; i++) {
        tlsf_walk_pool(pools[i], max_size_walker, &max_size);
    }
    // IDF does this. Not sure why.
    return tlsf_fit_size(heap, max_size);
}

void assert_post_action(const char *file, unsigned int line) {
    printk("Assertion failed at %s:%u\n", file, line);
    // Check that this is arm
    #if defined(__arm__)
    __asm__ ("bkpt");
    #endif
    while (1) {
        ;
    }
}
