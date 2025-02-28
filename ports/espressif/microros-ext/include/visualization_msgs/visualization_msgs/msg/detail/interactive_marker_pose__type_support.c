// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from visualization_msgs:msg/InteractiveMarkerPose.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "visualization_msgs/msg/detail/interactive_marker_pose__rosidl_typesupport_introspection_c.h"
#include "visualization_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "visualization_msgs/msg/detail/interactive_marker_pose__functions.h"
#include "visualization_msgs/msg/detail/interactive_marker_pose__struct.h"


// Include directives for member types
// Member `header`
#include "std_msgs/msg/header.h"
// Member `header`
#include "std_msgs/msg/detail/header__rosidl_typesupport_introspection_c.h"
// Member `pose`
#include "geometry_msgs/msg/pose.h"
// Member `pose`
#include "geometry_msgs/msg/detail/pose__rosidl_typesupport_introspection_c.h"
// Member `name`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  visualization_msgs__msg__InteractiveMarkerPose__init(message_memory);
}

void visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_fini_function(void * message_memory)
{
  visualization_msgs__msg__InteractiveMarkerPose__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_message_member_array[3] = {
  {
    "header",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__InteractiveMarkerPose, header),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "pose",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__InteractiveMarkerPose, pose),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__msg__InteractiveMarkerPose, name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_message_members = {
  "visualization_msgs__msg",  // message namespace
  "InteractiveMarkerPose",  // message name
  3,  // number of fields
  sizeof(visualization_msgs__msg__InteractiveMarkerPose),
  false,  // has_any_key_member_
  visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_message_member_array,  // message members
  visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_init_function,  // function to initialize message memory (memory has to be allocated)
  visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_message_type_support_handle = {
  0,
  &visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_message_members,
  get_message_typesupport_handle_function,
  &visualization_msgs__msg__InteractiveMarkerPose__get_type_hash,
  &visualization_msgs__msg__InteractiveMarkerPose__get_type_description,
  &visualization_msgs__msg__InteractiveMarkerPose__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_visualization_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, msg, InteractiveMarkerPose)() {
  visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, std_msgs, msg, Header)();
  visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, Pose)();
  if (!visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_message_type_support_handle.typesupport_identifier) {
    visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &visualization_msgs__msg__InteractiveMarkerPose__rosidl_typesupport_introspection_c__InteractiveMarkerPose_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
