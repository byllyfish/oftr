//  ===== ---- ofp/actiontype.h ----------------------------*- C++ -*- =====  //
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
/// \brief Defines the ActionType class.
//  ===== ------------------------------------------------------------ =====  //

#ifndef OFP_ACTIONTYPE_H_
#define OFP_ACTIONTYPE_H_

#include "ofp/byteorder.h"
#include "ofp/constants.h"

namespace ofp {

class ActionType {
public:
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

private:
  const UInt32 value32_;

  constexpr ActionType(UInt32 value) : value32_{value} {}

  constexpr static UInt32 make(UInt16 type, UInt16 length) {
    return BigEndianFromNative(UInt32_cast(type << 16) | length);
  }
};

static_assert(IsLiteralType<ActionType>(), "Literal type expected.");

inline ActionType ActionType::fromBytes(const UInt8 *data) {
  ActionType type{0};
  std::memcpy(&type, data, sizeof(type));
  return type;
}

}  // namespace ofp

#endif  // OFP_ACTIONTYPE_H_
