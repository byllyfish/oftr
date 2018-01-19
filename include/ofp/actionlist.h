// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_ACTIONLIST_H_
#define OFP_ACTIONLIST_H_

#include "ofp/actionrange.h"
#include "ofp/actions.h"
#include "ofp/protocollist.h"
#include "ofp/types.h"

namespace ofp {

class ActionList : public ProtocolList<ActionRange> {
  using Inherited = ProtocolList<ActionRange>;

 public:
  using Inherited::Inherited;

  template <class Type>
  void add(const Type &action) {
    // Make sure it's a fixed size action.
    assert(Type::type() != 0);
    buf_.add(&action, sizeof(action));
  }
};

template <>
inline void ActionList::add(const AT_SET_FIELD_CUSTOM &action) {
  ByteRange value = action.value_;
  if (action.experimenter_) {
    size_t paddedLen = PadLength(value.size() + 4);
    buf_.add(&action, AT_SET_FIELD_CUSTOM::FixedSize + 4);
    buf_.add(value.data(), value.size());
    buf_.addZeros(paddedLen - value.size() - 4);
  } else {
    size_t paddedLen = PadLength(value.size());
    buf_.add(&action, AT_SET_FIELD_CUSTOM::FixedSize);
    buf_.add(value.data(), value.size());
    buf_.addZeros(paddedLen - value.size());
  }
}

template <>
inline void ActionList::add(const AT_UNKNOWN &action) {
  ByteRange value = action.value_;
  size_t paddedLen = PadLength(AT_UNKNOWN::FixedSize + value.size());
  buf_.add(&action, AT_UNKNOWN::FixedSize);
  buf_.add(value.data(), value.size());
  buf_.addZeros(paddedLen - (AT_UNKNOWN::FixedSize + value.size()));
}

template <>
inline void ActionList::add(const AT_EXPERIMENTER &action) {
  ByteRange value = action.value_;
  size_t paddedLen = PadLength(AT_EXPERIMENTER::FixedSize + value.size());
  buf_.add(&action, AT_EXPERIMENTER::FixedSize);
  buf_.add(value.data(), value.size());
  buf_.addZeros(paddedLen - (AT_EXPERIMENTER::FixedSize + value.size()));
}

}  // namespace ofp

#endif  // OFP_ACTIONLIST_H_
