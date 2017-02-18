// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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
    out << value;
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
