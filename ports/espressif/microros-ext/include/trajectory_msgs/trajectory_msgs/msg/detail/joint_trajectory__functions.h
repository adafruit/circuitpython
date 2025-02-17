// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from trajectory_msgs:msg/JointTrajectory.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "trajectory_msgs/msg/joint_trajectory.h"


#ifndef TRAJECTORY_MSGS__MSG__DETAIL__JOINT_TRAJECTORY__FUNCTIONS_H_
#define TRAJECTORY_MSGS__MSG__DETAIL__JOINT_TRAJECTORY__FUNCTIONS_H_

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

#include "trajectory_msgs/msg/detail/joint_trajectory__struct.h"

/// Initialize msg/JointTrajectory message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * trajectory_msgs__msg__JointTrajectory
 * )) before or use
 * trajectory_msgs__msg__JointTrajectory__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
bool
trajectory_msgs__msg__JointTrajectory__init(trajectory_msgs__msg__JointTrajectory * msg);

/// Finalize msg/JointTrajectory message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
void
trajectory_msgs__msg__JointTrajectory__fini(trajectory_msgs__msg__JointTrajectory * msg);

/// Create msg/JointTrajectory message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * trajectory_msgs__msg__JointTrajectory__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
trajectory_msgs__msg__JointTrajectory *
trajectory_msgs__msg__JointTrajectory__create(void);

/// Destroy msg/JointTrajectory message.
/**
 * It calls
 * trajectory_msgs__msg__JointTrajectory__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
void
trajectory_msgs__msg__JointTrajectory__destroy(trajectory_msgs__msg__JointTrajectory * msg);

/// Check for msg/JointTrajectory message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
bool
trajectory_msgs__msg__JointTrajectory__are_equal(const trajectory_msgs__msg__JointTrajectory * lhs, const trajectory_msgs__msg__JointTrajectory * rhs);

/// Copy a msg/JointTrajectory message.
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
trajectory_msgs__msg__JointTrajectory__copy(
  const trajectory_msgs__msg__JointTrajectory * input,
  trajectory_msgs__msg__JointTrajectory * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
const rosidl_type_hash_t *
trajectory_msgs__msg__JointTrajectory__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
const rosidl_runtime_c__type_description__TypeDescription *
trajectory_msgs__msg__JointTrajectory__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
const rosidl_runtime_c__type_description__TypeSource *
trajectory_msgs__msg__JointTrajectory__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
trajectory_msgs__msg__JointTrajectory__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of msg/JointTrajectory messages.
/**
 * It allocates the memory for the number of elements and calls
 * trajectory_msgs__msg__JointTrajectory__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
bool
trajectory_msgs__msg__JointTrajectory__Sequence__init(trajectory_msgs__msg__JointTrajectory__Sequence * array, size_t size);

/// Finalize array of msg/JointTrajectory messages.
/**
 * It calls
 * trajectory_msgs__msg__JointTrajectory__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
void
trajectory_msgs__msg__JointTrajectory__Sequence__fini(trajectory_msgs__msg__JointTrajectory__Sequence * array);

/// Create array of msg/JointTrajectory messages.
/**
 * It allocates the memory for the array and calls
 * trajectory_msgs__msg__JointTrajectory__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
trajectory_msgs__msg__JointTrajectory__Sequence *
trajectory_msgs__msg__JointTrajectory__Sequence__create(size_t size);

/// Destroy array of msg/JointTrajectory messages.
/**
 * It calls
 * trajectory_msgs__msg__JointTrajectory__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
void
trajectory_msgs__msg__JointTrajectory__Sequence__destroy(trajectory_msgs__msg__JointTrajectory__Sequence * array);

/// Check for msg/JointTrajectory message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_trajectory_msgs
bool
trajectory_msgs__msg__JointTrajectory__Sequence__are_equal(const trajectory_msgs__msg__JointTrajectory__Sequence * lhs, const trajectory_msgs__msg__JointTrajectory__Sequence * rhs);

/// Copy an array of msg/JointTrajectory messages.
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
trajectory_msgs__msg__JointTrajectory__Sequence__copy(
  const trajectory_msgs__msg__JointTrajectory__Sequence * input,
  trajectory_msgs__msg__JointTrajectory__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // TRAJECTORY_MSGS__MSG__DETAIL__JOINT_TRAJECTORY__FUNCTIONS_H_
