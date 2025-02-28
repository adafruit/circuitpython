// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from test_msgs:srv/BasicTypes.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "test_msgs/srv/detail/basic_types__rosidl_typesupport_introspection_c.h"
#include "test_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "test_msgs/srv/detail/basic_types__functions.h"
#include "test_msgs/srv/detail/basic_types__struct.h"


// Include directives for member types
// Member `string_value`
#include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  test_msgs__srv__BasicTypes_Request__init(message_memory);
}

void test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_fini_function(void * message_memory)
{
  test_msgs__srv__BasicTypes_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_message_member_array[14] = {
  {
    "bool_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, bool_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "byte_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_OCTET,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, byte_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "char_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, char_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "float32_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, float32_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "float64_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, float64_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int8_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, int8_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint8_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, uint8_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int16_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, int16_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint16_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, uint16_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int32_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, int32_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint32_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, uint32_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int64_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, int64_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint64_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, uint64_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "string_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Request, string_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_message_members = {
  "test_msgs__srv",  // message namespace
  "BasicTypes_Request",  // message name
  14,  // number of fields
  sizeof(test_msgs__srv__BasicTypes_Request),
  false,  // has_any_key_member_
  test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_message_member_array,  // message members
  test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_message_type_support_handle = {
  0,
  &test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_message_members,
  get_message_typesupport_handle_function,
  &test_msgs__srv__BasicTypes_Request__get_type_hash,
  &test_msgs__srv__BasicTypes_Request__get_type_description,
  &test_msgs__srv__BasicTypes_Request__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_test_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes_Request)() {
  if (!test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_message_type_support_handle.typesupport_identifier) {
    test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "test_msgs/srv/detail/basic_types__rosidl_typesupport_introspection_c.h"
// already included above
// #include "test_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "test_msgs/srv/detail/basic_types__functions.h"
// already included above
// #include "test_msgs/srv/detail/basic_types__struct.h"


// Include directives for member types
// Member `string_value`
// already included above
// #include "rosidl_runtime_c/string_functions.h"

#ifdef __cplusplus
extern "C"
{
#endif

void test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  test_msgs__srv__BasicTypes_Response__init(message_memory);
}

void test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_fini_function(void * message_memory)
{
  test_msgs__srv__BasicTypes_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_message_member_array[14] = {
  {
    "bool_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_BOOLEAN,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, bool_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "byte_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_OCTET,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, byte_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "char_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, char_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "float32_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, float32_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "float64_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_DOUBLE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, float64_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int8_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, int8_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint8_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, uint8_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int16_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, int16_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint16_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT16,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, uint16_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int32_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, int32_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint32_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT32,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, uint32_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "int64_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, int64_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "uint64_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT64,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, uint64_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "string_value",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Response, string_value),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_message_members = {
  "test_msgs__srv",  // message namespace
  "BasicTypes_Response",  // message name
  14,  // number of fields
  sizeof(test_msgs__srv__BasicTypes_Response),
  false,  // has_any_key_member_
  test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_message_member_array,  // message members
  test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_message_type_support_handle = {
  0,
  &test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_message_members,
  get_message_typesupport_handle_function,
  &test_msgs__srv__BasicTypes_Response__get_type_hash,
  &test_msgs__srv__BasicTypes_Response__get_type_description,
  &test_msgs__srv__BasicTypes_Response__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_test_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes_Response)() {
  if (!test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_message_type_support_handle.typesupport_identifier) {
    test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "test_msgs/srv/detail/basic_types__rosidl_typesupport_introspection_c.h"
// already included above
// #include "test_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "test_msgs/srv/detail/basic_types__functions.h"
// already included above
// #include "test_msgs/srv/detail/basic_types__struct.h"


// Include directives for member types
// Member `info`
#include "service_msgs/msg/service_event_info.h"
// Member `info`
#include "service_msgs/msg/detail/service_event_info__rosidl_typesupport_introspection_c.h"
// Member `request`
// Member `response`
#include "test_msgs/srv/basic_types.h"
// Member `request`
// Member `response`
// already included above
// #include "test_msgs/srv/detail/basic_types__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  test_msgs__srv__BasicTypes_Event__init(message_memory);
}

void test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_fini_function(void * message_memory)
{
  test_msgs__srv__BasicTypes_Event__fini(message_memory);
}

size_t test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__size_function__BasicTypes_Event__request(
  const void * untyped_member)
{
  const test_msgs__srv__BasicTypes_Request__Sequence * member =
    (const test_msgs__srv__BasicTypes_Request__Sequence *)(untyped_member);
  return member->size;
}

const void * test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_const_function__BasicTypes_Event__request(
  const void * untyped_member, size_t index)
{
  const test_msgs__srv__BasicTypes_Request__Sequence * member =
    (const test_msgs__srv__BasicTypes_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void * test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_function__BasicTypes_Event__request(
  void * untyped_member, size_t index)
{
  test_msgs__srv__BasicTypes_Request__Sequence * member =
    (test_msgs__srv__BasicTypes_Request__Sequence *)(untyped_member);
  return &member->data[index];
}

void test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__fetch_function__BasicTypes_Event__request(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const test_msgs__srv__BasicTypes_Request * item =
    ((const test_msgs__srv__BasicTypes_Request *)
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_const_function__BasicTypes_Event__request(untyped_member, index));
  test_msgs__srv__BasicTypes_Request * value =
    (test_msgs__srv__BasicTypes_Request *)(untyped_value);
  *value = *item;
}

void test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__assign_function__BasicTypes_Event__request(
  void * untyped_member, size_t index, const void * untyped_value)
{
  test_msgs__srv__BasicTypes_Request * item =
    ((test_msgs__srv__BasicTypes_Request *)
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_function__BasicTypes_Event__request(untyped_member, index));
  const test_msgs__srv__BasicTypes_Request * value =
    (const test_msgs__srv__BasicTypes_Request *)(untyped_value);
  *item = *value;
}

bool test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__resize_function__BasicTypes_Event__request(
  void * untyped_member, size_t size)
{
  test_msgs__srv__BasicTypes_Request__Sequence * member =
    (test_msgs__srv__BasicTypes_Request__Sequence *)(untyped_member);
  test_msgs__srv__BasicTypes_Request__Sequence__fini(member);
  return test_msgs__srv__BasicTypes_Request__Sequence__init(member, size);
}

size_t test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__size_function__BasicTypes_Event__response(
  const void * untyped_member)
{
  const test_msgs__srv__BasicTypes_Response__Sequence * member =
    (const test_msgs__srv__BasicTypes_Response__Sequence *)(untyped_member);
  return member->size;
}

const void * test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_const_function__BasicTypes_Event__response(
  const void * untyped_member, size_t index)
{
  const test_msgs__srv__BasicTypes_Response__Sequence * member =
    (const test_msgs__srv__BasicTypes_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void * test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_function__BasicTypes_Event__response(
  void * untyped_member, size_t index)
{
  test_msgs__srv__BasicTypes_Response__Sequence * member =
    (test_msgs__srv__BasicTypes_Response__Sequence *)(untyped_member);
  return &member->data[index];
}

void test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__fetch_function__BasicTypes_Event__response(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const test_msgs__srv__BasicTypes_Response * item =
    ((const test_msgs__srv__BasicTypes_Response *)
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_const_function__BasicTypes_Event__response(untyped_member, index));
  test_msgs__srv__BasicTypes_Response * value =
    (test_msgs__srv__BasicTypes_Response *)(untyped_value);
  *value = *item;
}

void test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__assign_function__BasicTypes_Event__response(
  void * untyped_member, size_t index, const void * untyped_value)
{
  test_msgs__srv__BasicTypes_Response * item =
    ((test_msgs__srv__BasicTypes_Response *)
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_function__BasicTypes_Event__response(untyped_member, index));
  const test_msgs__srv__BasicTypes_Response * value =
    (const test_msgs__srv__BasicTypes_Response *)(untyped_value);
  *item = *value;
}

bool test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__resize_function__BasicTypes_Event__response(
  void * untyped_member, size_t size)
{
  test_msgs__srv__BasicTypes_Response__Sequence * member =
    (test_msgs__srv__BasicTypes_Response__Sequence *)(untyped_member);
  test_msgs__srv__BasicTypes_Response__Sequence__fini(member);
  return test_msgs__srv__BasicTypes_Response__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_member_array[3] = {
  {
    "info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is key
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(test_msgs__srv__BasicTypes_Event, info),  // bytes offset in struct
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
    offsetof(test_msgs__srv__BasicTypes_Event, request),  // bytes offset in struct
    NULL,  // default value
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__size_function__BasicTypes_Event__request,  // size() function pointer
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_const_function__BasicTypes_Event__request,  // get_const(index) function pointer
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_function__BasicTypes_Event__request,  // get(index) function pointer
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__fetch_function__BasicTypes_Event__request,  // fetch(index, &value) function pointer
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__assign_function__BasicTypes_Event__request,  // assign(index, value) function pointer
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__resize_function__BasicTypes_Event__request  // resize(index) function pointer
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
    offsetof(test_msgs__srv__BasicTypes_Event, response),  // bytes offset in struct
    NULL,  // default value
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__size_function__BasicTypes_Event__response,  // size() function pointer
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_const_function__BasicTypes_Event__response,  // get_const(index) function pointer
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__get_function__BasicTypes_Event__response,  // get(index) function pointer
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__fetch_function__BasicTypes_Event__response,  // fetch(index, &value) function pointer
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__assign_function__BasicTypes_Event__response,  // assign(index, value) function pointer
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__resize_function__BasicTypes_Event__response  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_members = {
  "test_msgs__srv",  // message namespace
  "BasicTypes_Event",  // message name
  3,  // number of fields
  sizeof(test_msgs__srv__BasicTypes_Event),
  false,  // has_any_key_member_
  test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_member_array,  // message members
  test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_init_function,  // function to initialize message memory (memory has to be allocated)
  test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_type_support_handle = {
  0,
  &test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_members,
  get_message_typesupport_handle_function,
  &test_msgs__srv__BasicTypes_Event__get_type_hash,
  &test_msgs__srv__BasicTypes_Event__get_type_description,
  &test_msgs__srv__BasicTypes_Event__get_type_description_sources,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_test_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes_Event)() {
  test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, service_msgs, msg, ServiceEventInfo)();
  test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes_Request)();
  test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes_Response)();
  if (!test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_type_support_handle.typesupport_identifier) {
    test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "test_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "test_msgs/srv/detail/basic_types__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers test_msgs__srv__detail__basic_types__rosidl_typesupport_introspection_c__BasicTypes_service_members = {
  "test_msgs__srv",  // service namespace
  "BasicTypes",  // service name
  // the following fields are initialized below on first access
  NULL,  // request message
  // test_msgs__srv__detail__basic_types__rosidl_typesupport_introspection_c__BasicTypes_Request_message_type_support_handle,
  NULL,  // response message
  // test_msgs__srv__detail__basic_types__rosidl_typesupport_introspection_c__BasicTypes_Response_message_type_support_handle
  NULL  // event_message
  // test_msgs__srv__detail__basic_types__rosidl_typesupport_introspection_c__BasicTypes_Response_message_type_support_handle
};


static rosidl_service_type_support_t test_msgs__srv__detail__basic_types__rosidl_typesupport_introspection_c__BasicTypes_service_type_support_handle = {
  0,
  &test_msgs__srv__detail__basic_types__rosidl_typesupport_introspection_c__BasicTypes_service_members,
  get_service_typesupport_handle_function,
  &test_msgs__srv__BasicTypes_Request__rosidl_typesupport_introspection_c__BasicTypes_Request_message_type_support_handle,
  &test_msgs__srv__BasicTypes_Response__rosidl_typesupport_introspection_c__BasicTypes_Response_message_type_support_handle,
  &test_msgs__srv__BasicTypes_Event__rosidl_typesupport_introspection_c__BasicTypes_Event_message_type_support_handle,
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_CREATE_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    test_msgs,
    srv,
    BasicTypes
  ),
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_DESTROY_EVENT_MESSAGE_SYMBOL_NAME(
    rosidl_typesupport_c,
    test_msgs,
    srv,
    BasicTypes
  ),
  &test_msgs__srv__BasicTypes__get_type_hash,
  &test_msgs__srv__BasicTypes__get_type_description,
  &test_msgs__srv__BasicTypes__get_type_description_sources,
};

// Forward declaration of message type support functions for service members
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes_Request)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes_Response)(void);

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes_Event)(void);

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_test_msgs
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes)(void) {
  if (!test_msgs__srv__detail__basic_types__rosidl_typesupport_introspection_c__BasicTypes_service_type_support_handle.typesupport_identifier) {
    test_msgs__srv__detail__basic_types__rosidl_typesupport_introspection_c__BasicTypes_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)test_msgs__srv__detail__basic_types__rosidl_typesupport_introspection_c__BasicTypes_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes_Response)()->data;
  }
  if (!service_members->event_members_) {
    service_members->event_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, test_msgs, srv, BasicTypes_Event)()->data;
  }

  return &test_msgs__srv__detail__basic_types__rosidl_typesupport_introspection_c__BasicTypes_service_type_support_handle;
}
