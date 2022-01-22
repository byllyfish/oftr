// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YADDRESS_H_
#define OFP_YAML_YADDRESS_H_

#include "ofp/ipv4address.h"
#include "ofp/ipv6address.h"
#include "ofp/ipv6endpoint.h"
#include "ofp/macaddress.h"
#include "ofp/yaml/yllvm.h"

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::IPv4Address> {
  static void output(const ofp::IPv4Address &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << value;
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::IPv4Address &value) {
    if (scalar.empty()) {
      value.clear();
      return "";
    }

    if (!value.parse(scalar)) {
      return "Invalid IPv4 address.";
    }
    return "";
  }

  static QuotingType mustQuote(StringRef) { return QuotingType::None; }
};

template <>
struct ScalarTraits<ofp::IPv6Address> {
  static void output(const ofp::IPv6Address &value, void *ctxt,
                     llvm::raw_ostream &out) {
    value.outputV6(out);
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::IPv6Address &value) {
    if (scalar.empty()) {
      value.clear();
      return "";
    }

    if (!value.parse(scalar, false)) {
      return "Invalid IPv6 address.";
    }
    return "";
  }

  // IPv6Address contains a ':'. If first char is in [1-9] (but not zero),
  // it may be parsed by a YAML 1.1 as a sexagesimal integer.
  //
  // e.g. YAML 1.1:  2001:0:0:0:0:59:9:59 ==> 5601519360212999
  //
  // Always quote the IPv6Address.
  static QuotingType mustQuote(StringRef) { return QuotingType::Single; }
};

template <>
struct ScalarTraits<ofp::MacAddress> {
  static void output(const ofp::MacAddress &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << value;
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::MacAddress &value) {
    if (scalar.empty()) {
      value.clear();
      return "";
    }

    if (!value.parse(scalar)) {
      return "Invalid Ethernet address.";
    }
    return "";
  }

  // MacAddress contains a ':'. If first char is in [1-9] (but not zero), it
  // may be parsed by YAML 1.1 as a sexagesimal integer.
  //
  // e.g. 33:33:00:00:00:00 ==> 26088480000
  //
  // Always quote the MacAddress.
  static QuotingType mustQuote(StringRef) { return QuotingType::Single; }
};

template <>
struct ScalarTraits<ofp::IPv6Endpoint> {
  static void output(const ofp::IPv6Endpoint &value, void *ctxt,
                     llvm::raw_ostream &out) {
    out << value;
  }

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::IPv6Endpoint &value) {
    if (scalar.empty()) {
      value.clear();
      return "";
    }

    if (!value.parse(scalar)) {
      return "Invalid IP endpoint.";
    }
    return "";
  }

  // Quote IPv6Endpoint if it begins with '['.
  static QuotingType mustQuote(StringRef s) {
    return (!s.empty() && s.front() == '[') ? QuotingType::Single
                                            : QuotingType::None;
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YADDRESS_H_
