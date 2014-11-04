// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_OXMITERATOR_H_
#define OFP_OXMITERATOR_H_

#include "ofp/oxmtype.h"

namespace ofp {

class OXMIterator {
 public:
  class Item {
   public:
    Item(const Item &) = delete;
    Item &operator=(const Item &) = delete;

    OXMType type() const { return OXMType::fromBytes(BytePtr(this)); }

    template <class ValueType>
    ValueType value() const {
      return ValueType::fromBytes(BytePtr(this) + sizeof(OXMType));
    }

    template <class ValueType>
    ValueType mask() const {
      return ValueType::fromBytes(BytePtr(this) + sizeof(OXMType) +
                                  sizeof(ValueType));
    }

    const UInt8 *unknownValuePtr() const {
      return BytePtr(this) + sizeof(OXMType);
    }

    OXMIterator position() const { return OXMIterator{BytePtr(this)}; }

   private:
    Item() = default;
  };

  const Item &operator*() const {
    return *reinterpret_cast<const Item *>(position_);
  }

  const Item *operator->() const {
    return reinterpret_cast<const Item *>(position_);
  }

  OXMType type() const { return OXMType::fromBytes(position_); }

  // No postfix ++

  void operator++() { position_ += size(); }

  bool operator==(const OXMIterator &rhs) const {
    return position_ == rhs.position_;
  }

  bool operator!=(const OXMIterator &rhs) const { return !(*this == rhs); }

  bool operator<=(const OXMIterator &rhs) const {
    return position_ <= rhs.position_;
  }

  bool operator<(const OXMIterator &rhs) const {
    return position_ < rhs.position_;
  }

  constexpr const UInt8 *data() const { return position_; }
  size_t size() const { return sizeof(OXMType) + position_[3]; }

  /// \returns Number of OXM elements between begin and end.
  static size_t distance(OXMIterator begin, OXMIterator end) {
    assert(begin <= end);

    size_t dist = 0;
    while (begin != end) {
      ++dist;
      ++begin;
    }
    return dist;
  }

 private:
  const UInt8 *position_;

  constexpr explicit OXMIterator(const void *pos)
      : position_{static_cast<const UInt8 *>(pos)} {}

  friend class OXMRange;
  friend class OXMList;
};

}  // namespace ofp

#endif  // OFP_OXMITERATOR_H_
