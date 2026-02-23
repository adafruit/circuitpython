// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mipidsi/Bus.h"
#include "bindings/espidf/__init__.h"
#include "py/runtime.h"
#include <esp_ldo_regulator.h>

void common_hal_mipidsi_bus_construct(mipidsi_bus_obj_t *self, mp_uint_t frequency, uint8_t num_lanes) {
    self->frequency = frequency;
    self->num_data_lanes = num_lanes;
    self->bus_handle = NULL;

    if (self->use_count > 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_mipidsi);
    }

    esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
    esp_ldo_channel_config_t ldo_mipi_phy_config = {
        .chan_id = 3,
        .voltage_mv = 2500,
    };
    ESP_ERROR_CHECK(esp_ldo_acquire_channel(&ldo_mipi_phy_config, &ldo_mipi_phy));

    // Create the DSI bus
    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = num_lanes,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = frequency / 1000000, // Convert Hz to MHz
    };

    CHECK_ESP_RESULT(esp_lcd_new_dsi_bus(&bus_config, &self->bus_handle));
}

void common_hal_mipidsi_bus_deinit(mipidsi_bus_obj_t *self) {
    if (self->use_count > 0) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_Bus);
    }
    if (common_hal_mipidsi_bus_deinited(self)) {
        return;
    }

    // Delete the DSI bus
    if (self->bus_handle != NULL) {
        esp_lcd_del_dsi_bus(self->bus_handle);
        self->bus_handle = NULL;
    }

    self->frequency = 0;
    self->num_data_lanes = 0;
}

bool common_hal_mipidsi_bus_deinited(mipidsi_bus_obj_t *self) {
    return self->bus_handle == NULL;
}

void mipidsi_bus_increment_use_count(mipidsi_bus_obj_t *self) {
    self->use_count++;
}
void mipidsi_bus_decrement_use_count(mipidsi_bus_obj_t *self) {
    self->use_count--;
    if (self->use_count == 0) {
        common_hal_mipidsi_bus_deinit(self);
    }
}
