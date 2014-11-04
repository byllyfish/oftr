// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_ACTIONITERATOR_H_
#define OFP_ACTIONITERATOR_H_

#include "ofp/protocoliterator.h"
#include "ofp/actiontype.h"
#include "ofp/oxmrange.h"

namespace ofp {
namespace detail {

class ActionIteratorItem : private NonCopyable {
 public:
  enum { ProtocolIteratorSizeOffset = 2, ProtocolIteratorAlignment = 8 };

  ActionType type() const { return type_; }
  UInt16 size() const { return type_.length(); }

  template <class Type>
  const Type *action() const {
    return reinterpret_cast<const Type *>(this);
  }

  OXMRange oxmRange() const { return value(); }
  ByteRange value() const {
    return {BytePtr(this) + sizeof(ActionType), size() - sizeof(ActionType)};
  }

 private:
  ActionType type_;
};

}  // namespace detail

using ActionIterator = ProtocolIterator<detail::ActionIteratorItem>;

}  // namespace ofp

#endif  // OFP_ACTIONITERATOR_H_
