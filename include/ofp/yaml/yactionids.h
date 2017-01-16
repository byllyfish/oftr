// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_YAML_YACTIONIDS_H_
#define OFP_YAML_YACTIONIDS_H_

#include "ofp/actionidlist.h"
#include "ofp/yaml/yactions.h"
#include "ofp/yaml/yactiontype.h"

namespace ofp {
namespace detail {

struct ActionIDInserter {};

}  // namespace detail
}  // namespace ofp

namespace llvm {
namespace yaml {

template <>
struct ScalarTraits<ofp::ActionID> {
  static void output(const ofp::ActionID &value, void *ctxt,
                     llvm::raw_ostream &out) {
    ofp::ActionType type = value.type();
    auto info = type.lookupInfo_IgnoreLength();
    if (info) {
      out << info->name;
    } else {
      out << llvm::format("0x%04X", type.enumType());
    }
  }

  static StringRef input(StringRef scalar, void *ctxt, ofp::ActionID &value) {
    ofp::ActionType type;
    StringRef result = ScalarTraits<ofp::ActionType>::input(scalar, ctxt, type);
    if (result.empty()) {
      value = ofp::ActionID{type, 0};
    }

    return result;
  }

  static bool mustQuote(StringRef) { return false; }
};

template <>
struct ScalarTraits<ofp::detail::ActionIDInserter> {
  static void output(const ofp::detail::ActionIDInserter &value, void *ctxt,
                     llvm::raw_ostream &out) {}

  static StringRef input(StringRef scalar, void *ctxt,
                         ofp::detail::ActionIDInserter &value) {
    ofp::ActionIDList &list = Ref_cast<ofp::ActionIDList>(value);

    ofp::ActionID id;
    StringRef result = ScalarTraits<ofp::ActionID>::input(scalar, ctxt, id);
    if (result.empty()) {
      list.add(id);
    }
    return result;
  }

  static bool mustQuote(StringRef) { return false; }
};

template <>
struct SequenceTraits<ofp::ActionIDRange> {
  using iterator = ofp::ActionIDIterator;

  static iterator begin(IO &io, ofp::ActionIDRange &range) {
    return range.begin();
  }

  static iterator end(IO &io, ofp::ActionIDRange &range) { return range.end(); }

  static void next(iterator &iter, iterator iterEnd) { ++iter; }

  static const bool flow = true;
};

template <>
struct SequenceTraits<ofp::ActionIDList> {
  static size_t size(IO &io, ofp::ActionIDList &list) { return 0; }

  static ofp::detail::ActionIDInserter &element(IO &io, ofp::ActionIDList &list,
                                                size_t index) {
    return Ref_cast<ofp::detail::ActionIDInserter>(list);
  }
};

}  // namespace yaml
}  // namespace llvm

#endif  // OFP_YAML_YACTIONIDS_H_
