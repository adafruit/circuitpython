// DO NOT EDIT MANUALLY - this copied file managed by copy_type_description_generated_sources.bash
// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from type_description_interfaces:msg/KeyValue.idl
// generated code does not contain a copyright notice

#ifndef ROSIDL_RUNTIME_C__TYPE_DESCRIPTION__KEY_VALUE__FUNCTIONS_H_
#define ROSIDL_RUNTIME_C__TYPE_DESCRIPTION__KEY_VALUE__FUNCTIONS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "rosidl_runtime_c/action_type_support_struct.h"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_runtime_c/type_hash.h"
#include "rosidl_runtime_c/visibility_control.h"

#include "rosidl_runtime_c/type_description/key_value__struct.h"

/// Initialize msg/KeyValue message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * rosidl_runtime_c__type_description__KeyValue
 * )) before or use
 * rosidl_runtime_c__type_description__KeyValue__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC
bool
rosidl_runtime_c__type_description__KeyValue__init(rosidl_runtime_c__type_description__KeyValue * msg);

/// Finalize msg/KeyValue message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC
void
rosidl_runtime_c__type_description__KeyValue__fini(rosidl_runtime_c__type_description__KeyValue * msg);

/// Create msg/KeyValue message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * rosidl_runtime_c__type_description__KeyValue__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC
rosidl_runtime_c__type_description__KeyValue *
rosidl_runtime_c__type_description__KeyValue__create();

/// Destroy msg/KeyValue message.
/**
 * It calls
 * rosidl_runtime_c__type_description__KeyValue__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC
void
rosidl_runtime_c__type_description__KeyValue__destroy(rosidl_runtime_c__type_description__KeyValue * msg);

/// Check for msg/KeyValue message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC
bool
rosidl_runtime_c__type_description__KeyValue__are_equal(const rosidl_runtime_c__type_description__KeyValue * lhs, const rosidl_runtime_c__type_description__KeyValue * rhs);

/// Copy a msg/KeyValue message.
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
ROSIDL_GENERATOR_C_PUBLIC
bool
rosidl_runtime_c__type_description__KeyValue__copy(
  const rosidl_runtime_c__type_description__KeyValue * input,
  rosidl_runtime_c__type_description__KeyValue * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC
const rosidl_type_hash_t *
rosidl_runtime_c__type_description__KeyValue__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC
const rosidl_runtime_c__type_description__TypeDescription *
rosidl_runtime_c__type_description__KeyValue__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC
const rosidl_runtime_c__type_description__TypeSource *
rosidl_runtime_c__type_description__KeyValue__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC
const rosidl_runtime_c__type_description__TypeSource__Sequence *
rosidl_runtime_c__type_description__KeyValue__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of msg/KeyValue messages.
/**
 * It allocates the memory for the number of elements and calls
 * rosidl_runtime_c__type_description__KeyValue__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC
bool
rosidl_runtime_c__type_description__KeyValue__Sequence__init(rosidl_runtime_c__type_description__KeyValue__Sequence * array, size_t size);

/// Finalize array of msg/KeyValue messages.
/**
 * It calls
 * rosidl_runtime_c__type_description__KeyValue__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC
void
rosidl_runtime_c__type_description__KeyValue__Sequence__fini(rosidl_runtime_c__type_description__KeyValue__Sequence * array);

/// Create array of msg/KeyValue messages.
/**
 * It allocates the memory for the array and calls
 * rosidl_runtime_c__type_description__KeyValue__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC
rosidl_runtime_c__type_description__KeyValue__Sequence *
rosidl_runtime_c__type_description__KeyValue__Sequence__create(size_t size);

/// Destroy array of msg/KeyValue messages.
/**
 * It calls
 * rosidl_runtime_c__type_description__KeyValue__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC
void
rosidl_runtime_c__type_description__KeyValue__Sequence__destroy(rosidl_runtime_c__type_description__KeyValue__Sequence * array);

/// Check for msg/KeyValue message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC
bool
rosidl_runtime_c__type_description__KeyValue__Sequence__are_equal(const rosidl_runtime_c__type_description__KeyValue__Sequence * lhs, const rosidl_runtime_c__type_description__KeyValue__Sequence * rhs);

/// Copy an array of msg/KeyValue messages.
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
ROSIDL_GENERATOR_C_PUBLIC
bool
rosidl_runtime_c__type_description__KeyValue__Sequence__copy(
  const rosidl_runtime_c__type_description__KeyValue__Sequence * input,
  rosidl_runtime_c__type_description__KeyValue__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // ROSIDL_RUNTIME_C__TYPE_DESCRIPTION__KEY_VALUE__FUNCTIONS_H_
