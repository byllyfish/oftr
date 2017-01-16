#ifndef OFP_YAML_YDURATIONSEC_H_
#define OFP_YAML_YDURATIONSEC_H_

#include "ofp/yaml/yllvm.h"

#include "ofp/durationsec.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::DurationSec> {
  static void output(const ofp::DurationSec &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << value;
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::DurationSec &value) {
    if (!value.parse(scalar)) {
      return "Invalid duration value";
    }
    return "";
  }

  static bool mustQuote(StringRef) { return false; }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YDURATIONSEC_H_
