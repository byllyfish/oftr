// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/durationsec.h"

#include <iomanip>

#include "llvm/Support/Format.h"

using namespace ofp;

/// Parse duration.
///
/// Formats:
///     SSS.NNNNNNNNN
///     SSS
/// To enter more than 999999999 nanoseconds, use format:
///     sss.xHHHHHHHH
///
bool DurationSec::parse(llvm::StringRef s) {
  UInt32 sec;
  if (s.consumeInteger(10, sec)) {
    // Invalid seconds.
    return false;
  }

  if (s.empty()) {
    // Just "seconds" is a valid duration.
    sec_ = sec;
    nsec_ = 0;
    return true;
  }

  if (!s.consume_front(".")) {
    // Missing decimal point.
    return false;
  }

  unsigned base = 10;
  if (s.consume_front("x")) {
    base = 16;
  }

  size_t digits = s.size();
  UInt32 nsec;
  if (s.consumeInteger(base, nsec)) {
    // Invalid nanoseconds.
    return false;
  }

  if (base == 10) {
    if (nsec > 999999999 || digits > 9) {
      // Invalid nanoseconds.
      return false;
    }

    // Add zeros to pad out nanoseconds.
    while (digits < 9) {
      nsec *= 10;
      ++digits;
    }
  }

  sec_ = sec;
  nsec_ = nsec;

  return true;
}

namespace ofp {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const DurationSec &value) {
  if (!value.sec_ && !value.nsec_) {
    return os << '0';
  }

  UInt32 sec = value.sec_;
  UInt32 nsec = value.nsec_;

  os << sec << '.';

  if (nsec < 1000000000) {
    // Legal nanoseconds values.
    return os << llvm::format("%09u", nsec);
  } else {
    // Illegal nanoseconds value.
    return os << llvm::format("x%x", nsec);
  }
}

}  // namespace ofp
