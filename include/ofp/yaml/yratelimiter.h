// Copyright (c) 2017-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YRATELIMITER_H_
#define OFP_YAML_YRATELIMITER_H_

#include "ofp/rpc/ratelimiter.h"
#include "ofp/yaml/yllvm.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::rpc::RateLimiter> {
  static void output(const ofp::rpc::RateLimiter &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << value;
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::rpc::RateLimiter &value) {
    if (!value.parse(scalar)) {
      return "Invalid rate limit";
    }

    return "";
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YRATELIMITER_H_
