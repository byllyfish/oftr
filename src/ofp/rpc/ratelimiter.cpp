// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#include "ofp/rpc/ratelimiter.h"
#include "ofp/log.h"

using namespace ofp::rpc;

bool RateLimiter::parse(llvm::StringRef s) {
  // Parse a string of the form:
  //
  // "true"
  // "false"
  // "n/t"
  // "n:p"
  // "n:p/t"

  if (s.equals_lower("true")) {
    *this = RateLimiter{true};
    return true;
  }

  if (s.equals_lower("false")) {
    *this = RateLimiter{false};
    return true;
  }

  UInt32 n = 0;
  UInt32 p = 0xffffffff;
  TimeInterval t;

  // N.B. StringRef consumeInt API returns true when input can't be consumed.
  if (s.consumeInteger(10, n)) {
    log_debug("RateLimiter: Invalid n: ", s);
    return false;
  }

  if (s.consume_front(":")) {
    if (s.consumeInteger(10, p)) {
      log_debug("RateLimiter: Invalid p:", s);
      return false;
    }
  }

  if (s.consume_front("/")) {
    if (!t.parse(s)) {
      log_debug("RateLimiter: Invalid t", s);
      return false;
    }
    s = {};
  }

  if (!s.empty()) {
    log_debug("RateLimiter: Remainder", s);
    return false;
  }

  if (t.valid()) {
    *this = RateLimiter{n, p, t};
  } else {
    *this = RateLimiter{n, p};
  }

  return true;
}

namespace ofp {
namespace rpc {

llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const RateLimiter &value) {
  if (value.n() == 0) {
    return os << "false";
  }

  if (value.n() == 1 && value.p() == 1) {
    return os << "true";
  }

  if (!value.t().valid()) {
    return os << value.n() << ':' << value.p();
  }

  if (value.p() == 0xffffffff) {
    return os << value.n() << '/' << value.t();
  }

  return os << value.n() << ':' << value.p() << '/' << value.t();
}

}  // namespace rpc
}  // namespace ofp
