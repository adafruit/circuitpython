// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from visualization_msgs:msg/MarkerArray.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "visualization_msgs/msg/marker_array.h"


#ifndef VISUALIZATION_MSGS__MSG__DETAIL__MARKER_ARRAY__FUNCTIONS_H_
#define VISUALIZATION_MSGS__MSG__DETAIL__MARKER_ARRAY__FUNCTIONS_H_

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
#include "visualization_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "visualization_msgs/msg/detail/marker_array__struct.h"

/// Initialize msg/MarkerArray message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * visualization_msgs__msg__MarkerArray
 * )) before or use
 * visualization_msgs__msg__MarkerArray__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
bool
visualization_msgs__msg__MarkerArray__init(visualization_msgs__msg__MarkerArray * msg);

/// Finalize msg/MarkerArray message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
void
visualization_msgs__msg__MarkerArray__fini(visualization_msgs__msg__MarkerArray * msg);

/// Create msg/MarkerArray message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * visualization_msgs__msg__MarkerArray__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
visualization_msgs__msg__MarkerArray *
visualization_msgs__msg__MarkerArray__create(void);

/// Destroy msg/MarkerArray message.
/**
 * It calls
 * visualization_msgs__msg__MarkerArray__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
void
visualization_msgs__msg__MarkerArray__destroy(visualization_msgs__msg__MarkerArray * msg);

/// Check for msg/MarkerArray message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
bool
visualization_msgs__msg__MarkerArray__are_equal(const visualization_msgs__msg__MarkerArray * lhs, const visualization_msgs__msg__MarkerArray * rhs);

/// Copy a msg/MarkerArray message.
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
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
bool
visualization_msgs__msg__MarkerArray__copy(
  const visualization_msgs__msg__MarkerArray * input,
  visualization_msgs__msg__MarkerArray * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_type_hash_t *
visualization_msgs__msg__MarkerArray__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_runtime_c__type_description__TypeDescription *
visualization_msgs__msg__MarkerArray__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_runtime_c__type_description__TypeSource *
visualization_msgs__msg__MarkerArray__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
visualization_msgs__msg__MarkerArray__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of msg/MarkerArray messages.
/**
 * It allocates the memory for the number of elements and calls
 * visualization_msgs__msg__MarkerArray__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
bool
visualization_msgs__msg__MarkerArray__Sequence__init(visualization_msgs__msg__MarkerArray__Sequence * array, size_t size);

/// Finalize array of msg/MarkerArray messages.
/**
 * It calls
 * visualization_msgs__msg__MarkerArray__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
void
visualization_msgs__msg__MarkerArray__Sequence__fini(visualization_msgs__msg__MarkerArray__Sequence * array);

/// Create array of msg/MarkerArray messages.
/**
 * It allocates the memory for the array and calls
 * visualization_msgs__msg__MarkerArray__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
visualization_msgs__msg__MarkerArray__Sequence *
visualization_msgs__msg__MarkerArray__Sequence__create(size_t size);

/// Destroy array of msg/MarkerArray messages.
/**
 * It calls
 * visualization_msgs__msg__MarkerArray__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
void
visualization_msgs__msg__MarkerArray__Sequence__destroy(visualization_msgs__msg__MarkerArray__Sequence * array);

/// Check for msg/MarkerArray message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
bool
visualization_msgs__msg__MarkerArray__Sequence__are_equal(const visualization_msgs__msg__MarkerArray__Sequence * lhs, const visualization_msgs__msg__MarkerArray__Sequence * rhs);

/// Copy an array of msg/MarkerArray messages.
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
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
bool
visualization_msgs__msg__MarkerArray__Sequence__copy(
  const visualization_msgs__msg__MarkerArray__Sequence * input,
  visualization_msgs__msg__MarkerArray__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // VISUALIZATION_MSGS__MSG__DETAIL__MARKER_ARRAY__FUNCTIONS_H_
