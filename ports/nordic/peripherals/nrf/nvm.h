// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2019 Nick Moore for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#define FLASH_PAGE_SIZE (4096)

#ifdef BLUETOOTH_SD
bool sd_flash_page_erase_sync(uint32_t page_number);
bool sd_flash_write_sync(uint32_t *dest_words, uint32_t *src_words, uint32_t num_words);
#endif

bool nrf_nvm_safe_flash_page_write(uint32_t page_addr, uint8_t *data);
