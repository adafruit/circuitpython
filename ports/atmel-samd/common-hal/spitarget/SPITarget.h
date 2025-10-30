#ifndef MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_SPI_TARGET_H
#define MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_SPI_TARGET_H

#include "common-hal/microcontroller/Pin.h"
#include "hal/include/hal_spi_m_sync.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;

    struct spi_m_sync_descriptor spi_desc;

    uint8_t clock_pin;
    uint8_t MOSI_pin;
    uint8_t MISO_pin;
    uint8_t SS_pin;

    uint8_t *mosi_packet;
    const uint8_t *miso_packet;

    dma_descr_t running_dma;
} spitarget_spi_target_obj_t;

#endif // MICROPY_INCLUDED_ATMEL_SAMD_COMMON_HAL_BUSIO_SPI_TARGET_H
