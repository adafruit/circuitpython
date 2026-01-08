// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"

#include "common-hal/canio/CAN.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "supervisor/port.h"

#include "hal/twai_types.h"

static byte reserved_can_instance; /* Bit assigned instances    0x01 (CAN0)  or 0x02 (CAN1)    0x00-NoneAssigned     0x03-Both Assigned*/

#define TWAI0              REG_TWAI_BASE(0) /**< \brief (CAN0) APB Base Address */
#define TWAI1              REG_TWAI_BASE(1) /**< \brief (CAN1) APB Base Address */
#define TWAI_INST_NUM      2                          /**< \brief (CAN) Number of instances */

static twai_handle_t twai_bus_0;
static twai_handle_t twai_bus_1;
static twai_handle_t *const twai_insts[TWAI_INST_NUM] = { &twai_bus_0, &twai_bus_1 };

static twai_timing_config_t get_t_config(int baudrate) {
    switch (baudrate) {
        case 1000000: {
            // TWAI_TIMING_CONFIG_abc expands to a C designated initializer list
            // { .brp = 4, ...}.  This is only acceptable to the compiler as an
            // initializer and 'return TWAI_TIMING_CONFIG_1MBITS()` is not valid.
            // Instead, introduce a temporary, named variable and return it.
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
            return t_config;
        }
        case 800000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_800KBITS();
            return t_config;
        }
        case 500000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
            return t_config;
        }
        case 250000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
            return t_config;
        }
        case 125000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
            return t_config;
        }
        case 100000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_100KBITS();
            return t_config;
        }
        case 50000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_50KBITS();
            return t_config;
        }
        case 25000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS();
            return t_config;
        }
        #if defined(TWAI_TIMING_CONFIG_20KBITS)
        case 20000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_20KBITS();
            return t_config;
        }
        #endif
        #if defined(TWAI_TIMING_CONFIG_16KBITS)
        case 16000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_16KBITS();
            return t_config;
        }
        #endif
        #if defined(TWAI_TIMING_CONFIG_12_5KBITS)
        case 12500: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_12_5KBITS();
            return t_config;
        }
        #endif
        #if defined(TWAI_TIMING_CONFIG_10KBITS)
        case 10000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_10KBITS();
            return t_config;
        }
        #endif
        #if defined(TWAI_TIMING_CONFIG_5KBITS)
        case 5000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_5KBITS();
            return t_config;
        }
        #endif
        #if defined(TWAI_TIMING_CONFIG_1KBITS)
        case 1000: {
            twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1KBITS();
            return t_config;
        }
        #endif
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("Baudrate not supported by peripheral"));
    }
}

void common_hal_canio_can_construct(canio_can_obj_t *self, const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx, int baudrate, bool loopback, bool silent) {
#define DIV_ROUND(a, b) (((a) + (b) / 2) / (b))
#define DIV_ROUND_UP(a, b) (((a) + (b) - 1) / (b))
    if (reserved_can_instance==3) {
        mp_raise_ValueError(MP_ERROR_TEXT("All CAN peripherals are in use"));
    }

    if (loopback && silent) {
        mp_raise_ValueError(MP_ERROR_TEXT("loopback + silent mode not supported by peripheral"));
    }

    byte this_can_instance=0;
    if (reserved_can_instance==0x00){   /* None assigned */
        reserved_can_instance=0x01;     /* First instance is assigned*/ 
        this_can_instance=0;            /* Idx of selected instance */
    }
    else if (reserved_can_instance==0x01) /* First instance allready assigned */
    {
        reserved_can_instance=0x03;     /* Second instance also assigned */
        this_can_instance=1;            /* Idx of selected instance */

    }else if (reserved_can_instance==0x02)  /* Second instance is assigned */
    {
        reserved_can_instance=0x03;     /* first instance also assigned*/ 
        this_can_instance=0;            /* Idx of selected instance */
    }
    
    twai_timing_config_t t_config = get_t_config(baudrate);
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT_V2(this_can_instance,-1, -1, TWAI_MODE_NORMAL);
    g_config.tx_io = tx->number;
    g_config.rx_io = rx->number;
    g_config.controller_id = this_can_instance;   /* To ease later de-initing of proper instance */

    if (loopback) {
        g_config.mode = TWAI_MODE_NO_ACK;
    }
    if (silent) {
        g_config.mode = TWAI_MODE_LISTEN_ONLY;
    }

    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    esp_err_t result = twai_driver_install_v2(&g_config, &t_config, &f_config , twai_insts[this_can_instance]);
    if (result == ESP_ERR_NO_MEM) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("ESP-IDF memory allocation failed"));
    } else if (result == ESP_ERR_INVALID_ARG) {
        raise_ValueError_invalid_pins();
    } else if (result != ESP_OK) {
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("twai_driver_install returned esp-idf error #%d"), (int)result);
    }
    
    result = twai_start_v2(*twai_insts[this_can_instance]);
    if (result != ESP_OK) {
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("twai_start returned esp-idf error #%d"), (int)result);
    }

    self->silent = silent;
    self->loopback = loopback;
    self->baudrate = baudrate;
    self->tx_pin = tx;
    self->rx_pin = rx;

    claim_pin(tx);
    claim_pin(rx);

    self->twai_driver_handle = twai_insts[this_can_instance];
    self->instance = this_can_instance;

    
}




