// Copyright 2017 Open Source Robotics Foundation, Inc.
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

#ifndef RMW__IMPL__CPP__KEY_VALUE_HPP_
#define RMW__IMPL__CPP__KEY_VALUE_HPP_

#include <map>
#include <string>
#include <vector>

namespace rmw
{
namespace impl
{
namespace cpp
{

// TODO(karsten1987): Implement based on
// https://github.com/PrismTech/opensplice/blob/master/docs/pdf/OpenSplice_refman_CPP.pdf
static std::map<std::string, std::vector<uint8_t>>
parse_key_value(const std::vector<uint8_t> & kv)
{
  std::map<std::string, std::vector<uint8_t>> m;

  bool keyfound = false;

  std::string key;
  std::vector<uint8_t> value;
  uint8_t prev = '\0';

  if (kv.size() == 0) {
    goto not_valid;
  }

  for (uint8_t u8 : kv) {
    if (keyfound) {
      if ((u8 == ';') && (prev != ';')) {
        prev = u8;
        continue;
      } else if ((u8 != ';') && (prev == ';')) {
        if (value.size() == 0) {
          goto not_valid;
        }
        m[key] = value;

        key.clear();
        value.clear();
        keyfound = false;
      } else {
        value.push_back(u8);
      }
    }
    if (!keyfound) {
      if (u8 == '=') {
        if (key.size() == 0) {
          goto not_valid;
        }
        keyfound = true;
      } else if (isalnum(u8)) {
        key.push_back(u8);
      } else if ((u8 == '\0') && (key.size() == 0) && (m.size() > 0)) {
        break;  // accept trailing '\0' characters
      } else if ((prev != ';') || (key.size() > 0)) {
        goto not_valid;
      }
    }
    prev = u8;
  }
  if (keyfound) {
    if (value.size() == 0) {
      goto not_valid;
    }
    m[key] = value;
  } else if (key.size() > 0) {
    goto not_valid;
  }
  return m;
not_valid:
  // This is not a failure this is something that can happen because the participant_qos userData
  // is used. Other participants in the system not created by rmw could use userData for something
  // else.
  return std::map<std::string, std::vector<uint8_t>>();
}

}  // namespace cpp
}  // namespace impl
}  // namespace rmw

#endif  // RMW__IMPL__CPP__KEY_VALUE_HPP_
