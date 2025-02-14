// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from visualization_msgs:msg/InteractiveMarkerUpdate.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "visualization_msgs/msg/detail/interactive_marker_update__rosidl_typesupport_introspection_c.h"
#include "visualization_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "visualization_msgs/msg/detail/interactive_marker_update__functions.h"
#include "visualization_msgs/msg/detail/interactive_marker_update__struct.h"


// Include directives for member types
// Member `server_id`
// Member `erases`
#include "rosidl_runtime_c/string_functions.h"
// Member `markers`
#include "visualization_msgs/msg/interactive_marker.h"
// Member `markers`
#include "visualization_msgs/msg/detail/interactive_marker__rosidl_typesupport_introspection_c.h"
// Member `poses`
#include "visualization_msgs/msg/interactive_marker_pose.h"
// Member `poses`
#include "visualization_msgs/msg/detail/interactive_marker_pose__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  visualization_msgs__msg__InteractiveMarkerUpdate__init(message_memory);
}

void visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_fini_function(void * message_memory)
{
  visualization_msgs__msg__InteractiveMarkerUpdate__fini(message_memory);
}

size_t visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__size_function__InteractiveMarkerUpdate__markers(
  const void * untyped_member)
{
  const visualization_msgs__msg__InteractiveMarker__Sequence * member =
    (const visualization_msgs__msg__InteractiveMarker__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_const_function__InteractiveMarkerUpdate__markers(
  const void * untyped_member, size_t index)
{
  const visualization_msgs__msg__InteractiveMarker__Sequence * member =
    (const visualization_msgs__msg__InteractiveMarker__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_function__InteractiveMarkerUpdate__markers(
  void * untyped_member, size_t index)
{
  visualization_msgs__msg__InteractiveMarker__Sequence * member =
    (visualization_msgs__msg__InteractiveMarker__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__fetch_function__InteractiveMarkerUpdate__markers(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const visualization_msgs__msg__InteractiveMarker * item =
    ((const visualization_msgs__msg__InteractiveMarker *)
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_const_function__InteractiveMarkerUpdate__markers(untyped_member, index));
  visualization_msgs__msg__InteractiveMarker * value =
    (visualization_msgs__msg__InteractiveMarker *)(untyped_value);
  *value = *item;
}

void visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__assign_function__InteractiveMarkerUpdate__markers(
  void * untyped_member, size_t index, const void * untyped_value)
{
  visualization_msgs__msg__InteractiveMarker * item =
    ((visualization_msgs__msg__InteractiveMarker *)
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_function__InteractiveMarkerUpdate__markers(untyped_member, index));
  const visualization_msgs__msg__InteractiveMarker * value =
    (const visualization_msgs__msg__InteractiveMarker *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__resize_function__InteractiveMarkerUpdate__markers(
  void * untyped_member, size_t size)
{
  visualization_msgs__msg__InteractiveMarker__Sequence * member =
    (visualization_msgs__msg__InteractiveMarker__Sequence *)(untyped_member);
  visualization_msgs__msg__InteractiveMarker__Sequence__fini(member);
  return visualization_msgs__msg__InteractiveMarker__Sequence__init(member, size);
}

size_t visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__size_function__InteractiveMarkerUpdate__poses(
  const void * untyped_member)
{
  const visualization_msgs__msg__InteractiveMarkerPose__Sequence * member =
    (const visualization_msgs__msg__InteractiveMarkerPose__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_const_function__InteractiveMarkerUpdate__poses(
  const void * untyped_member, size_t index)
{
  const visualization_msgs__msg__InteractiveMarkerPose__Sequence * member =
    (const visualization_msgs__msg__InteractiveMarkerPose__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_function__InteractiveMarkerUpdate__poses(
  void * untyped_member, size_t index)
{
  visualization_msgs__msg__InteractiveMarkerPose__Sequence * member =
    (visualization_msgs__msg__InteractiveMarkerPose__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__fetch_function__InteractiveMarkerUpdate__poses(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const visualization_msgs__msg__InteractiveMarkerPose * item =
    ((const visualization_msgs__msg__InteractiveMarkerPose *)
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_const_function__InteractiveMarkerUpdate__poses(untyped_member, index));
  visualization_msgs__msg__InteractiveMarkerPose * value =
    (visualization_msgs__msg__InteractiveMarkerPose *)(untyped_value);
  *value = *item;
}

void visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__assign_function__InteractiveMarkerUpdate__poses(
  void * untyped_member, size_t index, const void * untyped_value)
{
  visualization_msgs__msg__InteractiveMarkerPose * item =
    ((visualization_msgs__msg__InteractiveMarkerPose *)
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_function__InteractiveMarkerUpdate__poses(untyped_member, index));
  const visualization_msgs__msg__InteractiveMarkerPose * value =
    (const visualization_msgs__msg__InteractiveMarkerPose *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__resize_function__InteractiveMarkerUpdate__poses(
  void * untyped_member, size_t size)
{
  visualization_msgs__msg__InteractiveMarkerPose__Sequence * member =
    (visualization_msgs__msg__InteractiveMarkerPose__Sequence *)(untyped_member);
  visualization_msgs__msg__InteractiveMarkerPose__Sequence__fini(member);
  return visualization_msgs__msg__InteractiveMarkerPose__Sequence__init(member, size);
}

size_t visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__size_function__InteractiveMarkerUpdate__erases(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_const_function__InteractiveMarkerUpdate__erases(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_function__InteractiveMarkerUpdate__erases(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__fetch_function__InteractiveMarkerUpdate__erases(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_const_function__InteractiveMarkerUpdate__erases(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__assign_function__InteractiveMarkerUpdate__erases(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_function__InteractiveMarkerUpdate__erases(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__resize_function__InteractiveMarkerUpdate__erases(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_message_member_array[6] = {
  {
    "server_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__InteractiveMarkerUpdate, server_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "seq_num",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__InteractiveMarkerUpdate, seq_num),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "type",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__InteractiveMarkerUpdate, type),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "markers",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__InteractiveMarkerUpdate, markers),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__size_function__InteractiveMarkerUpdate__markers,  // size() function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_const_function__InteractiveMarkerUpdate__markers,  // get_const(index) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_function__InteractiveMarkerUpdate__markers,  // get(index) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__fetch_function__InteractiveMarkerUpdate__markers,  // fetch(index, &value) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__assign_function__InteractiveMarkerUpdate__markers,  // assign(index, value) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__resize_function__InteractiveMarkerUpdate__markers  // resize(index) function pointer
  },
  {
    "poses",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__InteractiveMarkerUpdate, poses),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__size_function__InteractiveMarkerUpdate__poses,  // size() function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_const_function__InteractiveMarkerUpdate__poses,  // get_const(index) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_function__InteractiveMarkerUpdate__poses,  // get(index) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__fetch_function__InteractiveMarkerUpdate__poses,  // fetch(index, &value) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__assign_function__InteractiveMarkerUpdate__poses,  // assign(index, value) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__resize_function__InteractiveMarkerUpdate__poses  // resize(index) function pointer
  },
  {
    "erases",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__InteractiveMarkerUpdate, erases),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__size_function__InteractiveMarkerUpdate__erases,  // size() function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_const_function__InteractiveMarkerUpdate__erases,  // get_const(index) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__get_function__InteractiveMarkerUpdate__erases,  // get(index) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__fetch_function__InteractiveMarkerUpdate__erases,  // fetch(index, &value) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__assign_function__InteractiveMarkerUpdate__erases,  // assign(index, value) function pointer
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__resize_function__InteractiveMarkerUpdate__erases  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_message_members = {
  "visualization_msgs__msg",  // message namespace
  "InteractiveMarkerUpdate",  // message name
  6,  // number of fields
  sizeof(visualization_msgs__msg__InteractiveMarkerUpdate),
  false,  // has_any_key_member_
  visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_message_member_array,  // message members
  visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_init_function,  // function to initialize message memory (memory has to be allocated)
  visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_message_type_support_handle = {
  0,
  &visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_message_members,
  get_message_typesupport_handle_function,
  &visualization_msgs__msg__InteractiveMarkerUpdate__get_type_hash,
  &visualization_msgs__msg__InteractiveMarkerUpdate__get_type_description,
  &visualization_msgs__msg__InteractiveMarkerUpdate__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_visualization_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, msg, InteractiveMarkerUpdate)() {
  visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_message_member_array[3].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, msg, InteractiveMarker)();
  visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_message_member_array[4].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, msg, InteractiveMarkerPose)();
  if (!visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_message_type_support_handle.typesupport_identifier) {
    visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &visualization_msgs__msg__InteractiveMarkerUpdate__rosidl_typesupport_introspection_c__InteractiveMarkerUpdate_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
