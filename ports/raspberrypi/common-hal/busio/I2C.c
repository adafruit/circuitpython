// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared-bindings/busio/I2C.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/bitbangio/I2C.h"

#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "supervisor/port.h"

#include <string.h>

// Synopsys  DW_apb_i2c  (v2.01)  IP

#define NO_PIN 0xff

// One second
#define BUS_TIMEOUT_US 1000000

struct i2c_transfer_state {
    uint dma_channel;
    uint paired_dma_channel;
    bool pending_last_command;
    i2c_inst_t *i2c;
    uint8_t *tx_copy;
    uint32_t repeated_command;
    uint32_t last_command;
};

typedef struct i2c_transfer_state i2c_transfer_state;

static i2c_transfer_state i2c_dma_channel_state[NUM_DMA_CHANNELS];

// Addresses of the form 000 0xxx or 111 1xxx are reserved. No slave should
// have these addresses.
#define i2c_reserved_addr(addr) (((addr) & 0x78) == 0 || ((addr) & 0x78) == 0x78)

static i2c_inst_t *i2c_hw_instances[2] = {i2c0, i2c1};

static i2c_transfer_state *common_hal_i2c_write_dma(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop) {
    invalid_params_if(HARDWARE_I2C, addr >= 0x80); // 7-bit addresses
    invalid_params_if(HARDWARE_I2C, i2c_reserved_addr(addr));
    invalid_params_if(HARDWARE_I2C, len == 0);
    invalid_params_if(HARDWARE_I2C, ((int)len) < 0);

    uint dma_channel = dma_claim_unused_channel(true);
    i2c_transfer_state *state = &i2c_dma_channel_state[dma_channel];
    state->dma_channel = dma_channel;

    i2c->hw->enable = 0;
    i2c->hw->tar = addr;
    i2c->hw->enable = 1;

    state->paired_dma_channel = NUM_DMA_CHANNELS;
    state->pending_last_command = false;
    state->i2c = i2c;
    state->tx_copy = NULL;
    state->repeated_command = 0;
    state->last_command = 0;

    if (len == 1) {
        uint32_t command =
            bool_to_bit(i2c->restart_on_next) << I2C_IC_DATA_CMD_RESTART_LSB |
                bool_to_bit(!nostop) << I2C_IC_DATA_CMD_STOP_LSB |
                src[0];
        i2c->hw->data_cmd = command;
        state->last_command = command;
        i2c->restart_on_next = nostop;
        return state;
    }

    i2c->hw->data_cmd =
        bool_to_bit(i2c->restart_on_next) << I2C_IC_DATA_CMD_RESTART_LSB |
            src[0];

    size_t middle_len = len - 2;
    if (middle_len > 0) {
        state->tx_copy = port_malloc(middle_len, true);
        if (state->tx_copy == NULL) {
            dma_channel_unclaim(dma_channel);
            m_malloc_fail(middle_len);
        }
        memcpy(state->tx_copy, src + 1, middle_len);

        dma_channel_config config = dma_channel_get_default_config(dma_channel);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
        channel_config_set_read_increment(&config, true);
        channel_config_set_write_increment(&config, false);
        channel_config_set_dreq(&config, I2C_DREQ_NUM(i2c, true));

        dma_channel_configure(dma_channel, &config, &i2c->hw->data_cmd,
            state->tx_copy, middle_len, true);
    }

    state->pending_last_command = true;
    state->last_command =
        bool_to_bit(!nostop) << I2C_IC_DATA_CMD_STOP_LSB |
            src[len - 1];
    i2c->restart_on_next = nostop;
    return state;
}

