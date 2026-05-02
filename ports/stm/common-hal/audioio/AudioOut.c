// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

// DAC-based audio output for STM32F405 / STM32F407.
//
// Architecture:
//   TIM6 (basic timer) generates an update event at the sample rate.
//   DAC_CHANNEL_1 (PA04) is configured with DAC_TRIGGER_T6_TRGO so each
//   TIM6 update latches the next sample from the DMA FIFO.
//   DMA1 Stream5 Channel7 operates in circular mode, feeding 16-bit samples
//   from a double-buffer (512 samples = two 256-sample halves).
//   The DMA half-complete and complete callbacks queue a background_callback
//   to refill the idle half from the audio sample source.
//
// The shared DAC handle (declared in AnalogOut.c) is reused here so both
// modules share state consistently and avoid double-init conflicts.

#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "common-hal/audioio/AudioOut.h"
#include "shared-bindings/audioio/AudioOut.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/audiocore/__init__.h"
#include "supervisor/background_callback.h"

#include STM32_HAL_H

// Shared DAC handle declared in common-hal/analogio/AnalogOut.h.
// AudioOut reconfigures channel 1 for DMA-triggered operation and restores
// it on deinit.
#include "common-hal/analogio/AnalogOut.h"

// TIM6 handle: only one instance ever active, so file-scope is fine.
static TIM_HandleTypeDef tim6_handle;

// Pointer to the currently active AudioOut object, used by IRQ handlers.
static audioio_audioout_obj_t *active_audioout = NULL;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Return the TIM6 input clock frequency in Hz.
// TIM6 sits on APB1. The clock is doubled when the APB1 prescaler != 1
// (same logic as stm_peripherals_timer_get_source_freq in timers.c but
// applied directly to APB1 since TIM6 is not in mcu_tim_banks[]).
static uint32_t get_tim6_freq(void) {
    uint32_t apb1 = HAL_RCC_GetPCLK1Freq();
    uint32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos;
    #if defined(RCC_DCKCFGR_TIMPRE)
    uint32_t timpre = RCC->DCKCFGR & RCC_DCKCFGR_TIMPRE;
    if (timpre == 0) {
        return (ppre1 >= 0b100) ? apb1 * 2 : apb1;
    } else {
        return (ppre1 > 0b101) ? apb1 * 4 : HAL_RCC_GetHCLKFreq();
    }
    #else
    return (ppre1 >= 0b100) ? apb1 * 2 : apb1;
    #endif
}

// Gently ramp the given DAC channel output from from_12 to to_12 (both 12-bit,
// 0-4095) to avoid audible clicks. 64 steps, ~100 µs per step = ~6.4 ms total.
static void dac_ramp_channel(uint32_t channel, uint32_t from_12, uint32_t to_12) {
    if (from_12 == to_12) {
        return;
    }
    int32_t delta = (int32_t)to_12 - (int32_t)from_12;
    int32_t step = delta / 64;
    if (step == 0) {
        step = (delta > 0) ? 1 : -1;
    }
    int32_t v = (int32_t)from_12;
    while (true) {
        v += step;
        if (step > 0 && v >= (int32_t)to_12) {
            v = (int32_t)to_12;
        } else if (step < 0 && v <= (int32_t)to_12) {
            v = (int32_t)to_12;
        }
        HAL_DAC_SetValue(&handle, channel, DAC_ALIGN_12B_R, (uint16_t)v);
        HAL_DAC_Start(&handle, channel);
        mp_hal_delay_us(100);
        if (v == (int32_t)to_12) {
            break;
        }
    }
}

static inline void dac_ramp(uint32_t from_12, uint32_t to_12) {
    dac_ramp_channel(DAC_CHANNEL_1, from_12, to_12);
}

