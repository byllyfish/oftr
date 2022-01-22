// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/types.h"

#include <signal.h>    // for sigaction
#include <sys/time.h>  // for setitimer
#include <unistd.h>    // for _exit

#include <array>

#include "ofp/log.h"

namespace ofp {

static const size_t kTwoGigabytes = 0x80000000UL;

static const char HexDigitsUpperCase[17] = "0123456789ABCDEF";
static const char HexDigitsLowerCase[17] = "0123456789abcdef";

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

static const char Base64Digits[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline char ToBase64(UInt32 value) {
  assert(value < 64);
  return Base64Digits[value];
}

template <size_t Length>
char *RawDataToHexDelimitedLowercase(const std::array<UInt8, Length> &data,
                                     char (&buf)[Length * 3]) {
  // Output is lower-case hexadecimal and delimited by ':'.
  const UInt8 *e = data.data();

  char *p = buf;
  *p++ = ToHexLowerCase(*e >> 4);
  *p++ = ToHexLowerCase(*e++ & 0x0F);
  for (unsigned i = 0; i < Length - 1; ++i) {
    *p++ = ':';
    *p++ = ToHexLowerCase(*e >> 4);
    *p++ = ToHexLowerCase(*e++ & 0x0F);
  }
  *p = 0;

  return buf;
}

}  // namespace ofp

// Instantiate functions for 6 and 8 byte arrays.
template char *ofp::RawDataToHexDelimitedLowercase(
    const std::array<UInt8, 8U> &, char (&)[24]);

template char *ofp::RawDataToHexDelimitedLowercase(
    const std::array<UInt8, 6U> &, char (&)[18]);

std::string ofp::RawDataToHex(const void *data, size_t len) {
  if (len > kTwoGigabytes) {
    return "== hex too big ==";
  }

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

void ofp::RawDataToHex(const void *data, size_t length, llvm::raw_ostream &os) {
  if (length > kTwoGigabytes) {
    os << "== hex too big ==";
    return;
  }

  const UInt8 *pos = static_cast<const UInt8 *>(data);
  size_t left = length;

  char buf[256];
  while (left >= 128) {
    char *out = buf;
    for (unsigned i = 0; i < 128; ++i) {
      *out++ = ToHexUpperCase(*pos >> 4);
      *out++ = ToHexUpperCase(*pos++ & 0x0F);
    }
    os.write(buf, 256);
    left -= 128;
  }

  char *out = buf;
  for (unsigned i = 0; i < left; ++i) {
    *out++ = ToHexUpperCase(*pos >> 4);
    *out++ = ToHexUpperCase(*pos++ & 0x0F);
  }
  os.write(buf, 2 * left);
}

std::string ofp::RawDataToHex(const void *data, size_t len, char delimiter,
                              int word) {
  if (len >= kTwoGigabytes) {
    return "== hex too big ==";
  }

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
  if (maxlen == 0) {
    return 0;
  }

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

size_t ofp::HexDelimitedToRawData(llvm::StringRef s, void *data, size_t length,
                                  char delimiter) {
  UInt8 *begin = MutableBytePtr(data);
  UInt8 *end = begin + length;
  UInt8 *out = begin;
  const char *input = s.data();
  size_t input_left = s.size();

  while (input_left >= 2) {
    if (out >= end) {
      return 0;
    }

    char a = *input++;
    char b = *input++;
    input_left -= 2;

    if (!isxdigit(a)) {
      return 0;
    }

    if (b == delimiter) {
      // Single hex digit case.
      *out++ = UInt8_narrow_cast(FromHex(a));
      continue;
    }

    if (!isxdigit(b)) {
      return 0;
    }

    *out++ = UInt8_narrow_cast((FromHex(a) << 4) | FromHex(b));

    if (input_left == 0) {
      return Unsigned_cast(out - begin);
    }

    char delim = *input++;
    if (delim != delimiter) {
      return 0;
    }
    --input_left;
  }

  if (input_left == 1) {
    if (out >= end) {
      return 0;
    }

    // Trailing single hex digit.
    char last = *input;
    if (!isxdigit(last)) {
      return 0;
    }

    *out++ = UInt8_narrow_cast(FromHex(last));
    return Unsigned_cast(out - begin);
  }

  return 0;
}

size_t ofp::HexStrictToRawData(llvm::StringRef s, void *data, size_t length) {
  UInt8 *begin = MutableBytePtr(data);
  UInt8 *end = begin + length;
  UInt8 *out = begin;
  const char *input = s.data();
  size_t input_left = s.size();

  while (input_left >= 2) {
    if (out >= end) {
      return 0;
    }

    char a = *input++;
    char b = *input++;
    input_left -= 2;

    if (!isxdigit(a) || !isxdigit(b)) {
      return 0;
    }

    *out++ = UInt8_narrow_cast((FromHex(a) << 4) | FromHex(b));
  }

  if (input_left != 0) {
    return 0;
  }

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

std::string ofp::RawDataToBase64(const void *data, size_t length) {
  // Prevent calculation overflow on 32 bit platforms.
  if (length > kTwoGigabytes) {
    return "== base64 too big ==";
  }

  size_t len = (length + 2) / 3 * 4;

  std::string result;
  result.resize(len);

  const UInt8 *src = BytePtr(data);
  size_t remaining = length;

  char *dst = &result[0];
  while (remaining >= 3) {
    UInt32 block24 = (UInt32_cast(src[0]) << 16) | (UInt32_cast(src[1]) << 8) |
                     UInt32_cast(src[2]);
    *dst++ = ToBase64((block24 >> 18) & 0x03F);
    *dst++ = ToBase64((block24 >> 12) & 0x03F);
    *dst++ = ToBase64((block24 >> 6) & 0x03F);
    *dst++ = ToBase64(block24 & 0x03F);
    src += 3;
    remaining -= 3;
  }

  if (remaining == 2) {
    UInt32 block24 = UInt32_cast(src[0] << 16) | (UInt32_cast(src[1]) << 8);
    *dst++ = ToBase64((block24 >> 18) & 0x03F);
    *dst++ = ToBase64((block24 >> 12) & 0x03F);
    *dst++ = ToBase64((block24 >> 6) & 0x03F);
    *dst++ = '=';
  } else if (remaining == 1) {
    UInt32 block24 = UInt32_cast(src[0] << 16);
    *dst++ = ToBase64((block24 >> 18) & 0x03F);
    *dst++ = ToBase64((block24 >> 12) & 0x03F);
    *dst++ = '=';
    *dst++ = '=';
  }

  assert(result.size() == len);
  assert(static_cast<size_t>(dst - &result[0]) == len);

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

void ofp::MemCopyMasked(void *dest, const void *data, const void *mask,
                        size_t length) {
  UInt8 *d = static_cast<UInt8 *>(dest);
  const UInt8 *s = static_cast<const UInt8 *>(data);
  const UInt8 *m = static_cast<const UInt8 *>(mask);
  while (length-- > 0) {
    *d++ = *s++ & *m++;
  }
}

[[noreturn]] static void watchdog_timer1(int signum) {
  // Avoid GCC -Wunused-result warning.
  if (::write(STDERR_FILENO, "watchdog_timer1\n", 16) != 16)
    ::_exit(200);
  ::_exit(200);
}

void ofp::SetWatchdogTimer(unsigned secs) {
  static bool installed = false;

  if (!installed) {
    installed = true;
    struct sigaction act;
    std::memset(&act, 0, sizeof(act));
    act.sa_handler = watchdog_timer1;
    act.sa_flags = SA_RESTART;

    if (::sigaction(SIGPROF, &act, nullptr) < 0) {
      log::fatal("SetWatchdogTimer: sigaction failed");
    }
  }

  struct itimerval itv;
  std::memset(&itv, 0, sizeof(itv));
  itv.it_value.tv_sec = secs;

  if (::setitimer(ITIMER_PROF, &itv, nullptr) < 0) {
    log::fatal("SetWatchdogTimer: setitimer failed");
  }
}
