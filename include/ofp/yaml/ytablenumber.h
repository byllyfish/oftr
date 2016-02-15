// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YTABLENUMBER_H_
#define OFP_YAML_YTABLENUMBER_H_

#include "ofp/tablenumber.h"
#include "ofp/yaml/enumconverter.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::TableNumber> {
  static const ofp::yaml::EnumConverterSparse<ofp::OFPTableNo> converter;

  static void output(const ofp::TableNumber &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    auto tableNum = static_cast<ofp::OFPTableNo>(value);
    if (converter.convert(tableNum, &scalar)) {
      out << scalar;
    } else {
      // Output TableNumber in hexadecimal.
      ScalarTraits<Hex8>::output(tableNum, ctxt, out);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::TableNumber &value) {
    if (!scalar.empty() && std::isalpha(scalar.front())) {
      ofp::OFPTableNo tableNum;
      if (!converter.convert(scalar, &tableNum)) {
        return "Invalid table number value";
      }
      value = tableNum;
      return "";
    }

    uint8_t num;
    auto err = ScalarTraits<uint8_t>::input(scalar, ctxt, num);
    if (err.empty()) {
      value = num;
    }
    return err;
  }

  static bool mustQuote(StringRef) { return false; }

  using json_type = ofp::TableNumber;
};

template <>
inline std::string primitive_to_json(ofp::TableNumber value) {
  llvm::StringRef scalar;
  auto tableNum = static_cast<ofp::OFPTableNo>(value);
  if (ScalarTraits<ofp::TableNumber>::converter.convert(tableNum, &scalar)) {
    std::string result = "\"";
    result += scalar;
    result += '\"';
    return result;
  } else {
    // Output TableNumber in hexadecimal.
    return std::to_string(tableNum);
  }
}

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YTABLENUMBER_H_