// Convert a buffer of audio samples into 12-bit unsigned DAC values and write
// them into dest[]. Returns the number of DAC samples written.
//
// src              - raw sample bytes from audiosample_get_buffer
// src_len          - byte length of src
// dest             - output array of 12-bit DAC values (uint16_t)
// dest_count       - capacity of dest in samples
// bytes_per_sample - 1 or 2
// samples_signed   - true if source samples are signed
// channel_count    - 1 (mono) or 2 (stereo)
// channel_offset   - 0 for left/mono, 1 for right channel of a stereo stream
// quiescent_12     - value to pad with if src runs short
static uint32_t convert_to_dac12(
    const uint8_t *src, uint32_t src_len,
    uint16_t *dest, uint32_t dest_count,
    uint8_t bytes_per_sample, bool samples_signed,
    uint8_t channel_count, uint8_t channel_offset,
    uint16_t quiescent_12) {

    // src_stride: bytes between consecutive samples of the same channel
    uint32_t src_stride = (uint32_t)bytes_per_sample * channel_count;
    // src_offset: byte offset to the desired channel within each frame
    uint32_t src_offset = (uint32_t)bytes_per_sample * channel_offset;
    uint32_t src_frames = src_len / src_stride;
    uint32_t frames = (src_frames < dest_count) ? src_frames : dest_count;

    if (bytes_per_sample == 1) {
        for (uint32_t i = 0; i < frames; i++) {
            uint8_t u8 = src[i * src_stride + src_offset];
            int32_t s = samples_signed
                ? (int32_t)(int8_t)u8
                : (int32_t)u8 - 128;
            dest[i] = (uint16_t)((s + 128) & 0xFF) << 4;
        }
    } else {
        for (uint32_t i = 0; i < frames; i++) {
            uint16_t u16;
            memcpy(&u16, src + i * src_stride + src_offset, 2);
            int32_t s = samples_signed
                ? (int32_t)(int16_t)u16
                : (int32_t)u16 - 0x8000;
            dest[i] = (uint16_t)((s + 0x8000) & 0xFFFF) >> 4;
        }
    }

    // Pad remainder with quiescent value.
    for (uint32_t i = frames; i < dest_count; i++) {
        dest[i] = quiescent_12;
    }
    return frames;
}

// Load one half of the DMA circular buffer from the audio sample source.
// half: 0 = lower half (indices 0..HALF-1), 1 = upper half (HALF..END-1).
//
// Tracks position within the source buffer (src_ptr / src_remaining_len)
// across calls so that large source buffers (e.g. a 1024-sample RawSample)
// are consumed incrementally rather than re-reading from the start each time.
static void load_dma_buffer_half(audioio_audioout_obj_t *self, uint8_t half) {
    uint16_t *dest_l = self->dma_buffer + ((uint32_t)half * AUDIOOUT_DMA_HALF_SAMPLES);
    uint16_t *dest_r = self->dma_buffer_r
        ? self->dma_buffer_r + ((uint32_t)half * AUDIOOUT_DMA_HALF_SAMPLES)
        : NULL;
    uint16_t quiescent_12 = self->quiescent_value >> 4;
    uint32_t src_stride = (uint32_t)self->bytes_per_sample * self->channel_count;
    uint32_t filled = 0;

    while (filled < AUDIOOUT_DMA_HALF_SAMPLES) {
        // Fetch new source data when the previous buffer is exhausted.
        if (self->src_remaining_len == 0) {
            // Handle end-of-stream from previous get_buffer call.
            if (self->src_done) {
                if (self->loop) {
                    audiosample_reset_buffer(self->sample, false, 0);
                    self->src_done = false;
                } else {
                    for (uint32_t i = filled; i < AUDIOOUT_DMA_HALF_SAMPLES; i++) {
                        dest_l[i] = quiescent_12;
                        if (dest_r) {
                            dest_r[i] = quiescent_12;
                        }
                    }
                    self->stopping = true;
                    return;
                }
            }

            uint8_t *buf;
            uint32_t len;
            audioio_get_buffer_result_t result =
                audiosample_get_buffer(self->sample, false, 0, &buf, &len);

            if (result == GET_BUFFER_ERROR) {
                for (uint32_t i = filled; i < AUDIOOUT_DMA_HALF_SAMPLES; i++) {
                    dest_l[i] = quiescent_12;
                    if (dest_r) {
                        dest_r[i] = quiescent_12;
                    }
                }
                self->stopping = true;
                return;
            }

            self->src_ptr = buf;
            self->src_remaining_len = len;
            self->src_done = (result == GET_BUFFER_DONE);
        }

        uint32_t written = convert_to_dac12(
            self->src_ptr, self->src_remaining_len,
            dest_l + filled, AUDIOOUT_DMA_HALF_SAMPLES - filled,
            self->bytes_per_sample, self->samples_signed,
            self->channel_count, 0, quiescent_12);

        if (dest_r) {
            uint8_t r_offset = (self->channel_count >= 2) ? 1 : 0;
            convert_to_dac12(
                self->src_ptr, self->src_remaining_len,
                dest_r + filled, AUDIOOUT_DMA_HALF_SAMPLES - filled,
                self->bytes_per_sample, self->samples_signed,
                self->channel_count, r_offset, quiescent_12);
        }

        // Advance source position by the amount consumed.
        uint32_t bytes_consumed = written * src_stride;
        self->src_ptr += bytes_consumed;
        self->src_remaining_len -= bytes_consumed;
        filled += written;
    }
}

