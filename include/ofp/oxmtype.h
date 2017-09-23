// Copyright (c) 2015-2017 William W. Fisher (at gmail dot com)
// This file is distributed under the MIT License.

#ifndef OFP_OXMTYPE_H_
#define OFP_OXMTYPE_H_

#include "ofp/byteorder.h"
#include "ofp/padding.h"

namespace ofp {

class OXMRange;
struct OXMTypeInfo;
enum class OXMInternalID : UInt16;

class OXMType {
 public:
  constexpr OXMType() : value32_{0} {}

  constexpr OXMType(UInt16 oxmClass, UInt8 oxmField, UInt16 oxmSize)
      : value32_{make(oxmClass, oxmField, oxmSize, false)} {}

  static OXMType fromBytes(const UInt8 *data) {
    OXMType result;
    std::memcpy(&result, data, sizeof(result));
    return result;
  }

  // Return `opaque` identifier. Value depends on host's byte order.
  constexpr operator UInt32() const { return value32_; }

  constexpr size_t length() const {
    return (value32_ & End8Bits) >> End8RightShift;
  }
  constexpr bool hasMask() const { return value32_ & MaskBits; }
  constexpr bool isIllegal() const { return hasMask() && length() == 0; }
  constexpr bool isExperimenter() const { return oxmClass() == 0xffff; }
  // constexpr bool isValid() const { return value32_ != 0; }

  // When we add the mask, double the length.
  constexpr OXMType withMask() const {
    return hasMask()
               ? *this
               : OXMType{make(oxmClass(), oxmField(),
                              (oxmSize() - expSize()) * 2 + expSize(), true)};
  }

  constexpr OXMType withoutMask() const {
    return !hasMask()
               ? *this
               : OXMType{make(oxmClass(), oxmField(),
                              (oxmSize() - expSize()) / 2 + expSize(), false)};
  }

  constexpr UInt16 oxmClass() const { return oxmNative() >> 16; }
  constexpr UInt8 oxmField() const { return (oxmNative() >> 9) & 0x07FU; }
  constexpr UInt8 oxmSize() const { return oxmNative() & 0xFFU; }

  constexpr UInt32 oxmNative() const { return BigEndianToNative(value32_); }
  void setOxmNative(UInt32 value) { value32_ = BigEndianFromNative(value); }
  void setValue32(UInt32 value) { value32_ = value; }

  const OXMTypeInfo *lookupInfo() const;
  const OXMTypeInfo *lookupInfo_IgnoreLength() const;

  OXMInternalID internalID() const;
  OXMInternalID internalID_IgnoreLength() const;
  OXMInternalID internalID_Experimenter(Big32 experimenter) const;

  bool parse(llvm::StringRef s);
  std::string toString() const { return detail::ToString(*this); }

  constexpr OXMType zeroLength() const { return OXMType(value32_ & ~End8Bits); }

 private:
  UInt32 value32_;

  enum : UInt32 {
    MaskBits = BigEndianFromNative(0x0100U),
    End7Bits = BigEndianFromNative(0x007FU),
    End8Bits = BigEndianFromNative(0x00FFU),
    End8RightShift = (End8Bits == 0x00FFU ? 0 : 24),
    Prefix24Bits = ~End8Bits
  };

  constexpr explicit OXMType(UInt32 value) : value32_{value} {}

  constexpr static UInt32 make(UInt16 oxmClass, UInt8 oxmField, UInt16 oxmSize,
                               bool oxmMask) {
    return BigEndianFromNative((UInt32_cast(oxmClass) << 16) |
                               (UInt32_cast(oxmField & 0x7F) << 9) |
                               (oxmMask ? 0x0100U : 0x0000U) | oxmSize);
  }

  /// Size of experimenter id.
  constexpr UInt8 expSize() const { return isExperimenter() ? 4 : 0; }
};

static_assert(sizeof(OXMType) == 4, "Unexpected size.");
static_assert(alignof(OXMType) == 4, "Unexpected alignment.");
static_assert(IsLiteralType<OXMType>(), "Literal type expected.");
static_assert(IsStandardLayout<OXMType>(), "Layout type expected.");
static_assert(IsTriviallyCopyable<OXMType>(), "Expected trivially copyable.");

OFP_BEGIN_IGNORE_PADDING

/// \brief Stores information about OXMTypes, such as prerequisites.
struct OXMTypeInfo {
  const char *name;
  const OXMRange *prerequisites;
  UInt32 value32;
  UInt32 experimenter;
  const char *type;
  const char *description;
};

OFP_END_IGNORE_PADDING

#if !defined(LIBOFP_LOGGING_DISABLED)
// Only used for logging. Disable when building oxm helpers.
inline llvm::raw_ostream &operator<<(llvm::raw_ostream &os,
                                     const OXMType &value) {
  auto info = value.lookupInfo();
  if (info) {
    return os << info->name;
  }
  return os << RawDataToHex(&value, sizeof(value));
}
#endif  // !defined(LIBOFP_LOGGING_DISABLED)

}  // namespace ofp

#endif  // OFP_OXMTYPE_H_
