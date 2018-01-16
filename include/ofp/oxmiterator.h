// Copyright (c) 2015-2018 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_OXMITERATOR_H_
#define OFP_OXMITERATOR_H_

#include "ofp/oxmtype.h"

namespace ofp {
namespace detail {

template <typename T, T>
struct SameType;

// Test if type `ValueType` has an `experimenter()` static member function.
template <class ValueType>
struct has_ExperimenterTraits {
  typedef UInt32 (*Signature_experimenter)();

  template <typename U>
  static char test(SameType<Signature_experimenter, &U::experimenter> *);

  template <typename U>
  static double test(...);

  static const bool value = (sizeof(test<ValueType>(nullptr)) == 1);
};

}  // namespace detail

class OXMIterator {
 public:
  class Item {
   public:
    Item(const Item &) = delete;
    Item &operator=(const Item &) = delete;

    OXMType type() const { return OXMType::fromBytes(BytePtr(this)); }
    Big32 experimenter() const {
      return isExperimenter() ? Big32::fromBytes(BytePtr(this) + 4) : Big32{0};
    }

    template <class ValueType>
    EnableIf<!detail::has_ExperimenterTraits<ValueType>::value, ValueType>
    value() const {
      return ValueType::fromBytes(BytePtr(this) + sizeof(OXMType));
    }

    template <class ValueType>
    EnableIf<detail::has_ExperimenterTraits<ValueType>::value, ValueType>
    value() const {
      assert(isExperimenter());
      return ValueType::fromBytes(BytePtr(this) + sizeof(OXMType) +
                                  sizeof(Big32));
    }

    template <class ValueType>
    EnableIf<!detail::has_ExperimenterTraits<ValueType>::value, ValueType>
    mask() const {
      return ValueType::fromBytes(BytePtr(this) + sizeof(OXMType) +
                                  sizeof(ValueType));
    }

    template <class ValueType>
    EnableIf<detail::has_ExperimenterTraits<ValueType>::value, ValueType> mask()
        const {
      assert(isExperimenter());
      return ValueType::fromBytes(BytePtr(this) + sizeof(OXMType) +
                                  sizeof(Big32) + sizeof(ValueType));
    }

    const UInt8 *unknownValuePtr() const {
      return BytePtr(this) + sizeof(OXMType) + (isExperimenter() ? 4 : 0);
    }

    size_t unknownValueLength() const {
      assert(!isExperimenter() || type().length() >= 4U);
      return type().length() - (isExperimenter() ? 4U : 0U);
    }

    OXMIterator position() const { return OXMIterator{BytePtr(this)}; }

   private:
    Item() = default;

    bool isExperimenter() const {
      auto p = BytePtr(this);
      return p[0] == 0xFF && p[1] == 0xFF && p[3] >= 4;
    }
  };

  const Item &operator*() const { return *Interpret_cast<Item>(position_); }

  const Item *operator->() const { return Interpret_cast<Item>(position_); }

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

 private:
  const UInt8 *position_;

  constexpr explicit OXMIterator(const void *pos) : position_{BytePtr(pos)} {}

  constexpr const UInt8 *data() const { return position_; }
  size_t size() const { return sizeof(OXMType) + position_[3]; }

  friend class OXMRange;
  friend class OXMList;
};

}  // namespace ofp

#endif  // OFP_OXMITERATOR_H_
