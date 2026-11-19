// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vegard Wærp
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/paralleldisplaybus/ParallelBus.h"

#include <stdint.h>

#include "common-hal/microcontroller/Pin.h"
#include "peripherals/periph.h"
#include "py/runtime.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

// The bus is FSMC bank 1 (NOR/SRAM 1) in 8-bit SRAM mode: each byte written to
// the bank is clocked out with a write (NWE) pulse. Chip select and
// command/data are plain GPIOs, so no FSMC address lines are used.
#define FSMC_BANK1_DATA (*(volatile uint8_t *)0x60000000)

static const mcu_periph_obj_t *find_fsmc_pin(const mcu_periph_obj_t *list, size_t len,
    const mcu_pin_obj_t *pin) {
    for (size_t i = 0; i < len; i++) {
        if (list[i].pin == pin) {
            return &list[i];
        }
    }
    return NULL;
}

static void fsmc_pin_init(const mcu_periph_obj_t *periph) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin_mask(periph->pin->number);
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = periph->altfn_index;
    HAL_GPIO_Init(pin_port(periph->pin->port), &GPIO_InitStruct);
    common_hal_mcu_pin_claim(periph->pin);
}

static void output_pin_init(digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin, bool value) {
    self->base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(self, pin);
    common_hal_digitalio_digitalinout_switch_to_output(self, value, DRIVE_MODE_PUSH_PULL);
}

void common_hal_paralleldisplaybus_parallelbus_construct(paralleldisplaybus_parallelbus_obj_t *self,
    const mcu_pin_obj_t *data0, const mcu_pin_obj_t *command, const mcu_pin_obj_t *chip_select,
    const mcu_pin_obj_t *write, const mcu_pin_obj_t *read, const mcu_pin_obj_t *reset, uint32_t frequency) {
    // The FSMC data lines are spread over several ports, so they must be given as data_pins.
    raise_ValueError_invalid_pin_name(MP_QSTR_data0);
}

void common_hal_paralleldisplaybus_parallelbus_construct_nonsequential(paralleldisplaybus_parallelbus_obj_t *self,
    uint8_t n_pins, const mcu_pin_obj_t **data_pins, const mcu_pin_obj_t *command, const mcu_pin_obj_t *chip_select,
    const mcu_pin_obj_t *write, const mcu_pin_obj_t *read, const mcu_pin_obj_t *reset, uint32_t frequency) {

    // Check every pin before claiming any of them.
    if (n_pins != MP_ARRAY_SIZE(self->data_pins)) {
        raise_ValueError_invalid_pins();
    }
    const mcu_periph_obj_t *data_periphs[MP_ARRAY_SIZE(self->data_pins)];
    for (uint8_t i = 0; i < n_pins; i++) {
        data_periphs[i] = find_fsmc_pin(mcu_fsmc_data_list, MP_ARRAY_SIZE(mcu_fsmc_data_list), data_pins[i]);
        if (data_periphs[i] == NULL || data_periphs[i]->periph_index != i) {
            raise_ValueError_invalid_pin_name(MP_QSTR_data_pins);
        }
    }
    const mcu_periph_obj_t *write_strobe = find_fsmc_pin(mcu_fsmc_nwe_list, MP_ARRAY_SIZE(mcu_fsmc_nwe_list), write);
    if (write_strobe == NULL) {
        raise_ValueError_invalid_pin_name(MP_QSTR_write);
    }
    const mcu_periph_obj_t *read_strobe = NULL;
    if (read != NULL) {
        read_strobe = find_fsmc_pin(mcu_fsmc_noe_list, MP_ARRAY_SIZE(mcu_fsmc_noe_list), read);
        if (read_strobe == NULL) {
            raise_ValueError_invalid_pin_name(MP_QSTR_read);
        }
    }
    // A write takes DATAST + 3 HCLK cycles (codal-stm32, used by MakeCode
    // Arcade, gives 72 ns for DATAST = 4 at 96 MHz). DATAST is at most 255, so
    // the slowest write takes 258 cycles. Refuse a lower frequency rather than
    // exceed it.
    mp_arg_validate_int_min(frequency, (SystemCoreClock + 257) / 258, MP_QSTR_frequency);

    for (uint8_t i = 0; i < n_pins; i++) {
        fsmc_pin_init(data_periphs[i]);
        self->data_pins[i] = data_pins[i];
    }
    fsmc_pin_init(write_strobe);
    self->write = write;
    if (read_strobe != NULL) {
        fsmc_pin_init(read_strobe);
    }
    self->read = read;

    // Use the shortest write that stays within frequency: at most 258 cycles,
    // so DATAST fits.
    uint32_t cycles = (SystemCoreClock + frequency - 1) / frequency;
    uint32_t datast = MAX(cycles, 4) - 3;

    __HAL_RCC_FSMC_CLK_ENABLE();
    // 8-bit SRAM with writes enabled and separate write timing. The write FIFO
    // is off so each byte is on the bus before command/data or chip select
    // changes. 0x80 keeps a reserved bit at its reset value. These are the
    // values of codal-stm32's FSMC driver, used by MakeCode Arcade, except
    // DATAST, which comes from frequency.
    FSMC_Bank1->BTCR[0] = FSMC_BCR1_EXTMOD | FSMC_BCR1_WREN | FSMC_BCR1_WFDIS | 0x80;
    FSMC_Bank1->BTCR[1] = 0x112419; // Read timing
    FSMC_Bank1E->BWTR[0] = 0x0ff00010 | (datast << FSMC_BWTR1_DATAST_Pos);
    FSMC_Bank1->BTCR[0] |= FSMC_BCR1_MBKEN;

    output_pin_init(&self->command, command, true);
    output_pin_init(&self->chip_select, chip_select, true);

    self->reset.base.type = &mp_type_NoneType;
    if (reset != NULL) {
        output_pin_init(&self->reset, reset, true);
        common_hal_paralleldisplaybus_parallelbus_reset(self);
    }
}

