// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include "common-hal/_bleio/ble_events.h"

#include <stdbool.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"

#include "py/gc.h"
#include "py/misc.h"
#include "py/mpstate.h"
#include "py/runtime.h"

#if CIRCUITPY_BLE_SERIAL_SERVICE && CIRCUITPY_VERBOSE_BLE
#include "supervisor/shared/bluetooth/serial.h"
#endif

// Do event list manipulation in a critical section.
static portMUX_TYPE handler_list_mutex = portMUX_INITIALIZER_UNLOCKED;

void ble_event_reset(void) {
    // Linked list items will be gc'd.
    portENTER_CRITICAL(&handler_list_mutex);
    MP_STATE_VM(ble_event_handler_entries) = NULL;
    portEXIT_CRITICAL(&handler_list_mutex);
}

void ble_event_remove_heap_handlers(void) {
    ble_event_handler_entry_t *it = MP_STATE_VM(ble_event_handler_entries);
    while (it != NULL) {
        // Save it->next before removing: the entry may be reused and relinked
        // once it is off the list.
        ble_event_handler_entry_t *next = it->next;
        // Remove the handler if the entry or its param is on the heap, which is
        // about to go away.
        if (gc_ptr_on_heap(it) || gc_ptr_on_heap(it->param)) {
            ble_event_remove_handler(it->func, it->param);
        }
        it = next;
    }
}

void ble_event_add_handler_entry(ble_event_handler_entry_t *entry,
    ble_gap_event_fn *func, void *param) {
    portENTER_CRITICAL(&handler_list_mutex);
    ble_event_handler_entry_t *it = MP_STATE_VM(ble_event_handler_entries);
    while (it != NULL) {
        // If event handler and its corresponding param are already on the list, don't add again.
        if ((it->func == func) && (it->param == param)) {
            portEXIT_CRITICAL(&handler_list_mutex);
            return;
        }
        it = it->next;
    }
    entry->next = MP_STATE_VM(ble_event_handler_entries);
    entry->param = param;
    entry->func = func;

    MP_STATE_VM(ble_event_handler_entries) = entry;
    portEXIT_CRITICAL(&handler_list_mutex);
}

void ble_event_add_handler(ble_gap_event_fn *func, void *param) {
    // Not in a critical section on purpose: this scan only avoids the
    // allocation below when the handler is already registered.
    // ble_event_add_handler_entry() repeats it in the critical section and is
    // the one that decides. The allocation must stay outside, because it can
    // collect or raise.
    ble_event_handler_entry_t *it = MP_STATE_VM(ble_event_handler_entries);
    while (it != NULL) {
        if ((it->func == func) && (it->param == param)) {
            return;
        }
        it = it->next;
    }

    // Add a new handler to the front of the list
    ble_event_handler_entry_t *handler = m_new_obj(ble_event_handler_entry_t);
    ble_event_add_handler_entry(handler, func, param);
}

void ble_event_remove_handler(ble_gap_event_fn *func, void *param) {
    portENTER_CRITICAL(&handler_list_mutex);
    ble_event_handler_entry_t *it = MP_STATE_VM(ble_event_handler_entries);
    ble_event_handler_entry_t **prev = &MP_STATE_VM(ble_event_handler_entries);
    while (it != NULL) {
        if ((it->func == func) && (it->param == param)) {
            // Splice out the matching handler. Leave its next pointer alone:
            // another walk in progress may already be holding this node as its
            // cursor. Clearing next would end that walk here, dropping the
            // event for every handler after it.
            *prev = it->next;
            portEXIT_CRITICAL(&handler_list_mutex);
            return;
        }
        prev = &(it->next);
        it = it->next;
    }
    portEXIT_CRITICAL(&handler_list_mutex);
}

int ble_event_run_handlers(struct ble_gap_event *event) {
    #if CIRCUITPY_BLE_SERIAL_SERVICE && CIRCUITPY_VERBOSE_BLE
    ble_serial_disable();
    #endif

    #if CIRCUITPY_VERBOSE_BLE
    mp_printf(&mp_plat_print, "BLE GAP event: 0x%04x\n", event->type);
    #endif

    portENTER_CRITICAL(&handler_list_mutex);
    ble_event_handler_entry_t *it = MP_STATE_VM(ble_event_handler_entries);
    portEXIT_CRITICAL(&handler_list_mutex);
    bool done = false;
    while (it != NULL) {
        // Take a consistent snapshot of the node, including next, before
        // calling the function: the function may remove itself from the list,
        // and so may the VM task while the call runs.
        portENTER_CRITICAL(&handler_list_mutex);
        ble_event_handler_entry_t *next = it->next;
        ble_gap_event_fn *func = it->func;
        void *param = it->param;
        portEXIT_CRITICAL(&handler_list_mutex);
        done = func(event, param) || done;
        it = next;
    }
    #if CIRCUITPY_BLE_SERIAL_SERVICE && CIRCUITPY_VERBOSE_BLE
    ble_serial_enable();
    #endif
    return 0;
}

MP_REGISTER_ROOT_POINTER(struct ble_event_handler_entry *ble_event_handler_entries);
