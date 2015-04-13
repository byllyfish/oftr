// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YBUFFERID_H_
#define OFP_YAML_YBUFFERID_H_

#include "ofp/yaml/yllvm.h"
#include "ofp/bufferid.h"
#include "ofp/yaml/enumconverter.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::BufferID> {
  static ofp::yaml::EnumConverterSparse<ofp::OFPBufferID> converter;

  static void output(const ofp::BufferID &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    auto bufferId = static_cast<ofp::OFPBufferID>(value);
    if (converter.convert(bufferId, &scalar)) {
      out << scalar;
    } else {
      // Output BufferID in hexadecimal.
      ScalarTraits<Hex32>::output(bufferId, ctxt, out);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::BufferID &value) {
    if (!scalar.empty() && std::isalpha(scalar.front())) {
      ofp::OFPBufferID bufferId;
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

  using json_type = ofp::BufferID;
};

template <>
inline std::string primitive_to_json(ofp::BufferID value) {
  llvm::StringRef scalar;
  auto bufferId = static_cast<ofp::OFPBufferID>(value);
  if (ScalarTraits<ofp::BufferID>::converter.convert(bufferId, &scalar)) {
    std::string result = "\"";
    result += scalar;
    result += '\"';
    return result;
  } else {
    // Output BufferID in hexadecimal.
    return std::to_string(bufferId);
  }
}

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YBUFFERID_H_
