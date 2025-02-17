// Copyright 2015 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__IMPL__CPP__MACROS_HPP_
#define RMW__IMPL__CPP__MACROS_HPP_

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "rmw/check_type_identifiers_match.h"
#include "rmw/error_handling.h"
#include "rmw/impl/cpp/demangle.hpp"  // For demangle.

#define RMW_TRY_PLACEMENT_NEW(Destination, BufferForNew, FailureAction, Type, ...) \
  try { \
    Destination = new(BufferForNew) Type(__VA_ARGS__); \
  } catch (const std::exception & exception) { \
    RMW_SET_ERROR_MSG( \
      ( \
        std::string("caught C++ exception ") + rmw::impl::cpp::demangle(exception) + \
        " constructing " #Type ": " + exception.what() \
      ).c_str()); \
    FailureAction; \
  } catch (...) { \
    RMW_SET_ERROR_MSG("caught unknown C++ exception constructing " #Type); \
    FailureAction; \
  }

#define RMW_TRY_DESTRUCTOR(Statement, Type, FailureAction) \
  try { \
    Statement; \
  } catch (const std::exception & exception) { \
    RMW_SET_ERROR_MSG( \
      ( \
        std::string("caught C++ exception in destructor of " #Type ": ") + \
        rmw::impl::cpp::demangle(exception) + ": " + exception.what() \
      ).c_str()); \
    FailureAction; \
  } catch (...) { \
    RMW_SET_ERROR_MSG("caught unknown C++ exception in destructor of " #Type); \
    FailureAction; \
  }

#define RMW_TRY_DESTRUCTOR_FROM_WITHIN_FAILURE(Statement, Type) \
  try { \
    Statement; \
  } catch (const std::exception & exception) { \
    std::stringstream ss; \
    ss << "caught C++ exception in destructor of " #Type " while handling a failure: " \
       << rmw::impl::cpp::demangle(exception) << ": " << exception.what() \
       << ", at: " << __FILE__ << ":" << __LINE__ << '\n'; \
    (std::cerr << ss.str()).flush(); \
  } catch (...) { \
    std::stringstream ss; \
    ss << "caught unknown C++ exception in destructor of " #Type << \
      " while handling a failure at: " << __FILE__ << ":" << __LINE__ << '\n'; \
    (std::cerr << ss.str()).flush(); \
  }

#endif  // RMW__IMPL__CPP__MACROS_HPP_
