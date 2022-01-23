// Copyright (c) 2016-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YDURATIONSEC_H_
#define OFP_YAML_YDURATIONSEC_H_

#include "ofp/durationsec.h"
#include "ofp/yaml/yllvm.h"

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

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YDURATIONSEC_H_
