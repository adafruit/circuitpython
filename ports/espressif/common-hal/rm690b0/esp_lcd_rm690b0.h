// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "driver/spi_master.h"
#include "esp-idf/components/esp_lcd/include/esp_lcd_panel_vendor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LCD panel initialization commands.
 *
 */
typedef struct {
    int cmd;                /*<! The specific LCD command */
    const void *data;       /*<! Buffer that holds the command specific data */
    size_t data_bytes;      /*<! Size of `data` in memory, in bytes */
    unsigned int delay_ms;  /*<! Delay in milliseconds after this command */
} rm690b0_lcd_init_cmd_t;

/**
 * @brief LCD panel vendor configuration.
 *
 * @note  This structure can be used to select interface type and override default initialization commands.
 * @note  This structure needs to be passed to the `vendor_config` field in `esp_lcd_panel_dev_config_t`.
 *
 */
typedef struct {
    const rm690b0_lcd_init_cmd_t *init_cmds;    /*!< Pointer to initialization commands array.
                                                 *  The array should be declared as `static const` and positioned outside the function.
                                                 *  Please refer to `vendor_specific_init_default` in source file
                                                 */
    uint16_t init_cmds_size;    /*<! Number of commands in above array */
    struct {
        unsigned int use_qspi_interface : 1;     /*<! Set to 1 if use QSPI interface, default is SPI interface */
    } flags;
} rm690b0_vendor_config_t;

/**
 * @brief Create LCD panel for model RM690B0
 *
 * @param[in]  io LCD panel IO handle
 * @param[in]  panel_dev_config General panel device configuration (Use `vendor_config` to select QSPI interface or override default initialization commands)
 * @param[out] ret_panel Returned LCD panel handle
 * @return
 *      - ESP_OK: Success
 *      - Otherwise: Fail
 */
#define RM690B0_PANEL_BUS_QSPI_CONFIG(sclk, d0, d1, d2, d3, max_trans_sz) \
    ((spi_bus_config_t) {                                        \
        .sclk_io_num = (sclk),                                  \
        .data0_io_num = (d0),                                   \
        .data1_io_num = (d1),                                   \
        .data2_io_num = (d2),                                   \
        .data3_io_num = (d3),                                   \
        .max_transfer_sz = (max_trans_sz),                      \
    })

#define RM690B0_PANEL_IO_QSPI_CONFIG(cs, cb, cb_ctx, clk_freq)          \
    ((esp_lcd_panel_io_spi_config_t) {                                   \
        .cs_gpio_num = (cs),                                            \
        .dc_gpio_num = -1,                                              \
        .spi_mode = 0,                                                  \
        .pclk_hz = (clk_freq),                                          \
        .trans_queue_depth = 10,                                        \
        .on_color_trans_done = (cb),                                    \
        .user_ctx = (cb_ctx),                                           \
        .lcd_cmd_bits = 32,                                             \
        .lcd_param_bits = 8,                                            \
        .flags = {                                                      \
            .quad_mode = true,                                          \
        },                                                              \
    })

#define RM690B0_PANEL_BUS_SPI_CONFIG(sclk, mosi, max_trans_sz)  \
    ((spi_bus_config_t) {                                        \
        .sclk_io_num = (sclk),                                  \
        .mosi_io_num = (mosi),                                  \
        .miso_io_num = -1,                                      \
        .quadhd_io_num = -1,                                    \
        .quadwp_io_num = -1,                                    \
        .max_transfer_sz = (max_trans_sz),                      \
    })

#define RM690B0_PANEL_IO_SPI_CONFIG(cs, dc, cb, cb_ctx, clk_freq)       \
    ((esp_lcd_panel_io_spi_config_t) {                                   \
        .cs_gpio_num = (cs),                                            \
        .dc_gpio_num = (dc),                                            \
        .spi_mode = 0,                                                  \
        .pclk_hz = (clk_freq),                                          \
        .trans_queue_depth = 10,                                        \
        .on_color_trans_done = (cb),                                    \
        .user_ctx = (cb_ctx),                                           \
        .lcd_cmd_bits = 8,                                              \
        .lcd_param_bits = 8,                                            \
    })


esp_err_t esp_lcd_new_panel_rm690b0(const esp_lcd_panel_io_handle_t io, const esp_lcd_panel_dev_config_t *panel_dev_config, esp_lcd_panel_handle_t *ret_panel);

#ifdef __cplusplus
}
#endif
