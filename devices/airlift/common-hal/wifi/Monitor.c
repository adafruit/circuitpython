// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/mpstate.h"
#include "py/runtime.h"

#include "shared-bindings/wifi/Monitor.h"
#include "shared-bindings/wifi/Packet.h"

#define MONITOR_PAYLOAD_FCS_LEN     (4)
#define MONITOR_QUEUE_TIMEOUT_TICK  (0)

typedef struct {
    void *payload;
    unsigned channel;
    uint32_t length;
    signed rssi;
} monitor_packet_t;

void common_hal_wifi_monitor_construct(wifi_monitor_obj_t *self, uint8_t channel, size_t queue) {
    mp_raise_NotImplementedError(NULL);
}

bool common_hal_wifi_monitor_deinited(void) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_wifi_monitor_deinit(wifi_monitor_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_wifi_monitor_set_channel(wifi_monitor_obj_t *self, uint8_t channel) {
    mp_raise_NotImplementedError(NULL);
}

mp_obj_t common_hal_wifi_monitor_get_channel(wifi_monitor_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

mp_obj_t common_hal_wifi_monitor_get_queue(wifi_monitor_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

mp_obj_t common_hal_wifi_monitor_get_lost(wifi_monitor_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

mp_obj_t common_hal_wifi_monitor_get_queued(wifi_monitor_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

mp_obj_t common_hal_wifi_monitor_get_packet(wifi_monitor_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}
