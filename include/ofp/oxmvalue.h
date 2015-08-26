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
  // TODO(bfish): Size is redundant; same as ValueType's size.
  static_assert(Class != 0xFFFF ? Size == sizeof(ValueType)
                                : Size == sizeof(ValueType) + 4,
                "Unexpected size.");

  using NativeType = typename NativeTypeOf<ValueType>::type;

  constexpr static OXMInternalID internalId() { return ID; }
  constexpr static OXMType type() { return OXMType{Class, Field, Size}; }
  constexpr static OXMType typeWithMask() { return type().withMask(); }
  constexpr static bool maskSupported() { return Mask; }
  static inline const OXMRange *prerequisites() { return Prereqs; }

  /* implicit NOLINT */ OXMValue(NativeType value) : value_{value} {}

  template <class T,
            typename = EnableIf<std::is_same<T, ValueType>::value &&
                                    !std::is_same<T, NativeType>::value,
                                T>>
  /* implicit NOLINT */ OXMValue(const T &value)
      : value_{value} {}

  NativeType value() const { return value_; }
  operator NativeType() const { return value_; }
  void operator=(NativeType value) { value_ = value; }

  static ValueType fromBytes(const UInt8 *data) {
    ValueType result{};
    std::memcpy(&result, data, sizeof(result));
    return result;
  }

  ValueType valueOriginal() const { return value_; }

 private:
  ValueType value_;

  // Used by fromBytes().
  OXMValue() = default;
};

template <OXMInternalID ID, UInt32 Experimenter, UInt8 Field, class ValueType,
          UInt16 Size, bool Mask, const OXMRange *Prereqs = nullptr>
class OXMValueExperimenter
    : public OXMValue<ID, 0xFFFF, Field, ValueType, Size, Mask, Prereqs> {
  using Inherited = OXMValue<ID, 0xFFFF, Field, ValueType, Size, Mask, Prereqs>;

 public:
  constexpr static UInt32 experimenter() { return Experimenter; }

  using Inherited::Inherited;
};

}  // namespace ofp

#endif  // OFP_OXMVALUE_H_