// Background callback: called from the main loop after a DMA half/full event.
static void audioout_fill_callback(void *arg) {
    audioio_audioout_obj_t *self = (audioio_audioout_obj_t *)arg;
    if (!self || active_audioout != self) {
        return;
    }
    if (self->stopping) {
        common_hal_audioio_audioout_stop(self);
        return;
    }
    uint8_t mask;
    __disable_irq();
    mask = self->halves_to_fill;
    self->halves_to_fill = 0;
    __enable_irq();
    if (mask & 0x1) {
        load_dma_buffer_half(self, 0);
    }
    if (mask & 0x2) {
        load_dma_buffer_half(self, 1);
    }
}

// ---------------------------------------------------------------------------
// IRQ handlers (must be defined at file scope, not inside functions)
// ---------------------------------------------------------------------------

void DMA1_Stream5_IRQHandler(void) {
    if (active_audioout) {
        HAL_DMA_IRQHandler(&active_audioout->dma_handle);
    }
}

void DMA1_Stream6_IRQHandler(void) {
    if (active_audioout && active_audioout->dma_buffer_r) {
        HAL_DMA_IRQHandler(&active_audioout->dma_handle_r);
    }
}

// HAL weak-symbol overrides: called from HAL_DMA_IRQHandler context.

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
    if (active_audioout && !active_audioout->paused) {
        active_audioout->halves_to_fill |= 0x1;
        background_callback_add(&active_audioout->callback,
            audioout_fill_callback, active_audioout);
    }
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
    if (active_audioout && !active_audioout->paused) {
        active_audioout->halves_to_fill |= 0x2;
        background_callback_add(&active_audioout->callback,
            audioout_fill_callback, active_audioout);
    }
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void common_hal_audioio_audioout_construct(audioio_audioout_obj_t *self,
    const mcu_pin_obj_t *left_channel, const mcu_pin_obj_t *right_channel,
    uint16_t quiescent_value) {

    #if !HAS_DAC
    mp_raise_ValueError(MP_ERROR_TEXT("No DAC on chip"));
    #else

    // Only PA04 (DAC_CH1) is supported as left channel.
    if (left_channel != &pin_PA04) {
        mp_raise_ValueError(MP_ERROR_TEXT("AudioOut requires pin A0 (PA04)"));
    }

    // Right channel must be PA05 (DAC_CH2 / A1) if provided.
    if (right_channel != NULL && right_channel != &pin_PA05) {
        mp_raise_ValueError(MP_ERROR_TEXT("AudioOut right channel requires pin A1 (PA05)"));
    }

    self->left_channel = left_channel;
    self->right_channel = right_channel;
    self->quiescent_value = quiescent_value;
    self->sample = MP_OBJ_NULL;
    self->dma_buffer = NULL;
    self->dma_buffer_r = NULL;
    memset(&self->dma_handle, 0, sizeof(self->dma_handle));
    memset(&self->dma_handle_r, 0, sizeof(self->dma_handle_r));
    memset(&self->callback, 0, sizeof(self->callback));
    self->stopping = false;
    self->paused = false;

    // Configure PA04 (and PA05 if stereo) for analog (DAC) mode.
    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = pin_mask(left_channel->number);
    gpio_init.Mode = GPIO_MODE_ANALOG;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(pin_port(left_channel->port), &gpio_init);

    if (right_channel != NULL) {
        gpio_init.Pin = pin_mask(right_channel->number);
        HAL_GPIO_Init(pin_port(right_channel->port), &gpio_init);
    }

    // Initialise the shared DAC handle if it hasn't been set up yet
    // (i.e. AnalogOut hasn't been used since last reset).
    if (handle.Instance == NULL || handle.State == HAL_DAC_STATE_RESET) {
        __HAL_RCC_DAC_CLK_ENABLE();
        handle.Instance = DAC;
        if (HAL_DAC_Init(&handle) != HAL_OK) {
            mp_raise_ValueError(MP_ERROR_TEXT("DAC init error"));
        }
    }

    // Configure DAC channel 1 with TIM6_TRGO trigger (set at play() time;
    // for now configure with no trigger so the ramp works correctly).
    DAC_ChannelConfTypeDef ch_cfg = {0};
    ch_cfg.DAC_Trigger = DAC_TRIGGER_NONE;
    ch_cfg.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    if (HAL_DAC_ConfigChannel(&handle, &ch_cfg, DAC_CHANNEL_1) != HAL_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("DAC channel config error"));
    }

    // Ramp DAC output up to quiescent value to prevent an audible pop.
    HAL_DAC_SetValue(&handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
    HAL_DAC_Start(&handle, DAC_CHANNEL_1);
    dac_ramp(0, quiescent_value >> 4);

    // Claim pins last so any error above doesn't leave them claimed.
    common_hal_mcu_pin_claim(left_channel);
    if (right_channel != NULL) {
        common_hal_mcu_pin_claim(right_channel);
    }
    #endif
}

