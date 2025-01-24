// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 qutefox
// SPDX-FileCopyrightText: Copyright (c) 2025 SamantazFox
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/canio/Message.h"
#include "sdk/drivers/flexcan/fsl_flexcan.h"

// There are 64 message buffers in each mimxrt10xx chip.
// Rx fifo will use the message buffers at the front (from index zero).
// As far as I can see rx fifo uses message buffer 0 to 5.
// Also id filter table might occupy message buffer memory area 6 to 37.
//
// Let's use the last few message buffers for sending.
//
// We use 8 (tx) message buffers in total.
// This makes tracking the state (free/busy) of each (tx) message buffer easy.
// We can use a single byte, where each bit represent the state.
// Zero means the message buffer can be used for sending.
// One means the message buffer is busy sending.
// If you make this larger then you have to change the type of 'tx_state' in struct 'mimxrt10xx_flexcan_data_t'.
#define MIMXRT10XX_FLEXCAN_TX_MB_NUM (8)

// For safety we use SDK provided macro to get the last message buffer index instead of hard coding it.
#define MIMXRT10XX_FLEXCAN_TX_MBID_MAX (FSL_FEATURE_FLEXCAN_HAS_MESSAGE_BUFFER_MAX_NUMBERn(0))
#define MIMXRT10XX_FLEXCAN_TX_MBID_MIN (MIMXRT10XX_FLEXCAN_TX_MBID_MAX - MIMXRT10XX_FLEXCAN_TX_MB_NUM)

// Convert from tx message buffer index to frame array index.
#define MIMXRT10XX_FLEXCAN_TX_MBID_TO_ARRID(x) (x - MIMXRT10XX_FLEXCAN_TX_MBID_MIN)

// Convert from frame array index to tx message buffer index.
#define MIMXRT10XX_FLEXCAN_TX_ARRID_TO_MBID(x) (x + MIMXRT10XX_FLEXCAN_TX_MBID_MIN)

// We limit the amount of filter+mask pairs to 8 because above that the filters
// are impacted by the global mask rather than individual masks alone, which is
// not compatible with the current canio implementation.
//
// See Table 44-22 of the i.MX RT1060 Processor Reference Manual, Rev. 3
// for more details.
#define MIMXRT10XX_FLEXCAN_RX_FILTER_COUNT (8)

// Enables/disables SDK calculated "improved" timing configuration.
#define MIMXRT10XX_FLEXCAN_USE_IMPROVED_TIMING_CONFIG (1)

typedef struct {
    CAN_Type *base; // FlexCAN peripheral base address.
    flexcan_handle_t handle; // FlexCAN handle which can be used for FlexCAN transactional APIs.
    uint8_t tx_state;
    uint32_t rx_fifo_filter[MIMXRT10XX_FLEXCAN_RX_FILTER_COUNT];
} mimxrt10xx_flexcan_data_t;
