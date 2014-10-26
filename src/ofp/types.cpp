//  ===== ---- ofp/types.cpp -------------------------------*- C++ -*- =====  //
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
/// \brief Implements common utility functions.
//  ===== ------------------------------------------------------------ =====  //

#include "ofp/types.h"
#include <array>

using namespace ofp;

constexpr char HexDigitsUpperCase[17] = "0123456789ABCDEF";
constexpr char HexDigitsLowerCase[17] = "0123456789abcdef";

inline unsigned FromHex(char hex) {
  assert(std::isxdigit(hex));
  return Unsigned_cast(hex <= '9' ? hex - '0' : (hex & ~0x20) - 'A' + 10);
}

inline char ToHexUpperCase(UInt8 value) {
  assert(value < 16);
  return HexDigitsUpperCase[value];
}

inline char ToHexLowerCase(UInt8 value) {
  assert(value < 16);
  return HexDigitsLowerCase[value];
}

namespace ofp {

template <size_t Length>
std::string RawDataToHexDelimitedLowercase(const std::array<UInt8, Length> &data) {
  // Output is lower-case hexadecimal and delimited by ':'.
  const UInt8 *e = data.data();
  const size_t bufLen = 2 * Length + Length - 1;

  char buf[bufLen];
  char *p = buf;
  *p++ = ToHexLowerCase(*e >> 4);
  *p++ = ToHexLowerCase(*e++ & 0x0F);
  for (unsigned i = 0; i < Length - 1; ++i) {
      *p++ = ':';
      *p++ = ToHexLowerCase(*e >> 4);
      *p++ = ToHexLowerCase(*e++ & 0x0F);
  }

  return std::string(buf, bufLen);
}

}  // namespace ofp


template std::string ofp::RawDataToHexDelimitedLowercase(const std::array<UInt8,8U> &);

std::string ofp::RawDataToHex(const void *data, size_t len) {
  std::string result;
  result.reserve(2 * len);

  const UInt8 *pos = static_cast<const UInt8 *>(data);
  const UInt8 *end = pos + len;

  char buf[2];
  while (pos < end) {
    buf[0] = ToHexUpperCase(*pos >> 4);
    buf[1] = ToHexUpperCase(*pos++ & 0x0F);
    result.append(buf, sizeof(buf));
  }

  return result;
}

std::string ofp::RawDataToHex(const void *data, size_t len, char delimiter,
                              int word) {
  std::string result;
  result.reserve(2 * len);

  const UInt8 *pos = static_cast<const UInt8 *>(data);
  const UInt8 *end = pos + len;

  int cnt = 0;
  char buf[2];
  while (pos < end) {
    if (++cnt > word) {
      result.push_back(delimiter);
      cnt = 1;
    }
    buf[0] = ToHexUpperCase(*pos >> 4);
    buf[1] = ToHexUpperCase(*pos++ & 0x0F);
    result.append(buf, sizeof(buf));
  }

  return result;
}

size_t ofp::HexToRawData(const std::string &hex, void *data, size_t maxlen,
                         bool *error) {
  UInt8 *begin = static_cast<UInt8 *>(data);
  UInt8 *end = begin + maxlen;

  bool nonhex = false;
  unsigned ch[2];
  unsigned idx = 0;
  UInt8 *out = begin;
  for (auto inp = hex.c_str(); *inp; ++inp) {
    if (std::isxdigit(*inp)) {
      ch[idx++] = FromHex(*inp);
      if (idx >= 2) {
        idx = 0;
        assert(ch[0] < 16 && ch[1] < 16);
        *out++ = UInt8_narrow_cast((ch[0] << 4) | ch[1]);
        if (out >= end)
          break;
      }
    } else if (!nonhex && !std::isspace(*inp)) {
      // Ignore space characters when checking for non-hexadecimal values.
      nonhex = true;
    }
  }

  if (error != nullptr) {
    *error = nonhex || (idx != 0);
  }

  assert(end >= out);
  std::memset(out, 0, Unsigned_cast(end - out));

  assert(out >= begin);
  return Unsigned_cast(out - begin);
}

std::string ofp::HexToRawData(const std::string &hex) {
  std::string result;
  result.reserve(hex.size() / 2);

  unsigned ch[2];
  unsigned idx = 0;
  for (auto inp = hex.c_str(); *inp; ++inp) {
    if (std::isxdigit(*inp)) {
      ch[idx++] = FromHex(*inp);
      if (idx >= 2) {
        assert(ch[0] < 16 && ch[1] < 16);
        result.push_back(static_cast<char>((ch[0] << 4) | ch[1]));
        idx = 0;
      }
    }
  }

  return result;
}

bool ofp::IsMemFilled(const void *data, size_t len, char ch) {
  const UInt8 *p = static_cast<const UInt8 *>(data);
  while (len-- > 0) {
    if (*p++ != UInt8_cast(ch))
      return false;
  }
  return true;
}
