// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_SMALLCSTRING_H_
#define OFP_SMALLCSTRING_H_

#include <algorithm>
#include <array>

#include "ofp/types.h"

namespace ofp {
namespace detail {

/// \returns smaller of string length or maxlen.
inline size_t strlen(const char *s, size_t maxlen) {
  const char *p = static_cast<const char *>(std::memchr(s, 0, maxlen));
  return p == nullptr ? maxlen : Unsigned_cast(p - s);
}

std::string validUtf8String(const char *b, const char *e);

}  // namespace detail

/// \brief Implements a fixed size string.
/// \remarks Binary representation is standard layout and trivially copyable.
template <size_t Size>
class SmallCString {
 public:
  using ArrayType = std::array<char, Size>;

  constexpr SmallCString() : str_{} {}

  /* implicit NOLINT */ SmallCString(const std::string &s) { operator=(s); }

  /* implicit NOLINT */ SmallCString(const char *cstr) { operator=(cstr); }

  constexpr size_t capacity() const { return Size; }

  bool empty() const { return str_.front() == 0; }

  size_t length() const { return detail::strlen(str_.data(), capacity()); }

  std::string toString() const {
    return detail::validUtf8String(str_.data(), str_.data() + length());
  }

  void operator=(const std::string &s);
  void operator=(const char *cstr);

  bool operator==(const SmallCString &rhs) const { return str_ == rhs.str_; }

 private:
  ArrayType str_;

  const ArrayType &toArray() const { return str_; }

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                       const SmallCString &value) {
    return os << value.toString();
  }
};

/// Copies string and sets the remaining bytes to zero.
template <size_t Size>
inline void SmallCString<Size>::operator=(const std::string &s) {
  size_t len = std::min(s.length(), capacity());
  assert(len <= capacity());
  std::memcpy(&str_, s.data(), len);
  if (capacity() != len) {
    std::memset(&str_[len], 0, capacity() - len);
  }
}

template <size_t Size>
inline void SmallCString<Size>::operator=(const char *cstr) {
  size_t len = detail::strlen(cstr, capacity());
  assert(len <= capacity());
  std::memcpy(&str_, cstr, len);
  if (capacity() != len) {
    std::memset(&str_[len], 0, capacity() - len);
  }
}

}  // namespace ofp

#endif  // OFP_SMALLCSTRING_H_