bool common_hal_audioio_audioout_deinited(audioio_audioout_obj_t *self) {
    return self->left_channel == NULL;
}

void common_hal_audioio_audioout_deinit(audioio_audioout_obj_t *self) {
    if (common_hal_audioio_audioout_deinited(self)) {
        return;
    }

    common_hal_audioio_audioout_stop(self);

    // Ramp DAC back to zero before disconnecting.
    dac_ramp(self->quiescent_value >> 4, 0);
    HAL_DAC_Stop(&handle, DAC_CHANNEL_1);

    // Restore channels to no-trigger mode so AnalogOut can use them again.
    DAC_ChannelConfTypeDef ch_cfg = {0};
    ch_cfg.DAC_Trigger = DAC_TRIGGER_NONE;
    ch_cfg.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&handle, &ch_cfg, DAC_CHANNEL_1);
    if (self->right_channel != NULL) {
        HAL_DAC_Stop(&handle, DAC_CHANNEL_2);
        HAL_DAC_ConfigChannel(&handle, &ch_cfg, DAC_CHANNEL_2);
        reset_pin_number(self->right_channel->port, self->right_channel->number);
        self->right_channel = NULL;
    }

    // Release the left pin. Let AnalogOut manage DAC clock disable.
    reset_pin_number(self->left_channel->port, self->left_channel->number);
    self->left_channel = NULL;
}

