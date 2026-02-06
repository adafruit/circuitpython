// SPDX-FileCopyrightText: Copyright (c) 2026 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/qspibus/QSPIBus.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "py/runtime.h"

#include "driver/gpio.h"
#include "soc/soc_caps.h"

#define QSPI_OPCODE_WRITE_CMD (0x02U)

static bool IRAM_ATTR qspibus_on_color_trans_done(
    esp_lcd_panel_io_handle_t io_handle,
    esp_lcd_panel_io_event_data_t *event_data,
    void *user_ctx) {
    (void)io_handle;
    (void)event_data;

    qspibus_qspibus_obj_t *self = (qspibus_qspibus_obj_t *)user_ctx;
    BaseType_t x_higher_priority_task_woken = pdFALSE;

    xSemaphoreGiveFromISR(self->transfer_done_sem, &x_higher_priority_task_woken);
    return x_higher_priority_task_woken == pdTRUE;
}

void common_hal_qspibus_qspibus_construct(
    qspibus_qspibus_obj_t *self,
    const mcu_pin_obj_t *clock,
    const mcu_pin_obj_t *data0,
    const mcu_pin_obj_t *data1,
    const mcu_pin_obj_t *data2,
    const mcu_pin_obj_t *data3,
    const mcu_pin_obj_t *cs,
    const mcu_pin_obj_t *reset,
    uint32_t frequency) {

    self->io_handle = NULL;
    self->host_id = SPI2_HOST;
    self->clock_pin = clock->number;
    self->data0_pin = data0->number;
    self->data1_pin = data1->number;
    self->data2_pin = data2->number;
    self->data3_pin = data3->number;
    self->cs_pin = cs->number;
    self->reset_pin = (reset != NULL) ? reset->number : -1;
    self->frequency = frequency;
    self->bus_initialized = false;
    self->transfer_done_sem = NULL;

    self->transfer_done_sem = xSemaphoreCreateBinary();
    if (self->transfer_done_sem == NULL) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Failed to create semaphore"));
    }

    const spi_bus_config_t bus_config = {
        .sclk_io_num = self->clock_pin,
        .data0_io_num = self->data0_pin,
        .data1_io_num = self->data1_pin,
        .data2_io_num = self->data2_pin,
        .data3_io_num = self->data3_pin,
        .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE,
        .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_GPIO_PINS,
    };

    esp_err_t err = spi_bus_initialize(self->host_id, &bus_config, SPI_DMA_CH_AUTO);
    if (err != ESP_OK) {
        vSemaphoreDelete(self->transfer_done_sem);
        self->transfer_done_sem = NULL;
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("SPI bus init failed: %d"), err);
    }

    const esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = self->cs_pin,
        .dc_gpio_num = -1,
        .spi_mode = 0,
        .pclk_hz = self->frequency,
        .trans_queue_depth = 1,
        .on_color_trans_done = qspibus_on_color_trans_done,
        .user_ctx = self,
        .lcd_cmd_bits = 32,
        .lcd_param_bits = 8,
        .flags = {
            .quad_mode = 1,
        },
    };

    err = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)self->host_id, &io_config, &self->io_handle);
    if (err != ESP_OK) {
        spi_bus_free(self->host_id);
        vSemaphoreDelete(self->transfer_done_sem);
        self->transfer_done_sem = NULL;
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("Panel IO init failed: %d"), err);
    }

    claim_pin(clock);
    claim_pin(data0);
    claim_pin(data1);
    claim_pin(data2);
    claim_pin(data3);
    claim_pin(cs);

    if (reset != NULL) {
        claim_pin(reset);

        gpio_set_direction((gpio_num_t)self->reset_pin, GPIO_MODE_OUTPUT);
        gpio_set_level((gpio_num_t)self->reset_pin, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level((gpio_num_t)self->reset_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(120));
    }

    self->bus_initialized = true;
}

void common_hal_qspibus_qspibus_deinit(qspibus_qspibus_obj_t *self) {
    if (!self->bus_initialized) {
        return;
    }

    if (self->io_handle != NULL) {
        esp_lcd_panel_io_del(self->io_handle);
        self->io_handle = NULL;
    }

    spi_bus_free(self->host_id);

    if (self->transfer_done_sem != NULL) {
        vSemaphoreDelete(self->transfer_done_sem);
        self->transfer_done_sem = NULL;
    }

    reset_pin_number(self->clock_pin);
    reset_pin_number(self->data0_pin);
    reset_pin_number(self->data1_pin);
    reset_pin_number(self->data2_pin);
    reset_pin_number(self->data3_pin);
    reset_pin_number(self->cs_pin);
    if (self->reset_pin >= 0) {
        reset_pin_number(self->reset_pin);
    }

    self->bus_initialized = false;
}

bool common_hal_qspibus_qspibus_deinited(qspibus_qspibus_obj_t *self) {
    return !self->bus_initialized;
}

void common_hal_qspibus_qspibus_send(
    qspibus_qspibus_obj_t *self,
    uint8_t command,
    const uint8_t *data,
    size_t len) {

    if (!self->bus_initialized) {
        mp_raise_ValueError(MP_ERROR_TEXT("QSPI bus deinitialized"));
    }

    uint32_t packed_cmd = ((uint32_t)QSPI_OPCODE_WRITE_CMD << 24) | ((uint32_t)command << 8);

    esp_err_t err = esp_lcd_panel_io_tx_param(self->io_handle, packed_cmd, data, len);
    if (err != ESP_OK) {
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("QSPI send failed: %d"), err);
    }

    // Some ESP-IDF paths complete tx_param synchronously and may not invoke
    // the callback for parameter transfers. Try to sync via semaphore when
    // available, but do not fail if no callback is emitted.
    (void)xSemaphoreTake(self->transfer_done_sem, pdMS_TO_TICKS(1000));
}

bool common_hal_qspibus_qspibus_bus_free(qspibus_qspibus_obj_t *self) {
    return self->bus_initialized;
}
