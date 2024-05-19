#pragma once

typedef struct {
  mp_obj_base_t base;
  uint32_t channel;
} hvs_channel_t;

extern hvs_channel_t hvs_channels[3];
extern const mp_obj_type_t hvs_channel_type;