void common_hal_audioio_audioout_play(audioio_audioout_obj_t *self,
    mp_obj_t sample, bool loop) {
    if (common_hal_audioio_audioout_deinited(self)) {
        mp_raise_ValueError(MP_ERROR_TEXT("AudioOut is deinited"));
    }
    common_hal_audioio_audioout_stop(self);

    // Extract sample format metadata.
    audiosample_base_t *base = audiosample_check(sample);
    self->bytes_per_sample = base->bits_per_sample / 8;
    self->samples_signed = base->samples_signed;
    self->channel_count = base->channel_count;
    uint32_t sample_rate = base->sample_rate;
    if (sample_rate == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("sample_rate must be > 0"));
    }

    self->sample = sample;
    self->loop = loop;
    self->stopping = false;
    self->paused = false;
    self->halves_to_fill = 0;
    self->src_ptr = NULL;
    self->src_remaining_len = 0;
    self->src_done = false;

    // Allocate DMA circular buffer(s).
    self->dma_buffer = (uint16_t *)m_malloc(
        AUDIOOUT_DMA_BUFFER_SAMPLES * sizeof(uint16_t));
    if (!self->dma_buffer) {
        mp_raise_msg(&mp_type_MemoryError,
            MP_ERROR_TEXT("insufficient memory for audio buffer"));
    }
    if (self->right_channel != NULL) {
        self->dma_buffer_r = (uint16_t *)m_malloc(
            AUDIOOUT_DMA_BUFFER_SAMPLES * sizeof(uint16_t));
        if (!self->dma_buffer_r) {
            m_free(self->dma_buffer);
            self->dma_buffer = NULL;
            mp_raise_msg(&mp_type_MemoryError,
                MP_ERROR_TEXT("insufficient memory for audio buffer"));
        }
    }

    // Pre-fill both halves before starting DMA.
    audiosample_reset_buffer(sample, false, 0);
    load_dma_buffer_half(self, 0);
    load_dma_buffer_half(self, 1);

    // Ramp from quiescent to the first sample so the transition into
    // DMA-driven output is glitch-free. The DAC is still in single-mode
    // (DAC_TRIGGER_NONE) at this point, set up by construct() / stop(),
    // so HAL_DAC_SetValue takes effect immediately. After the ramp the
    // pin is sitting at exactly dma_buffer[0], which is also the first
    // value the timer-triggered DMA will latch.
    uint16_t quiescent_12 = self->quiescent_value >> 4;
    dac_ramp_channel(DAC_CHANNEL_1, quiescent_12, self->dma_buffer[0]);
    if (self->right_channel != NULL) {
        // CH2 was not started in construct(), so it has been outputting
        // its reset value (0). Ramp from there.
        dac_ramp_channel(DAC_CHANNEL_2, 0, self->dma_buffer_r[0]);
    }

    // --- TIM6 setup ---
    // TIM6 is a basic timer on APB1. It is not managed by the common timer
    // infrastructure (stm_peripherals_find_timer etc.) because it has no GPIO
    // pins and is reserved for DAC use (mcu_tim_banks[5] == NULL).
    __HAL_RCC_TIM6_CLK_ENABLE();

    uint32_t tim6_clk = get_tim6_freq();
    // Round to nearest, not truncate, so the realised sample rate is the
    // closest TIM6 division to the requested rate.
    uint32_t period = (tim6_clk + sample_rate / 2) / sample_rate;
    if (period < 2) {
        period = 2;
    }
    period -= 1;

    tim6_handle.Instance = TIM6;
    tim6_handle.Init.Prescaler = 0;
    tim6_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim6_handle.Init.Period = period;
    tim6_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim6_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&tim6_handle);

    // TRGO = Update event → triggers DAC conversion each period.
    TIM_MasterConfigTypeDef master_cfg = {0};
    master_cfg.MasterOutputTrigger = TIM_TRGO_UPDATE;
    master_cfg.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&tim6_handle, &master_cfg);

    // --- DAC channel reconfiguration for DMA-triggered mode ---
    // Switch the trigger source to TIM6 *without* disabling the DAC channel.
    // HAL_DAC_Stop would disable the channel and momentarily drop the output
    // pin to 0 V — audible as a pop between samples.
    DAC_ChannelConfTypeDef ch_cfg = {0};
    ch_cfg.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
    ch_cfg.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&handle, &ch_cfg, DAC_CHANNEL_1);
    if (self->right_channel != NULL) {
        HAL_DAC_ConfigChannel(&handle, &ch_cfg, DAC_CHANNEL_2);
    }
    // Reset HAL state from BUSY (set by Start) back to READY so
    // HAL_DAC_Start_DMA below doesn't reject the request.
    handle.State = HAL_DAC_STATE_READY;

    // --- DMA1 Stream5 Channel7 setup (DAC CH1, left) ---
    __HAL_RCC_DMA1_CLK_ENABLE();

    DMA_HandleTypeDef *hdma = &self->dma_handle;
    memset(hdma, 0, sizeof(*hdma));
    hdma->Instance = DMA1_Stream5;
    hdma->Init.Channel = DMA_CHANNEL_7;
    hdma->Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma->Init.PeriphInc = DMA_PINC_DISABLE;
    hdma->Init.MemInc = DMA_MINC_ENABLE;
    hdma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma->Init.Mode = DMA_CIRCULAR;
    hdma->Init.Priority = DMA_PRIORITY_HIGH;
    hdma->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(hdma);
    __HAL_LINKDMA(&handle, DMA_Handle1, *hdma);

    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 6, 0);
    NVIC_ClearPendingIRQ(DMA1_Stream5_IRQn);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    // --- DMA1 Stream6 Channel7 setup (DAC CH2, right) ---
    if (self->right_channel != NULL) {
        DMA_HandleTypeDef *hdma_r = &self->dma_handle_r;
        memset(hdma_r, 0, sizeof(*hdma_r));
        hdma_r->Instance = DMA1_Stream6;
        hdma_r->Init.Channel = DMA_CHANNEL_7;
        hdma_r->Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_r->Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_r->Init.MemInc = DMA_MINC_ENABLE;
        hdma_r->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_r->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_r->Init.Mode = DMA_CIRCULAR;
        hdma_r->Init.Priority = DMA_PRIORITY_HIGH;
        hdma_r->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        HAL_DMA_Init(hdma_r);
        __HAL_LINKDMA(&handle, DMA_Handle2, *hdma_r);

        HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 6, 0);
        NVIC_ClearPendingIRQ(DMA1_Stream6_IRQn);
        HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
    }

    // --- Start DMA transfer(s) then timer ---
    active_audioout = self;

    if (HAL_DAC_Start_DMA(&handle, DAC_CHANNEL_1,
        (uint32_t *)self->dma_buffer,
        AUDIOOUT_DMA_BUFFER_SAMPLES,
        DAC_ALIGN_12B_R) != HAL_OK) {
        active_audioout = NULL;
        HAL_NVIC_DisableIRQ(DMA1_Stream5_IRQn);
        if (self->right_channel != NULL) {
            HAL_NVIC_DisableIRQ(DMA1_Stream6_IRQn);
            m_free(self->dma_buffer_r);
            self->dma_buffer_r = NULL;
        }
        m_free(self->dma_buffer);
        self->dma_buffer = NULL;
        mp_raise_RuntimeError(MP_ERROR_TEXT("DAC DMA start failed"));
    }

    if (self->right_channel != NULL) {
        if (HAL_DAC_Start_DMA(&handle, DAC_CHANNEL_2,
            (uint32_t *)self->dma_buffer_r,
            AUDIOOUT_DMA_BUFFER_SAMPLES,
            DAC_ALIGN_12B_R) != HAL_OK) {
            active_audioout = NULL;
            HAL_DAC_Stop_DMA(&handle, DAC_CHANNEL_1);
            HAL_NVIC_DisableIRQ(DMA1_Stream5_IRQn);
            HAL_NVIC_DisableIRQ(DMA1_Stream6_IRQn);
            m_free(self->dma_buffer);
            self->dma_buffer = NULL;
            m_free(self->dma_buffer_r);
            self->dma_buffer_r = NULL;
            mp_raise_RuntimeError(MP_ERROR_TEXT("DAC DMA start failed (right channel)"));
        }
    }

    HAL_TIM_Base_Start(&tim6_handle);
}

