// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from micro_ros_msgs:msg/Entity.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "micro_ros_msgs/msg/entity.h"


#ifndef MICRO_ROS_MSGS__MSG__DETAIL__ENTITY__FUNCTIONS_H_
#define MICRO_ROS_MSGS__MSG__DETAIL__ENTITY__FUNCTIONS_H_

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
#include "micro_ros_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "micro_ros_msgs/msg/detail/entity__struct.h"

/// Initialize msg/Entity message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * micro_ros_msgs__msg__Entity
 * )) before or use
 * micro_ros_msgs__msg__Entity__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
bool
micro_ros_msgs__msg__Entity__init(micro_ros_msgs__msg__Entity * msg);

/// Finalize msg/Entity message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
void
micro_ros_msgs__msg__Entity__fini(micro_ros_msgs__msg__Entity * msg);

/// Create msg/Entity message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * micro_ros_msgs__msg__Entity__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
micro_ros_msgs__msg__Entity *
micro_ros_msgs__msg__Entity__create(void);

/// Destroy msg/Entity message.
/**
 * It calls
 * micro_ros_msgs__msg__Entity__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
void
micro_ros_msgs__msg__Entity__destroy(micro_ros_msgs__msg__Entity * msg);

/// Check for msg/Entity message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
bool
micro_ros_msgs__msg__Entity__are_equal(const micro_ros_msgs__msg__Entity * lhs, const micro_ros_msgs__msg__Entity * rhs);

/// Copy a msg/Entity message.
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
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
bool
micro_ros_msgs__msg__Entity__copy(
  const micro_ros_msgs__msg__Entity * input,
  micro_ros_msgs__msg__Entity * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
const rosidl_type_hash_t *
micro_ros_msgs__msg__Entity__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
const rosidl_runtime_c__type_description__TypeDescription *
micro_ros_msgs__msg__Entity__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
const rosidl_runtime_c__type_description__TypeSource *
micro_ros_msgs__msg__Entity__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
micro_ros_msgs__msg__Entity__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of msg/Entity messages.
/**
 * It allocates the memory for the number of elements and calls
 * micro_ros_msgs__msg__Entity__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
bool
micro_ros_msgs__msg__Entity__Sequence__init(micro_ros_msgs__msg__Entity__Sequence * array, size_t size);

/// Finalize array of msg/Entity messages.
/**
 * It calls
 * micro_ros_msgs__msg__Entity__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
void
micro_ros_msgs__msg__Entity__Sequence__fini(micro_ros_msgs__msg__Entity__Sequence * array);

/// Create array of msg/Entity messages.
/**
 * It allocates the memory for the array and calls
 * micro_ros_msgs__msg__Entity__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
micro_ros_msgs__msg__Entity__Sequence *
micro_ros_msgs__msg__Entity__Sequence__create(size_t size);

/// Destroy array of msg/Entity messages.
/**
 * It calls
 * micro_ros_msgs__msg__Entity__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
void
micro_ros_msgs__msg__Entity__Sequence__destroy(micro_ros_msgs__msg__Entity__Sequence * array);

/// Check for msg/Entity message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
bool
micro_ros_msgs__msg__Entity__Sequence__are_equal(const micro_ros_msgs__msg__Entity__Sequence * lhs, const micro_ros_msgs__msg__Entity__Sequence * rhs);

/// Copy an array of msg/Entity messages.
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
ROSIDL_GENERATOR_C_PUBLIC_micro_ros_msgs
bool
micro_ros_msgs__msg__Entity__Sequence__copy(
  const micro_ros_msgs__msg__Entity__Sequence * input,
  micro_ros_msgs__msg__Entity__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // MICRO_ROS_MSGS__MSG__DETAIL__ENTITY__FUNCTIONS_H_
