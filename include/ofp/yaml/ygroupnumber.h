// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YGROUPNUMBER_H_
#define OFP_YAML_YGROUPNUMBER_H_

#include "ofp/groupnumber.h"
#include "ofp/yaml/enumconverter.h"
#include "ofp/yaml/yllvm.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::GroupNumber> {
  static const ofp::yaml::EnumConverterSparse<ofp::OFPGroupNo> converter;

  static void output(const ofp::GroupNumber &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    auto groupNum = static_cast<ofp::OFPGroupNo>(value);
    if (converter.convert(groupNum, &scalar)) {
      out << scalar;
    } else {
      // Output GroupNumber in hexadecimal.
      ScalarTraits<Hex32>::output(groupNum, ctxt, out);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::GroupNumber &value) {
    if (!scalar.empty() && std::isalpha(scalar.front())) {
      ofp::OFPGroupNo groupNum;
      if (!converter.convert(scalar, &groupNum)) {
        return "Invalid group number value";
      }
      value = groupNum;
      return "";
    }

    uint32_t num;
    auto err = ScalarTraits<uint32_t>::input(scalar, ctxt, num);
    if (err.empty()) {
      value = num;
    }
    return err;
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }

  using json_type = ofp::GroupNumber;
};

template <>
inline void primitive_to_json(ofp::GroupNumber value, llvm::raw_ostream &os) {
  llvm::StringRef scalar;
  auto groupNum = static_cast<ofp::OFPGroupNo>(value);
  if (ScalarTraits<ofp::GroupNumber>::converter.convert(groupNum, &scalar)) {
    os << '"' << scalar << '"';
  } else {
    os << groupNum;
  }
}

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YGROUPNUMBER_H_
