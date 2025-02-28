// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from trajectory_msgs:msg/MultiDOFJointTrajectoryPoint.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "trajectory_msgs/msg/multi_dof_joint_trajectory_point.h"


#ifndef TRAJECTORY_MSGS__MSG__DETAIL__MULTI_DOF_JOINT_TRAJECTORY_POINT__FUNCTIONS_H_
#define TRAJECTORY_MSGS__MSG__DETAIL__MULTI_DOF_JOINT_TRAJECTORY_POINT__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/action_type_support_struct.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_runtime_c/type_description/type_description__struct.h"
#include "rosidl_runtime_c/type_description/type_source__struct.h"
#include "rosidl_runtime_c/type_hash.h"
#include "rosidl_runtime_c/visibility_control.h"
#include "trajectory_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "trajectory_msgs/msg/detail/multi_dof_joint_trajectory_point__struct.h"

/// Initialize msg/MultiDOFJointTrajectoryPoint message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * trajectory_msgs__msg__MultiDOFJointTrajectoryPoint
 * )) before or use
 * trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
bool
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__init(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * msg);

/// Finalize msg/MultiDOFJointTrajectoryPoint message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
void
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * msg);

/// Create msg/MultiDOFJointTrajectoryPoint message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint *
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__create(void);

/// Destroy msg/MultiDOFJointTrajectoryPoint message.
/**
 * It calls
 * trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
void
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__destroy(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * msg);

/// Check for msg/MultiDOFJointTrajectoryPoint message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
bool
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__are_equal(const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * lhs, const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * rhs);

/// Copy a msg/MultiDOFJointTrajectoryPoint message.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source message pointer.
 * \param[out] output The target message pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer is null
 *   or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
bool
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__copy(
  const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * input,
  trajectory_msgs__msg__MultiDOFJointTrajectoryPoint * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
const rosidl_type_hash_t *
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
const rosidl_runtime_c__type_description__TypeDescription *
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
const rosidl_runtime_c__type_description__TypeSource *
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of msg/MultiDOFJointTrajectoryPoint messages.
/**
 * It allocates the memory for the number of elements and calls
 * trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
bool
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__init(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * array, size_t size);

/// Finalize array of msg/MultiDOFJointTrajectoryPoint messages.
/**
 * It calls
 * trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
void
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__fini(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * array);

/// Create array of msg/MultiDOFJointTrajectoryPoint messages.
/**
 * It allocates the memory for the array and calls
 * trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence *
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__create(size_t size);

/// Destroy array of msg/MultiDOFJointTrajectoryPoint messages.
/**
 * It calls
 * trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
void
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__destroy(trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * array);

/// Check for msg/MultiDOFJointTrajectoryPoint message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
bool
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__are_equal(const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * lhs, const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * rhs);

/// Copy an array of msg/MultiDOFJointTrajectoryPoint messages.
/**
 * This functions performs a deep copy, as opposed to the shallow copy that
 * plain assignment yields.
 *
 * \param[in] input The source array pointer.
 * \param[out] output The target array pointer, which must
 *   have been initialized before calling this function.
 * \return true if successful, or false if either pointer
 *   is null or memory allocation fails.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
bool
trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence__copy(
  const trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * input,
  trajectory_msgs__msg__MultiDOFJointTrajectoryPoint__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // TRAJECTORY_MSGS__MSG__DETAIL__MULTI_DOF_JOINT_TRAJECTORY_POINT__FUNCTIONS_H_
