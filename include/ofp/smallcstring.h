//  ===== ---- ofp/smallcstring.h --------------------------*- C++ -*- =====  //
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
/// \brief Defines the SmallCString class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_SMALLCSTRING_H_
#define OFP_SMALLCSTRING_H_

#include <array>
#include <algorithm>
#include "ofp/types.h"

namespace ofp {
namespace detail {

/// \returns smaller of string length or maxlen.
inline size_t strlen(const char *s, size_t maxlen) {
  auto p = std::find(s, s + maxlen, '\0');
  return Unsigned_cast(p - s);
}

std::string validUtf8String(const char *b, const char *e);

}  // namespace detail

/// \brief Implements a fixed size, null-terminated C string.
/// \remarks Binary representation is standard layout and trivially copyable.
template <size_t Size>
class SmallCString {
public:
  using ArrayType = std::array<char, Size>;

  constexpr SmallCString() : str_{} {}

  /* implicit NOLINT */ SmallCString(const std::string &s) { operator=(s); }

  /* implicit NOLINT */ SmallCString(const char *cstr) { operator=(cstr); }

  constexpr size_t capacity() const { return Size - 1; }

  bool empty() const { return str_.front() == 0; }

  size_t length() const { return detail::strlen(str_.data(), capacity()); }

  std::string toString() const {
    return detail::validUtf8String(str_.data(), str_.data() + length());
  }

  const ArrayType &toArray() const { return str_; }

  void operator=(const std::string &s);
  void operator=(const char *cstr);

private:
  ArrayType str_;
};

//template <size_t Size>
//bool operator==(const SmallCString<Size> &lhs, const SmallCString<Size> &rhs);

template <size_t Size>
inline bool operator==(const SmallCString<Size> &lhs,
                       const SmallCString<Size> &rhs) {
  return lhs.toArray() == rhs.toArray();
}

/// Copies string and sets the remaining bytes to zero. The last byte will
/// always be zero.
template <size_t Size>
inline void SmallCString<Size>::operator=(const std::string &s) {
  size_t len = std::min(s.length(), capacity());
  assert(len <= capacity());
  std::memcpy(&str_, s.data(), len);
  std::memset(&str_[len], 0, capacity() - len);
  str_.back() = 0;
}

template <size_t Size>
inline void SmallCString<Size>::operator=(const char *cstr) {
  size_t len = detail::strlen(cstr, capacity());
  assert(len <= capacity());
  std::memcpy(&str_, cstr, len);
  std::memset(&str_[len], 0, capacity() - len);
  str_.back() = 0;
}

}  // namespace ofp

#endif  // OFP_SMALLCSTRING_H_
