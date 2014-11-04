// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YSMALLCSTRING_H_
#define OFP_YAML_YSMALLCSTRING_H_

#include "ofp/smallcstring.h"

namespace llvm {
namespace yaml {

template <size_t Size>
struct ScalarTraits<ofp::SmallCString<Size>> {
  static void output(const ofp::SmallCString<Size> &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << value.toString();
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::SmallCString<Size> &value) {
    if (scalar.size() > value.capacity()) {
      return "Value is too long";
    }
    value = scalar;

    return "";
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YSMALLCSTRING_H_
