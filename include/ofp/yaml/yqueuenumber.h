// Copyright 2015-present Bill Fisher. All rights reserved.

#ifndef OFP_YAML_YQUEUENUMBER_H_
#define OFP_YAML_YQUEUENUMBER_H_

#include "ofp/queuenumber.h"
#include "ofp/yaml/enumconverter.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::QueueNumber> {
  static ofp::yaml::EnumConverterSparse<ofp::OFPQueueNo> converter;

  static void output(const ofp::QueueNumber &value, void *ctxt,
                     llvm::raw_ostream &out) {
    llvm::StringRef scalar;
    auto queueNum = static_cast<ofp::OFPQueueNo>(value);
    if (converter.convert(queueNum, &scalar)) {
      out << scalar;
    } else {
      // Output QueueNumber in hexadecimal.
      ScalarTraits<Hex32>::output(queueNum, ctxt, out);
    }
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::QueueNumber &value) {
    if (!scalar.empty() && std::isalpha(scalar.front())) {
      ofp::OFPQueueNo queueNum;
      if (!converter.convert(scalar, &queueNum)) {
        return "Invalid queue number value";
      }
      value = queueNum;
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

  using json_type = ofp::QueueNumber;
};

template <>
inline std::string primitive_to_json(ofp::QueueNumber value) {
  llvm::StringRef scalar;
  auto queueNum = static_cast<ofp::OFPQueueNo>(value);
  if (ScalarTraits<ofp::QueueNumber>::converter.convert(queueNum, &scalar)) {
    std::string result = "\"";
    result += scalar;
    result += '\"';
    return result;
  } else {
    // Output QueueNumber in hexadecimal.
    return std::to_string(queueNum);
  }
}

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YQUEUENUMBER_H_
