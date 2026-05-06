// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/audiobusio/I2SOut.h"

#include <string.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>

#include "bindings/zephyr_kernel/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/audiocore/__init__.h"
#include "py/runtime.h"

#if CIRCUITPY_AUDIOBUSIO_I2SOUT

#define AUDIO_THREAD_STACK_SIZE 2048
#define AUDIO_THREAD_PRIORITY 5

// Forward declarations
static void fill_buffer(audiobusio_i2sout_obj_t *self, uint8_t *buffer, size_t buffer_size);
static void audio_thread_func(void *self_in, void *unused1, void *unused2);

// Helper function for Zephyr-specific initialization from device tree
mp_obj_t common_hal_audiobusio_i2sout_construct_from_device(audiobusio_i2sout_obj_t *self, const struct device *i2s_device) {
    self->base.type = &audiobusio_i2sout_type;
    self->i2s_dev = i2s_device;
    self->left_justified = false;
    self->playing = false;
    self->paused = false;
    self->sample = NULL;
    self->slab_buffer = NULL;
    self->thread_stack = NULL;
    self->thread_id = NULL;
    self->block_size = 0;

    return MP_OBJ_FROM_PTR(self);
}

// Standard audiobusio construct - not used in Zephyr port (devices come from device tree)
void common_hal_audiobusio_i2sout_construct(audiobusio_i2sout_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, const mcu_pin_obj_t *main_clock, bool left_justified) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Use device tree to define %q devices"), MP_QSTR_I2S);
}

bool common_hal_audiobusio_i2sout_deinited(audiobusio_i2sout_obj_t *self) {
    return self->i2s_dev == NULL;
}

void common_hal_audiobusio_i2sout_deinit(audiobusio_i2sout_obj_t *self) {
    if (common_hal_audiobusio_i2sout_deinited(self)) {
        return;
    }

    // Stop playback (which will free buffers)
    common_hal_audiobusio_i2sout_stop(self);

    // Note: Pins and I2S device are managed by Zephyr, not released here
    self->i2s_dev = NULL;
}

static void fill_buffer(audiobusio_i2sout_obj_t *self, uint8_t *buffer, size_t buffer_size) {
    if (self->sample == NULL || self->paused || self->stopping) {
        // Fill with silence
        memset(buffer, 0, buffer_size);
        return;
    }

    uint32_t bytes_filled = 0;
    while (bytes_filled < buffer_size) {
        uint8_t *sample_buffer;
        uint32_t sample_buffer_length;

        audioio_get_buffer_result_t result = audiosample_get_buffer(
            self->sample, false, 0, &sample_buffer, &sample_buffer_length);

        if (result == GET_BUFFER_ERROR) {
            // Error getting buffer, stop playback
            self->stopping = true;
            memset(buffer + bytes_filled, 0, buffer_size - bytes_filled);
            return;
        }

        if (result == GET_BUFFER_DONE) {
            if (self->loop) {
                // Reset to beginning
                audiosample_reset_buffer(self->sample, false, 0);
            } else {
                // Done playing, fill rest with silence
                self->stopping = true;
                i2s_trigger(self->i2s_dev, I2S_DIR_TX, I2S_TRIGGER_DRAIN);
                memset(buffer + bytes_filled, 0, buffer_size - bytes_filled);
                return;
            }
        }

        // Copy data to buffer
        uint32_t bytes_to_copy = sample_buffer_length;
        if (bytes_filled + bytes_to_copy > buffer_size) {
            bytes_to_copy = buffer_size - bytes_filled;
        }

        memcpy(buffer + bytes_filled, sample_buffer, bytes_to_copy);
        bytes_filled += bytes_to_copy;
    }
}

static void audio_thread_func(void *self_in, void *unused1, void *unused2) {
    audiobusio_i2sout_obj_t *self = (audiobusio_i2sout_obj_t *)self_in;

    while (!self->stopping) {
        uint8_t *next_buffer = NULL;
        // Wait until I2S has freed the buffer it is sending.
        if (k_mem_slab_alloc(&self->mem_slab, (void **)&next_buffer, K_FOREVER) != 0) {
            break;
        }
        if (self->stopping) {
            // Stopping so break.
            k_mem_slab_free(&self->mem_slab, next_buffer);
            break;
        }
        fill_buffer(self, next_buffer, self->block_size);

        // Write to I2S
        int ret = i2s_write(self->i2s_dev, next_buffer, self->block_size);
        if (ret < 0) {
            printk("i2s_write failed: %d\n", ret);
            k_mem_slab_free(&self->mem_slab, next_buffer);
            // Error writing, stop playback
            self->playing = false;
            break;
        }
    }
}

