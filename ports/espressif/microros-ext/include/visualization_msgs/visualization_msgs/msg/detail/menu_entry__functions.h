// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from visualization_msgs:msg/MenuEntry.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "visualization_msgs/msg/menu_entry.h"


#ifndef VISUALIZATION_MSGS__MSG__DETAIL__MENU_ENTRY__FUNCTIONS_H_
#define VISUALIZATION_MSGS__MSG__DETAIL__MENU_ENTRY__FUNCTIONS_H_

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

#include "visualization_msgs/msg/detail/menu_entry__struct.h"

/// Initialize msg/MenuEntry message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * visualization_msgs__msg__MenuEntry
 * )) before or use
 * visualization_msgs__msg__MenuEntry__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
bool
visualization_msgs__msg__MenuEntry__init(visualization_msgs__msg__MenuEntry * msg);

/// Finalize msg/MenuEntry message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
void
visualization_msgs__msg__MenuEntry__fini(visualization_msgs__msg__MenuEntry * msg);

/// Create msg/MenuEntry message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * visualization_msgs__msg__MenuEntry__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
visualization_msgs__msg__MenuEntry *
visualization_msgs__msg__MenuEntry__create(void);

/// Destroy msg/MenuEntry message.
/**
 * It calls
 * visualization_msgs__msg__MenuEntry__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
void
visualization_msgs__msg__MenuEntry__destroy(visualization_msgs__msg__MenuEntry * msg);

/// Check for msg/MenuEntry message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
bool
visualization_msgs__msg__MenuEntry__are_equal(const visualization_msgs__msg__MenuEntry * lhs, const visualization_msgs__msg__MenuEntry * rhs);

/// Copy a msg/MenuEntry message.
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
visualization_msgs__msg__MenuEntry__copy(
  const visualization_msgs__msg__MenuEntry * input,
  visualization_msgs__msg__MenuEntry * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_type_hash_t *
visualization_msgs__msg__MenuEntry__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_runtime_c__type_description__TypeDescription *
visualization_msgs__msg__MenuEntry__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_runtime_c__type_description__TypeSource *
visualization_msgs__msg__MenuEntry__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
visualization_msgs__msg__MenuEntry__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of msg/MenuEntry messages.
/**
 * It allocates the memory for the number of elements and calls
 * visualization_msgs__msg__MenuEntry__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
bool
visualization_msgs__msg__MenuEntry__Sequence__init(visualization_msgs__msg__MenuEntry__Sequence * array, size_t size);

/// Finalize array of msg/MenuEntry messages.
/**
 * It calls
 * visualization_msgs__msg__MenuEntry__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
void
visualization_msgs__msg__MenuEntry__Sequence__fini(visualization_msgs__msg__MenuEntry__Sequence * array);

/// Create array of msg/MenuEntry messages.
/**
 * It allocates the memory for the array and calls
 * visualization_msgs__msg__MenuEntry__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
visualization_msgs__msg__MenuEntry__Sequence *
visualization_msgs__msg__MenuEntry__Sequence__create(size_t size);

/// Destroy array of msg/MenuEntry messages.
/**
 * It calls
 * visualization_msgs__msg__MenuEntry__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
void
visualization_msgs__msg__MenuEntry__Sequence__destroy(visualization_msgs__msg__MenuEntry__Sequence * array);

/// Check for msg/MenuEntry message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_visualization_msgs
bool
visualization_msgs__msg__MenuEntry__Sequence__are_equal(const visualization_msgs__msg__MenuEntry__Sequence * lhs, const visualization_msgs__msg__MenuEntry__Sequence * rhs);

/// Copy an array of msg/MenuEntry messages.
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
visualization_msgs__msg__MenuEntry__Sequence__copy(
  const visualization_msgs__msg__MenuEntry__Sequence * input,
  visualization_msgs__msg__MenuEntry__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // VISUALIZATION_MSGS__MSG__DETAIL__MENU_ENTRY__FUNCTIONS_H_
