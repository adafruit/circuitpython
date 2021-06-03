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

#include <math.h>
#include <string.h>

#include "py/mphal.h"
#include "common-hal/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

#if (0)
#include "src/rp2_common/hardware_adc/include/hardware/adc.h"
#include "src/rp2_common/hardware_clocks/include/hardware/clocks.h"

float common_hal_mcu_processor_get_temperature(void) {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
    uint16_t value = adc_read();
    adc_set_temp_sensor_enabled(false);
    float voltage = (float)value;
    voltage = voltage * (3.3f / (1.0f << 12));
    // TODO: turn the ADC back off
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}

float common_hal_mcu_processor_get_voltage(void) {
    return 3.3f;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return clock_get_hz(clk_sys);
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    pico_unique_board_id_t retrieved_id;
    pico_get_unique_board_id(&retrieved_id);
    memcpy(raw_id, retrieved_id.id, COMMON_HAL_MCU_PROCESSOR_UID_LENGTH);
}
#endif

float common_hal_mcu_processor_get_temperature(void)
{
#if (1)
    return 0.0f;
#else
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);
    uint16_t value = adc_read();
    adc_set_temp_sensor_enabled(false);
    float voltage = value * 3.3 / (1 << 12);
    // TODO: turn the ADC back off
    return 27 - (voltage - 0.706) / 0.001721;
#endif
}

float common_hal_mcu_processor_get_voltage(void)
{
#if (1)
    return 0.0f;
#else
    return 3.3f;
#endif
}

uint32_t common_hal_mcu_processor_get_frequency(void)
{
#if (1)
    return 0U;
#else
    return clock_get_hz(clk_sys);
#endif
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[])
{
#if (1)
    (void)raw_id;
    return;
#else
    pico_unique_board_id_t retrieved_id;
    pico_get_unique_board_id(&retrieved_id);
    memcpy(raw_id, retrieved_id.id, COMMON_HAL_MCU_PROCESSOR_UID_LENGTH);
#endif
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void)
{
    // TODO: Get the reason from RSID
    return RESET_REASON_UNKNOWN;
}
