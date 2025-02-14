// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from shape_msgs:msg/SolidPrimitive.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "shape_msgs/msg/detail/solid_primitive__rosidl_typesupport_introspection_c.h"
#include "shape_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "shape_msgs/msg/detail/solid_primitive__functions.h"
#include "shape_msgs/msg/detail/solid_primitive__struct.h"


// Include directives for member types
// Member `dimensions`
#include "rosidl_runtime_c/primitives_sequence_functions.h"
// Member `polygon`
#include "geometry_msgs/msg/polygon.h"
// Member `polygon`
#include "geometry_msgs/msg/detail/polygon__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  shape_msgs__msg__SolidPrimitive__init(message_memory);
}

void shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_fini_function(void * message_memory)
{
  shape_msgs__msg__SolidPrimitive__fini(message_memory);
}

size_t shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__size_function__SolidPrimitive__dimensions(
  const void * untyped_member)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return member->size;
}

const void * shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__get_const_function__SolidPrimitive__dimensions(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__double__Sequence * member =
    (const rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void * shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__get_function__SolidPrimitive__dimensions(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  return &member->data[index];
}

void shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__fetch_function__SolidPrimitive__dimensions(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const double * item =
    ((const double *)
    shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__get_const_function__SolidPrimitive__dimensions(untyped_member, index));
  double * value =
    (double *)(untyped_value);
  *value = *item;
}

void shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__assign_function__SolidPrimitive__dimensions(
  void * untyped_member, size_t index, const void * untyped_value)
{
  double * item =
    ((double *)
    shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__get_function__SolidPrimitive__dimensions(untyped_member, index));
  const double * value =
    (const double *)(untyped_value);
  *item = *value;
}

bool shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__resize_function__SolidPrimitive__dimensions(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__double__Sequence * member =
    (rosidl_runtime_c__double__Sequence *)(untyped_member);
  rosidl_runtime_c__double__Sequence__fini(member);
  return rosidl_runtime_c__double__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_message_member_array[3] = {
  {
    "type",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(shape_msgs__msg__SolidPrimitive, type),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "dimensions",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    3,  // array size
    true,  // is upper bound
    offsetof(shape_msgs__msg__SolidPrimitive, dimensions),  // bytes offset in struct
    NULL,  // default value
    shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__size_function__SolidPrimitive__dimensions,  // size() function pointer
    shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__get_const_function__SolidPrimitive__dimensions,  // get_const(index) function pointer
    shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__get_function__SolidPrimitive__dimensions,  // get(index) function pointer
    shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__fetch_function__SolidPrimitive__dimensions,  // fetch(index, &value) function pointer
    shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__assign_function__SolidPrimitive__dimensions,  // assign(index, value) function pointer
    shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__resize_function__SolidPrimitive__dimensions  // resize(index) function pointer
  },
  {
    "polygon",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(shape_msgs__msg__SolidPrimitive, polygon),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_message_members = {
  "shape_msgs__msg",  // message namespace
  "SolidPrimitive",  // message name
  3,  // number of fields
  sizeof(shape_msgs__msg__SolidPrimitive),
  false,  // has_any_key_member_
  shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_message_member_array,  // message members
  shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_init_function,  // function to initialize message memory (memory has to be allocated)
  shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_message_type_support_handle = {
  0,
  &shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_message_members,
  get_message_typesupport_handle_function,
  &shape_msgs__msg__SolidPrimitive__get_type_hash,
  &shape_msgs__msg__SolidPrimitive__get_type_description,
  &shape_msgs__msg__SolidPrimitive__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_shape_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, shape_msgs, msg, SolidPrimitive)() {
  shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Polygon)();
  if (!shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_message_type_support_handle.typesupport_identifier) {
    shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &shape_msgs__msg__SolidPrimitive__rosidl_typesupport_introspection_c__SolidPrimitive_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