void common_hal_audiobusio_i2sout_play(audiobusio_i2sout_obj_t *self,
    mp_obj_t sample, bool loop) {
    // Stop any existing playback
    if (self->playing) {
        common_hal_audiobusio_i2sout_stop(self);
    }

    // Get sample information
    uint8_t bits_per_sample = audiosample_get_bits_per_sample(sample);
    uint32_t sample_rate = audiosample_get_sample_rate(sample);
    uint8_t channel_count = audiosample_get_channel_count(sample);

    // Store sample parameters
    self->sample = sample;
    self->loop = loop;
    self->bytes_per_sample = bits_per_sample / 8;
    self->channel_count = channel_count;
    self->stopping = false;

    // Get buffer structure from the sample
    bool single_buffer, samples_signed;
    uint32_t max_buffer_length;
    uint8_t sample_spacing;
    audiosample_get_buffer_structure(sample, /* single_channel_output */ false,
        &single_buffer, &samples_signed, &max_buffer_length, &sample_spacing);

    // Use max_buffer_length from the sample as the block size
    self->block_size = max_buffer_length;
    if (channel_count == 1) {
        // Make room for stereo samples.
        self->block_size *= 2;
    }
    size_t block_size = self->block_size;
    uint32_t num_blocks = 4; // Use 4 blocks for buffering

    // Allocate memory slab buffer
    self->slab_buffer = m_malloc(self->block_size * num_blocks);

    // Initialize memory slab
    int ret = k_mem_slab_init(&self->mem_slab, self->slab_buffer, block_size, num_blocks);
    CHECK_ZEPHYR_RESULT(ret);

    // Configure I2S
    struct i2s_config config;
    config.word_size = bits_per_sample;
    config.channels = 2;
    config.format = self->left_justified ? I2S_FMT_DATA_FORMAT_LEFT_JUSTIFIED : I2S_FMT_DATA_FORMAT_I2S;
    config.options = I2S_OPT_BIT_CLK_MASTER | I2S_OPT_FRAME_CLK_MASTER;
    config.frame_clk_freq = sample_rate;
    config.mem_slab = &self->mem_slab;
    config.block_size = block_size;
    config.timeout = 1000; // Not a k_timeout_t. In milliseconds.

    // Configure returns EINVAL if the I2S device is not ready. We loop on this
    // because it should be ready after it comes to a complete stop.
    ret = -EAGAIN;
    while (ret == -EAGAIN) {
        ret = i2s_configure(self->i2s_dev, I2S_DIR_TX, &config);
    }
    if (ret != 0) {
        common_hal_audiobusio_i2sout_stop(self);
        raise_zephyr_error(ret);
    }

    // Fill every slab before starting playback to avoid underruns.
    for (uint32_t i = 0; i < num_blocks; i++) {
        uint8_t *buf = NULL;
        k_mem_slab_alloc(&self->mem_slab, (void **)&buf, K_NO_WAIT);
        fill_buffer(self, buf, block_size);
        ret = i2s_write(self->i2s_dev, buf, block_size);
        if (ret < 0) {
            printk("i2s_write failed: %d\n", ret);
            k_mem_slab_free(&self->mem_slab, buf);
            common_hal_audiobusio_i2sout_stop(self);
            raise_zephyr_error(ret);
        }
    }

    // Allocate thread stack with proper MPU alignment for HW stack protection
    self->thread_stack = k_thread_stack_alloc(AUDIO_THREAD_STACK_SIZE, 0);

    // Create and start audio processing thread
    self->thread_id = k_thread_create(&self->thread, self->thread_stack,
        AUDIO_THREAD_STACK_SIZE,
        audio_thread_func,
        self, NULL, NULL,
        AUDIO_THREAD_PRIORITY, 0, K_NO_WAIT);
    // Start I2S
    ret = i2s_trigger(self->i2s_dev, I2S_DIR_TX, I2S_TRIGGER_START);
    if (ret < 0) {
        common_hal_audiobusio_i2sout_stop(self);
        raise_zephyr_error(ret);
    }

    self->playing = true;
}

void common_hal_audiobusio_i2sout_stop(audiobusio_i2sout_obj_t *self) {
    if (!self->playing) {
        return;
    }

    self->playing = false;
    self->paused = false;
    self->stopping = true;

    // Stop I2S
    i2s_trigger(self->i2s_dev, I2S_DIR_TX, I2S_TRIGGER_DROP);

    // Wait for thread to finish
    if (self->thread_id != NULL) {
        k_thread_join(self->thread_id, K_FOREVER);
        self->thread_id = NULL;
    }

    // Free thread stack
    if (self->thread_stack != NULL) {
        k_thread_stack_free(self->thread_stack);
        self->thread_stack = NULL;
    }

    // Free buffers
    if (self->slab_buffer != NULL) {
        m_free(self->slab_buffer);
        self->slab_buffer = NULL;
    }

    self->sample = NULL;
}

bool common_hal_audiobusio_i2sout_get_playing(audiobusio_i2sout_obj_t *self) {
    return self->playing;
}

void common_hal_audiobusio_i2sout_pause(audiobusio_i2sout_obj_t *self) {
    if (!self->playing || self->paused) {
        return;
    }

    self->paused = true;
    i2s_trigger(self->i2s_dev, I2S_DIR_TX, I2S_TRIGGER_STOP);
}

void common_hal_audiobusio_i2sout_resume(audiobusio_i2sout_obj_t *self) {
    if (!self->playing || !self->paused) {
        return;
    }

    self->paused = false;
    // Thread will automatically resume filling buffers
    i2s_trigger(self->i2s_dev, I2S_DIR_TX, I2S_TRIGGER_START);
}

bool common_hal_audiobusio_i2sout_get_paused(audiobusio_i2sout_obj_t *self) {
    return self->paused;
}

#endif // CIRCUITPY_AUDIOBUSIO_I2SOUT
