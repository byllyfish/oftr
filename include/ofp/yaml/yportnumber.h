// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YPORTNUMBER_H_
#define OFP_YAML_YPORTNUMBER_H_

#include "ofp/portnumber.h"
#include "ofp/yaml/enumconverter.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::PortNumber> {
  static ofp::yaml::EnumConverterSparse<ofp::OFPPortNo> converter;

  static void output(const ofp::PortNumber &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    auto portNum = static_cast<ofp::OFPPortNo>(value);
    if (converter.convert(portNum, &scalar)) {
      out << scalar;
    } else {
      // Output PortNumber in hexadecimal.
      ScalarTraits<Hex32>::output(portNum, ctxt, out);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::PortNumber &value) {
    if (!scalar.empty() && std::isalpha(scalar.front())) {
      ofp::OFPPortNo portNum;
      if (!converter.convert(scalar, &portNum)) {
        return "Invalid port number value";
      }
      value = portNum;
      return "";
    }

    uint32_t num;
    auto err = ScalarTraits<uint32_t>::input(scalar, ctxt, num);
    if (err.empty()) {
      value = num;
    }
    return err;
  }

  static bool mustQuote(StringRef) { return false; }

  using json_type = ofp::PortNumber;
};

template <>
inline std::string primitive_to_json(ofp::PortNumber value) {
  llvm::StringRef scalar;
  auto portNum = static_cast<ofp::OFPPortNo>(value);
  if (ScalarTraits<ofp::PortNumber>::converter.convert(portNum, &scalar)) {
    std::string result = "\"";
    result += scalar;
    result += '\"';
    return result;
  } else {
    // Output PortNumber in hexadecimal.
    return std::to_string(portNum);
  }
}

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YPORTNUMBER_H_
