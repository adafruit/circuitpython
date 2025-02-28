// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from shape_msgs:msg/MeshTriangle.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "shape_msgs/msg/mesh_triangle.h"


#ifndef SHAPE_MSGS__MSG__DETAIL__MESH_TRIANGLE__FUNCTIONS_H_
#define SHAPE_MSGS__MSG__DETAIL__MESH_TRIANGLE__FUNCTIONS_H_

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
#include "shape_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "shape_msgs/msg/detail/mesh_triangle__struct.h"

/// Initialize msg/MeshTriangle message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * shape_msgs__msg__MeshTriangle
 * )) before or use
 * shape_msgs__msg__MeshTriangle__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
bool
shape_msgs__msg__MeshTriangle__init(shape_msgs__msg__MeshTriangle * msg);

/// Finalize msg/MeshTriangle message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
void
shape_msgs__msg__MeshTriangle__fini(shape_msgs__msg__MeshTriangle * msg);

/// Create msg/MeshTriangle message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * shape_msgs__msg__MeshTriangle__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
shape_msgs__msg__MeshTriangle *
shape_msgs__msg__MeshTriangle__create(void);

/// Destroy msg/MeshTriangle message.
/**
 * It calls
 * shape_msgs__msg__MeshTriangle__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
void
shape_msgs__msg__MeshTriangle__destroy(shape_msgs__msg__MeshTriangle * msg);

/// Check for msg/MeshTriangle message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
bool
shape_msgs__msg__MeshTriangle__are_equal(const shape_msgs__msg__MeshTriangle * lhs, const shape_msgs__msg__MeshTriangle * rhs);

/// Copy a msg/MeshTriangle message.
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
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
bool
shape_msgs__msg__MeshTriangle__copy(
  const shape_msgs__msg__MeshTriangle * input,
  shape_msgs__msg__MeshTriangle * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
const rosidl_type_hash_t *
shape_msgs__msg__MeshTriangle__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
const rosidl_runtime_c__type_description__TypeDescription *
shape_msgs__msg__MeshTriangle__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
const rosidl_runtime_c__type_description__TypeSource *
shape_msgs__msg__MeshTriangle__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
shape_msgs__msg__MeshTriangle__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of msg/MeshTriangle messages.
/**
 * It allocates the memory for the number of elements and calls
 * shape_msgs__msg__MeshTriangle__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
bool
shape_msgs__msg__MeshTriangle__Sequence__init(shape_msgs__msg__MeshTriangle__Sequence * array, size_t size);

/// Finalize array of msg/MeshTriangle messages.
/**
 * It calls
 * shape_msgs__msg__MeshTriangle__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
void
shape_msgs__msg__MeshTriangle__Sequence__fini(shape_msgs__msg__MeshTriangle__Sequence * array);

/// Create array of msg/MeshTriangle messages.
/**
 * It allocates the memory for the array and calls
 * shape_msgs__msg__MeshTriangle__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
shape_msgs__msg__MeshTriangle__Sequence *
shape_msgs__msg__MeshTriangle__Sequence__create(size_t size);

/// Destroy array of msg/MeshTriangle messages.
/**
 * It calls
 * shape_msgs__msg__MeshTriangle__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
void
shape_msgs__msg__MeshTriangle__Sequence__destroy(shape_msgs__msg__MeshTriangle__Sequence * array);

/// Check for msg/MeshTriangle message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
bool
shape_msgs__msg__MeshTriangle__Sequence__are_equal(const shape_msgs__msg__MeshTriangle__Sequence * lhs, const shape_msgs__msg__MeshTriangle__Sequence * rhs);

/// Copy an array of msg/MeshTriangle messages.
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
ROSIDL_GENERATOR_C_PUBLIC_shape_msgs
bool
shape_msgs__msg__MeshTriangle__Sequence__copy(
  const shape_msgs__msg__MeshTriangle__Sequence * input,
  shape_msgs__msg__MeshTriangle__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // SHAPE_MSGS__MSG__DETAIL__MESH_TRIANGLE__FUNCTIONS_H_
