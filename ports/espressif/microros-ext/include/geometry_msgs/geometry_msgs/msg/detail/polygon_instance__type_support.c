// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from geometry_msgs:msg/PolygonInstance.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "geometry_msgs/msg/detail/polygon_instance__rosidl_typesupport_introspection_c.h"
#include "geometry_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "geometry_msgs/msg/detail/polygon_instance__functions.h"
#include "geometry_msgs/msg/detail/polygon_instance__struct.h"


// Include directives for member types
// Member `polygon`
#include "geometry_msgs/msg/polygon.h"
// Member `polygon`
#include "geometry_msgs/msg/detail/polygon__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  geometry_msgs__msg__PolygonInstance__init(message_memory);
}

void geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_fini_function(void * message_memory)
{
  geometry_msgs__msg__PolygonInstance__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_message_member_array[2] = {
  {
    "polygon",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(geometry_msgs__msg__PolygonInstance, polygon),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(geometry_msgs__msg__PolygonInstance, id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_message_members = {
  "geometry_msgs__msg",  // message namespace
  "PolygonInstance",  // message name
  2,  // number of fields
  sizeof(geometry_msgs__msg__PolygonInstance),
  false,  // has_any_key_member_
  geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_message_member_array,  // message members
  geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_init_function,  // function to initialize message memory (memory has to be allocated)
  geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_message_type_support_handle = {
  0,
  &geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_message_members,
  get_message_typesupport_handle_function,
  &geometry_msgs__msg__PolygonInstance__get_type_hash,
  &geometry_msgs__msg__PolygonInstance__get_type_description,
  &geometry_msgs__msg__PolygonInstance__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_geometry_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, PolygonInstance)() {
  geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Polygon)();
  if (!geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_message_type_support_handle.typesupport_identifier) {
    geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &geometry_msgs__msg__PolygonInstance__rosidl_typesupport_introspection_c__PolygonInstance_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
