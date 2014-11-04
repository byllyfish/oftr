// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YOXMTYPE_H_
#define OFP_YAML_YOXMTYPE_H_

#include "ofp/oxmtype.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::OXMType> {
  static void output(const ofp::OXMType &value, void *ctxt,
                     llvm::raw_ostream &out) {
    const ofp::OXMTypeInfo *info = (value.length() == 0)
                                       ? value.lookupInfo_IgnoreLength()
                                       : value.lookupInfo();
    if (info) {
      out << info->name;
    } else {
      out << llvm::format("0x%08X", value.oxmNative());
    }
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::OXMType &value) {
    if (!value.parse(scalar)) {
      unsigned long long num;
      if (llvm::getAsUnsignedInteger(scalar, 0, num) == 0) {
        value.setOxmNative(ofp::UInt32_narrow_cast(num));
        return "";
      }

      return "Invalid OXM type.";
    }

    return "";
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YOXMTYPE_H_
