// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from nav_msgs:srv/GetPlan.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "nav_msgs/srv/detail/get_plan__rosidl_typesupport_introspection_c.h"
#include "nav_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "nav_msgs/srv/detail/get_plan__functions.h"
#include "nav_msgs/srv/detail/get_plan__struct.h"


// Include directives for member types
// Member `start`
// Member `goal`
#include "geometry_msgs/msg/pose_stamped.h"
// Member `start`
// Member `goal`
#include "geometry_msgs/msg/detail/pose_stamped__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  nav_msgs__srv__GetPlan_Request__init(message_memory);
}

void nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_fini_function(void * message_memory)
{
  nav_msgs__srv__GetPlan_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_member_array[3] = {
  {
    "start",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__srv__GetPlan_Request, start),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "goal",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__srv__GetPlan_Request, goal),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "tolerance",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__srv__GetPlan_Request, tolerance),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_members = {
  "nav_msgs__srv",  // message namespace
  "GetPlan_Request",  // message name
  3,  // number of fields
  sizeof(nav_msgs__srv__GetPlan_Request),
  false,  // has_any_key_member_
  nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_member_array,  // message members
  nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_type_support_handle = {
  0,
  &nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_members,
  get_message_typesupport_handle_function,
  &nav_msgs__srv__GetPlan_Request__get_type_hash,
  &nav_msgs__srv__GetPlan_Request__get_type_description,
  &nav_msgs__srv__GetPlan_Request__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_nav_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Request)() {
  nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, PoseStamped)();
  nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, PoseStamped)();
  if (!nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_type_support_handle.typesupport_identifier) {
    nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "nav_msgs/srv/detail/get_plan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "nav_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "nav_msgs/srv/detail/get_plan__functions.h"
// already included above
// #include "nav_msgs/srv/detail/get_plan__struct.h"


// Include directives for member types
// Member `plan`
#include "nav_msgs/msg/path.h"
// Member `plan`
#include "nav_msgs/msg/detail/path__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  nav_msgs__srv__GetPlan_Response__init(message_memory);
}

void nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_fini_function(void * message_memory)
{
  nav_msgs__srv__GetPlan_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_member_array[1] = {
  {
    "plan",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__srv__GetPlan_Response, plan),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_members = {
  "nav_msgs__srv",  // message namespace
  "GetPlan_Response",  // message name
  1,  // number of fields
  sizeof(nav_msgs__srv__GetPlan_Response),
  false,  // has_any_key_member_
  nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_member_array,  // message members
  nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle = {
  0,
  &nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_members,
  get_message_typesupport_handle_function,
  &nav_msgs__srv__GetPlan_Response__get_type_hash,
  &nav_msgs__srv__GetPlan_Response__get_type_description,
  &nav_msgs__srv__GetPlan_Response__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_nav_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Response)() {
  nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, msg, Path)();
  if (!nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle.typesupport_identifier) {
    nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "nav_msgs/srv/detail/get_plan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "nav_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "nav_msgs/srv/detail/get_plan__functions.h"
// already included above
// #include "nav_msgs/srv/detail/get_plan__struct.h"


// Include directives for member types
// Member `info`
#include "service_msgs/msg/service_event_info.h"
// Member `info`
#include "service_msgs/msg/detail/service_event_info__rosidl_typesupport_introspection_c.h"
// Member `request`
// Member `response`
#include "nav_msgs/srv/get_plan.h"
// Member `request`
// Member `response`
// already included above
// #include "nav_msgs/srv/detail/get_plan__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  nav_msgs__srv__GetPlan_Event__init(message_memory);
}

void nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_fini_function(void * message_memory)
{
  nav_msgs__srv__GetPlan_Event__fini(message_memory);
}

size_t nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__size_function__GetPlan_Event__request(
  const void * untyped_member)
{
  const nav_msgs__srv__GetPlan_Request__Sequence * member =
    (const nav_msgs__srv__GetPlan_Request__Sequence *)(untyped_member);
  return member->size;
}

const void * nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_const_function__GetPlan_Event__request(
  const void * untyped_member, size_t index)
{
  const nav_msgs__srv__GetPlan_Request__Sequence * member =
    (const nav_msgs__srv__GetPlan_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void * nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_function__GetPlan_Event__request(
  void * untyped_member, size_t index)
{
  nav_msgs__srv__GetPlan_Request__Sequence * member =
    (nav_msgs__srv__GetPlan_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__fetch_function__GetPlan_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const nav_msgs__srv__GetPlan_Request * item =
    ((const nav_msgs__srv__GetPlan_Request *)
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_const_function__GetPlan_Event__request(untyped_member, index));
  nav_msgs__srv__GetPlan_Request * value =
    (nav_msgs__srv__GetPlan_Request *)(untyped_value);
  *value = *item;
}

void nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__assign_function__GetPlan_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  nav_msgs__srv__GetPlan_Request * item =
    ((nav_msgs__srv__GetPlan_Request *)
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_function__GetPlan_Event__request(untyped_member, index));
  const nav_msgs__srv__GetPlan_Request * value =
    (const nav_msgs__srv__GetPlan_Request *)(untyped_value);
  *item = *value;
}

bool nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__resize_function__GetPlan_Event__request(
  void * untyped_member, size_t size)
{
  nav_msgs__srv__GetPlan_Request__Sequence * member =
    (nav_msgs__srv__GetPlan_Request__Sequence *)(untyped_member);
  nav_msgs__srv__GetPlan_Request__Sequence__fini(member);
  return nav_msgs__srv__GetPlan_Request__Sequence__init(member, size);
}

size_t nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__size_function__GetPlan_Event__response(
  const void * untyped_member)
{
  const nav_msgs__srv__GetPlan_Response__Sequence * member =
    (const nav_msgs__srv__GetPlan_Response__Sequence *)(untyped_member);
  return member->size;
}

const void * nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_const_function__GetPlan_Event__response(
  const void * untyped_member, size_t index)
{
  const nav_msgs__srv__GetPlan_Response__Sequence * member =
    (const nav_msgs__srv__GetPlan_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void * nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_function__GetPlan_Event__response(
  void * untyped_member, size_t index)
{
  nav_msgs__srv__GetPlan_Response__Sequence * member =
    (nav_msgs__srv__GetPlan_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__fetch_function__GetPlan_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const nav_msgs__srv__GetPlan_Response * item =
    ((const nav_msgs__srv__GetPlan_Response *)
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_const_function__GetPlan_Event__response(untyped_member, index));
  nav_msgs__srv__GetPlan_Response * value =
    (nav_msgs__srv__GetPlan_Response *)(untyped_value);
  *value = *item;
}

void nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__assign_function__GetPlan_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  nav_msgs__srv__GetPlan_Response * item =
    ((nav_msgs__srv__GetPlan_Response *)
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_function__GetPlan_Event__response(untyped_member, index));
  const nav_msgs__srv__GetPlan_Response * value =
    (const nav_msgs__srv__GetPlan_Response *)(untyped_value);
  *item = *value;
}

bool nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__resize_function__GetPlan_Event__response(
  void * untyped_member, size_t size)
{
  nav_msgs__srv__GetPlan_Response__Sequence * member =
    (nav_msgs__srv__GetPlan_Response__Sequence *)(untyped_member);
  nav_msgs__srv__GetPlan_Response__Sequence__fini(member);
  return nav_msgs__srv__GetPlan_Response__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_member_array[3] = {
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__srv__GetPlan_Event, info),  // bytes offset in struct
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
    offsetof(nav_msgs__srv__GetPlan_Event, request),  // bytes offset in struct
    NULL,  // default value
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__size_function__GetPlan_Event__request,  // size() function pointer
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_const_function__GetPlan_Event__request,  // get_const(index) function pointer
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_function__GetPlan_Event__request,  // get(index) function pointer
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__fetch_function__GetPlan_Event__request,  // fetch(index, &value) function pointer
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__assign_function__GetPlan_Event__request,  // assign(index, value) function pointer
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__resize_function__GetPlan_Event__request  // resize(index) function pointer
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
    offsetof(nav_msgs__srv__GetPlan_Event, response),  // bytes offset in struct
    NULL,  // default value
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__size_function__GetPlan_Event__response,  // size() function pointer
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_const_function__GetPlan_Event__response,  // get_const(index) function pointer
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__get_function__GetPlan_Event__response,  // get(index) function pointer
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__fetch_function__GetPlan_Event__response,  // fetch(index, &value) function pointer
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__assign_function__GetPlan_Event__response,  // assign(index, value) function pointer
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__resize_function__GetPlan_Event__response  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_members = {
  "nav_msgs__srv",  // message namespace
  "GetPlan_Event",  // message name
  3,  // number of fields
  sizeof(nav_msgs__srv__GetPlan_Event),
  false,  // has_any_key_member_
  nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_member_array,  // message members
  nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_type_support_handle = {
  0,
  &nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_members,
  get_message_typesupport_handle_function,
  &nav_msgs__srv__GetPlan_Event__get_type_hash,
  &nav_msgs__srv__GetPlan_Event__get_type_description,
  &nav_msgs__srv__GetPlan_Event__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_nav_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Event)() {
  nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, service_msgs, msg, ServiceEventInfo)();
  nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Request)();
  nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Response)();
  if (!nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_type_support_handle.typesupport_identifier) {
    nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "nav_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "nav_msgs/srv/detail/get_plan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_members = {
  "nav_msgs__srv",  // service namespace
  "GetPlan",  // service name
  // the following fields are initialized below on first access
  NULL,  // request message
  // nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_Request_message_type_support_handle,
  NULL,  // response message
  // nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle
  NULL  // event_message
  // nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle
};


static rosidl_service_type_support_t nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_type_support_handle = {
  0,
  &nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_members,
  get_service_typesupport_handle_function,
  &nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_type_support_handle,
  &nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle,
  &nav_msgs__srv__GetPlan_Event__rosidl_typesupport_introspection_c__GetPlan_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    nav_msgs,
    srv,
    GetPlan
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    nav_msgs,
    srv,
    GetPlan
  ),
  &nav_msgs__srv__GetPlan__get_type_hash,
  &nav_msgs__srv__GetPlan__get_type_description,
  &nav_msgs__srv__GetPlan__get_type_description_sources,
};

// Forward declaration of message type support functions for service members
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Request)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Response)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Event)(void);

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_nav_msgs
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan)(void) {
  if (!nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_type_support_handle.typesupport_identifier) {
    nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Response)()->data;
  }
  if (!service_members->event_members_) {
    service_members->event_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Event)()->data;
  }

  return &nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_type_support_handle;
}
