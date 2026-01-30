// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/objproperty.h"

#include "bindings/espsdcard/SDCard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "shared-bindings/busio/SPI.h"
#include "common-hal/busio/SPI.h"

//| class SDCard:
//|     """ESP32-native SD card interface using ESP-IDF drivers
//|
//|     This module is API-compatible with sdcardio for easy migration!
//|
//|     You can use either pattern:
//|
//|     Pattern 1 (sdcardio-compatible)::
//|
//|         import busio
//|         import espsdcard as sdcardio
//|         spi = busio.SPI(board.SD_CLK, MOSI=board.SD_MOSI, MISO=board.SD_MISO)
//|         sd = sdcardio.SDCard(spi, board.SD_CS)
//|
//|     Pattern 2 (espsdcard native)::
//|
//|         import espsdcard
//|         sd = espsdcard.SDCard(cs=board.SD_CS, miso=board.SD_MISO,
//|                               mosi=board.SD_MOSI, clk=board.SD_CLK)
//|     """
//|
//|     def __init__(
//|         self,
//|         spi_or_cs: Union[busio.SPI, microcontroller.Pin],
//|         cs: Optional[microcontroller.Pin] = None,
//|         *,
//|         miso: Optional[microcontroller.Pin] = None,
//|         mosi: Optional[microcontroller.Pin] = None,
//|         clk: Optional[microcontroller.Pin] = None,
//|         baudrate: int = 4000000,
//|         spi_host: int = 2,
//|         max_transfer_size: int = 4000,
//|         allocation_unit_size: int = 16384,
//|         max_files: int = 5,
//|         format_if_mount_failed: bool = False,
//|         dma_channel: int = -1
//|     ) -> None:
//|         """Initialize SD card with ESP-IDF drivers
//|
//|         :param spi_or_cs: Either a busio.SPI object (Pattern 1) or CS pin (Pattern 2)
//|         :param cs: CS pin (required for Pattern 1 with busio.SPI)
//|         :param miso: MISO pin (required for Pattern 2)
//|         :param mosi: MOSI pin (required for Pattern 2)
//|         :param clk: Clock pin (required for Pattern 2)
//|         :param baudrate: SPI frequency in Hz (currently unused, ESP-IDF manages speed)
//|         :param spi_host: SPI host (1=SPI2_HOST, 2=SPI3_HOST)
//|         :param max_transfer_size: Maximum DMA transfer size in bytes (default 4000)
//|         :param allocation_unit_size: FAT allocation unit size (default 16384)
//|         :param max_files: Maximum number of open files (default 5)
//|         :param format_if_mount_failed: Auto-format card if mount fails (default False)
//|         :param dma_channel: DMA channel (-1 for automatic)
//|         """
//|         ...
//|

