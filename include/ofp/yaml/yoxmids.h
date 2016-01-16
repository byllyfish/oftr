// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YOXMIDS_H_
#define OFP_YAML_YOXMIDS_H_

#include "ofp/yaml/yoxmtype.h"

namespace ofp {
namespace detail {

struct OXMIDInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::OXMID> {
  static void output(const ofp::OXMID &value, void *ctxt,
                     llvm::raw_ostream &out) {
    ofp::OXMType type = value.type();

    if (type.length() == 0) {
      out << '~';
    }
    ScalarTraits<ofp::OXMType>::output(type, ctxt, out);
    // If the oxmid is an experimenter type, tack on the experimenter code
    // following a period.
    if (type.isExperimenter()) {
      Hex32 experimenter = value.experimenter();
      out << '.';
      ScalarTraits<Hex32>::output(experimenter, ctxt, out);
    }
    if (type.hasMask()) {
      out << '/';
    }
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::OXMID &value) {
    bool hasZeroLength = false;
    bool hasMask = false;

    if (!scalar.empty() && scalar.front() == '~') {
      scalar = scalar.drop_front();
      hasZeroLength = true;
    }

    if (!scalar.empty() && scalar.back() == '/') {
      scalar = scalar.drop_back();
      hasMask = true;
    }

    // Check for experimenter format <type>.<code>
    Hex32 experimenter = 0;
    size_t period = scalar.find_last_of('.');
    if (period != StringRef::npos) {
      auto part = scalar.substr(period + 1);
      ScalarTraits<Hex32>::input(part, ctxt, experimenter);
      scalar = scalar.substr(0, period);
    }

    ofp::OXMType type;
    StringRef result = ScalarTraits<ofp::OXMType>::input(scalar, ctxt, type);
    if (result.empty()) {
      if (hasMask) {
        type = type.withMask();
      }
      if (hasZeroLength) {
        type = type.zeroLength();
      }
      value = ofp::OXMID{type, experimenter};
    }

    return result;
  }

  static bool mustQuote(StringRef s) { return !s.empty() && s.front() == '~'; }
};

template <>
struct ScalarTraits<ofp::detail::OXMIDInserter> {
  static void output(const ofp::detail::OXMIDInserter &value, void *ctxt,
                     llvm::raw_ostream &out) {}

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::detail::OXMIDInserter &value) {
    ofp::OXMIDList &list = Ref_cast<ofp::OXMIDList>(value);

    ofp::OXMID id;
    StringRef result = ScalarTraits<ofp::OXMID>::input(scalar, ctxt, id);
    if (result.empty()) {
      list.add(id);
    }
    return result;
  }

  static bool mustQuote(StringRef s) { return !s.empty() && s.front() == '~'; }
};

template <>
struct SequenceTraits<ofp::OXMIDRange> {
  using iterator = ofp::OXMIDIterator;

  static iterator begin(IO &io, ofp::OXMIDRange &range) {
    return range.begin();
  }

  static iterator end(IO &io, ofp::OXMIDRange &range) { return range.end(); }

  static void next(iterator &iter, iterator iterEnd) { ++iter; }

  static const bool flow = true;
};

template <>
struct SequenceTraits<ofp::OXMIDList> {
  static size_t size(IO &io, ofp::OXMIDList &list) { return 0; }

  static ofp::detail::OXMIDInserter &element(IO &io, ofp::OXMIDList &list,
                                             size_t index) {
    return Ref_cast<ofp::detail::OXMIDInserter>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YOXMIDS_H_
