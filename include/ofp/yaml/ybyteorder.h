// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YBYTEORDER_H_
#define OFP_YAML_YBYTEORDER_H_

#include "ofp/yaml/yllvm.h"

#include "ofp/byteorder.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::Big8> {
  static void output(const ofp::Big8 &value, void *ctxt,
                     llvm::raw_ostream &out) {
    // Output Big8 in hexadecimal.
    uint8_t num = value;
    ScalarTraits<Hex8>::output(num, ctxt, out);
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::Big8 &value) {
    uint8_t num = 0;
    auto err = ScalarTraits<uint8_t>::input(scalar, ctxt, num);
    if (err.empty()) {
      value = num;
    }
    return err;
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }

  using json_type = uint8_t;
};

template <>
struct ScalarTraits<ofp::Big16> {
  static void output(const ofp::Big16 &value, void *ctxt,
                     llvm::raw_ostream &out) {
    // Output Big16 in hexadecimal.
    uint16_t num = value;
    ScalarTraits<Hex16>::output(num, ctxt, out);
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::Big16 &value) {
    uint16_t num = 0;
    auto err = ScalarTraits<uint16_t>::input(scalar, ctxt, num);
    if (err.empty()) {
      value = num;
    }
    return err;
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }

  using json_type = uint16_t;
};

template <>
struct ScalarTraits<ofp::Big24> {
  static void output(const ofp::Big24 &value, void *ctxt,
                     llvm::raw_ostream &out) {
    // Output Big24 in hexadecimal.
    uint32_t num = value;
    ScalarTraits<Hex32>::output(num, ctxt, out);
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::Big24 &value) {
    uint32_t num = 0;
    auto err = ScalarTraits<uint32_t>::input(scalar, ctxt, num);
    if (err.empty()) {
      if (num > 0x00ffffff) {
        return "out of range number";
      }
      value = num;
    }
    return err;
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }

  using json_type = uint32_t;
};

template <>
struct ScalarTraits<ofp::Big32> {
  static void output(const ofp::Big32 &value, void *ctxt,
                     llvm::raw_ostream &out) {
    // Output Big32 in hexadecimal.
    uint32_t num = value;
    ScalarTraits<Hex32>::output(num, ctxt, out);
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::Big32 &value) {
    uint32_t num = 0;
    auto err = ScalarTraits<uint32_t>::input(scalar, ctxt, num);
    if (err.empty()) {
      value = num;
    }
    return err;
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }

  using json_type = uint32_t;
};

template <>
struct ScalarTraits<ofp::Big64> {
  static void output(const ofp::Big64 &value, void *ctxt,
                     llvm::raw_ostream &out) {
    // Output Big64 in hexadecimal.
    uint64_t num = value;
    ScalarTraits<Hex64>::output(num, ctxt, out);
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::Big64 &value) {
    uint64_t num = 0;
    auto err = ScalarTraits<uint64_t>::input(scalar, ctxt, num);
    if (err.empty()) {
      value = num;
    }
    return err;
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }

  using json_type = uint64_t;
};

template <>
struct ScalarTraits<ofp::SignedBig32> {
  static void output(const ofp::SignedBig32 &value, void *ctxt,
                     llvm::raw_ostream &out) {
    // Output SignedBig32 in decimal.
    int32_t num = value;
    ScalarTraits<int32_t>::output(num, ctxt, out);
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::SignedBig32 &value) {
    int32_t num = 0;
    auto err = ScalarTraits<int32_t>::input(scalar, ctxt, num);
    if (err.empty()) {
      value = num;
    }
    return err;
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }

  using json_type = int32_t;
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YBYTEORDER_H_
