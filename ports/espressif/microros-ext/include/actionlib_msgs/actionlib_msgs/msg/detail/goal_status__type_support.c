// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from actionlib_msgs:msg/GoalStatus.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "actionlib_msgs/msg/detail/goal_status__rosidl_typesupport_introspection_c.h"
#include "actionlib_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "actionlib_msgs/msg/detail/goal_status__functions.h"
#include "actionlib_msgs/msg/detail/goal_status__struct.h"


// Include directives for member types
// Member `goal_id`
#include "actionlib_msgs/msg/goal_id.h"
// Member `goal_id`
#include "actionlib_msgs/msg/detail/goal_id__rosidl_typesupport_introspection_c.h"
// Member `text`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  actionlib_msgs__msg__GoalStatus__init(message_memory);
}

void actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_fini_function(void * message_memory)
{
  actionlib_msgs__msg__GoalStatus__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_message_member_array[3] = {
  {
    "goal_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actionlib_msgs__msg__GoalStatus, goal_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "status",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actionlib_msgs__msg__GoalStatus, status),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "text",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(actionlib_msgs__msg__GoalStatus, text),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_message_members = {
  "actionlib_msgs__msg",  // message namespace
  "GoalStatus",  // message name
  3,  // number of fields
  sizeof(actionlib_msgs__msg__GoalStatus),
  false,  // has_any_key_member_
  actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_message_member_array,  // message members
  actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_init_function,  // function to initialize message memory (memory has to be allocated)
  actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_message_type_support_handle = {
  0,
  &actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_message_members,
  get_message_typesupport_handle_function,
  &actionlib_msgs__msg__GoalStatus__get_type_hash,
  &actionlib_msgs__msg__GoalStatus__get_type_description,
  &actionlib_msgs__msg__GoalStatus__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_actionlib_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, actionlib_msgs, msg, GoalStatus)() {
  actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, actionlib_msgs, msg, GoalID)();
  if (!actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_message_type_support_handle.typesupport_identifier) {
    actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &actionlib_msgs__msg__GoalStatus__rosidl_typesupport_introspection_c__GoalStatus_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
