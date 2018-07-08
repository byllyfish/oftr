// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YSMALLCSTRING_H_
#define OFP_YAML_YSMALLCSTRING_H_

#include "ofp/smallcstring.h"
#include "ofp/yaml/yllvm.h"

namespace llvm {
namespace yaml {

template <size_t Size>
struct ScalarTraits<ofp::SmallCString<Size>> {
  static void output(const ofp::SmallCString<Size> &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << value;
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::SmallCString<Size> &value) {
    if (scalar.size() > value.capacity()) {
      return "Value is too long";
    }
    value = scalar;

    return "";
  }

  static QuotingType mustQuote(StringRef s) { return needsQuotes(s); }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YSMALLCSTRING_H_