static i2c_transfer_state *common_hal_i2c_read_dma(i2c_inst_t *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop) {
    invalid_params_if(HARDWARE_I2C, addr >= 0x80); // 7-bit addresses
    invalid_params_if(HARDWARE_I2C, i2c_reserved_addr(addr));
    invalid_params_if(HARDWARE_I2C, len == 0);
    invalid_params_if(HARDWARE_I2C, ((int)len) < 0);
    uint tx_dma_channel = dma_claim_unused_channel(true);
    uint rx_dma_channel = dma_claim_unused_channel(true);
    i2c_transfer_state *state = &i2c_dma_channel_state[tx_dma_channel];
    state->dma_channel = tx_dma_channel;

    i2c->hw->enable = 0;
    i2c->hw->tar = addr;
    i2c->hw->enable = 1;

    state->paired_dma_channel = rx_dma_channel;
    state->pending_last_command = false;
    state->i2c = i2c;
    state->tx_copy = NULL;
    state->repeated_command = 0;
    state->last_command = 0;

    dma_channel_config rx_config = dma_channel_get_default_config(rx_dma_channel);
    channel_config_set_transfer_data_size(&rx_config, DMA_SIZE_8);
    channel_config_set_read_increment(&rx_config, false);
    channel_config_set_write_increment(&rx_config, true);
    channel_config_set_dreq(&rx_config, I2C_DREQ_NUM(i2c, false));

    dma_channel_configure(rx_dma_channel, &rx_config, dst, &i2c->hw->data_cmd, len, true);

    uint32_t first_command =
        bool_to_bit(i2c->restart_on_next) << I2C_IC_DATA_CMD_RESTART_LSB |
            I2C_IC_DATA_CMD_CMD_BITS;
    uint32_t middle_command = I2C_IC_DATA_CMD_CMD_BITS;
    uint32_t last_command =
        bool_to_bit(!nostop) << I2C_IC_DATA_CMD_STOP_LSB |
            I2C_IC_DATA_CMD_CMD_BITS;

    if (len == 1) {
        uint32_t command = first_command | last_command;
        i2c->hw->data_cmd = command;
        state->last_command = command;
    } else {
        i2c->hw->data_cmd = first_command;

        size_t middle_len = len - 2;
        if (middle_len > 0) {
            dma_channel_config tx_config = dma_channel_get_default_config(tx_dma_channel);
            channel_config_set_transfer_data_size(&tx_config, DMA_SIZE_32);
            channel_config_set_read_increment(&tx_config, false);
            channel_config_set_write_increment(&tx_config, false);
            channel_config_set_dreq(&tx_config, I2C_DREQ_NUM(i2c, true));

            state->repeated_command = middle_command;

            dma_channel_configure(tx_dma_channel, &tx_config, &i2c->hw->data_cmd,
                &state->repeated_command, middle_len, true);
        }

        state->pending_last_command = true;
        state->last_command = last_command;
    }

    i2c->restart_on_next = nostop;
    return state;
}

static bool common_hal_i2c_dma_is_busy(i2c_transfer_state *state) {
    if (state == NULL) {
        return false;
    }
    uint dma_channel = state->dma_channel;
    if (dma_channel_is_busy(dma_channel)) {
        return true;
    }

    if (state->pending_last_command) {
        if (!i2c_get_write_available(state->i2c)) {
            return true;
        }

        state->i2c->hw->data_cmd = state->last_command;
        state->pending_last_command = false;
        return true;
    }

    uint paired_dma_channel = state->paired_dma_channel;
    if (paired_dma_channel < NUM_DMA_CHANNELS && dma_channel_is_busy(paired_dma_channel)) {
        return true;
    }

    if (paired_dma_channel < NUM_DMA_CHANNELS) {
        dma_channel_unclaim(paired_dma_channel);
        state->paired_dma_channel = NUM_DMA_CHANNELS;
    }

    if (!(state->i2c->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS)) {
        return true;
    }

    if (state->last_command & I2C_IC_DATA_CMD_STOP_BITS) {
        if (!(state->i2c->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_STOP_DET_BITS)) {
            return true;
        }
        state->i2c->hw->clr_stop_det;
    }

    if (state->tx_copy != NULL) {
        port_free(state->tx_copy);
        state->tx_copy = NULL;
    }

    state->i2c = NULL;

    dma_channel_unclaim(dma_channel);
    return false;
}

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {

    // Ensure object starts in its deinit state.
    common_hal_busio_i2c_mark_deinit(self);

    self->peripheral = NULL;
    // I2C pins have a regular pattern. SCL is always odd and SDA is even. They match up in pairs
    // so we can divide by two to get the instance. This pattern repeats.
    size_t scl_instance = (scl->number / 2) % 2;
    size_t sda_instance = (sda->number / 2) % 2;
    if (scl->number % 2 == 1 && sda->number % 2 == 0 && scl_instance == sda_instance) {
        self->peripheral = i2c_hw_instances[sda_instance];
    }
    if (self->peripheral == NULL) {
        raise_ValueError_invalid_pins();
    }
    if ((i2c_get_hw(self->peripheral)->enable & I2C_IC_ENABLE_ENABLE_BITS) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("I2C peripheral in use"));
    }

    mp_arg_validate_int_max(frequency, 1000000, MP_QSTR_frequency);


    #if CIRCUITPY_REQUIRE_I2C_PULLUPS
    // Test that the pins are in a high state. (Hopefully indicating they are pulled up.)
    gpio_set_function(sda->number, GPIO_FUNC_SIO);
    gpio_set_function(scl->number, GPIO_FUNC_SIO);
    gpio_set_dir(sda->number, GPIO_IN);
    gpio_set_dir(scl->number, GPIO_IN);

    gpio_set_pulls(sda->number, false, true);
    gpio_set_pulls(scl->number, false, true);

    common_hal_mcu_delay_us(10);

    gpio_set_pulls(sda->number, false, false);
    gpio_set_pulls(scl->number, false, false);

    // We must pull up within 3us to achieve 400khz.
    common_hal_mcu_delay_us(3);

    if (!gpio_get(sda->number) || !gpio_get(scl->number)) {
        reset_pin_number(sda->number);
        reset_pin_number(scl->number);
        mp_raise_RuntimeError(MP_ERROR_TEXT("No pull up found on SDA or SCL; check your wiring"));
    }
    #endif

    // Create a bitbangio.I2C object to do 0 byte writes.
    //
    // These are used to non-invasively detect I2C devices by sending
    // the address and confirming an ACK.
    // They are not supported by the RP2040 hardware.
    //
    // Must be done before setting up the I2C pins, since they will be
    // set up as GPIO by the bitbangio.I2C object.
    //
    // Sets pins to open drain, high, and input.
    //
    // Do not use the default supplied clock stretching timeout here.
    // It is too short for some devices. Use the busio timeout instead.
    shared_module_bitbangio_i2c_construct(&self->bitbangio_i2c, MP_OBJ_FROM_PTR(scl), MP_OBJ_FROM_PTR(sda),
        frequency, BUS_TIMEOUT_US);

    self->baudrate = i2c_init(self->peripheral, frequency);

    self->scl_pin = scl->number;
    self->sda_pin = sda->number;
    claim_pin(scl);
    claim_pin(sda);

    gpio_set_function(self->scl_pin, GPIO_FUNC_I2C);
    gpio_set_function(self->sda_pin, GPIO_FUNC_I2C);
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda_pin == NO_PIN;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }

    i2c_deinit(self->peripheral);

    reset_pin_number(self->sda_pin);
    reset_pin_number(self->scl_pin);
    common_hal_busio_i2c_mark_deinit(self);
}

