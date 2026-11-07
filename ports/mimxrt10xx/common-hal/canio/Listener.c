// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 qutefox
// SPDX-FileCopyrightText: Copyright (c) 2025 SamantazFox
//
// SPDX-License-Identifier: MIT


#include <math.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#include "shared/runtime/interrupt_char.h"

#include "common-hal/canio/__init__.h"
#include "common-hal/canio/Listener.h"
#include "shared-bindings/canio/Listener.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/tick.h"
#include "sdk/drivers/flexcan/fsl_flexcan.h"


// Convert from back from FLEXCAN IDs to normal CAN IDs.
#define FLEXCAN_ID_TO_CAN_ID_STD(id) \
    ((uint32_t)((((uint32_t)(id)) & CAN_ID_STD_MASK) >> CAN_ID_STD_SHIFT))

#define FLEXCAN_ID_TO_CAN_ID_EXT(id) \
    ((uint32_t)((((uint32_t)(id)) & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK)) \
    >> CAN_ID_EXT_SHIFT))


void common_hal_canio_listener_construct(canio_listener_obj_t *self, canio_can_obj_t *can, size_t nmatch, canio_match_obj_t **matches, float timeout) {

    common_hal_canio_listener_set_timeout(self, timeout);

    if (nmatch > MIMXRT10XX_FLEXCAN_RX_FILTER_COUNT) {
        mp_raise_ValueError(MP_ERROR_TEXT("Filters too complex"));
    }

    self->can = can;

    // Init configuration variables
    flexcan_rx_fifo_config_t fifo_config;
    fifo_config.idFilterNum = nmatch;
    fifo_config.idFilterTable = self->can->data->rx_fifo_filter;
    fifo_config.idFilterType = kFLEXCAN_RxFifoFilterTypeA;
    fifo_config.priority = kFLEXCAN_RxFifoPrioHigh;

    if (nmatch == 0) {
        // If the user has provided no matches, we need to set at least one
        // filter that instructs the system to ignore all bits.
        fifo_config.idFilterNum = 1;
        self->can->data->rx_fifo_filter[0] = 0x0;
        FLEXCAN_SetRxIndividualMask(self->can->data->base, 0, 0x0);
    } else {
        // Required to touch any CAN registers
        FLEXCAN_EnterFreezeMode(self->can->data->base);

        for (size_t i = 0; i < nmatch; i++) {
            if (matches[i]->extended) {
                self->can->data->rx_fifo_filter[i] = FLEXCAN_RX_FIFO_EXT_FILTER_TYPE_A(matches[i]->id, 0, 1);
                self->can->data->base->RXIMR[i] = FLEXCAN_RX_FIFO_EXT_MASK_TYPE_A(matches[i]->mask, 0, 1);
            } else {
                self->can->data->rx_fifo_filter[i] = FLEXCAN_RX_FIFO_STD_FILTER_TYPE_A(matches[i]->id, 0, 0);
                self->can->data->base->RXIMR[i] = FLEXCAN_RX_FIFO_STD_MASK_TYPE_A(matches[i]->mask, 0, 0);
            }
        }

        // For consistency, even though FLEXCAN_SetRxFifoConfig() below will
        // enter and exit freeze mode again anyway
        FLEXCAN_ExitFreezeMode(self->can->data->base);
    }

    FLEXCAN_SetRxFifoConfig(self->can->data->base, &fifo_config, true);
}

void common_hal_canio_listener_set_timeout(canio_listener_obj_t *self, float timeout) {
    self->timeout_ms = (int)MICROPY_FLOAT_C_FUN(ceil)(timeout * 1000);
}

float common_hal_canio_listener_get_timeout(canio_listener_obj_t *self) {
    return self->timeout_ms / 1000.0f;
}

void common_hal_canio_listener_check_for_deinit(canio_listener_obj_t *self) {
    if (!self->can) {
        raise_deinited_error();
    }
    common_hal_canio_can_check_for_deinit(self->can);
}

int common_hal_canio_listener_in_waiting(canio_listener_obj_t *self) {
    if (FLEXCAN_GetMbStatusFlags(self->can->data->base, kFLEXCAN_RxFifoFrameAvlFlag)) {
        return 1;
    }
    return 0;
}

mp_obj_t common_hal_canio_listener_receive(canio_listener_obj_t *self) {
    if (!common_hal_canio_listener_in_waiting(self)) {
        uint64_t deadline = supervisor_ticks_ms64() + self->timeout_ms;
        do {
            if (supervisor_ticks_ms64() > deadline) {
                return NULL;
            }
            RUN_BACKGROUND_TASKS;
            // Allow user to break out of a timeout with a KeyboardInterrupt.
            if (mp_hal_is_interrupted()) {
                return NULL;
            }
        } while (!common_hal_canio_listener_in_waiting(self));
    }

    flexcan_frame_t rx_frame;
    if (FLEXCAN_ReadRxFifo(self->can->data->base, &rx_frame) != kStatus_Success) {
        mp_raise_OSError(MP_EIO);
    }

    // We've read from the FIFO, clear the "frame available" flag, which
    // allows the CPU to serve the next FIFO entry
    FLEXCAN_ClearMbStatusFlags(self->can->data->base, (uint32_t)kFLEXCAN_RxFifoFrameAvlFlag);

    const mp_obj_type_t *type;
    if (rx_frame.type == kFLEXCAN_FrameTypeRemote) {
        type = &canio_remote_transmission_request_type;
    } else {
        type = &canio_message_type;
    }
    canio_message_obj_t *message = mp_obj_malloc(canio_message_obj_t, type);
    memset(message, 0, sizeof(canio_message_obj_t));

    if (rx_frame.format == kFLEXCAN_FrameFormatExtend) {
        message->extended = true;
        message->id = rx_frame.id;
    } else {
        message->extended = false;
        message->id = rx_frame.id >> 18; // standard ids are left-aligned
    }


    message->size = rx_frame.length;

    // We can safely copy all bytes, as both flexcan_frame_t and
    // canio_message_obj_t define the data array as 8 bytes long.
    message->data[0] = rx_frame.dataByte0;
    message->data[1] = rx_frame.dataByte1;
    message->data[2] = rx_frame.dataByte2;
    message->data[3] = rx_frame.dataByte3;
    message->data[4] = rx_frame.dataByte4;
    message->data[5] = rx_frame.dataByte5;
    message->data[6] = rx_frame.dataByte6;
    message->data[7] = rx_frame.dataByte7;

    return message;
}

void common_hal_canio_listener_deinit(canio_listener_obj_t *self) {
    if (self->can) {
        // Clear all filters.
        flexcan_rx_fifo_config_t fifo_config;
        fifo_config.idFilterNum = 0;
        fifo_config.idFilterTable = self->can->data->rx_fifo_filter;
        fifo_config.idFilterType = kFLEXCAN_RxFifoFilterTypeA;
        fifo_config.priority = kFLEXCAN_RxFifoPrioHigh;
        FLEXCAN_SetRxFifoConfig(self->can->data->base, &fifo_config, true);
    }
    self->can = NULL;
}
