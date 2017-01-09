// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YDATAPATHID_H_
#define OFP_YAML_YDATAPATHID_H_

#include "ofp/datapathid.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::DatapathID> {
  static void output(const ofp::DatapathID &value, void *ctxt,
                     llvm::raw_ostream &out) {
    if (!value.empty()) {
      out << value;
    }
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::DatapathID &value) {
    // Accept "null" as empty datapath.
    if (scalar == "null") {
      value.clear();
      return "";
    }
    if (!value.parse(scalar)) {
      return "Invalid DatapathID.";
    }
    return "";
  }

  static bool mustQuote(StringRef) { return true; }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YDATAPATHID_H_
