//  ===== ---- ofp/instructioniterator.h -------------------*- C++ -*- =====  //
//
//  Copyright (c) 2013 William W. Fisher
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  ===== ------------------------------------------------------------ =====  //
/// \file
/// \brief Defines the InstructionIterator class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_INSTRUCTIONITERATOR_H_
#define OFP_INSTRUCTIONITERATOR_H_

#include "ofp/instructiontype.h"

namespace ofp {

class InstructionIterator {
public:
  class Item {
  public:
    Item(const Item &) = delete;
    Item &operator=(const Item &) = delete;

    InstructionType type() const { return position().type(); }

    template <class Type>
    const Type *instruction() {
      return position().instruction<Type>();
    }

    InstructionIterator position() const {
      return InstructionIterator{BytePtr(this)};
    }

  private:
    Item() = default;
  };

  const Item &operator*() const {
    return *reinterpret_cast<const Item *>(position_);
  }

  InstructionType type() const { return InstructionType::fromBytes(position_); }

  const UInt8 *data() const { return position_; }
  UInt16 size() const {
    return *reinterpret_cast<const Big16 *>(data() + sizeof(InstructionType));
  }

  template <class Type>
  const Type *instruction() {
    return reinterpret_cast<const Type *>(position_);
  }

  // No operator -> (FIXME?)
  // No postfix ++

  void operator++() {
    auto sz = size();
    assert(sz >= 4);
    position_ += sz;
  }

  bool operator==(const InstructionIterator &rhs) const {
    return position_ == rhs.position_;
  }

  bool operator!=(const InstructionIterator &rhs) const {
    return !(*this == rhs);
  }

  bool operator<=(const InstructionIterator &rhs) const {
    return position_ <= rhs.position_;
  }

  bool operator<(const InstructionIterator &rhs) const {
    return position_ < rhs.position_;
  }

  /// \returns Number of instructions between begin and end.
  static size_t distance(InstructionIterator begin, InstructionIterator end) {
    assert(begin <= end);

    size_t dist = 0;
    while (begin < end) {
      ++dist;
      ++begin;
    }
    return dist;
  }

private:
  const UInt8 *position_;

  explicit InstructionIterator(const void *pos)
      : position_{static_cast<const UInt8 *>(pos)} {}

  friend class InstructionList;
  friend class InstructionRange;
};

}  // namespace ofp

#endif  // OFP_INSTRUCTIONITERATOR_H_
