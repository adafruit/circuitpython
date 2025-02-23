// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from sensor_msgs:srv/SetCameraInfo.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "sensor_msgs/srv/set_camera_info.h"


#ifndef SENSOR_MSGS__SRV__DETAIL__SET_CAMERA_INFO__STRUCT_H_
#define SENSOR_MSGS__SRV__DETAIL__SET_CAMERA_INFO__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'camera_info'
#include "sensor_msgs/msg/detail/camera_info__struct.h"

/// Struct defined in srv/SetCameraInfo in the package sensor_msgs.
typedef struct sensor_msgs__srv__SetCameraInfo_Request
{
  /// The camera_info to store
  sensor_msgs__msg__CameraInfo camera_info;
} sensor_msgs__srv__SetCameraInfo_Request;

// Struct for a sequence of sensor_msgs__srv__SetCameraInfo_Request.
typedef struct sensor_msgs__srv__SetCameraInfo_Request__Sequence
{
  sensor_msgs__srv__SetCameraInfo_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__srv__SetCameraInfo_Request__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'status_message'
#include "rosidl_runtime_c/string.h"

/// Struct defined in srv/SetCameraInfo in the package sensor_msgs.
typedef struct sensor_msgs__srv__SetCameraInfo_Response
{
  /// True if the call succeeded
  bool success;
  /// Used to give details about success
  rosidl_runtime_c__String status_message;
} sensor_msgs__srv__SetCameraInfo_Response;

// Struct for a sequence of sensor_msgs__srv__SetCameraInfo_Response.
typedef struct sensor_msgs__srv__SetCameraInfo_Response__Sequence
{
  sensor_msgs__srv__SetCameraInfo_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__srv__SetCameraInfo_Response__Sequence;

// Constants defined in the message

// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.h"

// constants for array fields with an upper bound
// request
enum
{
  sensor_msgs__srv__SetCameraInfo_Event__request__MAX_SIZE = 1
};
// response
enum
{
  sensor_msgs__srv__SetCameraInfo_Event__response__MAX_SIZE = 1
};

/// Struct defined in srv/SetCameraInfo in the package sensor_msgs.
typedef struct sensor_msgs__srv__SetCameraInfo_Event
{
  service_msgs__msg__ServiceEventInfo info;
  sensor_msgs__srv__SetCameraInfo_Request__Sequence request;
  sensor_msgs__srv__SetCameraInfo_Response__Sequence response;
} sensor_msgs__srv__SetCameraInfo_Event;

// Struct for a sequence of sensor_msgs__srv__SetCameraInfo_Event.
typedef struct sensor_msgs__srv__SetCameraInfo_Event__Sequence
{
  sensor_msgs__srv__SetCameraInfo_Event * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} sensor_msgs__srv__SetCameraInfo_Event__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // SENSOR_MSGS__SRV__DETAIL__SET_CAMERA_INFO__STRUCT_H_