void common_hal_busio_i2c_mark_deinit(busio_i2c_obj_t *self) {
    self->sda_pin = NO_PIN;
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    return common_hal_busio_i2c_write(self, addr, NULL, 0) == 0;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return false;
    }
    bool grabbed_lock = false;
    if (!self->has_lock) {
        grabbed_lock = true;
        self->has_lock = true;
    }
    return grabbed_lock;
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
}

static mp_negative_errno_t _common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len, bool transmit_stop_bit) {
    if (len == 0) {
        // The RP2040 I2C peripheral will not perform 0 byte writes.
        // So use bitbangio.I2C to do the write.

        gpio_set_function(self->scl_pin, GPIO_FUNC_SIO);
        gpio_set_function(self->sda_pin, GPIO_FUNC_SIO);
        gpio_set_dir(self->scl_pin, GPIO_IN);
        gpio_set_dir(self->sda_pin, GPIO_IN);
        gpio_put(self->scl_pin, false);
        gpio_put(self->sda_pin, false);

        uint8_t status = shared_module_bitbangio_i2c_write(&self->bitbangio_i2c,
            addr, data, len, transmit_stop_bit);

        // The pins must be set back to GPIO_FUNC_I2C in the order given here,
        // SCL first, otherwise reads will hang.
        gpio_set_function(self->scl_pin, GPIO_FUNC_I2C);
        gpio_set_function(self->sda_pin, GPIO_FUNC_I2C);

        return status;
    }

    size_t result = i2c_write_timeout_us(self->peripheral, addr, data, len, !transmit_stop_bit, BUS_TIMEOUT_US);
    if (result == len) {
        return 0;
    }
    switch (result) {
        case PICO_ERROR_GENERIC:
            return -MP_ENODEV;
        case PICO_ERROR_TIMEOUT:
            return -MP_ETIMEDOUT;
        default:
            return -MP_EIO;
    }
}

mp_negative_errno_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len) {
    return _common_hal_busio_i2c_write(self, addr, data, len, true);
}

mp_negative_errno_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *data, size_t len) {
    size_t result = i2c_read_timeout_us(self->peripheral, addr, data, len, false, BUS_TIMEOUT_US);
    if (result == len) {
        return 0;
    }
    switch (result) {
        case PICO_ERROR_GENERIC:
            return -MP_ENODEV;
        case PICO_ERROR_TIMEOUT:
            return -MP_ETIMEDOUT;
        default:
            return -MP_EIO;
    }
}

mp_negative_errno_t common_hal_busio_i2c_write_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *out_data, size_t out_len, uint8_t *in_data, size_t in_len) {
    mp_negative_errno_t result = _common_hal_busio_i2c_write(self, addr, out_data, out_len, false);
    if (result != 0) {
        return result;
    }

    return common_hal_busio_i2c_read(self, addr, in_data, in_len);
}

void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    never_reset_pin_number(self->scl_pin);
    never_reset_pin_number(self->sda_pin);
}

i2c_transfer_state *common_hal_busio_i2c_start_read(busio_i2c_obj_t *i2c, uint8_t address, uint8_t *data, size_t len, bool nostop) {
    return common_hal_i2c_read_dma(i2c->peripheral, address, data, len, nostop);
}

i2c_transfer_state *common_hal_busio_i2c_start_write(busio_i2c_obj_t *i2c, uint8_t address, const uint8_t *data, size_t len, bool nostop) {
    return common_hal_i2c_write_dma(i2c->peripheral, address, data, len, nostop);
}

bool common_hal_busio_i2c_read_isbusy(i2c_transfer_state *state) {
    return common_hal_i2c_dma_is_busy(state);
}

bool common_hal_busio_i2c_write_isbusy(i2c_transfer_state *state) {
    return common_hal_i2c_dma_is_busy(state);
}
