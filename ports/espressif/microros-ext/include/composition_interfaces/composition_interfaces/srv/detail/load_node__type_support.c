// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from composition_interfaces:srv/LoadNode.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "composition_interfaces/srv/detail/load_node__rosidl_typesupport_introspection_c.h"
#include "composition_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "composition_interfaces/srv/detail/load_node__functions.h"
#include "composition_interfaces/srv/detail/load_node__struct.h"


// Include directives for member types
// Member `package_name`
// Member `plugin_name`
// Member `node_name`
// Member `node_namespace`
// Member `remap_rules`
#include "rosidl_runtime_c/string_functions.h"
// Member `parameters`
// Member `extra_arguments`
#include "rcl_interfaces/msg/parameter.h"
// Member `parameters`
// Member `extra_arguments`
#include "rcl_interfaces/msg/detail/parameter__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  composition_interfaces__srv__LoadNode_Request__init(message_memory);
}

void composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_fini_function(void * message_memory)
{
  composition_interfaces__srv__LoadNode_Request__fini(message_memory);
}

size_t composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__size_function__LoadNode_Request__remap_rules(
  const void * untyped_member)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return member->size;
}

const void * composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Request__remap_rules(
  const void * untyped_member, size_t index)
{
  const rosidl_runtime_c__String__Sequence * member =
    (const rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void * composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_function__LoadNode_Request__remap_rules(
  void * untyped_member, size_t index)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  return &member->data[index];
}

void composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__fetch_function__LoadNode_Request__remap_rules(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosidl_runtime_c__String * item =
    ((const rosidl_runtime_c__String *)
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Request__remap_rules(untyped_member, index));
  rosidl_runtime_c__String * value =
    (rosidl_runtime_c__String *)(untyped_value);
  *value = *item;
}

void composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__assign_function__LoadNode_Request__remap_rules(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosidl_runtime_c__String * item =
    ((rosidl_runtime_c__String *)
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_function__LoadNode_Request__remap_rules(untyped_member, index));
  const rosidl_runtime_c__String * value =
    (const rosidl_runtime_c__String *)(untyped_value);
  *item = *value;
}

bool composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__resize_function__LoadNode_Request__remap_rules(
  void * untyped_member, size_t size)
{
  rosidl_runtime_c__String__Sequence * member =
    (rosidl_runtime_c__String__Sequence *)(untyped_member);
  rosidl_runtime_c__String__Sequence__fini(member);
  return rosidl_runtime_c__String__Sequence__init(member, size);
}

size_t composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__size_function__LoadNode_Request__parameters(
  const void * untyped_member)
{
  const rcl_interfaces__msg__Parameter__Sequence * member =
    (const rcl_interfaces__msg__Parameter__Sequence *)(untyped_member);
  return member->size;
}

const void * composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Request__parameters(
  const void * untyped_member, size_t index)
{
  const rcl_interfaces__msg__Parameter__Sequence * member =
    (const rcl_interfaces__msg__Parameter__Sequence *)(untyped_member);
  return &member->data[index];
}

void * composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_function__LoadNode_Request__parameters(
  void * untyped_member, size_t index)
{
  rcl_interfaces__msg__Parameter__Sequence * member =
    (rcl_interfaces__msg__Parameter__Sequence *)(untyped_member);
  return &member->data[index];
}

void composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__fetch_function__LoadNode_Request__parameters(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rcl_interfaces__msg__Parameter * item =
    ((const rcl_interfaces__msg__Parameter *)
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Request__parameters(untyped_member, index));
  rcl_interfaces__msg__Parameter * value =
    (rcl_interfaces__msg__Parameter *)(untyped_value);
  *value = *item;
}

void composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__assign_function__LoadNode_Request__parameters(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rcl_interfaces__msg__Parameter * item =
    ((rcl_interfaces__msg__Parameter *)
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_function__LoadNode_Request__parameters(untyped_member, index));
  const rcl_interfaces__msg__Parameter * value =
    (const rcl_interfaces__msg__Parameter *)(untyped_value);
  *item = *value;
}

bool composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__resize_function__LoadNode_Request__parameters(
  void * untyped_member, size_t size)
{
  rcl_interfaces__msg__Parameter__Sequence * member =
    (rcl_interfaces__msg__Parameter__Sequence *)(untyped_member);
  rcl_interfaces__msg__Parameter__Sequence__fini(member);
  return rcl_interfaces__msg__Parameter__Sequence__init(member, size);
}

size_t composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__size_function__LoadNode_Request__extra_arguments(
  const void * untyped_member)
{
  const rcl_interfaces__msg__Parameter__Sequence * member =
    (const rcl_interfaces__msg__Parameter__Sequence *)(untyped_member);
  return member->size;
}

const void * composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Request__extra_arguments(
  const void * untyped_member, size_t index)
{
  const rcl_interfaces__msg__Parameter__Sequence * member =
    (const rcl_interfaces__msg__Parameter__Sequence *)(untyped_member);
  return &member->data[index];
}

void * composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_function__LoadNode_Request__extra_arguments(
  void * untyped_member, size_t index)
{
  rcl_interfaces__msg__Parameter__Sequence * member =
    (rcl_interfaces__msg__Parameter__Sequence *)(untyped_member);
  return &member->data[index];
}

void composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__fetch_function__LoadNode_Request__extra_arguments(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rcl_interfaces__msg__Parameter * item =
    ((const rcl_interfaces__msg__Parameter *)
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Request__extra_arguments(untyped_member, index));
  rcl_interfaces__msg__Parameter * value =
    (rcl_interfaces__msg__Parameter *)(untyped_value);
  *value = *item;
}

void composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__assign_function__LoadNode_Request__extra_arguments(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rcl_interfaces__msg__Parameter * item =
    ((rcl_interfaces__msg__Parameter *)
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_function__LoadNode_Request__extra_arguments(untyped_member, index));
  const rcl_interfaces__msg__Parameter * value =
    (const rcl_interfaces__msg__Parameter *)(untyped_value);
  *item = *value;
}

bool composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__resize_function__LoadNode_Request__extra_arguments(
  void * untyped_member, size_t size)
{
  rcl_interfaces__msg__Parameter__Sequence * member =
    (rcl_interfaces__msg__Parameter__Sequence *)(untyped_member);
  rcl_interfaces__msg__Parameter__Sequence__fini(member);
  return rcl_interfaces__msg__Parameter__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_message_member_array[8] = {
  {
    "package_name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Request, package_name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "plugin_name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Request, plugin_name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "node_name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Request, node_name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "node_namespace",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Request, node_namespace),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "log_level",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Request, log_level),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "remap_rules",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Request, remap_rules),  // bytes offset in struct
    NULL,  // default value
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__size_function__LoadNode_Request__remap_rules,  // size() function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Request__remap_rules,  // get_const(index) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_function__LoadNode_Request__remap_rules,  // get(index) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__fetch_function__LoadNode_Request__remap_rules,  // fetch(index, &value) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__assign_function__LoadNode_Request__remap_rules,  // assign(index, value) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__resize_function__LoadNode_Request__remap_rules  // resize(index) function pointer
  },
  {
    "parameters",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Request, parameters),  // bytes offset in struct
    NULL,  // default value
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__size_function__LoadNode_Request__parameters,  // size() function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Request__parameters,  // get_const(index) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_function__LoadNode_Request__parameters,  // get(index) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__fetch_function__LoadNode_Request__parameters,  // fetch(index, &value) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__assign_function__LoadNode_Request__parameters,  // assign(index, value) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__resize_function__LoadNode_Request__parameters  // resize(index) function pointer
  },
  {
    "extra_arguments",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Request, extra_arguments),  // bytes offset in struct
    NULL,  // default value
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__size_function__LoadNode_Request__extra_arguments,  // size() function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Request__extra_arguments,  // get_const(index) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__get_function__LoadNode_Request__extra_arguments,  // get(index) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__fetch_function__LoadNode_Request__extra_arguments,  // fetch(index, &value) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__assign_function__LoadNode_Request__extra_arguments,  // assign(index, value) function pointer
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__resize_function__LoadNode_Request__extra_arguments  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_message_members = {
  "composition_interfaces__srv",  // message namespace
  "LoadNode_Request",  // message name
  8,  // number of fields
  sizeof(composition_interfaces__srv__LoadNode_Request),
  false,  // has_any_key_member_
  composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_message_member_array,  // message members
  composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_message_type_support_handle = {
  0,
  &composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_message_members,
  get_message_typesupport_handle_function,
  &composition_interfaces__srv__LoadNode_Request__get_type_hash,
  &composition_interfaces__srv__LoadNode_Request__get_type_description,
  &composition_interfaces__srv__LoadNode_Request__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_composition_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode_Request)() {
  composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_message_member_array[6].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rcl_interfaces, msg, Parameter)();
  composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_message_member_array[7].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rcl_interfaces, msg, Parameter)();
  if (!composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_message_type_support_handle.typesupport_identifier) {
    composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "composition_interfaces/srv/detail/load_node__rosidl_typesupport_introspection_c.h"
// already included above
// #include "composition_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "composition_interfaces/srv/detail/load_node__functions.h"
// already included above
// #include "composition_interfaces/srv/detail/load_node__struct.h"


// Include directives for member types
// Member `error_message`
// Member `full_node_name`
// already included above
// #include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  composition_interfaces__srv__LoadNode_Response__init(message_memory);
}

void composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_fini_function(void * message_memory)
{
  composition_interfaces__srv__LoadNode_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_message_member_array[4] = {
  {
    "success",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Response, success),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "error_message",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Response, error_message),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "full_node_name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Response, full_node_name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "unique_id",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Response, unique_id),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_message_members = {
  "composition_interfaces__srv",  // message namespace
  "LoadNode_Response",  // message name
  4,  // number of fields
  sizeof(composition_interfaces__srv__LoadNode_Response),
  false,  // has_any_key_member_
  composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_message_member_array,  // message members
  composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_message_type_support_handle = {
  0,
  &composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_message_members,
  get_message_typesupport_handle_function,
  &composition_interfaces__srv__LoadNode_Response__get_type_hash,
  &composition_interfaces__srv__LoadNode_Response__get_type_description,
  &composition_interfaces__srv__LoadNode_Response__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_composition_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode_Response)() {
  if (!composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_message_type_support_handle.typesupport_identifier) {
    composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "composition_interfaces/srv/detail/load_node__rosidl_typesupport_introspection_c.h"
// already included above
// #include "composition_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "composition_interfaces/srv/detail/load_node__functions.h"
// already included above
// #include "composition_interfaces/srv/detail/load_node__struct.h"


// Include directives for member types
// Member `info`
#include "service_msgs/msg/service_event_info.h"
// Member `info`
#include "service_msgs/msg/detail/service_event_info__rosidl_typesupport_introspection_c.h"
// Member `request`
// Member `response`
#include "composition_interfaces/srv/load_node.h"
// Member `request`
// Member `response`
// already included above
// #include "composition_interfaces/srv/detail/load_node__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  composition_interfaces__srv__LoadNode_Event__init(message_memory);
}

void composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_fini_function(void * message_memory)
{
  composition_interfaces__srv__LoadNode_Event__fini(message_memory);
}

size_t composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__size_function__LoadNode_Event__request(
  const void * untyped_member)
{
  const composition_interfaces__srv__LoadNode_Request__Sequence * member =
    (const composition_interfaces__srv__LoadNode_Request__Sequence *)(untyped_member);
  return member->size;
}

const void * composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Event__request(
  const void * untyped_member, size_t index)
{
  const composition_interfaces__srv__LoadNode_Request__Sequence * member =
    (const composition_interfaces__srv__LoadNode_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void * composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_function__LoadNode_Event__request(
  void * untyped_member, size_t index)
{
  composition_interfaces__srv__LoadNode_Request__Sequence * member =
    (composition_interfaces__srv__LoadNode_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__fetch_function__LoadNode_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const composition_interfaces__srv__LoadNode_Request * item =
    ((const composition_interfaces__srv__LoadNode_Request *)
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Event__request(untyped_member, index));
  composition_interfaces__srv__LoadNode_Request * value =
    (composition_interfaces__srv__LoadNode_Request *)(untyped_value);
  *value = *item;
}

void composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__assign_function__LoadNode_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  composition_interfaces__srv__LoadNode_Request * item =
    ((composition_interfaces__srv__LoadNode_Request *)
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_function__LoadNode_Event__request(untyped_member, index));
  const composition_interfaces__srv__LoadNode_Request * value =
    (const composition_interfaces__srv__LoadNode_Request *)(untyped_value);
  *item = *value;
}

bool composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__resize_function__LoadNode_Event__request(
  void * untyped_member, size_t size)
{
  composition_interfaces__srv__LoadNode_Request__Sequence * member =
    (composition_interfaces__srv__LoadNode_Request__Sequence *)(untyped_member);
  composition_interfaces__srv__LoadNode_Request__Sequence__fini(member);
  return composition_interfaces__srv__LoadNode_Request__Sequence__init(member, size);
}

size_t composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__size_function__LoadNode_Event__response(
  const void * untyped_member)
{
  const composition_interfaces__srv__LoadNode_Response__Sequence * member =
    (const composition_interfaces__srv__LoadNode_Response__Sequence *)(untyped_member);
  return member->size;
}

const void * composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Event__response(
  const void * untyped_member, size_t index)
{
  const composition_interfaces__srv__LoadNode_Response__Sequence * member =
    (const composition_interfaces__srv__LoadNode_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void * composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_function__LoadNode_Event__response(
  void * untyped_member, size_t index)
{
  composition_interfaces__srv__LoadNode_Response__Sequence * member =
    (composition_interfaces__srv__LoadNode_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__fetch_function__LoadNode_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const composition_interfaces__srv__LoadNode_Response * item =
    ((const composition_interfaces__srv__LoadNode_Response *)
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Event__response(untyped_member, index));
  composition_interfaces__srv__LoadNode_Response * value =
    (composition_interfaces__srv__LoadNode_Response *)(untyped_value);
  *value = *item;
}

void composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__assign_function__LoadNode_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  composition_interfaces__srv__LoadNode_Response * item =
    ((composition_interfaces__srv__LoadNode_Response *)
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_function__LoadNode_Event__response(untyped_member, index));
  const composition_interfaces__srv__LoadNode_Response * value =
    (const composition_interfaces__srv__LoadNode_Response *)(untyped_value);
  *item = *value;
}

bool composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__resize_function__LoadNode_Event__response(
  void * untyped_member, size_t size)
{
  composition_interfaces__srv__LoadNode_Response__Sequence * member =
    (composition_interfaces__srv__LoadNode_Response__Sequence *)(untyped_member);
  composition_interfaces__srv__LoadNode_Response__Sequence__fini(member);
  return composition_interfaces__srv__LoadNode_Response__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_member_array[3] = {
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Event, info),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "request",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Event, request),  // bytes offset in struct
    NULL,  // default value
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__size_function__LoadNode_Event__request,  // size() function pointer
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Event__request,  // get_const(index) function pointer
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_function__LoadNode_Event__request,  // get(index) function pointer
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__fetch_function__LoadNode_Event__request,  // fetch(index, &value) function pointer
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__assign_function__LoadNode_Event__request,  // assign(index, value) function pointer
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__resize_function__LoadNode_Event__request  // resize(index) function pointer
  },
  {
    "response",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    true,  // is array
    1,  // array size
    true,  // is upper bound
    offsetof(composition_interfaces__srv__LoadNode_Event, response),  // bytes offset in struct
    NULL,  // default value
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__size_function__LoadNode_Event__response,  // size() function pointer
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_const_function__LoadNode_Event__response,  // get_const(index) function pointer
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__get_function__LoadNode_Event__response,  // get(index) function pointer
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__fetch_function__LoadNode_Event__response,  // fetch(index, &value) function pointer
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__assign_function__LoadNode_Event__response,  // assign(index, value) function pointer
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__resize_function__LoadNode_Event__response  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_members = {
  "composition_interfaces__srv",  // message namespace
  "LoadNode_Event",  // message name
  3,  // number of fields
  sizeof(composition_interfaces__srv__LoadNode_Event),
  false,  // has_any_key_member_
  composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_member_array,  // message members
  composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_type_support_handle = {
  0,
  &composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_members,
  get_message_typesupport_handle_function,
  &composition_interfaces__srv__LoadNode_Event__get_type_hash,
  &composition_interfaces__srv__LoadNode_Event__get_type_description,
  &composition_interfaces__srv__LoadNode_Event__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_composition_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode_Event)() {
  composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, service_msgs, msg, ServiceEventInfo)();
  composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode_Request)();
  composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode_Response)();
  if (!composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_type_support_handle.typesupport_identifier) {
    composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "composition_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "composition_interfaces/srv/detail/load_node__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers composition_interfaces__srv__detail__load_node__rosidl_typesupport_introspection_c__LoadNode_service_members = {
  "composition_interfaces__srv",  // service namespace
  "LoadNode",  // service name
  // the following fields are initialized below on first access
  NULL,  // request message
  // composition_interfaces__srv__detail__load_node__rosidl_typesupport_introspection_c__LoadNode_Request_message_type_support_handle,
  NULL,  // response message
  // composition_interfaces__srv__detail__load_node__rosidl_typesupport_introspection_c__LoadNode_Response_message_type_support_handle
  NULL  // event_message
  // composition_interfaces__srv__detail__load_node__rosidl_typesupport_introspection_c__LoadNode_Response_message_type_support_handle
};


static rosidl_service_type_support_t composition_interfaces__srv__detail__load_node__rosidl_typesupport_introspection_c__LoadNode_service_type_support_handle = {
  0,
  &composition_interfaces__srv__detail__load_node__rosidl_typesupport_introspection_c__LoadNode_service_members,
  get_service_typesupport_handle_function,
  &composition_interfaces__srv__LoadNode_Request__rosidl_typesupport_introspection_c__LoadNode_Request_message_type_support_handle,
  &composition_interfaces__srv__LoadNode_Response__rosidl_typesupport_introspection_c__LoadNode_Response_message_type_support_handle,
  &composition_interfaces__srv__LoadNode_Event__rosidl_typesupport_introspection_c__LoadNode_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    composition_interfaces,
    srv,
    LoadNode
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    composition_interfaces,
    srv,
    LoadNode
  ),
  &composition_interfaces__srv__LoadNode__get_type_hash,
  &composition_interfaces__srv__LoadNode__get_type_description,
  &composition_interfaces__srv__LoadNode__get_type_description_sources,
};

// Forward declaration of message type support functions for service members
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode_Request)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode_Response)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode_Event)(void);

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_composition_interfaces
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode)(void) {
  if (!composition_interfaces__srv__detail__load_node__rosidl_typesupport_introspection_c__LoadNode_service_type_support_handle.typesupport_identifier) {
    composition_interfaces__srv__detail__load_node__rosidl_typesupport_introspection_c__LoadNode_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)composition_interfaces__srv__detail__load_node__rosidl_typesupport_introspection_c__LoadNode_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode_Response)()->data;
  }
  if (!service_members->event_members_) {
    service_members->event_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, composition_interfaces, srv, LoadNode_Event)()->data;
  }

  return &composition_interfaces__srv__detail__load_node__rosidl_typesupport_introspection_c__LoadNode_service_type_support_handle;
}
