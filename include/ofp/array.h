// Copyright 2014-present Bill Fisher. All rights reserved.

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
