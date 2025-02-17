// generated from rosidl_generator_c/resource/idl__functions.h.em
// with input from test_msgs:msg/Empty.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "test_msgs/msg/empty.h"


#ifndef TEST_MSGS__MSG__DETAIL__EMPTY__FUNCTIONS_H_
#define TEST_MSGS__MSG__DETAIL__EMPTY__FUNCTIONS_H_

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
#include "test_msgs/msg/rosidl_generator_c__visibility_control.h"

#include "test_msgs/msg/detail/empty__struct.h"

/// Initialize msg/Empty message.
/**
 * If the init function is called twice for the same message without
 * calling fini inbetween previously allocated memory will be leaked.
 * \param[in,out] msg The previously allocated message pointer.
 * Fields without a default value will not be initialized by this function.
 * You might want to call memset(msg, 0, sizeof(
 * test_msgs__msg__Empty
 * )) before or use
 * test_msgs__msg__Empty__create()
 * to allocate and initialize the message.
 * \return true if initialization was successful, otherwise false
 */
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
bool
test_msgs__msg__Empty__init(test_msgs__msg__Empty * msg);

/// Finalize msg/Empty message.
/**
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
void
test_msgs__msg__Empty__fini(test_msgs__msg__Empty * msg);

/// Create msg/Empty message.
/**
 * It allocates the memory for the message, sets the memory to zero, and
 * calls
 * test_msgs__msg__Empty__init().
 * \return The pointer to the initialized message if successful,
 * otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
test_msgs__msg__Empty *
test_msgs__msg__Empty__create(void);

/// Destroy msg/Empty message.
/**
 * It calls
 * test_msgs__msg__Empty__fini()
 * and frees the memory of the message.
 * \param[in,out] msg The allocated message pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
void
test_msgs__msg__Empty__destroy(test_msgs__msg__Empty * msg);

/// Check for msg/Empty message equality.
/**
 * \param[in] lhs The message on the left hand size of the equality operator.
 * \param[in] rhs The message on the right hand size of the equality operator.
 * \return true if messages are equal, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
bool
test_msgs__msg__Empty__are_equal(const test_msgs__msg__Empty * lhs, const test_msgs__msg__Empty * rhs);

/// Copy a msg/Empty message.
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
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
bool
test_msgs__msg__Empty__copy(
  const test_msgs__msg__Empty * input,
  test_msgs__msg__Empty * output);

/// Retrieve pointer to the hash of the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_type_hash_t *
test_msgs__msg__Empty__get_type_hash(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_runtime_c__type_description__TypeDescription *
test_msgs__msg__Empty__get_type_description(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the single raw source text that defined this type.
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_runtime_c__type_description__TypeSource *
test_msgs__msg__Empty__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support);

/// Retrieve pointer to the recursive raw sources that defined the description of this type.
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
const rosidl_runtime_c__type_description__TypeSource__Sequence *
test_msgs__msg__Empty__get_type_description_sources(
  const rosidl_message_type_support_t * type_support);

/// Initialize array of msg/Empty messages.
/**
 * It allocates the memory for the number of elements and calls
 * test_msgs__msg__Empty__init()
 * for each element of the array.
 * \param[in,out] array The allocated array pointer.
 * \param[in] size The size / capacity of the array.
 * \return true if initialization was successful, otherwise false
 * If the array pointer is valid and the size is zero it is guaranteed
 # to return true.
 */
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
bool
test_msgs__msg__Empty__Sequence__init(test_msgs__msg__Empty__Sequence * array, size_t size);

/// Finalize array of msg/Empty messages.
/**
 * It calls
 * test_msgs__msg__Empty__fini()
 * for each element of the array and frees the memory for the number of
 * elements.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
void
test_msgs__msg__Empty__Sequence__fini(test_msgs__msg__Empty__Sequence * array);

/// Create array of msg/Empty messages.
/**
 * It allocates the memory for the array and calls
 * test_msgs__msg__Empty__Sequence__init().
 * \param[in] size The size / capacity of the array.
 * \return The pointer to the initialized array if successful, otherwise NULL
 */
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
test_msgs__msg__Empty__Sequence *
test_msgs__msg__Empty__Sequence__create(size_t size);

/// Destroy array of msg/Empty messages.
/**
 * It calls
 * test_msgs__msg__Empty__Sequence__fini()
 * on the array,
 * and frees the memory of the array.
 * \param[in,out] array The initialized array pointer.
 */
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
void
test_msgs__msg__Empty__Sequence__destroy(test_msgs__msg__Empty__Sequence * array);

/// Check for msg/Empty message array equality.
/**
 * \param[in] lhs The message array on the left hand size of the equality operator.
 * \param[in] rhs The message array on the right hand size of the equality operator.
 * \return true if message arrays are equal in size and content, otherwise false.
 */
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
bool
test_msgs__msg__Empty__Sequence__are_equal(const test_msgs__msg__Empty__Sequence * lhs, const test_msgs__msg__Empty__Sequence * rhs);

/// Copy an array of msg/Empty messages.
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
ROSIDL_GENERATOR_C_PUBLIC_test_msgs
bool
test_msgs__msg__Empty__Sequence__copy(
  const test_msgs__msg__Empty__Sequence * input,
  test_msgs__msg__Empty__Sequence * output);

#ifdef __cplusplus
}
#endif

#endif  // TEST_MSGS__MSG__DETAIL__EMPTY__FUNCTIONS_H_
