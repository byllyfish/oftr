//  ===== ---- ofp/oxmtype.h -------------------------------*- C++ -*- =====  //
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
/// \brief Defines the OXMType class.
//  ===== ------------------------------------------------------------ =====  //

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
      : value32_{make(oxmClass, oxmField, oxmSize)} {}

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

  // When we add the mask, double the length.
  constexpr OXMType withMask() const {
    return hasMask() ? *this : OXMType((value32_ & ~End8Bits) | MaskBits |
                                       ((value32_ & End7Bits) << 1));
  }

  constexpr OXMType withoutMask() const {
    return hasMask() ? OXMType((value32_ & ~End8Bits & ~MaskBits) |
                               ((value32_ & End8Bits) >> 1))
                     : *this;
  }

  constexpr UInt16 oxmClass() const { return oxmNative() >> 16; }
  constexpr UInt8 oxmField() const { return (oxmNative() >> 9) & 0x07FU; }

  constexpr UInt32 oxmNative() const { return BigEndianToNative(value32_); }
  void setOxmNative(UInt32 value) { value32_ = BigEndianFromNative(value); }

  const OXMTypeInfo *lookupInfo() const;
  const OXMTypeInfo *lookupInfo_IgnoreLength() const;

  OXMInternalID internalID() const;
  OXMInternalID internalID_IgnoreLength() const;

  bool parse(const std::string &s);

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

  constexpr static UInt32 make(UInt16 oxmClass, UInt8 oxmField,
                               UInt16 oxmSize) {
    return BigEndianFromNative((UInt32_cast(oxmClass) << 16) |
                               (UInt32_cast(oxmField & 0x7F) << 9) | oxmSize);
  }
};

static_assert(IsLiteralType<OXMType>(), "Literal type expected.");
static_assert(IsStandardLayout<OXMType>(), "Layout type expected.");

OFP_BEGIN_IGNORE_PADDING

/// \brief Stores information about OXMTypes, such as prerequisites.
struct OXMTypeInfo {
  const char *name;
  const OXMRange *prerequisites;
  UInt32 value32;
  bool isMaskSupported;
};

OFP_END_IGNORE_PADDING

}  // namespace ofp

#endif  // OFP_OXMTYPE_H_
