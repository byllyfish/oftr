//  ===== ---- ofp/array.h ---------------------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines a specialization of std::hash for std::array.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ARRAY_H_
#define OFP_ARRAY_H_

#include <array>

namespace std {

/// \brief Specialization of std::hash for std::array types.
template <class T, size_t N>
struct hash<std::array<T, N>> {
  size_t operator()(const std::array<T, N> &a) const {
    hash<T> hasher;
    size_t result = 0;

    auto iter = a.crbegin();
    auto end = a.crend();
    while (iter != end) {
      result = result * 31 + hasher(*iter++);
    }

    return result;
  }
};

}  // namespace std

#endif  // OFP_ARRAY_H_
