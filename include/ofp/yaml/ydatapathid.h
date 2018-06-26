// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
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

  // DatapathID contains a ':'. If first char is in [1-9] (but not zero), it
  // may be parsed by YAML 1.1 as a sexagesimal integer.
  //
  // e.g. 10:00:00:00:00:00:00:01 ==> 27993600000001
  //
  // Always quote the DatapathID.
  static QuotingType mustQuote(StringRef) { return QuotingType::Single; }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YDATAPATHID_H_
