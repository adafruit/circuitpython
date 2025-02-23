// Copyright 2019 Robert Bosch GmbH
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TRACETOOLS__UTILS_HPP_
#define TRACETOOLS__UTILS_HPP_

#include <stddef.h>
#include <functional>

#include "tracetools/config.h"

#ifndef TRACETOOLS_DISABLED

namespace tracetools
{

namespace detail
{

/// Demangle symbol string.
/**
 * \param[in] mangled the pointer to the mangled symbol string
 * \return the demangled symbol, or nullptr if demangling failed. The caller is responsible for
 * deallocating this memory using free.
 */
char * demangle_symbol(const char * mangled);

/// Get demangled symbol string from function pointer.
/**
 * \param[in] funcptr the function pointer
 * \return the demangled symbol, or nullptr if the pointer could not be matched to a shared object
 * or if demangling failed. The caller is responsible for deallocating this memory using free.
 */
char * get_symbol_funcptr(const void * funcptr);

}  // namespace detail

/// Get demangled symbol from an std::function object.
/**
 * \param[in] f the std::function object
 * \return the symbol, or nullptr if the symbol could not be retrieved or if demangling failed. The
 * caller is responsible for deallocating this memory using free.
 */
template<typename T, typename ... U>
char * get_symbol(std::function<T(U...)> f)
{
  typedef T (fnType)(U...);
  fnType ** fnPointer = f.template target<fnType *>();
  // If we get an actual address
  if (fnPointer != nullptr) {
    void * funcptr = reinterpret_cast<void *>(*fnPointer);
    return detail::get_symbol_funcptr(funcptr);
  }
  // Otherwise we have to go through target_type()
  return detail::demangle_symbol(f.target_type().name());
}

/// Get demangled symbol from a function-related object.
/**
 * Fallback meant for lambdas with captures.
 *
 * \param[in] l a generic object
 * \return the symbol, or nullptr if demangling failed. The caller is responsible for deallocating
 * this memory using free.
 */
template<typename L>
char * get_symbol(L && l)
{
  return detail::demangle_symbol(typeid(l).name());
}

}  // namespace tracetools

#endif  // TRACETOOLS_DISABLED

#endif  // TRACETOOLS__UTILS_HPP_
