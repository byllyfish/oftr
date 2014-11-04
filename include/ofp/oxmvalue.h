// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_OXMVALUE_H_
#define OFP_OXMVALUE_H_

#include "ofp/byteorder.h"
#include "ofp/oxmtype.h"
#include "ofp/oxmrange.h"

namespace ofp {

enum class OXMInternalID : UInt16;

template <OXMInternalID ID, UInt16 Class, UInt8 Field, class ValueType,
          UInt16 Size, bool Mask, const OXMRange *Prereqs = nullptr>
class OXMValue {
 public:
  using NativeType = typename NativeTypeOf<ValueType>::type;

  constexpr static OXMInternalID internalId() { return ID; }
  constexpr static OXMType type() { return OXMType{Class, Field, Size}; }
  constexpr static OXMType typeWithMask() { return type().withMask(); }
  constexpr static bool maskSupported() { return Mask; }
  static inline const OXMRange *prerequisites() { return Prereqs; }

  /* implicit NOLINT */ OXMValue(NativeType value) : value_{value} {}

  NativeType value() const { return value_; }
  operator NativeType() const { return value_; }
  void operator=(NativeType value) { value_ = value; }

  static OXMValue fromBytes(const UInt8 *data) {
    OXMValue result{};
    std::memcpy(&result.value_, data, sizeof(value_));
    return result;
  }

  ValueType valueOriginal() const { return value_; }

 private:
  ValueType value_;

  // Used by fromBytes().
  OXMValue() = default;
};

}  // namespace ofp

#endif  // OFP_OXMVALUE_H_
