#ifndef OFP_YAML_YRATELIMITER_H_
#define OFP_YAML_YRATELIMITER_H_

#include "ofp/yaml/yllvm.h"

#include "ofp/rpc/ratelimiter.h"

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

  static bool mustQuote(StringRef) { return false; }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YRATELIMITER_H_
