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
    out << value.toString();
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::DatapathID &value) {
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