bool common_hal_canio_can_loopback_get(canio_can_obj_t *self) {
    return self->loopback;
}

int common_hal_canio_can_baudrate_get(canio_can_obj_t *self) {
    return self->baudrate;
}

int common_hal_canio_can_transmit_error_count_get(canio_can_obj_t *self) {
    twai_status_info_t info;
    twai_get_status_info_v2( *self->twai_driver_handle,&info);
    return info.tx_error_counter;
}

int common_hal_canio_can_receive_error_count_get(canio_can_obj_t *self) {
    twai_status_info_t info;
    twai_get_status_info_v2( *self->twai_driver_handle,&info);
    return info.rx_error_counter;
}


canio_bus_state_t common_hal_canio_can_state_get(canio_can_obj_t *self) {
    twai_status_info_t info;
    
    twai_get_status_info_v2( *self->twai_driver_handle,&info);
    if (info.state == TWAI_STATE_BUS_OFF || info.state == TWAI_STATE_RECOVERING) {
        return BUS_STATE_OFF;
    }
    if (info.tx_error_counter > 127 || info.rx_error_counter > 127) {
        return BUS_STATE_ERROR_PASSIVE;
    }
    if (info.tx_error_counter > 96 || info.rx_error_counter > 96) {
        return BUS_STATE_ERROR_WARNING;
    }
    return BUS_STATE_ERROR_ACTIVE;
}


static void can_restart(canio_can_obj_t *self) {
    twai_status_info_t info;
    twai_get_status_info_v2(*self->twai_driver_handle, &info);
    if (info.state != TWAI_STATE_BUS_OFF) {
        return;
    }
    twai_initiate_recovery_v2(*self->twai_driver_handle);
    // wait 100ms (hard coded for now) for bus to recover
    uint64_t deadline = port_get_raw_ticks(NULL) + 100;
    do {
        twai_get_status_info_v2(*self->twai_driver_handle,&info);
    } while (port_get_raw_ticks(NULL) < deadline && (info.state == TWAI_STATE_BUS_OFF || info.state == TWAI_STATE_RECOVERING));
}

static void canio_maybe_auto_restart(canio_can_obj_t *self) {
    if (self->auto_restart) {
        can_restart(self);
    }
}

void common_hal_canio_can_restart(canio_can_obj_t *self) {
    if (!common_hal_canio_can_auto_restart_get(self)) {
        can_restart(self);
    }
}

bool common_hal_canio_can_auto_restart_get(canio_can_obj_t *self) {
    return self->auto_restart;
}

void common_hal_canio_can_auto_restart_set(canio_can_obj_t *self, bool value) {
    self->auto_restart = value;
    canio_maybe_auto_restart(self);
}




void common_hal_canio_can_send(canio_can_obj_t *self, mp_obj_t message_in) {
    canio_maybe_auto_restart(self);
    canio_message_obj_t *message = message_in;
    bool rtr = message->base.type == &canio_remote_transmission_request_type;
    twai_message_t message_out = {
        .extd = message->extended,
        .rtr = rtr,
        .self = self->loopback,
        .identifier = message->id,
        .data_length_code = message->size,
    };
    if (!rtr) {
        memcpy(message_out.data, message->data, message->size);
    }
    // Allow transmission to occur in background
    twai_transmit_v2(*self->twai_driver_handle,&message_out, 0);
}

bool common_hal_canio_can_silent_get(canio_can_obj_t *self) {
    return self->silent;
}

bool common_hal_canio_can_deinited(canio_can_obj_t *self) {
    return !self->tx_pin;
}

void common_hal_canio_can_check_for_deinit(canio_can_obj_t *self) {
    if (common_hal_canio_can_deinited(self)) {
        raise_deinited_error();
    }
}

void common_hal_canio_can_deinit(canio_can_obj_t *self) {
    if (self->tx_pin) {
        (void)twai_stop_v2(*self->twai_driver_handle);
        (void)twai_driver_uninstall_v2(*self->twai_driver_handle);
        reset_pin_number(self->tx_pin->number);
        reset_pin_number(self->rx_pin->number);

        if (self->instance==0){
            reserved_can_instance&=0x02; /* free first instance - clear 1st bit */
        }else if (self->instance==1){
            reserved_can_instance&=0x01; /* free second instance - clear 2nd bit */
        }
        
    }
    self->tx_pin = NULL;
    self->rx_pin = NULL;
}

void common_hal_canio_reset(void) {
    (void)twai_stop_v2(*twai_insts[0]);
    (void)twai_driver_uninstall_v2(*twai_insts[0]);
    (void)twai_stop_v2(*twai_insts[1]);
    (void)twai_driver_uninstall_v2(*twai_insts[1]);
    reserved_can_instance=0; /* unassign both bits */
}
