// Copyright (c) 2015-2016 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

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
    return Interpret_cast<Type>(this);
  }

  OXMRange oxmRange() const { return value(); }
  ByteRange value() const {
    return {BytePtr(this) + sizeof(ActionType), size() - sizeof(ActionType)};
  }

  // N.B. ActionRange overrides validateInput.
  bool validateInput(Validation *context) const { return true; }

 private:
  ActionType type_;
};

}  // namespace detail

using ActionIterator =
    ProtocolIterator<detail::ActionIteratorItem, ProtocolIteratorType::Action>;

}  // namespace ofp

#endif  // OFP_ACTIONITERATOR_H_
