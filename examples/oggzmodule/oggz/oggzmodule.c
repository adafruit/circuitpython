// Include MicroPython API.
#include "py/runtime.h"
#include "py/stream.h"

#include <string.h>

#include "oggz/oggz.h"
#include "opus.h"

// small standard library
int _fstat;
int __gnu_thumb1_case_shi;
int _close;
int _isatty;
int _lseek;

typedef struct _mp_obj_oggz_t
{
  mp_obj_base_t base;
  mp_obj_t stream; // retain a reference to prevent GC from reclaiming it
  OGGZ *oggz;
  OpusDecoder *opus;
  oggz_packet *current_packet; // NULL unless currently decoding opus
  const char *content_type;
} mp_obj_oggz_t;

STATIC int op_stream_posix_read(void *_stream, unsigned char *_ptr,
                                int _nbytes)
{
  int errcode;
  int bytes_read;
  bytes_read = mp_stream_rw(_stream, _ptr, _nbytes, &errcode, MP_STREAM_RW_READ);
  return bytes_read;
}

STATIC int oggz_packet_cb(OGGZ *oggz, oggz_packet *packet, long serialno,
                          void *user_data)
{
  mp_obj_oggz_t *self = (mp_obj_oggz_t *)user_data;
  self->current_packet = packet;
  self->content_type = oggz_stream_get_content_type(oggz, serialno);
  return OGGZ_STOP_OK;
}

// e.g.
// STATIC mp_obj_t framebuf_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {

STATIC mp_obj_t mp_oggz_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
  mp_arg_check_num(n_args, n_kw, 1, 1, false);
  mp_obj_oggz_t *self = m_new_obj(mp_obj_oggz_t);

  // make sure we have a stream
  mp_get_stream_raise(args[0], MP_STREAM_OP_READ);

  mp_obj_t *stream = args[0];

  self->base.type = (mp_obj_type_t *)type;
  self->stream = stream;

  self->oggz = oggz_new(OGGZ_READ);
  if (self->oggz == NULL)
  {
    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("oggz error"));
  }

  int error;

  self->opus = opus_decoder_create(48000, 2, &error);
  if (error != 0)
  {
    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("opus error %d"),
                      error);
  }

  error = oggz_io_set_read(self->oggz, (OggzIORead)&op_stream_posix_read,
                           self->stream);
  if (error != 0)
  {
    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("oggz error %d"),
                      error);
  }

  error = oggz_set_read_callback(self->oggz, -1, oggz_packet_cb, self);
  if (error != 0)
  {
    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("oggz error %d"),
                      error);
  }

  return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t mp_oggz_close(mp_obj_t self_in)
{
  mp_obj_oggz_t *self = MP_OBJ_TO_PTR(self_in);
  int error = oggz_close(self->oggz);
  self->oggz = NULL;
  opus_decoder_destroy(self->opus);
  self->opus = NULL;
  if (error != 0)
  {
    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("oggz error %d"),
                      error);
  }
  return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_oggz_close_obj, mp_oggz_close);

STATIC mp_obj_t mp_oggz_read(mp_obj_t self_in, mp_obj_t n)
{
  mp_obj_oggz_t *self = MP_OBJ_TO_PTR(self_in);
  int bytes_read = oggz_read(self->oggz, mp_obj_get_int(n));
  if (bytes_read < 0 && bytes_read != OGGZ_ERR_STOP_OK)
  {
    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("oggz error %d"),
                      bytes_read);
  }
  return mp_obj_new_int(bytes_read);
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_oggz_read_obj, mp_oggz_read);

// Decode opus samples to buffer
// Return number of samples decoded (bytes = samples * 16 bit * 2)
STATIC mp_obj_t mp_oggz_decode_opus(mp_obj_t self_in, mp_obj_t buf_obj)
{
  mp_obj_oggz_t *self = MP_OBJ_TO_PTR(self_in);
  int bytes_read, samples_read;
  mp_buffer_info_t bufinfo;

  mp_get_buffer_raise(buf_obj, &bufinfo, MP_BUFFER_READ);

  self->current_packet = NULL;
  bytes_read = oggz_read(self->oggz, 512);
  if ((bytes_read < 0) && (bytes_read != OGGZ_ERR_STOP_OK))
  {
    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("oggz error %d"),
                      bytes_read);
  }
  if (self->current_packet == NULL)
  {
    samples_read = 0;
  }
  else
  {
    samples_read = opus_decode(self->opus, self->current_packet->op.packet,
                               self->current_packet->op.bytes, bufinfo.buf,
                               bufinfo.len / 4, 0);

    // OPUS_BAD_ARG -1, BUFFER_TOO_SMALL -2, INTERNAL_ERROR -3,
    // INVALID_PACKET -4, UNIMPL -5, INVALID_STATE -6 ALLOC_FAIL -7
  }

  return mp_obj_new_tuple(
      4, ((mp_obj_t[]){
             mp_obj_new_str(self->content_type, strlen(self->content_type)),
             mp_obj_new_int(samples_read), mp_obj_new_int(bytes_read),
             mp_obj_new_int(bufinfo.len)}));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_oggz_decode_opus_obj, mp_oggz_decode_opus);

STATIC const mp_rom_map_elem_t oggz_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_oggz_close_obj)},
    {MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_oggz_read_obj)},
    {MP_ROM_QSTR(MP_QSTR_decode_opus), MP_ROM_PTR(&mp_oggz_decode_opus_obj)},
};
STATIC MP_DEFINE_CONST_DICT(oggz_locals_dict, oggz_locals_dict_table);

// This defines the type(opus) object.
#ifdef MP_DEFINE_CONST_OBJ_TYPE

MP_DEFINE_CONST_OBJ_TYPE(mp_oggz_type, MP_QSTR_oggz, MP_TYPE_FLAG_NONE,
                         make_new, mp_oggz_make_new, locals_dict,
                         &oggz_locals_dict);

#else

STATIC const mp_obj_type_t mp_oggz_type = {
    {&mp_type_type},
    .name = MP_QSTR_oggz,
    .make_new = mp_oggz_make_new,
    .locals_dict = (mp_obj_dict_t *)&oggz_locals_dict,
};

// e.g.
// STATIC const mp_obj_type_t uhashlib_md5_type = {
//     { &mp_type_type },
//     .name = MP_QSTR_md5,
//     .make_new = uhashlib_md5_make_new,
//     .locals_dict = (void *)&uhashlib_md5_locals_dict,
// };

#endif

// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t oggz_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_oggz)},
    {MP_ROM_QSTR(MP_QSTR_oggz), MP_ROM_PTR(&mp_oggz_type)},
};
STATIC MP_DEFINE_CONST_DICT(oggz_module_globals, oggz_module_globals_table);

// Define module object.
const mp_obj_module_t oggz_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&oggz_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_oggz, oggz_module, 1);

// Otherwise missing symbols
// int fprintf (void *__restrict, const char *__restrict, ...) {}

// extern int __errno = 0;

// void abort_(void) {
//     nlr_raise(mp_obj_new_exception(mp_load_global(MP_QSTR_RuntimeError)));
// }