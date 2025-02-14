// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from visualization_msgs:srv/GetInteractiveMarkers.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "visualization_msgs/srv/detail/get_interactive_markers__rosidl_typesupport_introspection_c.h"
#include "visualization_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "visualization_msgs/srv/detail/get_interactive_markers__functions.h"
#include "visualization_msgs/srv/detail/get_interactive_markers__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  visualization_msgs__srv__GetInteractiveMarkers_Request__init(message_memory);
}

void visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_fini_function(void * message_memory)
{
  visualization_msgs__srv__GetInteractiveMarkers_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_message_member_array[1] = {
  {
    "structure_needs_at_least_one_member",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__srv__GetInteractiveMarkers_Request, structure_needs_at_least_one_member),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_message_members = {
  "visualization_msgs__srv",  // message namespace
  "GetInteractiveMarkers_Request",  // message name
  1,  // number of fields
  sizeof(visualization_msgs__srv__GetInteractiveMarkers_Request),
  false,  // has_any_key_member_
  visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_message_member_array,  // message members
  visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_message_type_support_handle = {
  0,
  &visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_message_members,
  get_message_typesupport_handle_function,
  &visualization_msgs__srv__GetInteractiveMarkers_Request__get_type_hash,
  &visualization_msgs__srv__GetInteractiveMarkers_Request__get_type_description,
  &visualization_msgs__srv__GetInteractiveMarkers_Request__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_visualization_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers_Request)() {
  if (!visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_message_type_support_handle.typesupport_identifier) {
    visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "visualization_msgs/srv/detail/get_interactive_markers__rosidl_typesupport_introspection_c.h"
// already included above
// #include "visualization_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "visualization_msgs/srv/detail/get_interactive_markers__functions.h"
// already included above
// #include "visualization_msgs/srv/detail/get_interactive_markers__struct.h"


// Include directives for member types
// Member `markers`
#include "visualization_msgs/msg/interactive_marker.h"
// Member `markers`
#include "visualization_msgs/msg/detail/interactive_marker__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  visualization_msgs__srv__GetInteractiveMarkers_Response__init(message_memory);
}

void visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_fini_function(void * message_memory)
{
  visualization_msgs__srv__GetInteractiveMarkers_Response__fini(message_memory);
}

size_t visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__size_function__GetInteractiveMarkers_Response__markers(
  const void * untyped_member)
{
  const visualization_msgs__msg__InteractiveMarker__Sequence * member =
    (const visualization_msgs__msg__InteractiveMarker__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__get_const_function__GetInteractiveMarkers_Response__markers(
  const void * untyped_member, size_t index)
{
  const visualization_msgs__msg__InteractiveMarker__Sequence * member =
    (const visualization_msgs__msg__InteractiveMarker__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__get_function__GetInteractiveMarkers_Response__markers(
  void * untyped_member, size_t index)
{
  visualization_msgs__msg__InteractiveMarker__Sequence * member =
    (visualization_msgs__msg__InteractiveMarker__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__fetch_function__GetInteractiveMarkers_Response__markers(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const visualization_msgs__msg__InteractiveMarker * item =
    ((const visualization_msgs__msg__InteractiveMarker *)
    visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__get_const_function__GetInteractiveMarkers_Response__markers(untyped_member, index));
  visualization_msgs__msg__InteractiveMarker * value =
    (visualization_msgs__msg__InteractiveMarker *)(untyped_value);
  *value = *item;
}

void visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__assign_function__GetInteractiveMarkers_Response__markers(
  void * untyped_member, size_t index, const void * untyped_value)
{
  visualization_msgs__msg__InteractiveMarker * item =
    ((visualization_msgs__msg__InteractiveMarker *)
    visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__get_function__GetInteractiveMarkers_Response__markers(untyped_member, index));
  const visualization_msgs__msg__InteractiveMarker * value =
    (const visualization_msgs__msg__InteractiveMarker *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__resize_function__GetInteractiveMarkers_Response__markers(
  void * untyped_member, size_t size)
{
  visualization_msgs__msg__InteractiveMarker__Sequence * member =
    (visualization_msgs__msg__InteractiveMarker__Sequence *)(untyped_member);
  visualization_msgs__msg__InteractiveMarker__Sequence__fini(member);
  return visualization_msgs__msg__InteractiveMarker__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_member_array[2] = {
  {
    "sequence_number",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__srv__GetInteractiveMarkers_Response, sequence_number),  // bytes offset in struct
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
    offsetof(visualization_msgs__srv__GetInteractiveMarkers_Response, markers),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__size_function__GetInteractiveMarkers_Response__markers,  // size() function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__get_const_function__GetInteractiveMarkers_Response__markers,  // get_const(index) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__get_function__GetInteractiveMarkers_Response__markers,  // get(index) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__fetch_function__GetInteractiveMarkers_Response__markers,  // fetch(index, &value) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__assign_function__GetInteractiveMarkers_Response__markers,  // assign(index, value) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__resize_function__GetInteractiveMarkers_Response__markers  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_members = {
  "visualization_msgs__srv",  // message namespace
  "GetInteractiveMarkers_Response",  // message name
  2,  // number of fields
  sizeof(visualization_msgs__srv__GetInteractiveMarkers_Response),
  false,  // has_any_key_member_
  visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_member_array,  // message members
  visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_type_support_handle = {
  0,
  &visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_members,
  get_message_typesupport_handle_function,
  &visualization_msgs__srv__GetInteractiveMarkers_Response__get_type_hash,
  &visualization_msgs__srv__GetInteractiveMarkers_Response__get_type_description,
  &visualization_msgs__srv__GetInteractiveMarkers_Response__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_visualization_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers_Response)() {
  visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, msg, InteractiveMarker)();
  if (!visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_type_support_handle.typesupport_identifier) {
    visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "visualization_msgs/srv/detail/get_interactive_markers__rosidl_typesupport_introspection_c.h"
// already included above
// #include "visualization_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "visualization_msgs/srv/detail/get_interactive_markers__functions.h"
// already included above
// #include "visualization_msgs/srv/detail/get_interactive_markers__struct.h"


// Include directives for member types
// Member `info`
#include "service_msgs/msg/service_event_info.h"
// Member `info`
#include "service_msgs/msg/detail/service_event_info__rosidl_typesupport_introspection_c.h"
// Member `request`
// Member `response`
#include "visualization_msgs/srv/get_interactive_markers.h"
// Member `request`
// Member `response`
// already included above
// #include "visualization_msgs/srv/detail/get_interactive_markers__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  visualization_msgs__srv__GetInteractiveMarkers_Event__init(message_memory);
}

void visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_fini_function(void * message_memory)
{
  visualization_msgs__srv__GetInteractiveMarkers_Event__fini(message_memory);
}

size_t visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__size_function__GetInteractiveMarkers_Event__request(
  const void * untyped_member)
{
  const visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * member =
    (const visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_const_function__GetInteractiveMarkers_Event__request(
  const void * untyped_member, size_t index)
{
  const visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * member =
    (const visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_function__GetInteractiveMarkers_Event__request(
  void * untyped_member, size_t index)
{
  visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * member =
    (visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__fetch_function__GetInteractiveMarkers_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const visualization_msgs__srv__GetInteractiveMarkers_Request * item =
    ((const visualization_msgs__srv__GetInteractiveMarkers_Request *)
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_const_function__GetInteractiveMarkers_Event__request(untyped_member, index));
  visualization_msgs__srv__GetInteractiveMarkers_Request * value =
    (visualization_msgs__srv__GetInteractiveMarkers_Request *)(untyped_value);
  *value = *item;
}

void visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__assign_function__GetInteractiveMarkers_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  visualization_msgs__srv__GetInteractiveMarkers_Request * item =
    ((visualization_msgs__srv__GetInteractiveMarkers_Request *)
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_function__GetInteractiveMarkers_Event__request(untyped_member, index));
  const visualization_msgs__srv__GetInteractiveMarkers_Request * value =
    (const visualization_msgs__srv__GetInteractiveMarkers_Request *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__resize_function__GetInteractiveMarkers_Event__request(
  void * untyped_member, size_t size)
{
  visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence * member =
    (visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence *)(untyped_member);
  visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__fini(member);
  return visualization_msgs__srv__GetInteractiveMarkers_Request__Sequence__init(member, size);
}

size_t visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__size_function__GetInteractiveMarkers_Event__response(
  const void * untyped_member)
{
  const visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * member =
    (const visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence *)(untyped_member);
  return member->size;
}

const void * visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_const_function__GetInteractiveMarkers_Event__response(
  const void * untyped_member, size_t index)
{
  const visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * member =
    (const visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void * visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_function__GetInteractiveMarkers_Event__response(
  void * untyped_member, size_t index)
{
  visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * member =
    (visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__fetch_function__GetInteractiveMarkers_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const visualization_msgs__srv__GetInteractiveMarkers_Response * item =
    ((const visualization_msgs__srv__GetInteractiveMarkers_Response *)
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_const_function__GetInteractiveMarkers_Event__response(untyped_member, index));
  visualization_msgs__srv__GetInteractiveMarkers_Response * value =
    (visualization_msgs__srv__GetInteractiveMarkers_Response *)(untyped_value);
  *value = *item;
}

void visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__assign_function__GetInteractiveMarkers_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  visualization_msgs__srv__GetInteractiveMarkers_Response * item =
    ((visualization_msgs__srv__GetInteractiveMarkers_Response *)
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_function__GetInteractiveMarkers_Event__response(untyped_member, index));
  const visualization_msgs__srv__GetInteractiveMarkers_Response * value =
    (const visualization_msgs__srv__GetInteractiveMarkers_Response *)(untyped_value);
  *item = *value;
}

bool visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__resize_function__GetInteractiveMarkers_Event__response(
  void * untyped_member, size_t size)
{
  visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence * member =
    (visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence *)(untyped_member);
  visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__fini(member);
  return visualization_msgs__srv__GetInteractiveMarkers_Response__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_member_array[3] = {
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(visualization_msgs__srv__GetInteractiveMarkers_Event, info),  // bytes offset in struct
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
    offsetof(visualization_msgs__srv__GetInteractiveMarkers_Event, request),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__size_function__GetInteractiveMarkers_Event__request,  // size() function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_const_function__GetInteractiveMarkers_Event__request,  // get_const(index) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_function__GetInteractiveMarkers_Event__request,  // get(index) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__fetch_function__GetInteractiveMarkers_Event__request,  // fetch(index, &value) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__assign_function__GetInteractiveMarkers_Event__request,  // assign(index, value) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__resize_function__GetInteractiveMarkers_Event__request  // resize(index) function pointer
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
    offsetof(visualization_msgs__srv__GetInteractiveMarkers_Event, response),  // bytes offset in struct
    NULL,  // default value
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__size_function__GetInteractiveMarkers_Event__response,  // size() function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_const_function__GetInteractiveMarkers_Event__response,  // get_const(index) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__get_function__GetInteractiveMarkers_Event__response,  // get(index) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__fetch_function__GetInteractiveMarkers_Event__response,  // fetch(index, &value) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__assign_function__GetInteractiveMarkers_Event__response,  // assign(index, value) function pointer
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__resize_function__GetInteractiveMarkers_Event__response  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_members = {
  "visualization_msgs__srv",  // message namespace
  "GetInteractiveMarkers_Event",  // message name
  3,  // number of fields
  sizeof(visualization_msgs__srv__GetInteractiveMarkers_Event),
  false,  // has_any_key_member_
  visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_member_array,  // message members
  visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_type_support_handle = {
  0,
  &visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_members,
  get_message_typesupport_handle_function,
  &visualization_msgs__srv__GetInteractiveMarkers_Event__get_type_hash,
  &visualization_msgs__srv__GetInteractiveMarkers_Event__get_type_description,
  &visualization_msgs__srv__GetInteractiveMarkers_Event__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_visualization_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers_Event)() {
  visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, service_msgs, msg, ServiceEventInfo)();
  visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers_Request)();
  visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers_Response)();
  if (!visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_type_support_handle.typesupport_identifier) {
    visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "visualization_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "visualization_msgs/srv/detail/get_interactive_markers__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers visualization_msgs__srv__detail__get_interactive_markers__rosidl_typesupport_introspection_c__GetInteractiveMarkers_service_members = {
  "visualization_msgs__srv",  // service namespace
  "GetInteractiveMarkers",  // service name
  // the following fields are initialized below on first access
  NULL,  // request message
  // visualization_msgs__srv__detail__get_interactive_markers__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_message_type_support_handle,
  NULL,  // response message
  // visualization_msgs__srv__detail__get_interactive_markers__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_type_support_handle
  NULL  // event_message
  // visualization_msgs__srv__detail__get_interactive_markers__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_type_support_handle
};


static rosidl_service_type_support_t visualization_msgs__srv__detail__get_interactive_markers__rosidl_typesupport_introspection_c__GetInteractiveMarkers_service_type_support_handle = {
  0,
  &visualization_msgs__srv__detail__get_interactive_markers__rosidl_typesupport_introspection_c__GetInteractiveMarkers_service_members,
  get_service_typesupport_handle_function,
  &visualization_msgs__srv__GetInteractiveMarkers_Request__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Request_message_type_support_handle,
  &visualization_msgs__srv__GetInteractiveMarkers_Response__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Response_message_type_support_handle,
  &visualization_msgs__srv__GetInteractiveMarkers_Event__rosidl_typesupport_introspection_c__GetInteractiveMarkers_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    visualization_msgs,
    srv,
    GetInteractiveMarkers
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    visualization_msgs,
    srv,
    GetInteractiveMarkers
  ),
  &visualization_msgs__srv__GetInteractiveMarkers__get_type_hash,
  &visualization_msgs__srv__GetInteractiveMarkers__get_type_description,
  &visualization_msgs__srv__GetInteractiveMarkers__get_type_description_sources,
};

// Forward declaration of message type support functions for service members
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers_Request)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers_Response)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers_Event)(void);

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_visualization_msgs
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers)(void) {
  if (!visualization_msgs__srv__detail__get_interactive_markers__rosidl_typesupport_introspection_c__GetInteractiveMarkers_service_type_support_handle.typesupport_identifier) {
    visualization_msgs__srv__detail__get_interactive_markers__rosidl_typesupport_introspection_c__GetInteractiveMarkers_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)visualization_msgs__srv__detail__get_interactive_markers__rosidl_typesupport_introspection_c__GetInteractiveMarkers_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers_Response)()->data;
  }
  if (!service_members->event_members_) {
    service_members->event_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, visualization_msgs, srv, GetInteractiveMarkers_Event)()->data;
  }

  return &visualization_msgs__srv__detail__get_interactive_markers__rosidl_typesupport_introspection_c__GetInteractiveMarkers_service_type_support_handle;
}
