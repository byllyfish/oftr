// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YTIMESTAMP_H_
#define OFP_YAML_YTIMESTAMP_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/timestamp.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::Timestamp> {
  static void output(const ofp::Timestamp &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << value.toString();
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::Timestamp &value) {
    if (!value.parse(scalar)) {
      return "Invalid timestamp.";
    }
    return "";
  }

  static bool mustQuote(StringRef) { return false; }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YTIMESTAMP_H_
