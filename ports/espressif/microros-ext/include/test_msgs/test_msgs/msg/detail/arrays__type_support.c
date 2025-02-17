// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from test_msgs:msg/Arrays.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "test_msgs/msg/detail/arrays__rosidl_typesupport_introspection_c.h"
#include "test_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "test_msgs/msg/detail/arrays__functions.h"
#include "test_msgs/msg/detail/arrays__struct.h"


// Include directives for member types
// Member `string_values`
// Member `string_values_default`
#include "rosidl_runtime_c/string_functions.h"
// Member `basic_types_values`
#include "test_msgs/msg/basic_types.h"
// Member `basic_types_values`
#include "test_msgs/msg/detail/basic_types__rosidl_typesupport_introspection_c.h"
// Member `constants_values`
#include "test_msgs/msg/constants.h"
// Member `constants_values`
#include "test_msgs/msg/detail/constants__rosidl_typesupport_introspection_c.h"
// Member `defaults_values`
#include "test_msgs/msg/defaults.h"
// Member `defaults_values`
#include "test_msgs/msg/detail/defaults__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  test_msgs__msg__Arrays__init(message_memory);
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_fini_function(void * message_memory)
{
  test_msgs__msg__Arrays__fini(message_memory);
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__bool_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__bool_values(
  const void * untyped_member, size_t index)
{
  const bool * member =
    (const bool *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__bool_values(
  void * untyped_member, size_t index)
{
  bool * member =
    (bool *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__bool_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const bool * item =
    ((const bool *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__bool_values(untyped_member, index));
  bool * value =
    (bool *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__bool_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  bool * item =
    ((bool *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__bool_values(untyped_member, index));
  const bool * value =
    (const bool *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__byte_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__byte_values(
  const void * untyped_member, size_t index)
{
  const uint8_t * member =
    (const uint8_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__byte_values(
  void * untyped_member, size_t index)
{
  uint8_t * member =
    (uint8_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__byte_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint8_t * item =
    ((const uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__byte_values(untyped_member, index));
  uint8_t * value =
    (uint8_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__byte_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint8_t * item =
    ((uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__byte_values(untyped_member, index));
  const uint8_t * value =
    (const uint8_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__char_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__char_values(
  const void * untyped_member, size_t index)
{
  const uint8_t * member =
    (const uint8_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__char_values(
  void * untyped_member, size_t index)
{
  uint8_t * member =
    (uint8_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__char_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint8_t * item =
    ((const uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__char_values(untyped_member, index));
  uint8_t * value =
    (uint8_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__char_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint8_t * item =
    ((uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__char_values(untyped_member, index));
  const uint8_t * value =
    (const uint8_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__float32_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float32_values(
  const void * untyped_member, size_t index)
{
  const float * member =
    (const float *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float32_values(
  void * untyped_member, size_t index)
{
  float * member =
    (float *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__float32_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const float * item =
    ((const float *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float32_values(untyped_member, index));
  float * value =
    (float *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__float32_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  float * item =
    ((float *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float32_values(untyped_member, index));
  const float * value =
    (const float *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__float64_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float64_values(
  const void * untyped_member, size_t index)
{
  const double * member =
    (const double *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float64_values(
  void * untyped_member, size_t index)
{
  double * member =
    (double *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__float64_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float64_values(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__float64_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float64_values(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int8_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int8_values(
  const void * untyped_member, size_t index)
{
  const int8_t * member =
    (const int8_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int8_values(
  void * untyped_member, size_t index)
{
  int8_t * member =
    (int8_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int8_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int8_t * item =
    ((const int8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int8_values(untyped_member, index));
  int8_t * value =
    (int8_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int8_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int8_t * item =
    ((int8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int8_values(untyped_member, index));
  const int8_t * value =
    (const int8_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint8_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint8_values(
  const void * untyped_member, size_t index)
{
  const uint8_t * member =
    (const uint8_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint8_values(
  void * untyped_member, size_t index)
{
  uint8_t * member =
    (uint8_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint8_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint8_t * item =
    ((const uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint8_values(untyped_member, index));
  uint8_t * value =
    (uint8_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint8_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint8_t * item =
    ((uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint8_values(untyped_member, index));
  const uint8_t * value =
    (const uint8_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int16_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int16_values(
  const void * untyped_member, size_t index)
{
  const int16_t * member =
    (const int16_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int16_values(
  void * untyped_member, size_t index)
{
  int16_t * member =
    (int16_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int16_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int16_t * item =
    ((const int16_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int16_values(untyped_member, index));
  int16_t * value =
    (int16_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int16_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int16_t * item =
    ((int16_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int16_values(untyped_member, index));
  const int16_t * value =
    (const int16_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint16_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint16_values(
  const void * untyped_member, size_t index)
{
  const uint16_t * member =
    (const uint16_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint16_values(
  void * untyped_member, size_t index)
{
  uint16_t * member =
    (uint16_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint16_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint16_t * item =
    ((const uint16_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint16_values(untyped_member, index));
  uint16_t * value =
    (uint16_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint16_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint16_t * item =
    ((uint16_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint16_values(untyped_member, index));
  const uint16_t * value =
    (const uint16_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int32_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int32_values(
  const void * untyped_member, size_t index)
{
  const int32_t * member =
    (const int32_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int32_values(
  void * untyped_member, size_t index)
{
  int32_t * member =
    (int32_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int32_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int32_t * item =
    ((const int32_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int32_values(untyped_member, index));
  int32_t * value =
    (int32_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int32_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int32_t * item =
    ((int32_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int32_values(untyped_member, index));
  const int32_t * value =
    (const int32_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint32_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint32_values(
  const void * untyped_member, size_t index)
{
  const uint32_t * member =
    (const uint32_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint32_values(
  void * untyped_member, size_t index)
{
  uint32_t * member =
    (uint32_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint32_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint32_t * item =
    ((const uint32_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint32_values(untyped_member, index));
  uint32_t * value =
    (uint32_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint32_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint32_t * item =
    ((uint32_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint32_values(untyped_member, index));
  const uint32_t * value =
    (const uint32_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int64_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int64_values(
  const void * untyped_member, size_t index)
{
  const int64_t * member =
    (const int64_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int64_values(
  void * untyped_member, size_t index)
{
  int64_t * member =
    (int64_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int64_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int64_t * item =
    ((const int64_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int64_values(untyped_member, index));
  int64_t * value =
    (int64_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int64_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int64_t * item =
    ((int64_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int64_values(untyped_member, index));
  const int64_t * value =
    (const int64_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint64_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint64_values(
  const void * untyped_member, size_t index)
{
  const uint64_t * member =
    (const uint64_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint64_values(
  void * untyped_member, size_t index)
{
  uint64_t * member =
    (uint64_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint64_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint64_t * item =
    ((const uint64_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint64_values(untyped_member, index));
  uint64_t * value =
    (uint64_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint64_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint64_t * item =
    ((uint64_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint64_values(untyped_member, index));
  const uint64_t * value =
    (const uint64_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__string_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__string_values(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String * member =
    (const rosidl_runtime_c__String *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__string_values(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String * member =
    (rosidl_runtime_c__String *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__string_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__string_values(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__string_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__string_values(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__basic_types_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__basic_types_values(
  const void * untyped_member, size_t index)
{
  const test_msgs__msg__BasicTypes * member =
    (const test_msgs__msg__BasicTypes *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__basic_types_values(
  void * untyped_member, size_t index)
{
  test_msgs__msg__BasicTypes * member =
    (test_msgs__msg__BasicTypes *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__basic_types_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const test_msgs__msg__BasicTypes * item =
    ((const test_msgs__msg__BasicTypes *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__basic_types_values(untyped_member, index));
  test_msgs__msg__BasicTypes * value =
    (test_msgs__msg__BasicTypes *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__basic_types_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  test_msgs__msg__BasicTypes * item =
    ((test_msgs__msg__BasicTypes *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__basic_types_values(untyped_member, index));
  const test_msgs__msg__BasicTypes * value =
    (const test_msgs__msg__BasicTypes *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__constants_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__constants_values(
  const void * untyped_member, size_t index)
{
  const test_msgs__msg__Constants * member =
    (const test_msgs__msg__Constants *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__constants_values(
  void * untyped_member, size_t index)
{
  test_msgs__msg__Constants * member =
    (test_msgs__msg__Constants *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__constants_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const test_msgs__msg__Constants * item =
    ((const test_msgs__msg__Constants *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__constants_values(untyped_member, index));
  test_msgs__msg__Constants * value =
    (test_msgs__msg__Constants *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__constants_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  test_msgs__msg__Constants * item =
    ((test_msgs__msg__Constants *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__constants_values(untyped_member, index));
  const test_msgs__msg__Constants * value =
    (const test_msgs__msg__Constants *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__defaults_values(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__defaults_values(
  const void * untyped_member, size_t index)
{
  const test_msgs__msg__Defaults * member =
    (const test_msgs__msg__Defaults *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__defaults_values(
  void * untyped_member, size_t index)
{
  test_msgs__msg__Defaults * member =
    (test_msgs__msg__Defaults *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__defaults_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const test_msgs__msg__Defaults * item =
    ((const test_msgs__msg__Defaults *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__defaults_values(untyped_member, index));
  test_msgs__msg__Defaults * value =
    (test_msgs__msg__Defaults *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__defaults_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  test_msgs__msg__Defaults * item =
    ((test_msgs__msg__Defaults *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__defaults_values(untyped_member, index));
  const test_msgs__msg__Defaults * value =
    (const test_msgs__msg__Defaults *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__bool_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__bool_values_default(
  const void * untyped_member, size_t index)
{
  const bool * member =
    (const bool *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__bool_values_default(
  void * untyped_member, size_t index)
{
  bool * member =
    (bool *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__bool_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const bool * item =
    ((const bool *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__bool_values_default(untyped_member, index));
  bool * value =
    (bool *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__bool_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  bool * item =
    ((bool *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__bool_values_default(untyped_member, index));
  const bool * value =
    (const bool *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__byte_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__byte_values_default(
  const void * untyped_member, size_t index)
{
  const uint8_t * member =
    (const uint8_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__byte_values_default(
  void * untyped_member, size_t index)
{
  uint8_t * member =
    (uint8_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__byte_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint8_t * item =
    ((const uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__byte_values_default(untyped_member, index));
  uint8_t * value =
    (uint8_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__byte_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint8_t * item =
    ((uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__byte_values_default(untyped_member, index));
  const uint8_t * value =
    (const uint8_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__char_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__char_values_default(
  const void * untyped_member, size_t index)
{
  const uint8_t * member =
    (const uint8_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__char_values_default(
  void * untyped_member, size_t index)
{
  uint8_t * member =
    (uint8_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__char_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint8_t * item =
    ((const uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__char_values_default(untyped_member, index));
  uint8_t * value =
    (uint8_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__char_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint8_t * item =
    ((uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__char_values_default(untyped_member, index));
  const uint8_t * value =
    (const uint8_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__float32_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float32_values_default(
  const void * untyped_member, size_t index)
{
  const float * member =
    (const float *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float32_values_default(
  void * untyped_member, size_t index)
{
  float * member =
    (float *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__float32_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const float * item =
    ((const float *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float32_values_default(untyped_member, index));
  float * value =
    (float *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__float32_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  float * item =
    ((float *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float32_values_default(untyped_member, index));
  const float * value =
    (const float *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__float64_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float64_values_default(
  const void * untyped_member, size_t index)
{
  const double * member =
    (const double *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float64_values_default(
  void * untyped_member, size_t index)
{
  double * member =
    (double *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__float64_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float64_values_default(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__float64_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float64_values_default(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int8_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int8_values_default(
  const void * untyped_member, size_t index)
{
  const int8_t * member =
    (const int8_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int8_values_default(
  void * untyped_member, size_t index)
{
  int8_t * member =
    (int8_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int8_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int8_t * item =
    ((const int8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int8_values_default(untyped_member, index));
  int8_t * value =
    (int8_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int8_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int8_t * item =
    ((int8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int8_values_default(untyped_member, index));
  const int8_t * value =
    (const int8_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint8_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint8_values_default(
  const void * untyped_member, size_t index)
{
  const uint8_t * member =
    (const uint8_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint8_values_default(
  void * untyped_member, size_t index)
{
  uint8_t * member =
    (uint8_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint8_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint8_t * item =
    ((const uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint8_values_default(untyped_member, index));
  uint8_t * value =
    (uint8_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint8_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint8_t * item =
    ((uint8_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint8_values_default(untyped_member, index));
  const uint8_t * value =
    (const uint8_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int16_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int16_values_default(
  const void * untyped_member, size_t index)
{
  const int16_t * member =
    (const int16_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int16_values_default(
  void * untyped_member, size_t index)
{
  int16_t * member =
    (int16_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int16_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int16_t * item =
    ((const int16_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int16_values_default(untyped_member, index));
  int16_t * value =
    (int16_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int16_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int16_t * item =
    ((int16_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int16_values_default(untyped_member, index));
  const int16_t * value =
    (const int16_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint16_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint16_values_default(
  const void * untyped_member, size_t index)
{
  const uint16_t * member =
    (const uint16_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint16_values_default(
  void * untyped_member, size_t index)
{
  uint16_t * member =
    (uint16_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint16_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint16_t * item =
    ((const uint16_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint16_values_default(untyped_member, index));
  uint16_t * value =
    (uint16_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint16_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint16_t * item =
    ((uint16_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint16_values_default(untyped_member, index));
  const uint16_t * value =
    (const uint16_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int32_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int32_values_default(
  const void * untyped_member, size_t index)
{
  const int32_t * member =
    (const int32_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int32_values_default(
  void * untyped_member, size_t index)
{
  int32_t * member =
    (int32_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int32_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int32_t * item =
    ((const int32_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int32_values_default(untyped_member, index));
  int32_t * value =
    (int32_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int32_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int32_t * item =
    ((int32_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int32_values_default(untyped_member, index));
  const int32_t * value =
    (const int32_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint32_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint32_values_default(
  const void * untyped_member, size_t index)
{
  const uint32_t * member =
    (const uint32_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint32_values_default(
  void * untyped_member, size_t index)
{
  uint32_t * member =
    (uint32_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint32_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint32_t * item =
    ((const uint32_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint32_values_default(untyped_member, index));
  uint32_t * value =
    (uint32_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint32_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint32_t * item =
    ((uint32_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint32_values_default(untyped_member, index));
  const uint32_t * value =
    (const uint32_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int64_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int64_values_default(
  const void * untyped_member, size_t index)
{
  const int64_t * member =
    (const int64_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int64_values_default(
  void * untyped_member, size_t index)
{
  int64_t * member =
    (int64_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int64_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const int64_t * item =
    ((const int64_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int64_values_default(untyped_member, index));
  int64_t * value =
    (int64_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int64_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  int64_t * item =
    ((int64_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int64_values_default(untyped_member, index));
  const int64_t * value =
    (const int64_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint64_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint64_values_default(
  const void * untyped_member, size_t index)
{
  const uint64_t * member =
    (const uint64_t *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint64_values_default(
  void * untyped_member, size_t index)
{
  uint64_t * member =
    (uint64_t *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint64_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const uint64_t * item =
    ((const uint64_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint64_values_default(untyped_member, index));
  uint64_t * value =
    (uint64_t *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint64_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  uint64_t * item =
    ((uint64_t *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint64_values_default(untyped_member, index));
  const uint64_t * value =
    (const uint64_t *)(untyped_value);
  *item = *value;
}

size_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__string_values_default(
  const void * untyped_member)
{
  (void)untyped_member;
  return 3;
}

const void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__string_values_default(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String * member =
    (const rosidl_runtime_c__String *)(untyped_member);
  return &member[index];
}

void * test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__string_values_default(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String * member =
    (rosidl_runtime_c__String *)(untyped_member);
  return &member[index];
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__string_values_default(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__string_values_default(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__string_values_default(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__string_values_default(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

static rosidl_typesupport_introspection_c__MessageMember test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_message_member_array[32] = {
  {
    "bool_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, bool_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__bool_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__bool_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__bool_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__bool_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__bool_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "byte_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_OCTET,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, byte_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__byte_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__byte_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__byte_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__byte_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__byte_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "char_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, char_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__char_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__char_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__char_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__char_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__char_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "float32_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, float32_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__float32_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float32_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float32_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__float32_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__float32_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "float64_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, float64_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__float64_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float64_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float64_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__float64_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__float64_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int8_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, int8_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int8_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int8_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int8_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int8_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int8_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint8_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, uint8_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint8_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint8_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint8_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint8_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint8_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int16_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, int16_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int16_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int16_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int16_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int16_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int16_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint16_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, uint16_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint16_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint16_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint16_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint16_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint16_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int32_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, int32_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int32_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int32_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int32_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int32_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int32_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint32_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, uint32_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint32_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint32_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint32_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint32_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint32_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int64_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, int64_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int64_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int64_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int64_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int64_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int64_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint64_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, uint64_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint64_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint64_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint64_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint64_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint64_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "string_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, string_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__string_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__string_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__string_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__string_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__string_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "basic_types_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, basic_types_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__basic_types_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__basic_types_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__basic_types_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__basic_types_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__basic_types_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "constants_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, constants_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__constants_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__constants_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__constants_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__constants_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__constants_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "defaults_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, defaults_values),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__defaults_values,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__defaults_values,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__defaults_values,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__defaults_values,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__defaults_values,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "bool_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, bool_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__bool_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__bool_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__bool_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__bool_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__bool_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "byte_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_OCTET,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, byte_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__byte_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__byte_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__byte_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__byte_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__byte_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "char_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, char_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__char_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__char_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__char_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__char_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__char_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "float32_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, float32_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__float32_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float32_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float32_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__float32_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__float32_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "float64_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, float64_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__float64_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__float64_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__float64_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__float64_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__float64_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int8_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, int8_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int8_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int8_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int8_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int8_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int8_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint8_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, uint8_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint8_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint8_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint8_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint8_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint8_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int16_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, int16_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int16_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int16_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int16_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int16_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int16_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint16_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, uint16_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint16_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint16_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint16_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint16_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint16_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int32_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, int32_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int32_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int32_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int32_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int32_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int32_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint32_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, uint32_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint32_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint32_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint32_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint32_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint32_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int64_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, int64_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__int64_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__int64_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__int64_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__int64_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__int64_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint64_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, uint64_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__uint64_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__uint64_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__uint64_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__uint64_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__uint64_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "string_values_default",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, string_values_default),  // bytes offset in struct
    NULL,  // default value
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__size_function__Arrays__string_values_default,  // size() function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_const_function__Arrays__string_values_default,  // get_const(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__get_function__Arrays__string_values_default,  // get(index) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__fetch_function__Arrays__string_values_default,  // fetch(index, &value) function pointer
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__assign_function__Arrays__string_values_default,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "alignment_check",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__msg__Arrays, alignment_check),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_message_members = {
  "test_msgs__msg",  // message namespace
  "Arrays",  // message name
  32,  // number of fields
  sizeof(test_msgs__msg__Arrays),
  false,  // has_any_key_member_
  test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_message_member_array,  // message members
  test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_init_function,  // function to initialize message memory (memory has to be allocated)
  test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_message_type_support_handle = {
  0,
  &test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_message_members,
  get_message_typesupport_handle_function,
  &test_msgs__msg__Arrays__get_type_hash,
  &test_msgs__msg__Arrays__get_type_description,
  &test_msgs__msg__Arrays__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_test_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, msg, Arrays)() {
  test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_message_member_array[14].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, msg, BasicTypes)();
  test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_message_member_array[15].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, msg, Constants)();
  test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_message_member_array[16].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, msg, Defaults)();
  if (!test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_message_type_support_handle.typesupport_identifier) {
    test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &test_msgs__msg__Arrays__rosidl_typesupport_introspection_c__Arrays_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
