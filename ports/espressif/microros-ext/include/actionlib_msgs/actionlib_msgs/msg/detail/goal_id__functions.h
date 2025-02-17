// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from actionlib_msgs:msg/GoalID.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "actionlib_msgs/msg/goal_id.h"


#ifndef ACTIONLIB_MSGS__MSG__DETAIL__GOAL_ID__FUNCTIONS_H_
#define ACTIONLIB_MSGS__MSG__DETAIL__GOAL_ID__FUNCTIONS_H_

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
#include "actionlib_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "actionlib_msgs/msg/detail/goal_id__struct.h"

/// Initialize msg/GoalID message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * actionlib_msgs__msg__GoalID
 * )) before or use
 * actionlib_msgs__msg__GoalID__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
bool
actionlib_msgs__msg__GoalID__init(actionlib_msgs__msg__GoalID * msg);

/// Finalize msg/GoalID message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
void
actionlib_msgs__msg__GoalID__fini(actionlib_msgs__msg__GoalID * msg);

/// Create msg/GoalID message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * actionlib_msgs__msg__GoalID__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
actionlib_msgs__msg__GoalID *
actionlib_msgs__msg__GoalID__create(void);

/// Destroy msg/GoalID message.
/**
 * It calls
 * actionlib_msgs__msg__GoalID__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
void
actionlib_msgs__msg__GoalID__destroy(actionlib_msgs__msg__GoalID * msg);

/// Check for msg/GoalID message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
bool
actionlib_msgs__msg__GoalID__are_equal(const actionlib_msgs__msg__GoalID * lhs, const actionlib_msgs__msg__GoalID * rhs);

/// Copy a msg/GoalID message.
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
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
bool
actionlib_msgs__msg__GoalID__copy(
  const actionlib_msgs__msg__GoalID * input,
  actionlib_msgs__msg__GoalID * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
const rosidl_type_hash_t *
actionlib_msgs__msg__GoalID__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
const rosidl_runtime_c__type_description__TypeDescription *
actionlib_msgs__msg__GoalID__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
const rosidl_runtime_c__type_description__TypeSource *
actionlib_msgs__msg__GoalID__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
actionlib_msgs__msg__GoalID__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of msg/GoalID messages.
/**
 * It allocates the memory for the number of elements and calls
 * actionlib_msgs__msg__GoalID__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
bool
actionlib_msgs__msg__GoalID__Sequence__init(actionlib_msgs__msg__GoalID__Sequence * array, size_t size);

/// Finalize array of msg/GoalID messages.
/**
 * It calls
 * actionlib_msgs__msg__GoalID__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
void
actionlib_msgs__msg__GoalID__Sequence__fini(actionlib_msgs__msg__GoalID__Sequence * array);

/// Create array of msg/GoalID messages.
/**
 * It allocates the memory for the array and calls
 * actionlib_msgs__msg__GoalID__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
actionlib_msgs__msg__GoalID__Sequence *
actionlib_msgs__msg__GoalID__Sequence__create(size_t size);

/// Destroy array of msg/GoalID messages.
/**
 * It calls
 * actionlib_msgs__msg__GoalID__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
void
actionlib_msgs__msg__GoalID__Sequence__destroy(actionlib_msgs__msg__GoalID__Sequence * array);

/// Check for msg/GoalID message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
bool
actionlib_msgs__msg__GoalID__Sequence__are_equal(const actionlib_msgs__msg__GoalID__Sequence * lhs, const actionlib_msgs__msg__GoalID__Sequence * rhs);

/// Copy an array of msg/GoalID messages.
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
ROSIDL_GENERATOR_C_PUBLIC_actionlib_msgs
bool
actionlib_msgs__msg__GoalID__Sequence__copy(
  const actionlib_msgs__msg__GoalID__Sequence * input,
  actionlib_msgs__msg__GoalID__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // ACTIONLIB_MSGS__MSG__DETAIL__GOAL_ID__FUNCTIONS_H_
