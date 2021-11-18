/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "common-hal/busio/SPI.h"
#include "boards/lpcxpresso55s28/spi_config.h"
#include "boards/lpcxpresso55s28/CMSIS/Driver/Config/RTE_Device.h"

#include "fsl_clock.h"


extern ARM_DRIVER_SPI Driver_SPI3;
extern ARM_DRIVER_SPI Driver_SPI8;

const static spi_pin_set_t SPI3_pin_set[] =
{
    /* Pin set 0 */
    {
        .clock = NXP_PORT_GPIO_PIN(RTE_SPI3_SCK_PORT,  RTE_SPI3_SCK_BIT),
        .mosi = NXP_PORT_GPIO_PIN(RTE_SPI3_MOSI_PORT, RTE_SPI3_MOSI_BIT),
        .miso = NXP_PORT_GPIO_PIN(RTE_SPI3_MISO_PORT, RTE_SPI3_MISO_BIT),
    },
};

const static spi_pin_set_t SPI8_pin_set[] =
{
    /* Pin set 0 */
    {
        .clock = NXP_PORT_GPIO_PIN(RTE_SPI8_SCK_PORT,  RTE_SPI8_SCK_BIT),
        .mosi = NXP_PORT_GPIO_PIN(RTE_SPI8_MOSI_PORT, RTE_SPI8_MOSI_BIT),
        .miso = NXP_PORT_GPIO_PIN(RTE_SPI8_MISO_PORT, RTE_SPI8_MISO_BIT),
    },
};

spi_inst_t spi_instances[SPI_INSTANCES_NUM] = {
    /* SPI Instance 0 */
    {
        .driver = &Driver_SPI3,
        .is_used = false,
        .pin_map = &SPI3_pin_set[0U],
        .pin_map_len = MP_ARRAY_SIZE(SPI3_pin_set),
    },

    /* SPI Instance 1 */
    {
        .driver = &Driver_SPI8,
        .is_used = false,
        .pin_map = &SPI8_pin_set[0U],
        .pin_map_len = MP_ARRAY_SIZE(SPI8_pin_set),
    },
};

void spi_enable(spi_inst_t *spi_instance) {
    if (3U == spi_instance->id) {
        CLOCK_AttachClk(kFRO12M_to_FLEXCOMM0);
    } else if (8U == spi_instance->id) {
        CLOCK_AttachClk(kFRO12M_to_HSLSPI);
    }
}

void spi_disable(spi_inst_t *spi_instance) {
    if (3U == spi_instance->id) {
        CLOCK_AttachClk(kNONE_to_FLEXCOMM0);
    } else if (8U == spi_instance->id) {
        CLOCK_AttachClk(kNONE_to_HSLSPI);
    }
}
