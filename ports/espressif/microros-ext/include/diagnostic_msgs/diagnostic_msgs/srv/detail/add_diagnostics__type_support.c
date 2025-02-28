// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from diagnostic_msgs:srv/AddDiagnostics.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "diagnostic_msgs/srv/detail/add_diagnostics__rosidl_typesupport_introspection_c.h"
#include "diagnostic_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "diagnostic_msgs/srv/detail/add_diagnostics__functions.h"
#include "diagnostic_msgs/srv/detail/add_diagnostics__struct.h"


// Include directives for member types
// Member `load_namespace`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  diagnostic_msgs__srv__AddDiagnostics_Request__init(message_memory);
}

void diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_fini_function(void * message_memory)
{
  diagnostic_msgs__srv__AddDiagnostics_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_message_member_array[1] = {
  {
    "load_namespace",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(diagnostic_msgs__srv__AddDiagnostics_Request, load_namespace),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_message_members = {
  "diagnostic_msgs__srv",  // message namespace
  "AddDiagnostics_Request",  // message name
  1,  // number of fields
  sizeof(diagnostic_msgs__srv__AddDiagnostics_Request),
  false,  // has_any_key_member_
  diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_message_member_array,  // message members
  diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_message_type_support_handle = {
  0,
  &diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_message_members,
  get_message_typesupport_handle_function,
  &diagnostic_msgs__srv__AddDiagnostics_Request__get_type_hash,
  &diagnostic_msgs__srv__AddDiagnostics_Request__get_type_description,
  &diagnostic_msgs__srv__AddDiagnostics_Request__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_diagnostic_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics_Request)() {
  if (!diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_message_type_support_handle.typesupport_identifier) {
    diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "diagnostic_msgs/srv/detail/add_diagnostics__rosidl_typesupport_introspection_c.h"
// already included above
// #include "diagnostic_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "diagnostic_msgs/srv/detail/add_diagnostics__functions.h"
// already included above
// #include "diagnostic_msgs/srv/detail/add_diagnostics__struct.h"


// Include directives for member types
// Member `message`
// already included above
// #include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  diagnostic_msgs__srv__AddDiagnostics_Response__init(message_memory);
}

void diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_fini_function(void * message_memory)
{
  diagnostic_msgs__srv__AddDiagnostics_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_message_member_array[2] = {
  {
    "success",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(diagnostic_msgs__srv__AddDiagnostics_Response, success),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "message",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(diagnostic_msgs__srv__AddDiagnostics_Response, message),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_message_members = {
  "diagnostic_msgs__srv",  // message namespace
  "AddDiagnostics_Response",  // message name
  2,  // number of fields
  sizeof(diagnostic_msgs__srv__AddDiagnostics_Response),
  false,  // has_any_key_member_
  diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_message_member_array,  // message members
  diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_message_type_support_handle = {
  0,
  &diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_message_members,
  get_message_typesupport_handle_function,
  &diagnostic_msgs__srv__AddDiagnostics_Response__get_type_hash,
  &diagnostic_msgs__srv__AddDiagnostics_Response__get_type_description,
  &diagnostic_msgs__srv__AddDiagnostics_Response__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_diagnostic_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics_Response)() {
  if (!diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_message_type_support_handle.typesupport_identifier) {
    diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "diagnostic_msgs/srv/detail/add_diagnostics__rosidl_typesupport_introspection_c.h"
// already included above
// #include "diagnostic_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "diagnostic_msgs/srv/detail/add_diagnostics__functions.h"
// already included above
// #include "diagnostic_msgs/srv/detail/add_diagnostics__struct.h"


// Include directives for member types
// Member `info`
#include "service_msgs/msg/service_event_info.h"
// Member `info`
#include "service_msgs/msg/detail/service_event_info__rosidl_typesupport_introspection_c.h"
// Member `request`
// Member `response`
#include "diagnostic_msgs/srv/add_diagnostics.h"
// Member `request`
// Member `response`
// already included above
// #include "diagnostic_msgs/srv/detail/add_diagnostics__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  diagnostic_msgs__srv__AddDiagnostics_Event__init(message_memory);
}

void diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_fini_function(void * message_memory)
{
  diagnostic_msgs__srv__AddDiagnostics_Event__fini(message_memory);
}

size_t diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__size_function__AddDiagnostics_Event__request(
  const void * untyped_member)
{
  const diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * member =
    (const diagnostic_msgs__srv__AddDiagnostics_Request__Sequence *)(untyped_member);
  return member->size;
}

const void * diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_const_function__AddDiagnostics_Event__request(
  const void * untyped_member, size_t index)
{
  const diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * member =
    (const diagnostic_msgs__srv__AddDiagnostics_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void * diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_function__AddDiagnostics_Event__request(
  void * untyped_member, size_t index)
{
  diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * member =
    (diagnostic_msgs__srv__AddDiagnostics_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__fetch_function__AddDiagnostics_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const diagnostic_msgs__srv__AddDiagnostics_Request * item =
    ((const diagnostic_msgs__srv__AddDiagnostics_Request *)
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_const_function__AddDiagnostics_Event__request(untyped_member, index));
  diagnostic_msgs__srv__AddDiagnostics_Request * value =
    (diagnostic_msgs__srv__AddDiagnostics_Request *)(untyped_value);
  *value = *item;
}

void diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__assign_function__AddDiagnostics_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  diagnostic_msgs__srv__AddDiagnostics_Request * item =
    ((diagnostic_msgs__srv__AddDiagnostics_Request *)
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_function__AddDiagnostics_Event__request(untyped_member, index));
  const diagnostic_msgs__srv__AddDiagnostics_Request * value =
    (const diagnostic_msgs__srv__AddDiagnostics_Request *)(untyped_value);
  *item = *value;
}

bool diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__resize_function__AddDiagnostics_Event__request(
  void * untyped_member, size_t size)
{
  diagnostic_msgs__srv__AddDiagnostics_Request__Sequence * member =
    (diagnostic_msgs__srv__AddDiagnostics_Request__Sequence *)(untyped_member);
  diagnostic_msgs__srv__AddDiagnostics_Request__Sequence__fini(member);
  return diagnostic_msgs__srv__AddDiagnostics_Request__Sequence__init(member, size);
}

size_t diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__size_function__AddDiagnostics_Event__response(
  const void * untyped_member)
{
  const diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * member =
    (const diagnostic_msgs__srv__AddDiagnostics_Response__Sequence *)(untyped_member);
  return member->size;
}

const void * diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_const_function__AddDiagnostics_Event__response(
  const void * untyped_member, size_t index)
{
  const diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * member =
    (const diagnostic_msgs__srv__AddDiagnostics_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void * diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_function__AddDiagnostics_Event__response(
  void * untyped_member, size_t index)
{
  diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * member =
    (diagnostic_msgs__srv__AddDiagnostics_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__fetch_function__AddDiagnostics_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const diagnostic_msgs__srv__AddDiagnostics_Response * item =
    ((const diagnostic_msgs__srv__AddDiagnostics_Response *)
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_const_function__AddDiagnostics_Event__response(untyped_member, index));
  diagnostic_msgs__srv__AddDiagnostics_Response * value =
    (diagnostic_msgs__srv__AddDiagnostics_Response *)(untyped_value);
  *value = *item;
}

void diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__assign_function__AddDiagnostics_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  diagnostic_msgs__srv__AddDiagnostics_Response * item =
    ((diagnostic_msgs__srv__AddDiagnostics_Response *)
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_function__AddDiagnostics_Event__response(untyped_member, index));
  const diagnostic_msgs__srv__AddDiagnostics_Response * value =
    (const diagnostic_msgs__srv__AddDiagnostics_Response *)(untyped_value);
  *item = *value;
}

bool diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__resize_function__AddDiagnostics_Event__response(
  void * untyped_member, size_t size)
{
  diagnostic_msgs__srv__AddDiagnostics_Response__Sequence * member =
    (diagnostic_msgs__srv__AddDiagnostics_Response__Sequence *)(untyped_member);
  diagnostic_msgs__srv__AddDiagnostics_Response__Sequence__fini(member);
  return diagnostic_msgs__srv__AddDiagnostics_Response__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_member_array[3] = {
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(diagnostic_msgs__srv__AddDiagnostics_Event, info),  // bytes offset in struct
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
    offsetof(diagnostic_msgs__srv__AddDiagnostics_Event, request),  // bytes offset in struct
    NULL,  // default value
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__size_function__AddDiagnostics_Event__request,  // size() function pointer
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_const_function__AddDiagnostics_Event__request,  // get_const(index) function pointer
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_function__AddDiagnostics_Event__request,  // get(index) function pointer
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__fetch_function__AddDiagnostics_Event__request,  // fetch(index, &value) function pointer
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__assign_function__AddDiagnostics_Event__request,  // assign(index, value) function pointer
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__resize_function__AddDiagnostics_Event__request  // resize(index) function pointer
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
    offsetof(diagnostic_msgs__srv__AddDiagnostics_Event, response),  // bytes offset in struct
    NULL,  // default value
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__size_function__AddDiagnostics_Event__response,  // size() function pointer
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_const_function__AddDiagnostics_Event__response,  // get_const(index) function pointer
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__get_function__AddDiagnostics_Event__response,  // get(index) function pointer
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__fetch_function__AddDiagnostics_Event__response,  // fetch(index, &value) function pointer
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__assign_function__AddDiagnostics_Event__response,  // assign(index, value) function pointer
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__resize_function__AddDiagnostics_Event__response  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_members = {
  "diagnostic_msgs__srv",  // message namespace
  "AddDiagnostics_Event",  // message name
  3,  // number of fields
  sizeof(diagnostic_msgs__srv__AddDiagnostics_Event),
  false,  // has_any_key_member_
  diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_member_array,  // message members
  diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_type_support_handle = {
  0,
  &diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_members,
  get_message_typesupport_handle_function,
  &diagnostic_msgs__srv__AddDiagnostics_Event__get_type_hash,
  &diagnostic_msgs__srv__AddDiagnostics_Event__get_type_description,
  &diagnostic_msgs__srv__AddDiagnostics_Event__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_diagnostic_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics_Event)() {
  diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, service_msgs, msg, ServiceEventInfo)();
  diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics_Request)();
  diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics_Response)();
  if (!diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_type_support_handle.typesupport_identifier) {
    diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "diagnostic_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "diagnostic_msgs/srv/detail/add_diagnostics__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers diagnostic_msgs__srv__detail__add_diagnostics__rosidl_typesupport_introspection_c__AddDiagnostics_service_members = {
  "diagnostic_msgs__srv",  // service namespace
  "AddDiagnostics",  // service name
  // the following fields are initialized below on first access
  NULL,  // request message
  // diagnostic_msgs__srv__detail__add_diagnostics__rosidl_typesupport_introspection_c__AddDiagnostics_Request_message_type_support_handle,
  NULL,  // response message
  // diagnostic_msgs__srv__detail__add_diagnostics__rosidl_typesupport_introspection_c__AddDiagnostics_Response_message_type_support_handle
  NULL  // event_message
  // diagnostic_msgs__srv__detail__add_diagnostics__rosidl_typesupport_introspection_c__AddDiagnostics_Response_message_type_support_handle
};


static rosidl_service_type_support_t diagnostic_msgs__srv__detail__add_diagnostics__rosidl_typesupport_introspection_c__AddDiagnostics_service_type_support_handle = {
  0,
  &diagnostic_msgs__srv__detail__add_diagnostics__rosidl_typesupport_introspection_c__AddDiagnostics_service_members,
  get_service_typesupport_handle_function,
  &diagnostic_msgs__srv__AddDiagnostics_Request__rosidl_typesupport_introspection_c__AddDiagnostics_Request_message_type_support_handle,
  &diagnostic_msgs__srv__AddDiagnostics_Response__rosidl_typesupport_introspection_c__AddDiagnostics_Response_message_type_support_handle,
  &diagnostic_msgs__srv__AddDiagnostics_Event__rosidl_typesupport_introspection_c__AddDiagnostics_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    diagnostic_msgs,
    srv,
    AddDiagnostics
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    diagnostic_msgs,
    srv,
    AddDiagnostics
  ),
  &diagnostic_msgs__srv__AddDiagnostics__get_type_hash,
  &diagnostic_msgs__srv__AddDiagnostics__get_type_description,
  &diagnostic_msgs__srv__AddDiagnostics__get_type_description_sources,
};

// Forward declaration of message type support functions for service members
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics_Request)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics_Response)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics_Event)(void);

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_diagnostic_msgs
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics)(void) {
  if (!diagnostic_msgs__srv__detail__add_diagnostics__rosidl_typesupport_introspection_c__AddDiagnostics_service_type_support_handle.typesupport_identifier) {
    diagnostic_msgs__srv__detail__add_diagnostics__rosidl_typesupport_introspection_c__AddDiagnostics_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)diagnostic_msgs__srv__detail__add_diagnostics__rosidl_typesupport_introspection_c__AddDiagnostics_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics_Response)()->data;
  }
  if (!service_members->event_members_) {
    service_members->event_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, diagnostic_msgs, srv, AddDiagnostics_Event)()->data;
  }

  return &diagnostic_msgs__srv__detail__add_diagnostics__rosidl_typesupport_introspection_c__AddDiagnostics_service_type_support_handle;
}