static mp_obj_t espsdcard_sdcard_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {
        ARG_spi_or_cs, ARG_cs,
        ARG_miso, ARG_mosi, ARG_clk,
        ARG_baudrate, ARG_spi_host, ARG_max_transfer_size, ARG_allocation_unit_size,
        ARG_max_files, ARG_format_if_mount_failed, ARG_dma_channel
    };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi_or_cs, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_cs, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_miso, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_mosi, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_clk, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 4000000} },
        { MP_QSTR_spi_host, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 2} },
        { MP_QSTR_max_transfer_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 4000} },
        { MP_QSTR_allocation_unit_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 16384} },
        { MP_QSTR_max_files, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 5} },
        { MP_QSTR_format_if_mount_failed, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_dma_channel, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Detect API pattern and extract pins
    const mcu_pin_obj_t *cs_pin;
    const mcu_pin_obj_t *miso_pin;
    const mcu_pin_obj_t *mosi_pin;
    const mcu_pin_obj_t *clk_pin;

    // Check if first argument is a busio.SPI object (sdcardio compatibility)
    if (args[ARG_spi_or_cs].u_obj != mp_const_none &&
        mp_obj_is_type(args[ARG_spi_or_cs].u_obj, &busio_spi_type)) {
        // Pattern 1: sdcardio-compatible (spi, cs)
        busio_spi_obj_t *spi = MP_OBJ_TO_PTR(args[ARG_spi_or_cs].u_obj);

        // CS must be provided as second argument
        if (args[ARG_cs].u_obj == mp_const_none) {
            mp_raise_TypeError(MP_ERROR_TEXT("cs pin required when using busio.SPI"));
        }
        cs_pin = validate_obj_is_free_pin(args[ARG_cs].u_obj, MP_QSTR_cs);

        // Extract pins from SPI object
        clk_pin = spi->clock;
        mosi_pin = spi->MOSI;
        miso_pin = spi->MISO;

        // Deinit the user's SPI (we're taking over the bus)
        common_hal_busio_spi_deinit(spi);

    } else if (args[ARG_spi_or_cs].u_obj != mp_const_none) {
        // Pattern 2a: espsdcard native with positional cs (cs_pin, miso=, mosi=, clk=)
        cs_pin = validate_obj_is_free_pin(args[ARG_spi_or_cs].u_obj, MP_QSTR_spi_or_cs);

        // miso, mosi, clk must be provided
        if (args[ARG_miso].u_obj == mp_const_none ||
            args[ARG_mosi].u_obj == mp_const_none ||
            args[ARG_clk].u_obj == mp_const_none) {
            mp_raise_TypeError(MP_ERROR_TEXT("miso, mosi, and clk pins required"));
        }

        miso_pin = validate_obj_is_free_pin(args[ARG_miso].u_obj, MP_QSTR_miso);
        mosi_pin = validate_obj_is_free_pin(args[ARG_mosi].u_obj, MP_QSTR_mosi);
        clk_pin = validate_obj_is_free_pin(args[ARG_clk].u_obj, MP_QSTR_clk);

    } else {
        // Pattern 2b: espsdcard native with keyword-only (cs=, miso=, mosi=, clk=)
        if (args[ARG_cs].u_obj == mp_const_none) {
            mp_raise_TypeError(MP_ERROR_TEXT("cs pin required"));
        }
        if (args[ARG_miso].u_obj == mp_const_none ||
            args[ARG_mosi].u_obj == mp_const_none ||
            args[ARG_clk].u_obj == mp_const_none) {
            mp_raise_TypeError(MP_ERROR_TEXT("cs, miso, mosi, and clk pins required"));
        }

        cs_pin = validate_obj_is_free_pin(args[ARG_cs].u_obj, MP_QSTR_cs);
        miso_pin = validate_obj_is_free_pin(args[ARG_miso].u_obj, MP_QSTR_miso);
        mosi_pin = validate_obj_is_free_pin(args[ARG_mosi].u_obj, MP_QSTR_mosi);
        clk_pin = validate_obj_is_free_pin(args[ARG_clk].u_obj, MP_QSTR_clk);
    }

    // Create object
    espsdcard_sdcard_obj_t *self = mp_obj_malloc(espsdcard_sdcard_obj_t, &espsdcard_sdcard_type);

    // Initialize with ESP-IDF (same for both patterns)
    common_hal_espsdcard_sdcard_construct(
        self,
        cs_pin, miso_pin, mosi_pin, clk_pin,
        args[ARG_spi_host].u_int,
        args[ARG_max_transfer_size].u_int,
        args[ARG_allocation_unit_size].u_int,
        args[ARG_max_files].u_int,
        args[ARG_format_if_mount_failed].u_bool,
        args[ARG_dma_channel].u_int
        );

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialize the SD card and free resources"""
//|         ...
//|
static mp_obj_t espsdcard_sdcard_deinit(mp_obj_t self_in) {
    espsdcard_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_espsdcard_sdcard_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(espsdcard_sdcard_deinit_obj, espsdcard_sdcard_deinit);

//|     def __enter__(self) -> SDCard:
//|         """Context manager entry"""
//|         ...
//|
static mp_obj_t espsdcard_sdcard___enter__(mp_obj_t self_in) {
    return self_in;
}
static MP_DEFINE_CONST_FUN_OBJ_1(espsdcard_sdcard___enter___obj, espsdcard_sdcard___enter__);

//|     def __exit__(self) -> None:
//|         """Context manager exit"""
//|         ...
//|
static mp_obj_t espsdcard_sdcard___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_espsdcard_sdcard_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espsdcard_sdcard___exit___obj, 4, 4, espsdcard_sdcard___exit__);

//|     def is_present(self) -> bool:
//|         """Check if card is present and responding"""
//|         ...
//|
static mp_obj_t espsdcard_sdcard_is_present(mp_obj_t self_in) {
    espsdcard_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_espsdcard_sdcard_is_present(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(espsdcard_sdcard_is_present_obj, espsdcard_sdcard_is_present);

//|     def get_status(self) -> int:
//|         """Get card status (0=OK, 1=ERROR, 2=NOT_PRESENT)"""
//|         ...
//|
static mp_obj_t espsdcard_sdcard_get_status(mp_obj_t self_in) {
    espsdcard_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_espsdcard_sdcard_get_status(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(espsdcard_sdcard_get_status_obj, espsdcard_sdcard_get_status);

//|     capacity_mb: float
//|     """Card capacity in megabytes"""
static mp_obj_t espsdcard_sdcard_get_capacity_mb(mp_obj_t self_in) {
    espsdcard_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_espsdcard_sdcard_get_capacity_mb(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(espsdcard_sdcard_get_capacity_mb_obj, espsdcard_sdcard_get_capacity_mb);

MP_PROPERTY_GETTER(espsdcard_sdcard_capacity_mb_obj,
    (mp_obj_t)&espsdcard_sdcard_get_capacity_mb_obj);

//|     card_type: str
//|     """Card type ('SD', 'SDHC/SDXC', or 'Unknown')"""
static mp_obj_t espsdcard_sdcard_get_card_type(mp_obj_t self_in) {
    espsdcard_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_espsdcard_sdcard_get_card_type(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(espsdcard_sdcard_get_card_type_obj, espsdcard_sdcard_get_card_type);

MP_PROPERTY_GETTER(espsdcard_sdcard_card_type_obj,
    (mp_obj_t)&espsdcard_sdcard_get_card_type_obj);

//|     count: int
//|     """Number of 512-byte blocks (required by storage.VfsFat)"""
static mp_obj_t espsdcard_sdcard_get_count(mp_obj_t self_in) {
    espsdcard_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(common_hal_espsdcard_sdcard_get_count(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(espsdcard_sdcard_get_count_obj, espsdcard_sdcard_get_count);

MP_PROPERTY_GETTER(espsdcard_sdcard_count_obj,
    (mp_obj_t)&espsdcard_sdcard_get_count_obj);

//|     def readblocks(self, block_num: int, buf: WriteableBuffer) -> None:
//|         """Read blocks from SD card (required by storage.VfsFat)"""
//|         ...
//|
static mp_obj_t espsdcard_sdcard_readblocks(mp_obj_t self_in, mp_obj_t block_num_in, mp_obj_t buf_in) {
    espsdcard_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t block_num = mp_obj_get_int(block_num_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    common_hal_espsdcard_sdcard_readblocks(self, block_num, &bufinfo);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(espsdcard_sdcard_readblocks_obj, espsdcard_sdcard_readblocks);

//|     def writeblocks(self, block_num: int, buf: ReadableBuffer) -> None:
//|         """Write blocks to SD card (required by storage.VfsFat)"""
//|         ...
//|
static mp_obj_t espsdcard_sdcard_writeblocks(mp_obj_t self_in, mp_obj_t block_num_in, mp_obj_t buf_in) {
    espsdcard_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t block_num = mp_obj_get_int(block_num_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    common_hal_espsdcard_sdcard_writeblocks(self, block_num, &bufinfo);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(espsdcard_sdcard_writeblocks_obj, espsdcard_sdcard_writeblocks);

//|     def ioctl(self, op: int, arg: int) -> Optional[int]:
//|         """Control operations (required by storage.VfsFat)"""
//|         ...
//|
static mp_obj_t espsdcard_sdcard_ioctl(mp_obj_t self_in, mp_obj_t op_in, mp_obj_t arg_in) {
    espsdcard_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t op = mp_obj_get_int(op_in);

    switch (op) {
        case 1: // BP_IOCTL_INIT
            return mp_const_none;
        case 2: // BP_IOCTL_DEINIT
            common_hal_espsdcard_sdcard_deinit(self);
            return mp_const_none;
        case 3: // BP_IOCTL_SYNC
            return mp_obj_new_int(common_hal_espsdcard_sdcard_writeblocks_sync(self));
        case 4: // BP_IOCTL_SEC_COUNT
            return mp_obj_new_int_from_uint(common_hal_espsdcard_sdcard_get_count(self));
        case 5: // BP_IOCTL_SEC_SIZE
            return mp_obj_new_int(512);
        default:
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(espsdcard_sdcard_ioctl_obj, espsdcard_sdcard_ioctl);

static const mp_rom_map_elem_t espsdcard_sdcard_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&espsdcard_sdcard_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&espsdcard_sdcard___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&espsdcard_sdcard___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_is_present), MP_ROM_PTR(&espsdcard_sdcard_is_present_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_status), MP_ROM_PTR(&espsdcard_sdcard_get_status_obj) },

    { MP_ROM_QSTR(MP_QSTR_capacity_mb), MP_ROM_PTR(&espsdcard_sdcard_capacity_mb_obj) },
    { MP_ROM_QSTR(MP_QSTR_card_type), MP_ROM_PTR(&espsdcard_sdcard_card_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&espsdcard_sdcard_count_obj) },

    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&espsdcard_sdcard_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&espsdcard_sdcard_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&espsdcard_sdcard_ioctl_obj) },
};
static MP_DEFINE_CONST_DICT(espsdcard_sdcard_locals_dict, espsdcard_sdcard_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    espsdcard_sdcard_type,
    MP_QSTR_SDCard,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, espsdcard_sdcard_make_new,
    locals_dict, &espsdcard_sdcard_locals_dict
    );
