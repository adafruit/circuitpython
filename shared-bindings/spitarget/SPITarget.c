#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/spitarget/SPITarget.h"
#include "shared-bindings/time/__init__.h"
#include "shared-bindings/util.h"

#include "shared/runtime/buffer_helper.h"
#include "shared/runtime/context_manager_helpers.h"
#include "shared/runtime/interrupt_char.h"

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

//| class SPITarget:
//|     """Serial Peripheral Interface protocol target"""
//|
//|     def __init__(
//|         self,
//|         sck: microcontroller.Pin,
//|         mosi: microcontroller.Pin,
//|         miso: microcontroller.Pin,
//|         ss: microcontroller.Pin,
//|     ) -> None:
//|         """SPI is a four-wire protocol for communicating between devices.
//|         This implements the secondary (aka target or peripheral) side.
//|
//|         :param ~microcontroller.Pin sck: The SPI clock pin
//|         :param ~microcontroller.Pin mosi: The pin transferring data from the main to the secondary
//|         :param ~microcontroller.Pin miso: The pin transferring data from the secondary to the main
//|         :param ~microcontroller.Pin ss: The secondary selection pin"""
//|         ...
//|
static mp_obj_t spitarget_spi_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    spitarget_spi_target_obj_t *self = mp_obj_malloc(spitarget_spi_target_obj_t, &spitarget_spi_target_type);
    enum { ARG_sck, ARG_mosi, ARG_miso, ARG_ss };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sck, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mosi, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_miso, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_ss, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *sck = validate_obj_is_free_pin(args[ARG_sck].u_obj, MP_QSTR_sck);
    const mcu_pin_obj_t *mosi = validate_obj_is_free_pin(args[ARG_mosi].u_obj, MP_QSTR_mosi);
    const mcu_pin_obj_t *miso = validate_obj_is_free_pin(args[ARG_miso].u_obj, MP_QSTR_miso);
    const mcu_pin_obj_t *ss = validate_obj_is_free_pin(args[ARG_ss].u_obj, MP_QSTR_ss);

    common_hal_spitarget_spi_target_construct(self, sck, mosi, miso, ss);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Releases control of the underlying hardware so other classes can use it."""
//|         ...
//|
static mp_obj_t spitarget_spi_target_obj_deinit(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &spitarget_spi_target_type));
    spitarget_spi_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_spitarget_spi_target_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(spitarget_spi_target_deinit_obj, spitarget_spi_target_obj_deinit);

//|     def __enter__(self) -> SPITarget:
//|         """No-op used in Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware on context exit. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
static mp_obj_t spitarget_spi_target_obj___exit__(size_t n_args, const mp_obj_t *args) {
    mp_check_self(mp_obj_is_type(args[0], &spitarget_spi_target_target_type));
    spitarget_spi_target_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    common_hal_spitarget_spi_target_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(spitarget_spi_target___exit___obj, 4, 4, spitarget_spi_target_obj___exit__);

//|     def load_packet(self, mosi_packet: bytearray, miso_packet: bytearray) -> None:
//|         """Queue data for the next SPI transfer from the main device.
//|         If a packet has already been queued for this SPI bus but has not yet been transferred, an error will be raised.
//|
//|         :param bytearray miso_packet: Packet data to be sent from secondary to main on next request.
//|         :param bytearray mosi_packet: Packet to be filled with data from main on next request.
//|         """
//|
static mp_obj_t spitarget_spi_target_load_packet(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_check_self(mp_obj_is_type(pos_args[0], &spitarget_spi_target_type));
    spitarget_spi_target_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    if (common_hal_spitarget_spi_target_deinited(self)) {
        raise_deinited_error();
    }
    enum { ARG_mosi_packet, ARG_miso_packet };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mosi_packet,       MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_miso_packet,       MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t mosi_bufinfo;
    mp_get_buffer_raise(args[ARG_mosi_packet].u_obj, &mosi_bufinfo, MP_BUFFER_WRITE);

    mp_buffer_info_t miso_bufinfo;
    mp_get_buffer_raise(args[ARG_miso_packet].u_obj, &miso_bufinfo, MP_BUFFER_READ);

    if (miso_bufinfo.len != mosi_bufinfo.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("Packet buffers for an SPI transfer must have the same length."));
    }

    common_hal_spitarget_spi_target_transfer_start(self, ((uint8_t *)mosi_bufinfo.buf), ((uint8_t *)miso_bufinfo.buf), mosi_bufinfo.len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(spitarget_spi_target_load_packet_obj, 1, spitarget_spi_target_load_packet);

//|     def wait_transfer(self, *, timeout: float = -1) -> bool:
//|         """Wait for an SPI transfer from the main device.
//|
//|         :param float timeout: Timeout in seconds. Zero means wait forever, a negative value means check once
//|         :return: True if the transfer is complete, or False if no response received before the timeout
//|         """
//|
//|
static mp_obj_t spitarget_spi_target_wait_transfer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_check_self(mp_obj_is_type(pos_args[0], &spitarget_spi_target_type));
    spitarget_spi_target_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    if (common_hal_spitarget_spi_target_deinited(self)) {
        raise_deinited_error();
    }
    enum { ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timeout,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #if MICROPY_PY_BUILTINS_FLOAT
    float f = mp_obj_get_float(args[ARG_timeout].u_obj) * 1000;
    int timeout_ms = (int)f;
    #else
    int timeout_ms = mp_obj_get_int(args[ARG_timeout].u_obj) * 1000;
    #endif

    bool forever = false;
    uint64_t timeout_end = 0;
    if (timeout_ms == 0) {
        forever = true;
    } else if (timeout_ms > 0) {
        timeout_end = common_hal_time_monotonic_ms() + timeout_ms;
    }

    do {
        if (common_hal_spitarget_spi_target_transfer_is_finished(self)) {
            common_hal_spitarget_spi_target_transfer_close(self); // implicitly discards error indicator code
            return mp_const_true;
        }
        mp_hal_delay_us(10);
    } while (forever || common_hal_time_monotonic_ms() < timeout_end);

    return mp_const_false;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(spitarget_spi_target_wait_transfer_obj, 1, spitarget_spi_target_wait_transfer);

static const mp_rom_map_elem_t spitarget_spi_target_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&spitarget_spi_target_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&spitarget_spi_target___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_load_packet), MP_ROM_PTR(&spitarget_spi_target_load_packet_obj) },
    { MP_ROM_QSTR(MP_QSTR_wait_transfer), MP_ROM_PTR(&spitarget_spi_target_wait_transfer_obj) },

};

static MP_DEFINE_CONST_DICT(spitarget_spi_target_locals_dict, spitarget_spi_target_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    spitarget_spi_target_type,
    MP_QSTR_SPITarget,
    MP_TYPE_FLAG_NONE,
    make_new, spitarget_spi_target_make_new,
    locals_dict, &spitarget_spi_target_locals_dict
    );