void common_hal_paralleldisplaybus_parallelbus_deinit(paralleldisplaybus_parallelbus_obj_t *self) {
    // The bus lives in a display bus slot, which is deinited even if construct
    // raised. write is set only once every check passed, so guard on it:
    // otherwise this would turn off the FSMC bank under another display.
    if (self->write == NULL) {
        return;
    }

    FSMC_Bank1->BTCR[0] &= ~FSMC_BCR1_MBKEN;

    for (uint8_t i = 0; i < MP_ARRAY_SIZE(self->data_pins); i++) {
        common_hal_reset_pin(self->data_pins[i]);
    }
    common_hal_reset_pin(self->write);
    if (self->read != NULL) {
        common_hal_reset_pin(self->read);
    }

    common_hal_digitalio_digitalinout_deinit(&self->command);
    common_hal_digitalio_digitalinout_deinit(&self->chip_select);
    if (self->reset.base.type == &digitalio_digitalinout_type) {
        common_hal_digitalio_digitalinout_deinit(&self->reset);
    }
}

bool common_hal_paralleldisplaybus_parallelbus_reset(mp_obj_t obj) {
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    if (self->reset.base.type == &mp_type_NoneType) {
        return false;
    }

    // Long enough for common controllers: the ILI9341 needs a 10 us pulse and
    // 5 ms before the first command.
    common_hal_digitalio_digitalinout_set_value(&self->reset, false);
    common_hal_mcu_delay_us(10);
    common_hal_digitalio_digitalinout_set_value(&self->reset, true);
    common_hal_mcu_delay_us(5000);
    return true;
}

bool common_hal_paralleldisplaybus_parallelbus_bus_free(mp_obj_t obj) {
    return true;
}

bool common_hal_paralleldisplaybus_parallelbus_begin_transaction(mp_obj_t obj) {
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, false);
    return true;
}

// This ignores chip_select behaviour because data is clocked in by the write line toggling.
void common_hal_paralleldisplaybus_parallelbus_send(mp_obj_t obj, display_byte_type_t byte_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length) {
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->command, byte_type == DISPLAY_DATA);
    for (uint32_t i = 0; i < data_length; i++) {
        FSMC_BANK1_DATA = data[i];
    }
    // Let the last write finish before command/data or chip select changes.
    __DSB();
}

void common_hal_paralleldisplaybus_parallelbus_end_transaction(mp_obj_t obj) {
    paralleldisplaybus_parallelbus_obj_t *self = MP_OBJ_TO_PTR(obj);
    common_hal_digitalio_digitalinout_set_value(&self->chip_select, true);
}
