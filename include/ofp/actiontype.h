// Copyright 2014-present Bill Fisher. All rights reserved.

#ifndef OFP_ACTIONTYPE_H_
#define OFP_ACTIONTYPE_H_

#include "ofp/byteorder.h"
#include "ofp/constants.h"

namespace ofp {

struct ActionTypeInfo;

class ActionType {
 public:
  constexpr ActionType() : value32_{0} {}

  constexpr ActionType(OFPActionType type, UInt16 length)
      : value32_{make(type, length)} {}

  static ActionType fromBytes(const UInt8 *data);

  constexpr OFPActionType enumType() const {
    return static_cast<OFPActionType>(nativeType() >> 16);
  }

  // FIXME(bfish) rename to size()?
  constexpr UInt16 length() const { return UInt16_narrow_cast(nativeType()); }

  constexpr operator UInt32() const { return value32_; }

  constexpr static ActionType fromNative(UInt32 value) {
    return ActionType{value};
  }

  constexpr UInt32 nativeType() const { return BigEndianToNative(value32_); }

  constexpr ActionType zeroLength() const {
    return BigEndianFromNative(nativeType() & 0xffff0000U);
  }

  const ActionTypeInfo *lookupInfo() const;
  const ActionTypeInfo *lookupInfo_IgnoreLength() const;

  bool parse(const std::string &s);
  void setNative(UInt16 type) {
    value32_ = BigEndianFromNative(UInt32_cast(type) << 16);
  }

 private:
  UInt32 value32_;

  constexpr ActionType(UInt32 value) : value32_{value} {}

  constexpr static UInt32 make(UInt16 type, UInt16 length) {
    return BigEndianFromNative(UInt32_cast(type << 16) | length);
  }
};

static_assert(sizeof(ActionType) == 4, "Unexpected size.");
static_assert(IsLiteralType<ActionType>(), "Literal type expected.");
static_assert(IsStandardLayout<ActionType>(), "Standard Layout type expected.");

inline ActionType ActionType::fromBytes(const UInt8 *data) {
  ActionType type{0};
  std::memcpy(&type, data, sizeof(type));
  return type;
}

OFP_BEGIN_IGNORE_PADDING

struct ActionTypeInfo {
  ActionType type;
  const char *name;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_ACTIONTYPE_H_