void common_hal_audioio_audioout_stop(audioio_audioout_obj_t *self) {
    if (active_audioout != self) {
        return;
    }

    // Stop the sample clock first so no more DMA requests are generated.
    TIM6->CR1 &= ~TIM_CR1_CEN;

    // Switch the DAC channels to no-trigger mode while leaving them enabled.
    // This is the key to a click-free stop: HAL_DAC_Stop_DMA / HAL_DAC_Stop
    // disable the channel, which briefly pulls the output pin to 0 V before
    // the ramp can run. Reconfiguring the trigger only keeps the channel
    // enabled; the DAC keeps holding its last DHR value while DMA is aborted.
    DAC_ChannelConfTypeDef ch_cfg = {0};
    ch_cfg.DAC_Trigger = DAC_TRIGGER_NONE;
    ch_cfg.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    HAL_DAC_ConfigChannel(&handle, &ch_cfg, DAC_CHANNEL_1);
    if (self->dma_buffer_r) {
        HAL_DAC_ConfigChannel(&handle, &ch_cfg, DAC_CHANNEL_2);
    }

    // Capture the last DAC output now that no further DMA writes can land.
    uint16_t last_l = (uint16_t)(DAC->DOR1 & 0xFFF);
    uint16_t last_r = self->dma_buffer_r ? (uint16_t)(DAC->DOR2 & 0xFFF) : 0;
    uint16_t quiescent_12 = self->quiescent_value >> 4;

    // Abort DMA without disabling the DAC channels (HAL_DAC_Stop_DMA would).
    HAL_DMA_Abort(&self->dma_handle);
    HAL_NVIC_DisableIRQ(DMA1_Stream5_IRQn);
    NVIC_ClearPendingIRQ(DMA1_Stream5_IRQn);
    if (self->dma_buffer_r) {
        HAL_DMA_Abort(&self->dma_handle_r);
        HAL_NVIC_DisableIRQ(DMA1_Stream6_IRQn);
        NVIC_ClearPendingIRQ(DMA1_Stream6_IRQn);
        m_free(self->dma_buffer_r);
        self->dma_buffer_r = NULL;
    }

    // Free the left DMA buffer.
    if (self->dma_buffer) {
        m_free(self->dma_buffer);
        self->dma_buffer = NULL;
    }

    // Ramp left channel from last sample back to quiescent.
    dac_ramp_channel(DAC_CHANNEL_1, last_l, quiescent_12);

    // Ramp right channel back to 0 (its reset value) before disabling it,
    // so the next play() can ramp cleanly from 0 again.
    if (self->right_channel != NULL) {
        dac_ramp_channel(DAC_CHANNEL_2, last_r, 0);
        HAL_DAC_Stop(&handle, DAC_CHANNEL_2);
    }

    self->sample = MP_OBJ_NULL;
    self->stopping = false;
    self->paused = false;
    active_audioout = NULL;

    __HAL_RCC_TIM6_CLK_DISABLE();
}

