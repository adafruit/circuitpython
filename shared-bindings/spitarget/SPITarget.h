#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BUSIO_SPI_TARGET_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BUSIO_SPI_TARGET_H

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/spitarget/SPITarget.h"

extern const mp_obj_type_t spitarget_spi_target_type;

extern void common_hal_spitarget_spi_target_construct(spitarget_spi_target_obj_t *self,
    const mcu_pin_obj_t *sck, const mcu_pin_obj_t *mosi, const mcu_pin_obj_t *miso, const mcu_pin_obj_t *ss);
extern void common_hal_spitarget_spi_target_deinit(spitarget_spi_target_obj_t *self);
extern bool common_hal_spitarget_spi_target_deinited(spitarget_spi_target_obj_t *self);

extern void common_hal_spitarget_spi_target_transfer_start(spitarget_spi_target_obj_t *self,
    uint8_t *mosi_packet, const uint8_t *miso_packet, size_t len);
extern bool common_hal_spitarget_spi_target_transfer_is_finished(spitarget_spi_target_obj_t *self);
extern int common_hal_spitarget_spi_target_transfer_close(spitarget_spi_target_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BUSIO_SPI_TARGET_H
