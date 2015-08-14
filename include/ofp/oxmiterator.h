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
    Big32 experimenter() const { return isExperimenter() ? Big32::fromBytes(BytePtr(this) + 4) : Big32{0}; }

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

    bool isExperimenter() const { return BytePtr(this)[0] == 0xFF && BytePtr(this)[1] == 0xFF; }
  };

  const Item &operator*() const {
    return *reinterpret_cast<const Item *>(position_);
  }

  const Item *operator->() const {
    return reinterpret_cast<const Item *>(position_);
  }

  //OXMType type() const { return OXMType::fromBytes(position_); }

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

 private:
  const UInt8 *position_;

  constexpr explicit OXMIterator(const void *pos) : position_{BytePtr(pos)} {}

  friend class OXMRange;
  friend class OXMList;
};

}  // namespace ofp

#endif  // OFP_OXMITERATOR_H_