bool common_hal_audioio_audioout_get_playing(audioio_audioout_obj_t *self) {
    return active_audioout == self;
}

void common_hal_audioio_audioout_pause(audioio_audioout_obj_t *self) {
    if (active_audioout != self) {
        return;
    }
    self->paused = true;
    // Pause the sample clock; DMA remains armed but no new triggers fire.
    TIM6->CR1 &= ~TIM_CR1_CEN;
}

void common_hal_audioio_audioout_resume(audioio_audioout_obj_t *self) {
    if (active_audioout != self || !self->paused) {
        return;
    }
    self->paused = false;
    // Restart the sample clock.
    TIM6->CR1 |= TIM_CR1_CEN;
}

bool common_hal_audioio_audioout_get_paused(audioio_audioout_obj_t *self) {
    return self->paused;
}

// ---------------------------------------------------------------------------
// Reset hook
// ---------------------------------------------------------------------------

void audioout_reset(void) {
    if (active_audioout != NULL) {
        // Emergency stop: halt timer and DMA without ramping.
        TIM6->CR1 &= ~TIM_CR1_CEN;
        HAL_DAC_Stop_DMA(&handle, DAC_CHANNEL_1);
        HAL_NVIC_DisableIRQ(DMA1_Stream5_IRQn);
        if (active_audioout->dma_buffer_r) {
            HAL_DAC_Stop_DMA(&handle, DAC_CHANNEL_2);
            HAL_NVIC_DisableIRQ(DMA1_Stream6_IRQn);
            m_free(active_audioout->dma_buffer_r);
            active_audioout->dma_buffer_r = NULL;
        }
        if (active_audioout->dma_buffer) {
            m_free(active_audioout->dma_buffer);
            active_audioout->dma_buffer = NULL;
        }
        active_audioout->sample = MP_OBJ_NULL;
        active_audioout->stopping = false;
        active_audioout->paused = false;
        active_audioout->left_channel = NULL; // mark deinited
        active_audioout = NULL;
    }
    __HAL_RCC_TIM6_CLK_DISABLE();
}
