
#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "py/obj.h"

#include "supervisor/background_callback.h"
#include "shared-module/audiocore/__init__.h"

#include "driver/dac_continuous.h"


#define NUM_DMA_BUFFERS 6
#define DMA_BUFFER_SIZE 512

#define DEFAULT_SAMPLE_RATE 32000

typedef bool (*audioout_sample_convert_func_t)(void *in_buffer, size_t in_buffer_size, uint8_t **out_buffer, uint32_t *out_buffer_size);

typedef struct {
    uint8_t *ptr;
    size_t size;
} buf_info_t;

typedef struct {
    mp_obj_base_t base;
    dac_continuous_handle_t handle;
    dac_channel_mask_t channel_mask;
    uint32_t freq_hz;
    uint8_t num_channels;
    dac_continuous_channel_mode_t channel_mode;
    mp_obj_t sample;
    bool playing;
    bool paused;
    bool looping;
    uint8_t *sample_buffer;
    size_t sample_buffer_size;
    audioio_get_buffer_result_t sample_buffer_result;
    uint8_t get_buffer_index;
    uint8_t put_buffer_index;
    buf_info_t dma_buffers[NUM_DMA_BUFFERS + 1];
    background_callback_t callback;
    uint8_t *scratch_buffer;
    size_t scratch_buffer_size;
    audioout_sample_convert_func_t samples_convert;
} audioio_audioout_obj_t;
