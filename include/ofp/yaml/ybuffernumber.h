// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YBUFFERNUMBER_H_
#define OFP_YAML_YBUFFERNUMBER_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/buffernumber.h"
#include "ofp/yaml/enumconverter.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::BufferNumber> {
  static ofp::yaml::EnumConverterSparse<ofp::OFPBufferNo> converter;

  static void output(const ofp::BufferNumber &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    auto bufferId = static_cast<ofp::OFPBufferNo>(value);
    if (converter.convert(bufferId, &scalar)) {
      out << scalar;
    } else {
      // Output BufferNumber in hexadecimal.
      ScalarTraits<Hex32>::output(bufferId, ctxt, out);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::BufferNumber &value) {
    if (!scalar.empty() && std::isalpha(scalar.front())) {
      ofp::OFPBufferNo bufferId;
      if (!converter.convert(scalar, &bufferId)) {
        return "Invalid buffer id value";
      }
      value = bufferId;
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

  using json_type = ofp::BufferNumber;
};

template <>
inline std::string primitive_to_json(ofp::BufferNumber value) {
  llvm::StringRef scalar;
  auto bufferId = static_cast<ofp::OFPBufferNo>(value);
  if (ScalarTraits<ofp::BufferNumber>::converter.convert(bufferId, &scalar)) {
    std::string result = "\"";
    result += scalar;
    result += '\"';
    return result;
  } else {
    // Output BufferNumber in hexadecimal.
    return std::to_string(bufferId);
  }
}

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YBUFFERNUMBER_H_
