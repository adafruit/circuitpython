// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from example_interfaces:srv/AddTwoInts.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "example_interfaces/srv/detail/add_two_ints__rosidl_typesupport_introspection_c.h"
#include "example_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "example_interfaces/srv/detail/add_two_ints__functions.h"
#include "example_interfaces/srv/detail/add_two_ints__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  example_interfaces__srv__AddTwoInts_Request__init(message_memory);
}

void example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_fini_function(void * message_memory)
{
  example_interfaces__srv__AddTwoInts_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_message_member_array[2] = {
  {
    "a",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(example_interfaces__srv__AddTwoInts_Request, a),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "b",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(example_interfaces__srv__AddTwoInts_Request, b),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_message_members = {
  "example_interfaces__srv",  // message namespace
  "AddTwoInts_Request",  // message name
  2,  // number of fields
  sizeof(example_interfaces__srv__AddTwoInts_Request),
  false,  // has_any_key_member_
  example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_message_member_array,  // message members
  example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_message_type_support_handle = {
  0,
  &example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_message_members,
  get_message_typesupport_handle_function,
  &example_interfaces__srv__AddTwoInts_Request__get_type_hash,
  &example_interfaces__srv__AddTwoInts_Request__get_type_description,
  &example_interfaces__srv__AddTwoInts_Request__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_example_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts_Request)() {
  if (!example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_message_type_support_handle.typesupport_identifier) {
    example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "example_interfaces/srv/detail/add_two_ints__rosidl_typesupport_introspection_c.h"
// already included above
// #include "example_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "example_interfaces/srv/detail/add_two_ints__functions.h"
// already included above
// #include "example_interfaces/srv/detail/add_two_ints__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  example_interfaces__srv__AddTwoInts_Response__init(message_memory);
}

void example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_fini_function(void * message_memory)
{
  example_interfaces__srv__AddTwoInts_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_message_member_array[1] = {
  {
    "sum",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(example_interfaces__srv__AddTwoInts_Response, sum),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_message_members = {
  "example_interfaces__srv",  // message namespace
  "AddTwoInts_Response",  // message name
  1,  // number of fields
  sizeof(example_interfaces__srv__AddTwoInts_Response),
  false,  // has_any_key_member_
  example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_message_member_array,  // message members
  example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_message_type_support_handle = {
  0,
  &example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_message_members,
  get_message_typesupport_handle_function,
  &example_interfaces__srv__AddTwoInts_Response__get_type_hash,
  &example_interfaces__srv__AddTwoInts_Response__get_type_description,
  &example_interfaces__srv__AddTwoInts_Response__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_example_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts_Response)() {
  if (!example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_message_type_support_handle.typesupport_identifier) {
    example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "example_interfaces/srv/detail/add_two_ints__rosidl_typesupport_introspection_c.h"
// already included above
// #include "example_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "example_interfaces/srv/detail/add_two_ints__functions.h"
// already included above
// #include "example_interfaces/srv/detail/add_two_ints__struct.h"


// Include directives for member types
// Member `info`
#include "service_msgs/msg/service_event_info.h"
// Member `info`
#include "service_msgs/msg/detail/service_event_info__rosidl_typesupport_introspection_c.h"
// Member `request`
// Member `response`
#include "example_interfaces/srv/add_two_ints.h"
// Member `request`
// Member `response`
// already included above
// #include "example_interfaces/srv/detail/add_two_ints__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  example_interfaces__srv__AddTwoInts_Event__init(message_memory);
}

void example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_fini_function(void * message_memory)
{
  example_interfaces__srv__AddTwoInts_Event__fini(message_memory);
}

size_t example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__size_function__AddTwoInts_Event__request(
  const void * untyped_member)
{
  const example_interfaces__srv__AddTwoInts_Request__Sequence * member =
    (const example_interfaces__srv__AddTwoInts_Request__Sequence *)(untyped_member);
  return member->size;
}

const void * example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_const_function__AddTwoInts_Event__request(
  const void * untyped_member, size_t index)
{
  const example_interfaces__srv__AddTwoInts_Request__Sequence * member =
    (const example_interfaces__srv__AddTwoInts_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void * example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_function__AddTwoInts_Event__request(
  void * untyped_member, size_t index)
{
  example_interfaces__srv__AddTwoInts_Request__Sequence * member =
    (example_interfaces__srv__AddTwoInts_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__fetch_function__AddTwoInts_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const example_interfaces__srv__AddTwoInts_Request * item =
    ((const example_interfaces__srv__AddTwoInts_Request *)
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_const_function__AddTwoInts_Event__request(untyped_member, index));
  example_interfaces__srv__AddTwoInts_Request * value =
    (example_interfaces__srv__AddTwoInts_Request *)(untyped_value);
  *value = *item;
}

void example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__assign_function__AddTwoInts_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  example_interfaces__srv__AddTwoInts_Request * item =
    ((example_interfaces__srv__AddTwoInts_Request *)
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_function__AddTwoInts_Event__request(untyped_member, index));
  const example_interfaces__srv__AddTwoInts_Request * value =
    (const example_interfaces__srv__AddTwoInts_Request *)(untyped_value);
  *item = *value;
}

bool example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__resize_function__AddTwoInts_Event__request(
  void * untyped_member, size_t size)
{
  example_interfaces__srv__AddTwoInts_Request__Sequence * member =
    (example_interfaces__srv__AddTwoInts_Request__Sequence *)(untyped_member);
  example_interfaces__srv__AddTwoInts_Request__Sequence__fini(member);
  return example_interfaces__srv__AddTwoInts_Request__Sequence__init(member, size);
}

size_t example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__size_function__AddTwoInts_Event__response(
  const void * untyped_member)
{
  const example_interfaces__srv__AddTwoInts_Response__Sequence * member =
    (const example_interfaces__srv__AddTwoInts_Response__Sequence *)(untyped_member);
  return member->size;
}

const void * example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_const_function__AddTwoInts_Event__response(
  const void * untyped_member, size_t index)
{
  const example_interfaces__srv__AddTwoInts_Response__Sequence * member =
    (const example_interfaces__srv__AddTwoInts_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void * example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_function__AddTwoInts_Event__response(
  void * untyped_member, size_t index)
{
  example_interfaces__srv__AddTwoInts_Response__Sequence * member =
    (example_interfaces__srv__AddTwoInts_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__fetch_function__AddTwoInts_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const example_interfaces__srv__AddTwoInts_Response * item =
    ((const example_interfaces__srv__AddTwoInts_Response *)
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_const_function__AddTwoInts_Event__response(untyped_member, index));
  example_interfaces__srv__AddTwoInts_Response * value =
    (example_interfaces__srv__AddTwoInts_Response *)(untyped_value);
  *value = *item;
}

void example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__assign_function__AddTwoInts_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  example_interfaces__srv__AddTwoInts_Response * item =
    ((example_interfaces__srv__AddTwoInts_Response *)
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_function__AddTwoInts_Event__response(untyped_member, index));
  const example_interfaces__srv__AddTwoInts_Response * value =
    (const example_interfaces__srv__AddTwoInts_Response *)(untyped_value);
  *item = *value;
}

bool example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__resize_function__AddTwoInts_Event__response(
  void * untyped_member, size_t size)
{
  example_interfaces__srv__AddTwoInts_Response__Sequence * member =
    (example_interfaces__srv__AddTwoInts_Response__Sequence *)(untyped_member);
  example_interfaces__srv__AddTwoInts_Response__Sequence__fini(member);
  return example_interfaces__srv__AddTwoInts_Response__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_member_array[3] = {
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(example_interfaces__srv__AddTwoInts_Event, info),  // bytes offset in struct
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
    offsetof(example_interfaces__srv__AddTwoInts_Event, request),  // bytes offset in struct
    NULL,  // default value
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__size_function__AddTwoInts_Event__request,  // size() function pointer
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_const_function__AddTwoInts_Event__request,  // get_const(index) function pointer
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_function__AddTwoInts_Event__request,  // get(index) function pointer
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__fetch_function__AddTwoInts_Event__request,  // fetch(index, &value) function pointer
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__assign_function__AddTwoInts_Event__request,  // assign(index, value) function pointer
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__resize_function__AddTwoInts_Event__request  // resize(index) function pointer
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
    offsetof(example_interfaces__srv__AddTwoInts_Event, response),  // bytes offset in struct
    NULL,  // default value
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__size_function__AddTwoInts_Event__response,  // size() function pointer
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_const_function__AddTwoInts_Event__response,  // get_const(index) function pointer
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__get_function__AddTwoInts_Event__response,  // get(index) function pointer
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__fetch_function__AddTwoInts_Event__response,  // fetch(index, &value) function pointer
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__assign_function__AddTwoInts_Event__response,  // assign(index, value) function pointer
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__resize_function__AddTwoInts_Event__response  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_members = {
  "example_interfaces__srv",  // message namespace
  "AddTwoInts_Event",  // message name
  3,  // number of fields
  sizeof(example_interfaces__srv__AddTwoInts_Event),
  false,  // has_any_key_member_
  example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_member_array,  // message members
  example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_type_support_handle = {
  0,
  &example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_members,
  get_message_typesupport_handle_function,
  &example_interfaces__srv__AddTwoInts_Event__get_type_hash,
  &example_interfaces__srv__AddTwoInts_Event__get_type_description,
  &example_interfaces__srv__AddTwoInts_Event__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_example_interfaces
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts_Event)() {
  example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, service_msgs, msg, ServiceEventInfo)();
  example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts_Request)();
  example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts_Response)();
  if (!example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_type_support_handle.typesupport_identifier) {
    example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "example_interfaces/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "example_interfaces/srv/detail/add_two_ints__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers example_interfaces__srv__detail__add_two_ints__rosidl_typesupport_introspection_c__AddTwoInts_service_members = {
  "example_interfaces__srv",  // service namespace
  "AddTwoInts",  // service name
  // the following fields are initialized below on first access
  NULL,  // request message
  // example_interfaces__srv__detail__add_two_ints__rosidl_typesupport_introspection_c__AddTwoInts_Request_message_type_support_handle,
  NULL,  // response message
  // example_interfaces__srv__detail__add_two_ints__rosidl_typesupport_introspection_c__AddTwoInts_Response_message_type_support_handle
  NULL  // event_message
  // example_interfaces__srv__detail__add_two_ints__rosidl_typesupport_introspection_c__AddTwoInts_Response_message_type_support_handle
};


static rosidl_service_type_support_t example_interfaces__srv__detail__add_two_ints__rosidl_typesupport_introspection_c__AddTwoInts_service_type_support_handle = {
  0,
  &example_interfaces__srv__detail__add_two_ints__rosidl_typesupport_introspection_c__AddTwoInts_service_members,
  get_service_typesupport_handle_function,
  &example_interfaces__srv__AddTwoInts_Request__rosidl_typesupport_introspection_c__AddTwoInts_Request_message_type_support_handle,
  &example_interfaces__srv__AddTwoInts_Response__rosidl_typesupport_introspection_c__AddTwoInts_Response_message_type_support_handle,
  &example_interfaces__srv__AddTwoInts_Event__rosidl_typesupport_introspection_c__AddTwoInts_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    example_interfaces,
    srv,
    AddTwoInts
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    example_interfaces,
    srv,
    AddTwoInts
  ),
  &example_interfaces__srv__AddTwoInts__get_type_hash,
  &example_interfaces__srv__AddTwoInts__get_type_description,
  &example_interfaces__srv__AddTwoInts__get_type_description_sources,
};

// Forward declaration of message type support functions for service members
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts_Request)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts_Response)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts_Event)(void);

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_example_interfaces
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts)(void) {
  if (!example_interfaces__srv__detail__add_two_ints__rosidl_typesupport_introspection_c__AddTwoInts_service_type_support_handle.typesupport_identifier) {
    example_interfaces__srv__detail__add_two_ints__rosidl_typesupport_introspection_c__AddTwoInts_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)example_interfaces__srv__detail__add_two_ints__rosidl_typesupport_introspection_c__AddTwoInts_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts_Response)()->data;
  }
  if (!service_members->event_members_) {
    service_members->event_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, example_interfaces, srv, AddTwoInts_Event)()->data;
  }

  return &example_interfaces__srv__detail__add_two_ints__rosidl_typesupport_introspection_c__AddTwoInts_service_type_support_handle;
}
